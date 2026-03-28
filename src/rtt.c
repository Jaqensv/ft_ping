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
    ctx->rtt_sum = 0;
    ctx->rtt_sum_sq = 0;
    
    for (size_t i = 0; i < ctx->rtt_count; i++)
    {
        ctx->rtt_sum += ctx->rtt_tab[i];
        ctx->rtt_sum_sq += ctx->rtt_tab[i] * ctx->rtt_tab[i];
    }
    ctx->avg_rtt = ctx->rtt_sum / ctx->rtt_count;
}

void calculate_stddev(t_ping *ctx)
{
    double variance = (ctx->rtt_sum_sq / ctx->rtt_count) - (ctx->avg_rtt * ctx->avg_rtt);
    if (variance < 0)
        variance = 0;
    ctx->stddev = sqrt(variance);
}

static void update_rtt_stats(t_ping *ctx, double rtt)
{
    if (ctx->rtt_count >= ctx->capacity)
    {
        ctx->capacity *= 2;
        double *tmp = realloc(ctx->rtt_tab, sizeof(double) * ctx->capacity);
        if (!tmp)
            return;
        ctx->rtt_tab = tmp;
    }
    if (rtt < ctx->min_rtt || ctx->min_rtt == 0)
        ctx->min_rtt = rtt;
    if (rtt > ctx->max_rtt || ctx->max_rtt == 0)
        ctx->max_rtt = rtt;

    ctx->rtt_tab[ctx->rtt_count] = rtt;
    ctx->rtt_count++;
}
