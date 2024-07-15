#include "../includes/ft_ping.h"
// #include "../includes/ip_icmp.h"
// #include "../includes/ip.h"

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
		fprintf(stderr, "getaddrinfo error\n");
		return NULL;
	}

	struct sockaddr_in *addr = (struct sockaddr_in *)res->ai_addr;
	inet_ntop(AF_INET, &addr->sin_addr, ip_addr, sizeof(ip_addr));
	freeaddrinfo(res);
	return ip_addr;
}

void print_packet_content(t_data *data, t_icmp_pckt *pckt) {

	(void)data;

	printf("\npckt type: %d\n",pckt->hdr.type);
	printf("pckt code: %d\n", pckt->hdr.code);
	printf("pckt pid: %d\n", pckt->hdr.un.echo.id);
	printf("pckt checksum: %d\n", pckt->hdr.checksum);
	printf("payload size: %hhu\n", data->payload_size);
	printf("payload: %s \n", pckt->payload);
}
