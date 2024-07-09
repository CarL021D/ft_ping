#include "ft_ping.h"
#include "ip_icmp.h"

void init_sock_addr(struct sockaddr_in *addr_con, const char *ip_addr) {    
	memset(addr_con, 0, sizeof(struct sockaddr_in));
	addr_con->sin_family = AF_INET;
	addr_con->sin_port = htons(0);
	addr_con->sin_addr.s_addr = inet_addr(ip_addr);
}

void	init_data(t_data *data, char **av) {

	// (void)av;
	data->ip_addr = resolve_hostname_to_ip(av[1]);
	if (!data->ip_addr) {
		
		fprintf(stderr, "Failed to resolve hostname %s\n", av[1]);
		exit(EXIT_FAILURE);
	}

	if ((data.sockfd = init_icmp_socket()) < 0) {
		
		fprintf(stderr, "Failed to initialize socket\n");
		exit(EXIT_FAILURE);
	}
	
	data->payloa_size = 64; // to adjust depending on the command option
	data->sleep_time = 1;	// to adjust depending on the command option
	data->sequence = 0;
}

void	init_icmp_pckt(t_icmp_pckt *pckt) {

		pckt_hdr->type = ICMP_ECHO;
		pckt_hdr->code = 0;
		pckt_hdr->un.echo.sequence = sequence;
		pckt_hdr->un.echo.id = getpid();
		pckt_hdr->checksum = 0;
}
