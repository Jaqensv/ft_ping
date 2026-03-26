#include "../include/ft_ping.h"

static int  parse_ip(const char *buffer);
static int  parse_icmp(t_ping *ctx, const char *buffer, int icmp_offset);
static double calculate_rtt(const char *buffer, int icmp_offset, struct timespec recv_ts);

ssize_t recv_packet(t_ping *ctx, char *buffer)
{
    struct sockaddr_in src_addr;
    socklen_t addr_len = sizeof(src_addr);
    struct timespec recv_ts;
    ssize_t bytes;
    int icmp_offset;

    while (1)
    {
        bytes = recvfrom(ctx->sockfd,
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

        
        //printf("proto=%d ttl=%d type=%d id=%u seq=%u\n", ip->protocol, ip->ttl, icmp->type, ntohs(icmp->un.echo.id), ntohs(icmp->un.echo.sequence));
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

    double rtt = calculate_rtt(buffer, icmp_offset, recv_ts);
    struct iphdr *ip = (struct iphdr *)buffer;
    struct icmphdr *icmp = (struct icmphdr *)(buffer + icmp_offset);
    display_stats(bytes, ip->saddr, icmp->un.echo.sequence, ip->ttl, rtt);

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
    if (id != ctx->id)
        return PACKET_IGNORE;
    if (seq != ctx->seq)
        return PACKET_IGNORE;
    return PACKET_OK;
}

static double calculate_rtt(const char *buffer, int icmp_offset, struct timespec recv_ts)
{
    const struct icmphdr *icmp = (const struct icmphdr *)(buffer + icmp_offset);
    const struct timespec *sent_ts = (const struct timespec *)((const char *)icmp + sizeof(struct icmphdr));

    time_t sec = recv_ts.tv_sec - sent_ts->tv_sec;
    long nsec = recv_ts.tv_nsec - sent_ts->tv_nsec;

    if (nsec < 0)
    {
        sec--;
        nsec += 1000000000L;
    }

    return sec * 1000.0 + nsec / 1000000.0;
}