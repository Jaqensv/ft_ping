#include "../include/ft_ping.h"

static void build_icmp_packet(t_ping *ctx, const char *buffer);

ssize_t send_packet(t_ping *ctx, const char *buffer)
{
    build_icmp_packet(ctx, buffer);
    size_t buffer_len = sizeof(struct icmphdr) + ICMP_PAYLOAD_SIZE;
    socklen_t addr_len = sizeof(struct sockaddr_in);

    ssize_t bytes = sendto(ctx->sockfd, 
                           buffer, 
                           buffer_len, 
                           0, 
                           (struct sockaddr *)&ctx->addr, 
                           addr_len);
    if (bytes == -1)
        return -1;

    ctx->packets_sent++;

    return bytes;
}

static void build_icmp_packet(t_ping *ctx, const char *buffer)
{
    struct icmphdr *icmp = (struct icmphdr *)buffer;
    unsigned char *payload = (unsigned char *)(buffer + sizeof(struct icmphdr));
    struct timespec ts;

    icmp->type = ICMP_ECHO;
    icmp->code = 0;
    icmp->un.echo.id = htons(ctx->id);
    icmp->un.echo.sequence = htons(ctx->seq);
    icmp->checksum = 0;

    memset(payload, 0, ICMP_PAYLOAD_SIZE);
    clock_gettime(CLOCK_MONOTONIC, &ts);
    memcpy(payload, &ts, sizeof(ts));

    icmp->checksum = htons(calculate_checksum(buffer, sizeof(struct icmphdr) + ICMP_PAYLOAD_SIZE));
}