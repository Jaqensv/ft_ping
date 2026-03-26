#include "../include/ft_ping.h"

static int create_socket();
static int get_ip(const char *host, struct sockaddr_in *addr_in);

int init_connection(t_ping *ctx, const char *host)
{
    ctx->seq = 0;
    ctx->sockfd = -1; // 0 existe, donc mettre -1 par sécurité
    ctx->pid = getpid();
    ctx->id = (uint16_t)getpid();
    ctx->packets_sent = 0;
    ctx->packets_received = 0;

    int ret = get_ip(host, &ctx->addr);
    if (ret != 0)
        return gai_ret("getaddrinfo", ret);

    ctx->sockfd = create_socket();
    if (ctx->sockfd == -1)
        return perror_ret("socket");

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
    struct addrinfo hints;                 // ce que tu demandes
    struct addrinfo *res = NULL;           // ce que tu obtiens

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_RAW;

    int ret = getaddrinfo(host, NULL, &hints, &res);
    // ret = return value, getaddrinfo envoie 0 ou code erreur.
    if (ret != 0)
        return -1;

    *addr_in = *(struct sockaddr_in *)res->ai_addr;
    // le type de cast dépend de ai_family. sockaddr_in veut dire "c'est une ipv4"

    freeaddrinfo(res);
    return ret;
}
