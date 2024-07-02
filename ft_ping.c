#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <time.h>
#include <errno.h>

#define PING_PKT_SIZE 64
#define PING_SLEEP_RATE 1000000

// Checksum function to ensure no packet's alteration
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

// Function to send and receive ping
void ping(int ping_sockfd, struct sockaddr_in *ping_addr, const char *ping_ip) {
    struct icmp pckt;
    struct timespec time_start, time_end;
    long double rtt_msec = 0;

    // Packet preparation
    memset(&pckt, 0, sizeof(pckt));
    pckt.icmp_type = ICMP_ECHO;
    pckt.icmp_code = 0;
    pckt.icmp_cksum = 0;
    pckt.icmp_seq = 0;
    pckt.icmp_id = getpid();
    
    pckt.icmp_cksum = checksum(&pckt, sizeof(pckt));

    clock_gettime(CLOCK_MONOTONIC, &time_start);

    if (sendto(ping_sockfd, &pckt, sizeof(pckt), 0, (struct sockaddr *)ping_addr, sizeof(*ping_addr)) <= 0) {
        perror("sendto");
        return;
    }

    socklen_t addr_len = sizeof(*ping_addr);
    if (recvfrom(ping_sockfd, &pckt, sizeof(pckt), 0, (struct sockaddr *)ping_addr, &addr_len) <= 0) {
        perror("recvfrom");
        return;
    }

    clock_gettime(CLOCK_MONOTONIC, &time_end);

    rtt_msec = (time_end.tv_sec - time_start.tv_sec) * 1000.0;
    rtt_msec += (time_end.tv_nsec - time_start.tv_nsec) / 1000000.0;

    printf("Ping to %s: rtt = %Lf ms\n", ping_ip, rtt_msec);
}

// Function to initialize ICMP socket
int32_t init_icmp_socket() {
    int32_t sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        perror("socket init error");
        return -1;
    }
    return sockfd;
}

void init_sock_addr(struct sockaddr_in *addr_con, const char *ip_addr) {

    memset(addr_con, 0, sizeof(struct sockaddr_in));
    addr_con->sin_family = AF_INET;
    addr_con->sin_port = htons(0);
    addr_con->sin_addr.s_addr = inet_addr(ip_addr);
}

int main() {
    int32_t sockfd;
    struct sockaddr_in addr_con;
    const char *ip_addr = "127.0.0.1";

    if ((sockfd = init_icmp_socket()) < 0)
        exit(EXIT_FAILURE);
    init_sock_addr(&addr_con, ip_addr);
    
    printf("PING localhost (%s):\n", ip_addr);

    while (1) {
        ping(sockfd, &addr_con, ip_addr);
        sleep(1);
    }

    close(sockfd);
    return 0;
}
