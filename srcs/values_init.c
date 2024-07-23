#include "../includes/ft_ping.h"

static int32_t init_icmp_socket() {

    int32_t sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        fprintf(stderr, "socket init error\n");
        return -1;
    }
    return sockfd;
}

void	init_data(t_data *data, int ac, char **av) {

	memset(data, 0, sizeof(t_data));
	cmd_options_init(data, ac, av);
	
	data->ip_addr = resolve_hostname_to_ip(av[ac - 1]);
	if (!data->ip_addr)
		exit(EXIT_FAILURE);

	if ((data->sockfd = init_icmp_socket()) < 0) {
		
		fprintf(stderr, "Failed to initialize socket\n");
		exit(EXIT_FAILURE);
	}
	data->payload_size = 56;
	if (data->option.i)
		data->sleep_time = data->option.i;
	else if (data->option.f || data->option.l)
		data->sleep_time = 0.000001;
	else
		data->sleep_time = 1;		
	data->dns_name = av[ac - 1];
	data->icmp_pckt_size =  sizeof(struct icmphdr) + data->payload_size;
	data->sent_pckt_count = 0;
	data->rcvd_pckt_count = 0;
	data->sequence = 0;
	data->rtt_arr = malloc(data->sequence * sizeof(long double));
	if (!data->rtt_arr) {
		close(data->sockfd);
		fprintf(stderr, "\n");
		exit(EXIT_FAILURE);
	}
}

void init_sock_addr(struct sockaddr_in *addr_con, char *ip_addr) {    
	
	memset(addr_con, 0, sizeof(struct sockaddr_in));
	addr_con->sin_family = AF_INET;
	addr_con->sin_port = htons(0);
	addr_con->sin_addr.s_addr = inet_addr(ip_addr);
}

void	init_icmp_pckt(t_icmp_pckt *pckt, t_data *data) {

	memset(pckt, 0, sizeof(t_icmp_pckt));
	pckt->hdr.type = ICMP_ECHO;
	pckt->hdr.code = 0;
	pckt->hdr.un.echo.id = getpid();
	pckt->hdr.checksum = 0;
	pckt->hdr.un.echo.sequence = data->sequence;

	for (uint16_t i = 0; i < data->payload_size - 1; i++)
    	pckt->payload[i] = (rand() % 95) + 32;
    pckt->payload[data->payload_size - 1] = '\0';
	pckt->hdr.checksum = checksum(pckt, sizeof(t_icmp_pckt));
}

void cmd_options_init(t_data *data, int ac, char **av) {

	for (uint8_t i = 1; i < (ac - 1); i += 2) {
		
		if(!av[i + 1])
			arg_error_exit_program(data);
		
		if (!strcmp(av[i], "-v")) {
			data->option.v = 1;
			i--;
			continue;
		}

		if (!strcmp(av[i], "-q")) {
			data->option.q = 1;
			i--;
			continue;
		}

		if (!strcmp(av[i], "-f")) {
			data->option.f = 1;
			i--;
			return;
		}


		for (uint8_t j = 0; av[i + 1][j]; j++)
			if (!isdigit(av[i + 1][j]))
				arg_error_exit_program(data);

		if (!strcmp(av[i], "-i")) {
			data->option.i = atoi(av[i + 1]);
			return;
		}

		if (!strcmp(av[i], "-l")) {
			data->option.l = atoi(av[i + 1]);
			return;
		}

		if (!strcmp(av[i], "-c")) {
			data->option.c = atoi(av[i + 1]);
			return;
		}
	}
}
