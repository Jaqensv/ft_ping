#include "../include/ft_ping.h"

static void update_rtt_stats(t_ping *ctx, double rtt);

double calculate_rtt(t_ping *ctx, const char *buffer, int icmp_offset, struct timespec recv_ts)
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

    double rtt = sec * 1000.0 + nsec / 1000000.0;
    update_rtt_stats(ctx, rtt);

    return rtt;
}


void calculate_avg_rtt(t_ping *ctx)
{
    ctx->stats.rtt_sum = 0;
    ctx->stats.rtt_sum_sq = 0;

    if (ctx->stats.rtt_count == 0)
    {
        ctx->stats.avg_rtt = 0;
        return;
    }
    
    for (size_t i = 0; i < ctx->stats.rtt_count; i++)
    {
        ctx->stats.rtt_sum += ctx->stats.rtt_tab[i];
        ctx->stats.rtt_sum_sq += ctx->stats.rtt_tab[i] * ctx->stats.rtt_tab[i];
    }

    ctx->stats.avg_rtt = ctx->stats.rtt_sum / ctx->stats.rtt_count;
}

void calculate_stddev(t_ping *ctx)
{
    if (ctx->stats.rtt_count == 0)
    {
        ctx->stats.stddev = 0;
        return;
    }

    double variance = (ctx->stats.rtt_sum_sq / ctx->stats.rtt_count) - (ctx->stats.avg_rtt * ctx->stats.avg_rtt);
    if (variance < 0)
        variance = 0;

    ctx->stats.stddev = sqrt(variance);
}

static void update_rtt_stats(t_ping *ctx, double rtt)
{
    if (ctx->stats.rtt_count >= ctx->stats.capacity)
    {
        ctx->stats.capacity *= 2;
        double *tmp = realloc(ctx->stats.rtt_tab, sizeof(double) * ctx->stats.capacity);
        if (!tmp)
            return;
        ctx->stats.rtt_tab = tmp;
    }
    if (rtt < ctx->stats.min_rtt || ctx->stats.min_rtt == 0)
        ctx->stats.min_rtt = rtt;
    if (rtt > ctx->stats.max_rtt || ctx->stats.max_rtt == 0)
        ctx->stats.max_rtt = rtt;

    ctx->stats.rtt_tab[ctx->stats.rtt_count] = rtt;
    ctx->stats.rtt_count++;
}
