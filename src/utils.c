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

void display_packet(ssize_t bytes, uint32_t saddr, uint16_t seq, uint8_t ttl, double rtt)
{
    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &saddr, ip_str, sizeof(ip_str));

    printf("%zd bytes from %s: icmp_seq=%u ttl=%u time=%.3f ms\n", bytes, ip_str, ntohs(seq), ttl, rtt);
}

void display_statistics(t_ping *ctx, const char *host, bool packet_received)
{
    double loss_percent = ((ctx->stats.packets_sent - ctx->stats.packets_received) * 100.0) / ctx->stats.packets_sent;
    printf("\n--- %s ping statistics ---\n", host);
    printf("%u packets transmitted, %u packets received, %.1f%% packet loss\n", ctx->stats.packets_sent, ctx->stats.packets_received, loss_percent);
    if (packet_received == true)
        printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n", ctx->stats.min_rtt, ctx->stats.max_rtt, ctx->stats.avg_rtt, ctx->stats.stddev);
}

void free_resources(t_ping *ctx, char *buffer)
{
    free(buffer);
    free(ctx->stats.rtt_tab);
    close(ctx->sys.sockfd);
}