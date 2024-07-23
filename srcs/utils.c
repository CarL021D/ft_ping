#include "../includes/ft_ping.h"

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

char *resolve_hostname_to_ip(const char *hostname) {    
	struct addrinfo hints, *res;
	static char ip_addr[INET_ADDRSTRLEN];

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if (getaddrinfo(hostname, NULL, &hints, &res) != 0) {
		fprintf(stderr, "ping: unknown host\n");
		return NULL;
	}

	struct sockaddr_in *addr = (struct sockaddr_in *)res->ai_addr;
	inet_ntop(AF_INET, &addr->sin_addr, ip_addr, sizeof(ip_addr));
	freeaddrinfo(res);
	return ip_addr;
}

long double get_ping_duration(struct timespec *time_start, struct timespec *time_end) {

	long double rtt_msec;

	clock_gettime(CLOCK_MONOTONIC, time_end);
    rtt_msec = (time_end->tv_sec - time_start->tv_sec) * 1000.0;
    rtt_msec += (time_end->tv_nsec - time_start->tv_nsec) / 1000000.0;
	
	return rtt_msec;
}

long double calculate_average(t_data *data) {
    long double sum = 0.0;
    for (int i = 0; i < data->sequence; i++) {
        sum += data->rtt_arr[i];
    }
    return sum / data->sequence;
}

long double calculate_stddev(t_data *data) {
    long double avg = calculate_average(data);
    long double sum_sq_diff = 0.0;
    for (int i = 0; i < data->sequence; i++) {
        sum_sq_diff += (data->rtt_arr[i] - avg) * (data->rtt_arr[i] - avg);
    }
    return sqrt(sum_sq_diff / data->sequence);
}

void print_packet_content(t_data *data, t_icmp_pckt *pckt) {

	printf("\npckt type: %d\n",pckt->hdr.type);
	printf("pckt code: %d\n", pckt->hdr.code);
	printf("pckt pid: %d\n", pckt->hdr.un.echo.id);
	printf("pckt checksum: %d\n", pckt->hdr.checksum);
	printf("payload size: %hhu\n", data->payload_size);
	printf("payload: %s \n", pckt->payload);
}
