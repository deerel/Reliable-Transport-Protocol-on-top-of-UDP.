#include <string.h>
#include <ifaddrs.h>
#include <stdlib.h>
#include <time.h>

#include "../headers/rtputilities.h"
#include "../headers/rtpglobal.h"

void getTimestamp(char * timestamp)
{
    time_t current_time;
    struct tm * time_info;

    time(&current_time);
    time_info = localtime(&current_time);

    strftime(timestamp, 9, "%H:%M:%S", time_info);

}

char * getPacketType(rtp * pkt)
{
    /* Message */
    if(pkt->flags[0] == 0 && pkt->flags[1] == 0 && pkt->flags[2] == 0)
    {
        return "Message";
    }

    /* ACK */
    if(pkt->flags[0] == 1 && pkt->flags[1] == 0 && pkt->flags[2] == 0)
    {
        return "ACK";
    }

    /* SYN */
    if(pkt->flags[0] == 0 && pkt->flags[1] == 1 && pkt->flags[2] == 0)
    {
        return "SYN";
    }

    /* SYN ACK */
    if(pkt->flags[0] == 1 && pkt->flags[1] == 1 && pkt->flags[2] == 0)
    {
        return "SYN ACK";
    }

    /* FIN */
    if(pkt->flags[0] == 0 && pkt->flags[1] == 0 && pkt->flags[2] == 1)
    {
        return "FIN";
    }

    return "Unknown";
}

int causeErrors()
{
    return rand()%ERROR_PROBABILITY;
}