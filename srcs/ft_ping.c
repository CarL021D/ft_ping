#include "../includes/ft_ping.h"

volatile sig_atomic_t c_sig = 0;

void sig_handler(int _)
{
	(void)_;
	c_sig = 1;
}

void check_args_count(int ac, char **av) {

	if (ac < 2) {
		fprintf(stderr, "not enough arguments\n");
		exit(EXIT_FAILURE);
	}

	for (uint8_t i = 1; i < ac; i++) {
		if (!strcmp(av[i], "-?") || !strcmp(av[i], "--help")) {
			help_option_exec();
			exit(EXIT_SUCCESS);
		}
	}
}

void print_ping_first_output(t_data *data) {
	
	if (!data->option.v)
		printf("PING %s (%s): 56 data bytes\n", data->dns_name,
				data->ip_addr);
	else
		printf("PING %s (%s): %hd data bytes, id 0x%04x = %u\n", data->dns_name,
				data->ip_addr, PAYLOAD_SIZE, getpid(), getpid());
}

void ping_exit_output(t_data *data) {

	printf("--- %s ping statistics ---\n", data->dns_name);
	uint8_t pckts_success_rate;
	
	if (!data->rcvd_pckt_count && data->sent_pckt_count)
		pckts_success_rate = 100;
	else
		pckts_success_rate = 100 - ((data->sent_pckt_count / data->rcvd_pckt_count) * 100);
	printf("%d packets transmitted, %d packets received, %d%% packets loss\n",
		data->sent_pckt_count, data->rcvd_pckt_count, pckts_success_rate);

	if (data->option.v == 2)
		return;

	long double avrg_rtt, stddev_rtt;
	long double min_rtt = data->rtt_arr[0], max_rtt = data->rtt_arr[0];
	for (uint16_t i = 1; i < data->sequence; i++) {
	
		if (data->rtt_arr[i] < min_rtt)	
			min_rtt = data->rtt_arr[i];
		if (data->rtt_arr[i] > max_rtt)
			max_rtt = data->rtt_arr[i];	
	}
	avrg_rtt = calculate_average(data);
	stddev_rtt = calculate_stddev(data);

	printf("round-trip min/avg/max/stddev = %.3Lf/%.3Lf/%.3Lf/%.3Lf\n",
			min_rtt, avrg_rtt, max_rtt,stddev_rtt);
}

void update_data(t_data *data, long double rtt_msec) {

	long double *new_arr = realloc(data->rtt_arr, (data->sequence) * sizeof(long double));
	if (!new_arr)
		error_exit_program(data, "failed to reallocate memory");

	data->rtt_arr = new_arr;
	data->rtt_arr[data->sequence - 1] = rtt_msec;

	if (data->option.l && (data->sent_pckt_count >= data->option.l))
		data->sleep_time = 1;
}

static bool wait_response(t_data *data)
{
	 struct timeval tv = {0, 100000};

	fd_set readfds;
	FD_ZERO(&readfds);
	FD_SET(data->sockfd, &readfds);

	int32_t retval = select(data->sockfd + 1, &readfds, NULL, NULL, &tv);
	if (retval == -1)
		error_exit_program(data, "select error");
	else if (!retval) {
		data->option.v = 2;
		return false;
	}
	return true;
}

void ping(t_data *data, struct sockaddr_in *addr_con) {
	
	t_icmp_pckt pckt;
	struct timespec time_start, time_end;
	socklen_t addr_len = sizeof(*addr_con);
	char buffer[sizeof(struct iphdr) + sizeof(t_icmp_pckt)];
	long double rtt_msec;

	init_icmp_pckt(&pckt, data);
	clock_gettime(CLOCK_MONOTONIC, &time_start);

	if (sendto(data->sockfd, &pckt, sizeof(t_icmp_pckt), 0, (struct sockaddr *)addr_con, sizeof(*addr_con)) <= 0)
		error_exit_program(data, "sendto error");
	data->sent_pckt_count++;

	if (!wait_response(data))
		return;

	memset(buffer, 0, sizeof(buffer));
	while (1) {
		if (recvfrom(data->sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)addr_con, &addr_len) <= 0)
			error_exit_program(data, "recvfrom error");
		if (analyse_pckt_addr(data, buffer))
			break;
	}
	data->rcvd_pckt_count++;
	rtt_msec = get_ping_duration(&time_start, &time_end);
	print_rcvd_packet_response(data, buffer, &pckt, rtt_msec);
	update_data(data, rtt_msec);
}

int main(int ac, char **av) {

	t_data				data;
	struct sockaddr_in	addr_con;

	signal(SIGINT, sig_handler);
	check_args_count(ac, av);
	init_data(&data, ac, av);
	init_sock_addr(&addr_con, data.ip_addr);
	print_ping_first_output(&data);

	while (!c_sig && !c_option_exec(&data)) {
		ping(&data, &addr_con);
		usleep(data.sleep_time * 1000000);
	}

	ping_exit_output(&data);
	free(data.rtt_arr);
	close(data.sockfd);
}
