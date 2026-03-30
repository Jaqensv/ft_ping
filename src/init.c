#include "../include/ft_ping.h"

static int create_socket();
static int get_ip(const char *host, struct sockaddr_in *addr_in);

int init_connection(t_ping *ctx, const char *host)
{
    ctx->sys.sockfd = -1;
    ctx->sys.pid = getpid();
    ctx->sys.id = (uint16_t)getpid();
    ctx->sys.seq = 0;

    ctx->stats.rtt = 0;
    ctx->stats.min_rtt = DBL_MAX;
    ctx->stats.max_rtt = 0;
    ctx->stats.avg_rtt = 0;
    ctx->stats.rtt_sum = 0;
    ctx->stats.rtt_sum_sq = 0;
    ctx->stats.stddev = 0;

    ctx->stats.packets_sent = 0;
    ctx->stats.packets_received = 0;

    ctx->stats.rtt_tab = NULL;
    ctx->stats.rtt_count = 0;
    ctx->stats.capacity = 8;

    int ret = get_ip(host, &ctx->sys.addr);
    if (ret != 0)
        return gai_ret("getaddrinfo", ret);

    ctx->sys.sockfd = create_socket();
    if (ctx->sys.sockfd == -1)
        return perror_ret("socket");

    ctx->stats.rtt_tab = malloc(sizeof(double) * ctx->stats.capacity);
    if (!ctx->stats.rtt_tab)
        return perror_ret("rtt_tab");

    return 0;
}

static int create_socket()
{
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd == -1)
        return -1;

    return sockfd;
}

static int get_ip(const char *host, struct sockaddr_in *addr_in)
{
    struct addrinfo hints;
    struct addrinfo *res = NULL;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_RAW;

    int ret = getaddrinfo(host, NULL, &hints, &res);
    if (ret != 0)
        return -1;

    *addr_in = *(struct sockaddr_in *)res->ai_addr;

    freeaddrinfo(res);
    return ret;
}
