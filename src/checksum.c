#include "../include/ft_ping.h"

uint16_t calculate_checksum(const char *buffer, uint16_t len)
{
    uint16_t checksum = 0;
    uint32_t sum = 0;
    const uint8_t *ptr = (const uint8_t *)buffer;

    while (len > 1)
    {
        sum += ((uint16_t)ptr[0] << 8) | ptr[1];
        ptr += 2;
        len -= 2;
    }

    if (len != 0)
        sum += (uint16_t)ptr[0] << 8;

    while ((sum >> 16) != 0)
        sum = (sum & 0xFFFF) + (sum >> 16);

    checksum = (uint16_t)(~sum);
    return checksum;
}