#include <stdlib.h>
#include <string.h>
#include "../headers/rtpwindow.h"
#include "../crc/crc.h"
#include "../headers/rtpglobal.h"

transferPkt * winOut; /* Container for sliding window */
rtp * winIn; /* Container for incoming packets */

/* Sliding window parameters */
int base;
int nextSeq;

/* Variables to handle buffer for incoming messages */
char buffer[MSG_BUFFER_SIZE];
int buffReadPos = 0;

/* Initializing sliding window container and parameters */
void initWinPacket()
{
    int i;
    base = ownSeq % (windowSize*2); /* Sync base with ownSeq */
    nextSeq = base; /* base and nextSeq is the same when window is empty */

    winOut = calloc(windowSize*2, sizeof(transferPkt));
    winIn = calloc(1, sizeof(rtp));

    for(i = 0; i < windowSize*2; i++)
    {
        winOut[i].rtpPkt = calloc(1, sizeof(rtp));
    }
}

/* Free all allocated memory */
void disposeWinPacket()
{
    int i;

    for(i = 0; i < windowSize*2; i++)
    {
        free(winOut[i].rtpPkt);
    }

    free(winOut);

}

/* Return pointer to the window slot use */
rtp * getWinPacketSlot()
{
    if(nextSeq - base < windowSize)
    {
        memset(winOut[nextSeq].rtpPkt, 0, sizeof(rtp)); /* Zero the values since it will be reused */
        return winOut[nextSeq].rtpPkt;
    }

    return NULL; /* No packet available */
}

/* Returns the packet for incoming messages, ACKs etc */
rtp * getWinInPacketSlot()
{
    return winIn;
}

/* Returns the type packet in incoming packet (winIn) */
int getWinPacketType()
{
    rtp * pkt;

    if((pkt = getWinInPacketSlot()) == NULL)
    {
        return -1;
    }

    if(pkt->flags[0] == 0 && pkt->flags[1] == 0 && pkt->flags[2] == 0)
    {
        return PTYPE_MSG;
    }

    if(pkt->flags[0] == 1 && pkt->flags[1] == 0 && pkt->flags[2] == 0)
    {
        return PTYPE_ACK;
    }

    if(pkt->flags[0] == 0 && pkt->flags[1] == 0 && pkt->flags[2] == 1)
    {
        return PTYPE_FIN;
    }

    return -1;
}

/* Setting packet to ACK */
void setWinPacketToAck(rtp *pkt)
{
    memset(pkt, 0, sizeof(rtp));

    pkt->flags[0] = 1;

    /* Checking if incoming packet is new (higher sequence) */
    if(otherSeq == winIn->seq)
    {
        otherSeq = winIn->seq + 1;
    }

    pkt->ackseq = otherSeq;
    pkt->seq = ownSeq;

    pkt->crc = getCRC(pkt);
}

/* Find \n in string*/
int getLineBreak(char * s)
{
    int i;
    int len = strlen(s);
    for(i = len; i >= 0; --i)
    {
        if(s[i] == '\n')
        {
            return (int)i+1;
        }
    }

    return -1;
}

/* Add string s to buffer */
int fillWinBuffer(char * s)
{
    int dataLength, spaceLeft;
    dataLength = strlen(s);
    spaceLeft = (int)MSG_BUFFER_SIZE-buffReadPos;

    /* Buffer overflow */
    if(dataLength > spaceLeft)
    {
        printf("Buffer overflow, message is truncated.\n");
        strncpy(buffer + buffReadPos, s, spaceLeft);
        buffer[MSG_BUFFER_SIZE-2] = '\n';
        buffer[MSG_BUFFER_SIZE-1] = '\0';
        return -1;
    }

    strncpy(buffer + buffReadPos, s, dataLength);

    /* Add null termination to buffer for correct string behavior */
    buffer[buffReadPos+dataLength] = '\0';

    return (int)dataLength;
}

/* Print string s formated as a message */
void printWinMessage(char * s)
{
    if(strlen(s) > 1) /* Discard scrap */
    {
        printf("%s%s%s%s", MAGENTA, BOLD, s, NORMAL);
    }

}

/* Create and send ACK*/
void sendWinAck()
{
    rtp *pkt = calloc(1, sizeof(rtp));

    setWinPacketToAck(pkt);
    sendPacket(pkt);

    free(pkt);
    return;
}

