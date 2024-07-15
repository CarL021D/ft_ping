#ifndef FT_PING_H
#define FT_PING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>
#include <errno.h>
#include <signal.h>

#define MAX_PACKET_SIZE 1024
#define PAYLOAD_SIZE 58
#define PING_PKT_SIZE 64
#define PING_DELAY 1000000

typedef struct	s_data {

	int32_t		sockfd;
	uint8_t    	payload_size;
	uint32_t	sleep_time;
	char 		*ip_addr;
	uint16_t    icmp_pckt_size;
}				t_data;

typedef struct	s_icmp_pckt {

	struct icmphdr	hdr;
	char payload[PAYLOAD_SIZE];
}				t_icmp_pckt;


void	init_sock_addr(struct sockaddr_in *addr_con, char *ip_addr);
void	init_data(t_data *data, char **av);
// void	init_icmp_pckt(t_icmp_pckt *pckt, t_data *data);
void	init_icmp_pckt(t_icmp_pckt *pckt, t_data *data, uint16_t sequence);

unsigned short 	checksum(void *b, int len);   
char			*resolve_hostname_to_ip(const char *hostname);
void			print_packet_content(t_data *data, t_icmp_pckt *pckt);


#endif