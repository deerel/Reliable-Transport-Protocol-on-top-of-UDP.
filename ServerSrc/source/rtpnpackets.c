#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../headers/rtpglobal.h"
#include "../crc/crc.h"

/* Variables to keep track of packet, sequence numbers and window size */
rtp * negPacket;
unsigned int ownSeq, otherSeq, windowSize;
bool synAckSent;

/* Initialize negotiation packet */
void initNegPacket()
{
    if( (negPacket = calloc(1, sizeof(rtp))) == NULL )
    {
        printf("Fail to initiate packet.\n");
        exit(1); /* Exit program */
    }
    ownSeq = initSeq();
    otherSeq = 0;
    synAckSent = FALSE;
}

/* Free allocated memory */
void disposeNegPacket()
{
    free(negPacket);
}

/* Initializing sequence number to a random number */
unsigned int initSeq()
{
    srand((unsigned int) time(NULL));
    return (unsigned int) (rand() % MAX_SEQ);
}

/* Return negotiation packet */
rtp * getNegPacket()
{
    return negPacket;
}

/* Resetting negotiation packet */
void resetNegPacket()
{
    memset(negPacket, 0, sizeof(rtp));
}

/* Returns sequence number of negotiation packet */
unsigned int getNegPacketSeq()
{
    return negPacket->seq;
}

/* Returns window size from negotiation packet */
unsigned int getNegPacketWinSize()
{
    return negPacket->windowsize;
}

/* Check CRC of negotiation packet */
bool validateNegPacket()
{
    return crcCompare(negPacket);
}

/* Check if negotiation packet is a valid ACK */
bool isCorrectNegAck()
{
    if(negPacket->flags[0] == 1 && negPacket->flags[1] == 0 && negPacket->flags[2] == 0)
    {
        if(negPacket->ackseq == ownSeq+1 && negPacket->seq == otherSeq)
        {
            ownSeq++;
            return TRUE;
        }

    }

    return FALSE;
}

/* Check if negotiation packet is a valid SYN */
bool isCorrectNegSyn()
{
    if(negPacket->flags[0] == 0 && negPacket->flags[1] == 1 && negPacket->flags[2] == 0)
    {
        otherSeq = getNegPacketSeq(); /* Init otherSeq */
        windowSize = getNegPacketWinSize(); /* Init windowSize */
        return TRUE;
    }

    return FALSE;
}

/* Check if negotiation packet is a valid FIN */
bool isCorrectNegFin()
{
    if(negPacket->flags[0] == 0 && negPacket->flags[1] == 0 && negPacket->flags[2] == 1)
    {
        return TRUE;
    }

    return FALSE;
}

/* Check if negotiation packet is a valid SYN ACK */
bool isCorrectNegSynAck() {
    if(negPacket->flags[0] == 1 && negPacket->flags[1] == 1 && negPacket->flags[2] == 0)
    {
        if(negPacket->ackseq == ownSeq+1)
        {
            ownSeq++;
            otherSeq = getNegPacketSeq();
            return TRUE;
        }
    }

    return FALSE;
}

/* Check if negotiation packet is a valid Message */
bool isCorrectWinPkt()
{
    if(negPacket->flags[0] == 0 && negPacket->flags[1] == 0 && negPacket->flags[2] == 0)
    {
        return TRUE;
    }

    return FALSE;
}

/* Setting negotiation packet to ACK */
void setNegPacketToAck()
{
    unsigned int seq = negPacket->seq;

    resetNegPacket();

    /* Checking if incoming packet is new (higher sequence) */
    if(otherSeq < seq + 1)
    {
        otherSeq = seq + 1;
    }

    negPacket->flags[0] = 1;
    negPacket->ackseq = otherSeq;
    negPacket->seq = ownSeq;
    negPacket->crc = getCRC(negPacket);
}

/* Setting negotiation packet to SYN ACK */
void setNegPacketToSynAck()
{
    if(!synAckSent)
    {
        synAckSent = TRUE;
        otherSeq = negPacket->seq + 1;
    }

    resetNegPacket();

    negPacket->flags[0] = 1;
    negPacket->flags[1] = 1;
    negPacket->ackseq = otherSeq;
    negPacket->seq = ownSeq;
    negPacket->crc = getCRC(negPacket);
}

/* Setting negotiation packet to SYN */
void setNegPacketToSyn()
{
    resetNegPacket();
    negPacket->flags[1] = 1;
    negPacket->seq = ownSeq;
    negPacket->windowsize = windowSize;
    negPacket->crc = getCRC(negPacket);
}

/* Setting negotiation packet to FIN */
void setNegPacketToFin()
{
    resetNegPacket();
    negPacket->flags[2] = 1;
    negPacket->seq = ownSeq;
    negPacket->crc = getCRC(negPacket);
}
