#include "../includes/ft_ping.h"

#define PING_PKT_SIZE 64

volatile sig_atomic_t c_sig = 0;

void sig_handler(int _)
{
	(void)_;
	c_sig = 1;
}

void check_args(int ac) {

	if (ac < 2) {
		fprintf(stderr, "not enough arguments\n");
		exit(EXIT_FAILURE);
	}

	// TODO bonus: options to set here later
}

void ping(t_data *data, struct sockaddr_in *addr_con) {
	t_icmp_pckt pckt;
	struct timespec time_start, time_end;
	socklen_t addr_len = sizeof(*addr_con);
	char buffer[sizeof(struct iphdr) + sizeof(t_icmp_pckt)];
	long double rtt_msec;

	init_icmp_pckt(&pckt, data);
	clock_gettime(CLOCK_MONOTONIC, &time_start);

    if (sendto(data->sockfd, &pckt, sizeof(t_icmp_pckt), 0, (struct sockaddr *)addr_con, sizeof(*addr_con)) <= 0) {
        perror("sendto");
        exit(EXIT_FAILURE);
    }

	memset(buffer, 0,sizeof(buffer));
	if (recvfrom(data->sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)addr_con, &addr_len) <= 0) {
		perror("recvfrom");
		exit(EXIT_FAILURE);
	}

	rtt_msec = get_ping_duration(&time_start, &time_end);
	print_rcvd_packet_response(data, buffer, &pckt, rtt_msec);

}


int main(int ac, char **av) {

	t_data				data;
	struct sockaddr_in	addr_con;

	signal(SIGINT, sig_handler);

	check_args(ac);
	init_data(&data, av);
	init_sock_addr(&addr_con, data.ip_addr);
	printf("PING %s (%s): %hu data bytes\n", av[1], data.ip_addr, data.icmp_pckt_size);

	while (!c_sig) {

		ping(&data, &addr_con);

		// time to adjust
		usleep(data.sleep_time * 100000);
	}

	// TODO print exit control c output
	printf("\ncontrol C successfull\n");

	close(data.sockfd);
	return 0;
}
