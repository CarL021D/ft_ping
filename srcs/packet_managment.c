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

bool analyse_pckt_addr(t_data *data, char *buffer) {

	struct iphdr *ip_hdr = (struct iphdr *)buffer;
	struct icmphdr *icmp_hdr = (struct icmphdr *)(buffer + (ip_hdr->ihl * 4));
	char src_ip[INET_ADDRSTRLEN];
	char dst_ip[INET_ADDRSTRLEN];

	if (inet_ntop(AF_INET, &ip_hdr->saddr, src_ip, sizeof(src_ip)) == NULL)
		error_exit_program(data, "inet_ntop src_ip error");

	if (inet_ntop(AF_INET, &ip_hdr->daddr, dst_ip, sizeof(dst_ip)) == NULL)
		error_exit_program(data, "inet_ntop dst_ip error");
	if (strcmp(src_ip, dst_ip) || (!strcmp(src_ip, dst_ip) && !icmp_hdr->type))
		return true;
	return false;
}

void print_rcvd_packet_response(t_data *data, char *buffer, t_icmp_pckt *pckt, long double rtt_msec) {

	t_icmp_pckt rcvd_pckt;
	struct iphdr *ip_hdr = (struct iphdr *)buffer;
	struct icmphdr *icmp_hdr = (struct icmphdr *)(buffer + (ip_hdr->ihl * 4));

	memcpy(&rcvd_pckt.hdr, icmp_hdr, sizeof(struct icmphdr));
	memcpy(rcvd_pckt.payload, buffer + (ip_hdr->ihl * 4) + sizeof(struct icmphdr), PAYLOAD_SIZE);

	printf("---> %u\n", ntohs(icmp_hdr->un.echo.id));
	printf("---> 0x%04x\n", ntohs(icmp_hdr->un.echo.id));

	if (!cheksums_compar(pckt, &rcvd_pckt)) {
		fprintf(stderr, "payload got corrupted\n");
		return;
	}
	if (icmp_hdr->type == ICMP_ECHOREPLY && icmp_hdr->un.echo.id == getpid()) {
		printf("64 bytes from %s: icmp_seq=%d ttl=%d time=%.3Lf ms\n", data->ip_addr, data->sequence, ip_hdr->ttl, rtt_msec);
		data->sequence++;
		return;
	}
	packet_rcvd_error_check(&rcvd_pckt);
}

void packet_rcvd_error_check(t_icmp_pckt *rcvd_pckt) {

	if (rcvd_pckt->hdr.type == 3) {

		switch (rcvd_pckt->hdr.code) {

			case 0:
				printf("Net Unreachable\n");
				break;
			case 1:
				printf("Host Unreachable\n");
				break;
			case 2:
				printf("Protocol Unreachable\n");
				break;
			case 3:
				printf("Port Unreachable\n");
				break;
			case 4:
				printf("Fragmentation Needed and Don't Fragment was Se\n");
				break;
			case 5:
				printf("Source Route Failed\n");
				break;
			case 6:
				printf("Destination Network Unknown\n");
				break;
			case 7:
				printf("Destination Host Unknown\n");
				break;
			case 8:
				printf("Source Host Isolated\n");
				break;
			case 9:
				printf("Communication with Destination Network is Administratively Prohibited\n");
				break;
			case 10:
				printf("Communication with Destination Host is Administratively Prohibited\n");
				break;
			case 11:
				printf("Destination Network Unreachable for Type of Service\n");
				break;
			case 12:
				printf("Destination Host Unreachable for Type of Service\n");
				break;
			case 13:
				printf("Communication Administratively Prohibited\n");
				break;
			case 14:
				printf("Host Precedence Violation\n");
				break;
			case 15:
				printf("Precedence cutoff in effect\n");
				break;
			default:
				break;
		}
	}

	if (rcvd_pckt->hdr.type == 5) {

		switch (rcvd_pckt->hdr.code) {
			case 0:
				printf("Redirect Datagram for the Network (or subnet)\n");
					break;
			case 1:
				printf("Redirect Datagram for the Host\n");
					break;
			case 2:
				printf("Redirect Datagram for the Type of Service and Network\n");
					break;
			case 3:
				printf("Redirect Datagram for the Type of Service and Host\n");
					break;
			default:
				break;
		}
	}

	if (rcvd_pckt->hdr.type == 9 && rcvd_pckt->hdr.code == 0)
		printf("Normal router advertisement\n");
	
	if (rcvd_pckt->hdr.type == 11) {

		switch (rcvd_pckt->hdr.code) {
			case 0:
				printf("Time to Live exceeded in Transit\n");
				break;
			case 1:
				printf("Fragment Reassembly Time Exceeded\n");
				break;
		default:
			break;
		}
	}

	if (rcvd_pckt->hdr.type == 12) {

		switch (rcvd_pckt->hdr.code) {

			case 0:
				printf("The Pointer indicates the error\n");
				break;
			case 1:
				printf("Missing a Required Option\n");
				break;
			case 2:
				printf("Bad Length\n");
				break;
			default:
				break;	
		}
	}

	if (rcvd_pckt->hdr.type == 3) {

		switch (rcvd_pckt->hdr.code) {

			case 0:
				printf("Bad SPI\n");
				break;
			case 1:
				printf("Authentication Failed\n");
				break;
			case 2:
				printf("Decompression Failed\n");
				break;
			case 3:
				printf("Decryption Failed\n");
				break;
			case 4:
				printf("Need Authentication\n");
				break;
			case 5:
				printf("Need Authorization\n");
				break;
			default:
				break;
		}
	}
}