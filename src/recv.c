#include "../include/ft_ping.h"

static int parse_ip(const char *buffer);
static int parse_icmp(t_ping *ctx, const char *buffer, int icmp_offset);
static int wait_for_packet(t_ping *ctx);

ssize_t recv_packet(t_ping *ctx, char *buffer)
{
    struct sockaddr_in src_addr;
    socklen_t addr_len = sizeof(src_addr);
    struct timespec recv_ts;
    ssize_t bytes;
    int icmp_offset;

    while (1)
    {
        int status = wait_for_packet(ctx);
        if (status != PACKET_OK)
            return (status);
        bytes = recvfrom(ctx->sys.sockfd,
                         buffer,
                         BUFFER_SIZE,
                         0,
                         (struct sockaddr *)&src_addr,
                         &addr_len);

        if (bytes == -1)
            return PACKET_ERROR;

        clock_gettime(CLOCK_MONOTONIC, &recv_ts);

        if (bytes < (ssize_t)sizeof(struct iphdr))
            continue;

        icmp_offset = parse_ip(buffer);

        if (icmp_offset == PACKET_IGNORE)
        continue;
    
        if (bytes < icmp_offset + (ssize_t)sizeof(struct icmphdr))
            continue;
        
        if (bytes < icmp_offset + (ssize_t)sizeof(struct icmphdr) + (ssize_t)sizeof(struct timespec))
            continue;
        
        if (parse_icmp(ctx, buffer, icmp_offset) == PACKET_IGNORE)
            continue;
        break;
    }
    
    double rtt = calculate_rtt(ctx, buffer, icmp_offset, recv_ts);
    struct iphdr *ip = (struct iphdr *)buffer;
    struct icmphdr *icmp = (struct icmphdr *)(buffer + icmp_offset);
    display_loop(bytes, ip->saddr, icmp->un.echo.sequence, ip->ttl, rtt);
    ctx->stats.packets_received++;

    return bytes;
}

static int parse_ip(const char *buffer)
{
    const struct iphdr *ip = (const struct iphdr *)buffer;
    char ip_saddr_str[INET_ADDRSTRLEN];
    char ip_daddr_str[INET_ADDRSTRLEN];

    if (ip->protocol != IPPROTO_ICMP)
        return PACKET_IGNORE;
    if (ip->ihl < 5)
        return PACKET_IGNORE;

    inet_ntop(AF_INET, &ip->saddr, ip_saddr_str, sizeof(ip_saddr_str));
    inet_ntop(AF_INET, &ip->daddr, ip_daddr_str, sizeof(ip_daddr_str));

    return ip->ihl * 4;
}

static int parse_icmp(t_ping *ctx, const char *buffer, int icmp_offset)
{
    const struct icmphdr *icmp = (const struct icmphdr *)(buffer + icmp_offset);
    uint16_t id = ntohs(icmp->un.echo.id);
    uint16_t seq = ntohs(icmp->un.echo.sequence);

    if (icmp->type != ICMP_ECHOREPLY)
        return PACKET_IGNORE;
    if (id != ctx->sys.id)
        return PACKET_IGNORE;
    if (seq != ctx->sys.seq)
        return PACKET_IGNORE;
    return PACKET_OK;
}

static int wait_for_packet(t_ping *ctx)
{
    fd_set readfds;
    struct timeval timeout;

    FD_ZERO(&readfds);
    FD_SET(ctx->sys.sockfd, &readfds);

    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    int ready = select(
                ctx->sys.sockfd + 1,
                &readfds,
                NULL,
                NULL,
                &timeout
    );
    if (ready == -1)
        return PACKET_ERROR;
    if (ready == 0)
        return PACKET_TIMEOUT;
    return PACKET_OK;
}
