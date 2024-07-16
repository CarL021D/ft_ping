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

void chekcums_compar(char *buffer, t_icmp_pckt *pckt) {
    t_icmp_pckt rcv_pckt;
    
    struct iphdr *ip_hdr = (struct iphdr *)buffer;
    struct icmphdr *icmp_hdr = (struct icmphdr *)(buffer + (ip_hdr->ihl * 4));

    memcpy(&rcv_pckt.hdr, icmp_hdr, sizeof(struct icmphdr));
    memcpy(rcv_pckt.payload, buffer + (ip_hdr->ihl * 4) + sizeof(struct icmphdr), PAYLOAD_SIZE);

    printf("Sent payload ------>   %s\n", pckt->payload);
    printf("Received payload  ------>   %s\n", rcv_pckt.payload);
    printf("Sent ICMP checksum ------>   %d\n", pckt->hdr.checksum);
    
    rcv_pckt.hdr.checksum = 0;
    printf("Received ICMP checksum ------>   %d\n", checksum(&rcv_pckt, sizeof(struct icmphdr) + PAYLOAD_SIZE));
}

// bool check_rcv_pckt_error(uint16_t sent_checksum, char *buffer) {

// ;

	
// }




void ping(t_data *data, struct sockaddr_in *addr_con) {
    t_icmp_pckt pckt;
    struct timespec time_start, time_end;
    socklen_t addr_len = sizeof(*addr_con);
    long double rtt_msec;
    static uint16_t sequence = 0;

	init_icmp_pckt(&pckt, data, sequence);
    clock_gettime(CLOCK_MONOTONIC, &time_start);

    if (sendto(data->sockfd, &pckt, sizeof(t_icmp_pckt), 0, (struct sockaddr *)addr_con, sizeof(*addr_con)) <= 0) {
        perror("sendto");
        exit(EXIT_FAILURE);
    }

    char buffer[sizeof(struct iphdr) + sizeof(t_icmp_pckt)];
	memset(buffer, 0,sizeof(buffer));
    if (recvfrom(data->sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)addr_con, &addr_len) <= 0) {
        perror("recvfrom");
        exit(EXIT_FAILURE);
    }

	// TODO encapsulate within a stop time function
    clock_gettime(CLOCK_MONOTONIC, &time_end);
    rtt_msec = (time_end.tv_sec - time_start.tv_sec) * 1000.0;
    rtt_msec += (time_end.tv_nsec - time_start.tv_nsec) / 1000000.0;

 
	struct iphdr 	ip_hdr;
	struct icmphdr	icmp_hdr;

	memcpy(&ip_hdr, buffer, sizeof(struct iphdr));

	int ip_header_length = ip_hdr.ihl * 4;
	
	memcpy(&icmp_hdr, buffer + ip_header_length, sizeof(struct icmphdr));


	// TODO create a function that returns the icmp response into a packet
	// TODO check checksum of both packets see if payload corruption

	chekcums_compar(buffer, &pckt);

	if (icmp_hdr.type == ICMP_ECHOREPLY && icmp_hdr.un.echo.id == getpid()) {
		sequence++;
        printf("64 bytes from %s: icmp_seq=%d ttl=%d time=%.3Lf ms\n", data->ip_addr, sequence, ip_hdr.ttl, rtt_msec);
	}
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
