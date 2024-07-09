#include "ft_ping.h"
#include "ip_icmp.h"

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
	
	data->payloa_size = 56; // to adjust depending on the command option
	data->sleep_time = 1;	// to adjust depending on the command option
	data->sequence = 0;
}

void init_sock_addr(struct sockaddr_in *addr_con, const char *ip_addr) {    
	memset(addr_con, 0, sizeof(struct sockaddr_in));
	addr_con->sin_family = AF_INET;
	addr_con->sin_port = htons(0);
	addr_con->sin_addr.s_addr = inet_addr(ip_addr);
}

void	init_icmp_pckt(t_data *data, t_icmp_pckt *pckt) {

		pckt->type = ICMP_ECHO;
		pckt->code = 0;
		pckt->un.echo.sequence = sequence;
		pckt->un.echo.id = getpid();
		pckt->checksum = 0;

        pck->payload = malloc(sizeof(data->payload_size))
        if (!pck->payload) {
            perror("malloc error\n");
            close(data->sockfd);
            exit(EXIT_FAILURE);
        }
}
