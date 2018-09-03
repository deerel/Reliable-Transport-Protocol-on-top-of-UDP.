/********************************************************************
 * FILENAME:
 *       rtpwindow.h
 * DESCRIPTION:
 *       Handles sliding window using Go-Back-N ARQ
 * AUTHOR:
 *       Rickard Lundberg & Pontus Wedén
 *******************************************************************/
#pragma once

#include "rtptypes.h"

#define PACKET_TIMEOUT 2000000 /* 2s default value for packets, set in microseconds */
#define SELECT_TIMEOUT 3600 /* 1 hour default value for the select, used if no packets are left to be sent */

#define PTYPE_MSG 0
#define PTYPE_ACK 1
#define PTYPE_FIN 2

#define MSG_BUFFER_SIZE 512

/* Struct for sliding window */
typedef struct _transferPkt
{
    long int timeout; /* Specified in milliseconds, long since that's what timeval uses */
    rtp * rtpPkt;
} transferPkt;

/* Sliding window container and parameters */
extern transferPkt * winOut;
extern int base;
extern int nextSeq;

/* Initializing sliding window container and parameters */
void initWinPacket();
/* Free all allocated memory */
void disposeWinPacket();

/* Return pointer to the window slot use */
rtp * getWinPacketSlot();
/* Returns the packet for incoming messages, ACKs etc */
rtp * getWinInPacketSlot();
/* Returns the type packet in incoming packet (winIn) */
int getWinPacketType();
/* Setting packet to ACK */
void setWinPacketToAck(rtp *pkt);

/* Find (reversed lookup) \n in string */
int getLineBreak(char * s);
/* Add string s to buffer */
int fillWinBuffer(char * s);
/* Print string s formated as a message */
void printWinMessage(char * s);

/* Create and send ACK*/
void sendWinAck();
/* Resend all non acked active packets */
void resendNonAckedPackets();

/* Handles packet for a received message */
void winMsgReceived();
/* Handles packet for a received ACK */
void winAckReceived();
/* Create a message packet where data is set to messageString */
rtp * createWinPacket(char * messageString);
/* Validate CRC for packet */
bool validateWinPacket(rtp * pkt);

/* Reset timeout for non-acked packets */
void resetPacketsTimeoutTime();
/* Update timeout variable for all active packets */
void updatePacketsTimeoutTime(long int time);
/* Return timeout time from oldest packet in window */
struct timeval getOldestPacketTimeout();





