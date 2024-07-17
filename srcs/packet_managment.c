#include "../includes/ft_ping.h"

void send_packet(t_data *data, t_icmp_pckt *pckt, struct sockaddr_in *addr_con) {

    if (sendto(data->sockfd, pckt, sizeof(t_icmp_pckt), 0, (struct sockaddr *)addr_con, sizeof(*addr_con)) <= 0) {
        perror("sendto");
        exit(EXIT_FAILURE);
    }
}

void receive_packet(t_data *data, char *buffer, struct sockaddr_in *addr_con) {

	socklen_t addr_len = sizeof(*addr_con);

    if (recvfrom(data->sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)addr_con, &addr_len) <= 0) {
		perror("recvfrom");
		exit(EXIT_FAILURE);
	} 
}
void print_packet_diagnostic() {}