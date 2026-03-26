#include "../include/ft_ping.h"

volatile sig_atomic_t g_running = 1;

void handle_sigint(int sig)
{
    (void)sig;
    g_running = 0;
}

int handle_connection(t_ping *ctx, const char *host)
{
    char *send_buffer = malloc(sizeof(struct icmphdr) + ICMP_PAYLOAD_SIZE);
    memset(send_buffer, 0, sizeof(struct icmphdr) + ICMP_PAYLOAD_SIZE);
    if (!send_buffer)
        return (perror_ret("buffer"));
    char recv_buffer[BUFFER_SIZE];

    while (g_running)
    {    
        if (send_packet(ctx, send_buffer) == -1)
            return perror_ret("sendto");
        if (recv_packet(ctx, recv_buffer) == PACKET_ERROR)
            return perror_ret("recvfrom");

        ctx->seq++;
        usleep(1000000);
    }

    display_statistics(ctx, host);
    free(send_buffer);
    close(ctx->sockfd);

    return 0;
}

int main(int argc, char **argv)
{
    t_ping ctx;

    signal(SIGINT, handle_sigint);

    if (argc < 2)
        return EXIT_FAILURE;

    if (init_connection(&ctx, argv[1]) != 0)
        return EXIT_FAILURE;

    if (handle_connection(&ctx, argv[1]) != 0)
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
