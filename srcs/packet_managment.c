#include "../includes/ft_ping.h"

bool cheksums_compar(t_icmp_pckt *sent_pckt, t_icmp_pckt *rcvd_pckt) {
	
	uint16_t rcvd_checksum;

	rcvd_pckt->hdr.checksum = 0;
	checksum(rcvd_pckt, sizeof(struct icmphdr) + PAYLOAD_SIZE);
	rcvd_checksum = checksum(rcvd_pckt, sizeof(struct icmphdr) + PAYLOAD_SIZE);

	// printf("Sent payload ------>   %s\n", sent_pckt->payload);
	// printf("Received payload  ------>   %s\n", rcvd_pckt->payload);
	// printf("Sent ICMP checksum ------>   %d\n", sent_pckt->hdr.checksum);
	// printf("Received ICMP checksum ------>   %d\n", rcvd_checksum);

	if (sent_pckt->hdr.checksum == rcvd_checksum - 8)
		return true;
	return false;
}

void print_rcvd_packet_response(t_data *data, char *buffer, t_icmp_pckt *pckt, long double rtt_msec) {

	t_icmp_pckt rcvd_pckt;
	struct iphdr *ip_hdr = (struct iphdr *)buffer;
	struct icmphdr *icmp_hdr = (struct icmphdr *)(buffer + (ip_hdr->ihl * 4));

	memcpy(&rcvd_pckt.hdr, icmp_hdr, sizeof(struct icmphdr));
	memcpy(rcvd_pckt.payload, buffer + (ip_hdr->ihl * 4) + sizeof(struct icmphdr), PAYLOAD_SIZE);

	if (!cheksums_compar(pckt, &rcvd_pckt) && strcmp(data->ip_addr, "127.0.0.1"))
		printf("CORRUPTED PAYLOAD\n");

	if (icmp_hdr->type == ICMP_ECHOREPLY && icmp_hdr->un.echo.id == getpid()) {
		printf("64 bytes from %s: icmp_seq=%d ttl=%d time=%.3Lf ms\n", data->ip_addr, data->sequence, ip_hdr->ttl, rtt_msec);
		data->sequence++;
	}

	//  Todo code and type error check to add here
}