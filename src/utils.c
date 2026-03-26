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

void display_loop(ssize_t bytes, uint32_t saddr, uint16_t seq, uint8_t ttl, double rtt)
{
    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &saddr, ip_str, sizeof(ip_str));

    printf("%zd bytes from %s: icmp_seq=%u ttl=%u time=%.3f ms\n", bytes, ip_str, ntohs(seq), ttl, rtt);
}

void display_statistics(t_ping *ctx, const char *host)
{
    double loss_percent = ((ctx->packets_sent - ctx->packets_received) * 100.0) / ctx->packets_sent;
    printf("\n--- %s ping statistics ---\n", host);
    printf("%u packets transmitted, %u packets received, %.1f%% packet loss\n", ctx->packets_sent, ctx->packets_received, loss_percent);

    //--- 8.8.8.8 ping statistics ---
    // 3 packets transmitted, 3 packets received, 0.0% packet loss
    // round-trip min/avg/max/stddev = 9.270/10.043/11.285/0.887 ms
}
