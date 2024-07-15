#include "../includes/ft_ping.h"
// #include "../includes/ip_icmp.h"
// #include "../includes/ip.h"

#define TTL_VALUE 64
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



// void send_pckt(t_icmp_pckt *pckt, struct sockaddr_in *addr_con, t_data *data, struct timespec *time_start) {

// 	printf("\n\n-----------\nSEND FUNC\n");

// 	for (uint16_t i = 0; i < data->payload_size; i++) {

// 		char c = i < (data->payload_size) ? ((rand() % 95) + 32) : '\0';
// 		pckt->payload[i] = c;		
// 		// if (i < data->payload_size)
// 		// 	pckt->payload[i] = (rand() % 95) + 32;
// 		// else
// 		// 	pckt->payload[i] = '\0'; 
// 	}
// 	print_packet_content(data, pckt);

// 	clock_gettime(CLOCK_MONOTONIC, time_start);
// 	pckt->hdr.checksum = checksum(pckt, sizeof(*pckt));


// 	if (sendto(data->sockfd, pckt, sizeof(*pckt), 0,
// 			   (struct sockaddr *)addr_con, sizeof(*addr_con)) <= 0) {
// 		fprintf(stderr, "sendto error");
// 		exit(EXIT_FAILURE);
// 	}
	
// 	printf("packet successfully sent\n");
// }



// // void ping(int32_t sockfd, struct sockaddr_in *addr_con, const char *ip_addr) {
// // 	static uint32_t sequence = 0;
// // 	struct icmphdr pckt_hdr;
// // 	char buffer[PING_PKT_SIZE];
// // 	long double rtt_msec = 0;

// // 	memset(&buffer, 0, sizeof(buffer));

// // 	pckt_hdr.type = ICMP_ECHO;
// // 	pckt_hdr.code = 0;
// // 	pckt_hdr.un.echo.sequence = sequence;
// // 	pckt_hdr.un.echo.id = getpid();
// // 	pckt_hdr.checksum = 0;

// // 	memcpy(buffer, &pckt_hdr, sizeof(pckt_hdr));

// // 	pckt_hdr.checksum = checksum(buffer, sizeof(buffer));

// // 	memcpy(buffer, &pckt_hdr, sizeof(pckt_hdr));

// // 	clock_gettime(CLOCK_MONOTONIC, &time_start);

// // 	if (sendto(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)addr_con, sizeof(*addr_con)) <= 0) {
// // 		perror("sendto");
// // 		return;
// // 	}




// // 	socklen_t addr_len = sizeof(*addr_con);


// // 	if (recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)addr_con, &addr_len) <= 0) {
// // 		perror("recvfrom");
// // 		return;
// // 	}

// // 	clock_gettime(CLOCK_MONOTONIC, &time_end);

// // 	rtt_msec = (time_end.tv_sec - time_start.tv_sec) * 1000.0;
// // 	rtt_msec += (time_end.tv_nsec - time_start.tv_nsec) / 1000000.0;

// // 	uint16_t ttl_value = get_received_ttl(sockfd);
// // 	sequence++;
// // 	printf("64 bytes from %s: icmp_seq=%d ttl=%d time=%.3Lf ms\n", ip_addr, sequence, ttl_value, rtt_msec);
// // }

// void receive_pckt(t_icmp_pckt *pckt, struct sockaddr_in *addr_con, t_data *data, struct timespec *time_start, uint32_t sequence) {
	
// 	// char 				buffer[84];
// 	char 				buffer[data->icmp_pckt_size + sizeof(struct iphdr)];
// 	socklen_t			addr_len = sizeof(*addr_con);
// 	struct timespec		time_end;

// 	printf("\n-----------\nRCV FUNC\n");

// 	// printf("--->>> %lu\n", data->icmp_pckt_size + sizeof(struct iphdr));
// 	// print_packet_content(data, pckt);

// 	memset(buffer, 0, sizeof(buffer));

// 	// printf("addr family %d\n", addr_con->sin_family);
 
// 	// printf("\nrecvfrom start\n");
// 	printf("SEGFAULT\n");
// 	if (recvfrom(data->sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)addr_con, &addr_len) <= 0) {
// 		fprintf(stderr, "recvfrom");
// 		exit(EXIT_FAILURE);
// 	}


// 	// printf("\nrecvfrom end\n");


// 	struct iphdr *ip_hdr = (struct iphdr *)buffer;
// 	struct icmphdr *icmp_hdr = (struct icmphdr *)(buffer + (ip_hdr->ihl * 4));
// 	char *payload = buffer + (ip_hdr->ihl * 4) + sizeof(struct icmphdr);
// 	long double rtt_msec;

// 	printf("icmp hdr type %d\n", icmp_hdr->type);

// 	if (icmp_hdr->type == ICMP_ECHOREPLY) {

// 		clock_gettime(CLOCK_MONOTONIC, &time_end);

// 		rtt_msec = (time_end.tv_sec - time_start->tv_sec) * 1000.0;
// 		rtt_msec += (time_end.tv_nsec - time_start->tv_nsec) / 1000000.0;

// 		sequence++;

// 		printf("64 bytes from %s: icmp_seq=%d ttl=%d time=%.3Lf ms\n", data->ip_addr, sequence, ip_hdr->ttl, rtt_msec);

// 	}
// 	free(pckt->payload);
// 	printf("Packket successfully received\n");

// 	(void)pckt;
// 	(void)payload;
// 	(void)sequence;
// 	(void)rtt_msec;


// 	// uint16_t ttl_value = get_received_ttl(sockfd);
// 	// sequence++;
// 	// printf("64 bytes from %s: icmp_seq=%d ttl=%d time=%.3Lf ms\n", ip_addr, sequence, ttl_value, rtt_msec);
// }



// bool check_received_error(uint16_t pckt_sent_sum, struct icmphdr *rcvd_icmp_hdr) {

	// TODO check checksum of both packets see if payload corruption

	// TODO icmphdr code and type checks
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

    struct iphdr *ip_hdr = (struct iphdr *)buffer;
    struct icmphdr *icmp_hdr = (struct icmphdr *)(buffer + (ip_hdr->ihl * 4));

    if (icmp_hdr->type == ICMP_ECHOREPLY && icmp_hdr->un.echo.id == getpid()) {
		sequence++;
        printf("64 bytes from %s: icmp_seq=%d ttl=%d time=%.3Lf ms\n", data->ip_addr, sequence, ip_hdr->ttl, rtt_msec);
    }
}


int main(int ac, char **av) {

	t_data				data;
	struct sockaddr_in	addr_con;

	// TODO add signal
	signal(SIGINT, sig_handler);


	check_args(ac);
	init_data(&data, av);
	init_sock_addr(&addr_con, data.ip_addr);
	printf("PING %s (%s): %hu data bytes\n", av[1], data.ip_addr, data.icmp_pckt_size);

	// while (1) {
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
