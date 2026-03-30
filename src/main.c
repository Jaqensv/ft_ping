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
    if (!send_buffer)
        return (perror_ret("send_buffer"));
    memset(send_buffer, 0, sizeof(struct icmphdr) + ICMP_PAYLOAD_SIZE);
    char recv_buffer[BUFFER_SIZE];

    while (g_running)
    {    
        int status;
    
        if (send_packet(ctx, send_buffer) == -1)
        {
            display_statistics(ctx, host, false);
            free_resources(ctx, send_buffer);
            return perror_ret("sendto");
        }

        status = recv_packet(ctx, recv_buffer);
        if (status == PACKET_ERROR)
        {
            display_statistics(ctx, host, false);
            free_resources(ctx, send_buffer);
            return (perror_ret("recvfrom"));
        }
        if (status == PACKET_TIMEOUT)
        {
            printf("Request timeout for icmp_seq=%u\n", ctx->sys.seq);
            ctx->sys.seq++;
            usleep(1000000);
            continue;
        }
        ctx->sys.seq++;
        usleep(1000000);
    }
    if (ctx->stats.rtt_count > 0)
    {
        calculate_avg_rtt(ctx);
        calculate_stddev(ctx);
        display_statistics(ctx, host, true);
    }

    free_resources(ctx, send_buffer);

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
