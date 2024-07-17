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

// void chekcums_compar(char *buffer, t_icmp_pckt *pckt) {
// 	t_icmp_pckt rcv_pckt;
	
// 	struct iphdr *ip_hdr = (struct iphdr *)buffer;
// 	struct icmphdr *icmp_hdr = (struct icmphdr *)(buffer + (ip_hdr->ihl * 4));

// 	memcpy(&rcv_pckt.hdr, icmp_hdr, sizeof(struct icmphdr));
// 	memcpy(rcv_pckt.payload, buffer + (ip_hdr->ihl * 4) + sizeof(struct icmphdr), PAYLOAD_SIZE);

// 	printf("Sent payload ------>   %s\n", pckt->payload);
// 	printf("Received payload  ------>   %s\n", rcv_pckt.payload);
// 	printf("Sent ICMP checksum ------>   %d\n", pckt->hdr.checksum);
	
// 	rcv_pckt.hdr.checksum = 0;
// 	printf("Received ICMP checksum ------>   %d\n", checksum(&rcv_pckt, sizeof(struct icmphdr) + PAYLOAD_SIZE));
// }


bool chekcums_compar(t_icmp_pckt *sent_pckt, t_icmp_pckt *rcvd_pckt) {
	
	uint16_t rcvd_checksum;

	
	rcvd_pckt->hdr.checksum = 0;
	checksum(rcvd_pckt, sizeof(struct icmphdr) + PAYLOAD_SIZE);
	rcvd_checksum = checksum(rcvd_pckt, sizeof(struct icmphdr) + PAYLOAD_SIZE);

	// printf("Sent payload ------>   %s\n", sent_pckt->payload);
	// printf("Received payload  ------>   %s\n", rcvd_pckt->payload);
	// printf("Sent ICMP checksum ------>   %d\n", sent_pckt->hdr.checksum);
	// printf("Received ICMP checksum ------>   %d\n", rcvd_checksum);

	if (sent_pckt->hdr.checksum == rcvd_checksum - 8)
		return true;
	return false;
}

// bool check_rcv_pckt_error(uint16_t sent_checksum, char *buffer) {

// ;

	
// }

void print_rcvd_packet_response(t_data *data, char *buffer, t_icmp_pckt *pckt, uint16_t sequence, long double rtt_msec) {

	t_icmp_pckt rcvd_pckt;
	
	struct iphdr *ip_hdr = (struct iphdr *)buffer;
	struct icmphdr *icmp_hdr = (struct icmphdr *)(buffer + (ip_hdr->ihl * 4));

	memcpy(&rcvd_pckt.hdr, icmp_hdr, sizeof(struct icmphdr));
	memcpy(rcvd_pckt.payload, buffer + (ip_hdr->ihl * 4) + sizeof(struct icmphdr), PAYLOAD_SIZE);

	
	rcvd_pckt.hdr.checksum = 0;

	if (!chekcums_compar(pckt, &rcvd_pckt) && strcmp(data->ip_addr, "127.0.0.1"))
		printf("CORRUPTED PAYLOAD\n");

	
	if (icmp_hdr->type == ICMP_ECHOREPLY && icmp_hdr->un.echo.id == getpid()) {
		// sequence++;
		printf("64 bytes from %s: icmp_seq=%d ttl=%d time=%.3Lf ms\n", data->ip_addr, sequence, ip_hdr->ttl, rtt_msec);
	}

	(void)pckt;
}


void ping(t_data *data, struct sockaddr_in *addr_con) {
	t_icmp_pckt pckt;
	struct timespec time_start, time_end;
	socklen_t addr_len = sizeof(*addr_con);
	char buffer[sizeof(struct iphdr) + sizeof(t_icmp_pckt)];
	long double rtt_msec;
	static uint16_t sequence = 0;

	init_icmp_pckt(&pckt, data, sequence);
	clock_gettime(CLOCK_MONOTONIC, &time_start);

for (int i = 0; i < 2; i++) {

    if (sendto(data->sockfd, &pckt, sizeof(t_icmp_pckt), 0, (struct sockaddr *)addr_con, sizeof(*addr_con)) <= 0) {
        perror("sendto");
        exit(EXIT_FAILURE);
    }

	memset(buffer, 0,sizeof(buffer));
	if (recvfrom(data->sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)addr_con, &addr_len) <= 0) {
		perror("recvfrom");
		exit(EXIT_FAILURE);
	}
}

	rtt_msec = get_ping_duration(&time_start, &time_end);
	print_rcvd_packet_response(data, buffer, &pckt, sequence, rtt_msec);
	sequence++;

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