/* Resend all non acked active packets */
void resendNonAckedPackets()
{
    int i;

    resetPacketsTimeoutTime();

    if(base != nextSeq)
    {
        i = base;

        while (i != nextSeq)
        {
            sendPacket(winOut[i].rtpPkt);
            i = (i + 1) % (windowSize * 2);
        }
    }
}

/* Handles packet for a received message */
void winMsgReceived()
{
    /* Only send ACK if message already received */
    if(winIn->seq != otherSeq)
    {
        sendWinAck();
        return;
    }

    int dataLength;

    /* Assume that all (full) messages ends with \n */
    if(getLineBreak(winIn->data) == -1) /* Not end of msg */
    {
        dataLength = fillWinBuffer(winIn->data);

        /* If dataLength is less than 0 there is probably an buffer overflow */
        if(dataLength > 0)
        {
            buffReadPos += dataLength;
        }
        else
        {
            buffReadPos = 0;
            printWinMessage(buffer);
        }

    }
    else if (buffReadPos > 0) /* End of msg but buffer exists */
    {
        fillWinBuffer(winIn->data);
        buffReadPos = 0;
        printWinMessage(buffer);
    }
    else /* Short message */
    {
        printWinMessage(winIn->data);
    }

    sendWinAck();
}

/* Handles packet for a received ACK */
void winAckReceived()
{
    rtp * pkt;

    if((pkt = getWinInPacketSlot()) == NULL)
    {
        return;
    }

    /* Window is empty, leave parameters unchanged */
    if(base == nextSeq)
    {
        return;
    }

    int ack = pkt->ackseq % (windowSize*2);
    int limit = (base + windowSize - 1) % (windowSize*2);

    /* Check if ack is within open window range
     * if so change parameters */
    if(base < limit)
    {
        if(ack >= base && ack <= limit)
        {
            ownSeq = pkt->ackseq;
            base = ownSeq % (windowSize*2);
        }
    }
    else
    {
        if(ack >= base || ack <= limit)
        {
            ownSeq = pkt->ackseq;
            base = ownSeq % (windowSize*2);
        }
    }

}

/* Create a message packet where data is set to messageString */
rtp * createWinPacket(char * messageString)
{
    int seqOffset;
    rtp * pkt;

    if((pkt = getWinPacketSlot()) == NULL) /* Check if packet slot is available */
    {
        return NULL;
    }

    winOut[nextSeq].timeout = PACKET_TIMEOUT; /* Sets the timeout for current rtp packet */

    /* Calculate offset for packet sequnce number */
    seqOffset = abs(nextSeq - base);

    if(seqOffset > windowSize)
    {
        seqOffset = ((windowSize*2)-seqOffset);
    }

    /* Fill packet with parameters and data */
    pkt->seq = ownSeq + seqOffset;
    memcpy(pkt->data, messageString, strlen(messageString)+1);
    pkt->crc = getCRC(pkt);

    nextSeq = (nextSeq+1) % (windowSize*2);

    return pkt;
}

/* Validate CRC for packet */
bool validateWinPacket(rtp * pkt)
{

    return crcCompare(pkt);
}


/* Reset timeout for non-acked packets */
void resetPacketsTimeoutTime()
{
    int i;
    i = base;

    while (i != nextSeq)
    {
        winOut[i].timeout = PACKET_TIMEOUT;
        i = (i + 1) % (windowSize * 2);
    }
}

/* Update timeout variable for all active packets */
void updatePacketsTimeoutTime(long int time)
{
    int i;
    i = base;

    while (i != nextSeq)
    {
        winOut[i].timeout = winOut[i].timeout - time;
        i = (i + 1) % (windowSize * 2);
    }
}

/* Return timeout time from oldest packet in window */
struct timeval getOldestPacketTimeout()
{
    struct timeval tm;

    if(nextSeq == base || winOut[base].timeout == 0) /* Check if no packets or timeout is zero */
    {
        tm.tv_sec = SELECT_TIMEOUT;
        tm.tv_usec = 0;
    }
    else
    {
        tm.tv_sec = 0;
        tm.tv_usec = winOut[base].timeout;
    }

    return tm;
}

