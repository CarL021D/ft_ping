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

#define TTL_VALUE 64
#define PING_PKT_SIZE 64

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

uint16_t print_received_ttl(int sockfd) {
    
    uint16_t received_ttl;
    socklen_t ttl_len = sizeof(received_ttl);
    if (getsockopt(sockfd, IPPROTO_IP, IP_TTL, &received_ttl, &ttl_len) != 0) {
        perror("getsockopt");
        exit(EXIT_FAILURE);
    }
    return received_ttl;
}

void ping(int sockfd, struct sockaddr_in *ping_addr, const char *ping_ip) {
    
    static uint32_t sequence = 0;
    struct icmp pckt;
    struct timespec time_start, time_end;
    long double rtt_msec = 0;
    // on mac
    char payload[PING_PKT_SIZE - sizeof(struct icmp)];

    /*
    // on linux
    char payload[PING_PKT_SIZE - sizeof(struct icmphdr)];
    */

    memset(&pckt, 0, sizeof(pckt));
    pckt.icmp_type = ICMP_ECHO;
    pckt.icmp_code = 0;
    pckt.icmp_seq = sequence++;
    pckt.icmp_id = getpid();

    for (uint16_t i = 0; i < sizeof(payload); ++i)
        payload[i] = rand() % 256;

    memcpy(pckt.icmp_data, payload, sizeof(pckt.icmp_data));

    pckt.icmp_cksum = checksum(&pckt, sizeof(pckt));

    clock_gettime(CLOCK_MONOTONIC, &time_start);

    if (sendto(sockfd, &pckt, sizeof(pckt), 0, (struct sockaddr *)ping_addr, sizeof(*ping_addr)) <= 0) {
        perror("sendto");
        return;
    }

    socklen_t addr_len = sizeof(*ping_addr);
    if (recvfrom(sockfd, &pckt, sizeof(pckt), 0, (struct sockaddr *)ping_addr, &addr_len) <= 0) {
        perror("recvfrom");
        return;
    }

    clock_gettime(CLOCK_MONOTONIC, &time_end);

    rtt_msec = (time_end.tv_sec - time_start.tv_sec) * 1000.0;
    rtt_msec += (time_end.tv_nsec - time_start.tv_nsec) / 1000000.0;

    uint16_t ttl_value = print_received_ttl(sockfd);
    printf("64 bytes from %s: icmp_seq=%d ttl=%d time=%.3Lf ms\n", ping_ip, sequence, ttl_value, rtt_msec);
}





int32_t init_icmp_socket() {
    
    int32_t sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        perror("socket init error");
        return -1;
    }

    int32_t ttl_value = TTL_VALUE;
    if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl_value, sizeof(ttl_value)) < 0) {
        perror("setsockopt");
        close(sockfd);
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
