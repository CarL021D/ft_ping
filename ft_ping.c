#include "ft_ping.h"
#include "ip_icmp.h"

#define TTL_VALUE 64
#define PING_PKT_SIZE 64

unsigned short checksum(void *b, int len) {    
	unsigned short *buf = b; 
	unsigned int sum = 0;
	unsigned short result;

	for (sum = 0; len > 1; len -= 2)
		sum += *buf++;
	if (len == 1)
		sum += *(unsigned char *)buf;
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	result = ~sum;
	return result;
}

uint16_t get_received_ttl(int sockfd) {    
	uint16_t received_ttl;
	socklen_t ttl_len = sizeof(received_ttl);
	if (getsockopt(sockfd, IPPROTO_IP, IP_TTL, &received_ttl, &ttl_len) != 0) {
		perror("getsockopt");
		exit(EXIT_FAILURE);
	}
	return received_ttl;
}


void send_ping(int32_t ping, int32_t sockfd, struct sockaddr_in *addr_con, struct icmphdr pckt_hdr) {

	char buffer[PING_PKT_SIZE];

	memset(&buffer, 0, sizeof(buffer));

	memcpy(buffer, &pckt_hdr, sizeof(pckt_hdr));

	for (uint16_t i = sizeof(pckt_hdr); i < sizeof(buffer); ++i)
		buffer[i] = rand() % 256;

	pckt_hdr.checksum = checksum(buffer, sizeof(buffer));

	memcpy(buffer, &pckt_hdr, sizeof(pckt_hdr));

	clock_gettime(CLOCK_MONOTONIC, &time_start);

	if (sendto(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)addr_con, sizeof(*addr_con)) <= 0) {
		perror("sendto");
		return;
	}
}

void ping(int32_t sockfd, struct sockaddr_in *addr_con, const char *ip_addr) {
	static uint32_t sequence = 0;
	struct icmphdr pckt_hdr;
	char buffer[PING_PKT_SIZE];
	struct timespec time_start, time_end;
	long double rtt_msec = 0;

	memset(&buffer, 0, sizeof(buffer));

	pckt_hdr.type = ICMP_ECHO;
	pckt_hdr.code = 0;
	pckt_hdr.un.echo.sequence = sequence;
	pckt_hdr.un.echo.id = getpid();
	pckt_hdr.checksum = 0;

	memcpy(buffer, &pckt_hdr, sizeof(pckt_hdr));

	for (uint16_t i = sizeof(pckt_hdr); i < sizeof(buffer); ++i)
		buffer[i] = rand() % 256;

	pckt_hdr.checksum = checksum(buffer, sizeof(buffer));

	memcpy(buffer, &pckt_hdr, sizeof(pckt_hdr));

	clock_gettime(CLOCK_MONOTONIC, &time_start);

	if (sendto(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)addr_con, sizeof(*addr_con)) <= 0) {
		perror("sendto");
		return;
	}



	socklen_t addr_len = sizeof(*addr_con);
	if (recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)addr_con, &addr_len) <= 0) {
		perror("recvfrom");
		return;
	}

	clock_gettime(CLOCK_MONOTONIC, &time_end);

	rtt_msec = (time_end.tv_sec - time_start.tv_sec) * 1000.0;
	rtt_msec += (time_end.tv_nsec - time_start.tv_nsec) / 1000000.0;

	uint16_t ttl_value = get_received_ttl(sockfd);
	sequence++;
	printf("64 bytes from %s: icmp_seq=%d ttl=%d time=%.3Lf ms\n", ip_addr, sequence, ttl_value, rtt_msec);
}

void receive_ping() {
	
	socklen_t addr_len = sizeof(*addr_con);
	if (recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)addr_con, &addr_len) <= 0) {
		perror("recvfrom");
		return;
	}

	clock_gettime(CLOCK_MONOTONIC, &time_end);

	rtt_msec = (time_end.tv_sec - time_start.tv_sec) * 1000.0;
	rtt_msec += (time_end.tv_nsec - time_start.tv_nsec) / 1000000.0;

	uint16_t ttl_value = get_received_ttl(sockfd);
	sequence++;
	printf("64 bytes from %s: icmp_seq=%d ttl=%d time=%.3Lf ms\n", ip_addr, sequence, ttl_value, rtt_msec);
}


int32_t init_icmp_socket() {   
	
	int32_t sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sockfd < 0) {
		perror("socket init error");
		return -1;
	}

	int32_t ttl_value = TTL_VALUE;
	if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl_value, sizeof(ttl_value)) < 0) {
		perror("setsockopt");
		close(sockfd);
		return -1;
	}

	return sockfd;
}


const char *resolve_hostname_to_ip(const char *hostname) {    
	struct addrinfo hints, *res;
	static char ip_addr[INET_ADDRSTRLEN];

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if (getaddrinfo(hostname, NULL, &hints, &res) != 0) {
		perror("getaddrinfo");
		return NULL;
	}

	struct sockaddr_in *addr = (struct sockaddr_in *)res->ai_addr;
	inet_ntop(AF_INET, &addr->sin_addr, ip_addr, sizeof(ip_addr));
	freeaddrinfo(res);
	return ip_addr;
}


int main(int ac, char **av) {

	t_data				data;
	t_icmp_pckt			icmp_pckt;
	struct sockaddr_in	addr_con;

	(void)ac;


	init_data(&data);
	init_sock_addr(&addr_con, &(dataip_addr));
	printf("PING %s (%s): 56 data bytes\n", av[1], ip_addr);

	while (1) {
		
		init_icmp_pckt(&icmp_pckt);

		send_ping(&data, &addr_con, &icmp_pckt);
		// receive_ping(&data, &addr_con, &icmp_pckt);

		// to remove later
		// ping(sockfd, &addr_con, ip_addr);

		// time to adjust
		sleep(data.sleep_time);
	}

	close(sockfd);
	return 0;
}
