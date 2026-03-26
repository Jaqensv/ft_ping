#include "../include/ft_ping.h"

int perror_ret(const char *msg)
{
    perror(msg);
    return -1;
}

int gai_ret(const char *msg, int errcode)
{
    fprintf(stderr, "%s: %s\n", msg, gai_strerror(errcode));
    return -1;
}

void display_stats(ssize_t bytes, uint32_t saddr, uint16_t seq, uint8_t ttl, double rtt)
{
    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &saddr, ip_str, sizeof(ip_str));

    printf("%zd bytes from %s: icmp_seq=%u ttl=%u time=%.3f ms\n", bytes, ip_str, ntohs(seq), ttl, rtt);
}
