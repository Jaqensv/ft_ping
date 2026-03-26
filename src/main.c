#include "../include/ft_ping.h"

int handle_connection(t_ping *ctx)
{
    //char ip[INET_ADDRSTRLEN];
    char *send_buffer = malloc(sizeof(struct icmphdr) + ICMP_PAYLOAD_SIZE);
    memset(send_buffer, 0, sizeof(struct icmphdr) + ICMP_PAYLOAD_SIZE);
    if (!send_buffer)
        return (perror_ret("buffer"));
    char recv_buffer[BUFFER_SIZE];

    while (1)
    {    
        if (send_packet(ctx, send_buffer) == -1)
            return perror_ret("sendto");
        if (recv_packet(ctx, recv_buffer) == PACKET_ERROR)
            return perror_ret("recvfrom");

        ctx->seq++;
        usleep(1000000);
    }

    free(send_buffer);
    close(ctx->sockfd);

    return 0;
}

int main(int argc, char **argv)
{
    t_ping ctx;

    if (argc < 2)
        return EXIT_FAILURE;

    if (init_connection(&ctx, argv[1]) != 0)
        return EXIT_FAILURE;

    if (handle_connection(&ctx) != 0)
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
