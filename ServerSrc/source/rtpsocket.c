#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "../headers/rtpsocket.h"
#include "../headers/rtputilities.h"
#include "../headers/rtpglobal.h"

struct sockaddr_in addrIn, addrOther;
int sock;

/* Initializes the socket that will be used for inbound communication */
void initServerSocket(uint16_t port)
{
    int optval = 1;
    /* Creating and defining a socket */
    sock=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock==-1)
    {
        perror("socket");
    }

    /* Do not lock local addresses */
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const void*)&optval, sizeof(int)) < 0)
    {
        perror("setsockopt(SO_REUSEADDR)");
    }

    /* TIMEOUT */
    setDefaultSocketTimeout();

    /* Clearing listening socket (filling with zeros) */
    memset((char *) &addrIn, 0, sizeof(struct sockaddr_in));
    memset((char *) &addrOther, 0, sizeof(struct sockaddr_in));

    /* Setup listening socket */
    addrIn.sin_family       = AF_INET;
    addrIn.sin_port         = htons(port);
    addrIn.sin_addr.s_addr  = htonl(INADDR_ANY);

    /* Bind socket to (listening) address */
    if (bind(sock, (struct sockaddr*)&addrIn, sizeof(struct sockaddr_in))==-1)
    {
        perror("bind");
    }
}

/* Initializes the socket used for outbound communication */
void initClientSocket(uint16_t port, char * hostName)
{
    int optval = 1;

    sock=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); /* Creating and defining a socket */
    if (sock==-1)
    {
        perror("socket");
    }

    /* Do not lock local addresses */
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const void*)&optval, sizeof(int)) < 0)
    {
        perror("setsockopt(SO_REUSEADDR)");
    }

    setDefaultSocketTimeout(); /* Setting the default timeout for the socket */

    memset((char *) &addrOther, 0, sizeof(struct sockaddr_in)); /* Clearing send to address (filling with zeros) */

    addrOther.sin_family = AF_INET;
    addrOther.sin_port = htons(port);

    if(inet_aton(hostName, &addrOther.sin_addr) == 0)
    {
        perror("inet_aton() failed");
    }
}

/* Receives packet sent to the program and prints a timestamp */
int receivePacket(rtp * packet)
{
    char time[9];
    unsigned int slen = sizeof(addrOther);

    /* -1 means error, and in our case the error most often is a timeout. We will wait here
     * until we receive a packet or we timeout */
    if (recvfrom(sock, packet, sizeof(rtp), 0, (struct sockaddr*)&addrOther, &slen)==-1)
    {
        return -1;
    }

    getTimestamp(time);
    printf("%s%s Rcvd    %-7s (A: %5d, S: %5d)%s\n", GREY, time, getPacketType(packet), packet->ackseq, packet->seq, NORMAL);

    return 0;
}

/* Sends a packet sent to it and prints a timestamp for the operation*/
int sendPacket(rtp * packet)
{
    /* For timestamp */
    char timestamp[9];
    getTimestamp(timestamp);

    /* If packet is NULL there is nothing to send */
    if(packet == NULL)
    {
        return -1;
    }

    /* Copy packet to be able to non destructively
     * change content for debugging and testing */
    rtp * pkt = malloc(sizeof(rtp));
    memcpy(pkt, packet, sizeof(rtp));


    /* If DEBUG is activated randomize error/no error */
    if(DEBUG)
    {
        switch(causeErrors())
        {
            case 0: /* Change data */
                printf("%s%s Corrupt %-7s (A: %5d, S: %5d)%s\n", RED, timestamp, getPacketType(pkt), pkt->ackseq, pkt->seq, NORMAL);
                pkt->data[0] = (char)(pkt->data[0] ^ 0x1); /* Changing left most bit */
                break;
            case 1: /* Lose packet & packet out-of-order on long messages */
                getTimestamp(timestamp);
                printf("%s%s Losing  %-7s (A: %5d, S: %5d)%s\n", RED, timestamp, getPacketType(pkt), pkt->ackseq, pkt->seq, NORMAL);
                free(pkt);
                return 0;
                break;
            default:
                break;
        }

    }

    if (sendto(sock, pkt, sizeof(rtp), 0, (struct sockaddr*)&addrOther, sizeof(addrOther))==-1)
    {
        free(pkt);
        return -1;
    }

    /* Mandatory timestamp */
    printf("%s%s Sent    %-7s (A: %5d, S: %5d)%s\n", GREY, timestamp, getPacketType(pkt), pkt->ackseq, pkt->seq, NORMAL);
    free(pkt);
    return 0; /* All good */
}

/* Sets the timeout option for the server socket */
void setSocketTimeout(int sec)
{
    /* Setting the timeout to seconds + nanoseconds, i.e int sec + 0 */
    struct timeval tv;
    tv.tv_sec = sec;
    tv.tv_usec = 0;
    if(setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(struct timeval)) == -1)
    {
        perror("Socket opt");
    }
}

/* Sets the socket timeout to a default value defined by SOCKET_TIMEOUT */
void setDefaultSocketTimeout()
{
    setSocketTimeout(SOCKET_TIMEOUT);
}

/* Returns the server socket to those who ask for it */
int getSocket()
{
    return sock;
}