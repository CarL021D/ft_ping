#include "../includes/ft_ping.h"
#include "../includes/ip_icmp.h"
#include "../includes/ip.h"

void print_packet_content(t_data *data, t_icmp_pckt *pckt) {

	(void)data;

	printf("\npckt type: %d\n",pckt->hdr->type);
	printf("pckt code: %d\n", pckt->hdr->code);
	printf("pckt pid: %d\n",	pckt->hdr->un.echo.id);
	printf("pckt checksum: %d\n",	pckt->hdr->checksum);
	printf("payload size: %hhu\n", data->payload_size);
	printf("payload:   %s \n", pckt->payload);
}
