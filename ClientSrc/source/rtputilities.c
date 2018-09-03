#include <string.h>
#include <time.h>

#include "../headers/rtputilities.h"
#include "../headers/rtpglobal.h"

/* Returns the current time formatted as hh:mm:ss*/
void getTimestamp(char * timestamp)
{
    time_t current_time;
    struct tm * time_info;

    time(&current_time);
    time_info = localtime(&current_time);

    strftime(timestamp, 9, "%H:%M:%S", time_info);

}

/* Returns the type of the package sent to it */
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

/* Determines the error frequency for the error generator */
int causeErrors()
{
    return rand()%ERROR_PROBABILITY;
}