#ifndef FT_PING_H
#define FT_PING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
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

typedef struct s_option {

	uint8_t v;
	uint8_t f;
	uint8_t l;
	uint8_t c;
	uint8_t q;
	uint8_t p;
}			t_option;

typedef struct	s_data {

	int32_t			sockfd;
	uint8_t    		payload_size;
	double		sleep_time;
	char			*dns_name;
	char 			*ip_addr;
	uint16_t   		icmp_pckt_size;
	uint16_t		sent_pckt_count;
	uint16_t		rcvd_pckt_count;
	uint16_t    	sequence;
	long double		*rtt_arr;
	t_option		option;
}			t_data;


typedef struct	s_icmp_pckt {

	struct icmphdr	hdr;
	char payload[PAYLOAD_SIZE];
}			t_icmp_pckt;


void	init_sock_addr(struct sockaddr_in *addr_con, char *ip_addr);
void	init_data(t_data *data, int ac, char **av);
void	init_icmp_pckt(t_icmp_pckt *pckt, t_data *data);
void 	cmd_options_init(t_data *data, int ac, char **av);


bool	cheksums_compar(t_icmp_pckt *sent_pckt, t_icmp_pckt *rcvd_pckt);
bool	analyse_pckt_addr(t_data *data, char *buffer);
void	print_rcvd_packet_response(t_data *data, char *buffer, t_icmp_pckt *pckt, long double rtt_msec);
void	packet_rcvd_error_check(t_icmp_pckt *rcvd_pckt);

// void	update_data(t_data *data, long double rtt_msec);

unsigned short 	checksum(void *b, int len);   
char			*resolve_hostname_to_ip(const char *hostname);
long double		get_ping_duration(struct timespec *time_start, struct timespec *time_end);
long double		calculate_average(t_data *data);
long double		calculate_stddev(t_data *data);
void			print_packet_content(t_data *data, t_icmp_pckt *pckt);

bool	c_option_exec(t_data *data);
void	v_option_exec(t_data *data, char *buffer);
void	help_option_exec();

void	arg_error_exit_program();
void	error_exit_program(t_data *data, char *error_message);

#endif