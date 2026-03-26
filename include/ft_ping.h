#pragma once

#include <netdb.h>                  // getaddrinfo()
#include <sys/socket.h>             // AF_INET, sockaddr...
#include <sys/types.h>              // portabilité des types // pid_t
#include <arpa/inet.h>              // inet_ntop // network-to-presentation (conversion adresse ip) // INET_ADDRSTRLEN // htons, ntohs
#include <stdio.h>  
#include <stdlib.h>                 // EXIT
#include <string.h>                 
#include <time.h>                   // clock_gettime(CLOCK_MONOTONIC)
#include <unistd.h>                 // close() // getpid()
#include <netinet/ip_icmp.h>        // icmphdr
#include <netinet/ip.h>             // iphdr

#define ICMP_PAYLOAD_SIZE 56
#define BUFFER_SIZE 1024
#define PACKET_OK 1
#define PACKET_IGNORE 0
#define PACKET_ERROR -1

typedef struct s_ping
{
    int sockfd;
    pid_t pid;
    uint16_t id;
    uint16_t seq;
    double rtt;
    struct sockaddr_in addr;
} t_ping;

/* -init- */
int init_connection(t_ping *ctx, const char *host);

/* -utils- */
int perror_ret(const char *msg);
int gai_ret(const char *msg, int errcode);
void display_stats(ssize_t bytes, uint32_t saddr, uint16_t seq, uint8_t ttl, double rtt);

/* -packet */
ssize_t send_packet(t_ping *ctx, const char *buffer);

/* -recv- */
ssize_t recv_packet(t_ping *ctx, char *buffer);

/* -checksum- */
uint16_t calculate_checksum(const char *buffer, uint16_t len);