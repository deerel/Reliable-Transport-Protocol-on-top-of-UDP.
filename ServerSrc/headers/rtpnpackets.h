/********************************************************************
 * FILENAME:
 *       rtpnpackets.h
 * DESCRIPTION:
 *       Handles negotiation packets for three way handshake
 *       setup and for tear down.
 * AUTHOR:
 *       Rickard Lundberg & Pontus Wedén
 *******************************************************************/
#pragma once

#include "rtptypes.h"

/* Variables to keep track of packet, sequence numbers and window size */
extern rtp * negPacket;
//extern unsigned int ownSeq, otherSeq, windowSize;

/* Initialize negotiation packet */
void initNegPacket();
/* Free allocated memory */
void disposeNegPacket();
/* Initializing sequence number to a random number */
unsigned int initSeq();

/* Return negotiation packet */
rtp * getNegPacket();
/* Resetting negotiation packet */
void resetNegPacket();
/* Returns sequence number of negotiation packet */
unsigned int getNegPacketSeq();
/* Returns window size from negotiation packet */
unsigned int getNegPacketWinSize();

/* Check CRC of negotiation packet */
bool validateNegPacket();
/* Check if negotiation packet is a valid ACK */
bool isCorrectNegAck();
/* Check if negotiation packet is a valid SYN */
bool isCorrectNegSyn();
/* Check if negotiation packet is a valid FIN */
bool isCorrectNegFin();
/* Check if negotiation packet is a valid SYN ACK */
bool isCorrectNegSynAck();
/* Check if negotiation packet is a valid Message */
bool isCorrectWinPkt();

/* Setting negotiation packet to ACK */
void setNegPacketToAck();
/* Setting negotiation packet to SYN ACK */
void setNegPacketToSynAck();
/* Setting negotiation packet to SYN */
void setNegPacketToSyn();
/* Setting negotiation packet to FIN */
void setNegPacketToFin();
