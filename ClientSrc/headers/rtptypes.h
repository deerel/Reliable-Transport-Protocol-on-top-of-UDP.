/********************************************************************
 * FILENAME:
 *       rtptypes.h
 * DESCRIPTION:
 *       Definition of global constants, types and structs.
 * AUTHOR:
 *       Rickard Lundberg & Pontus Wedén
 *******************************************************************/
#pragma once

#define NOT_DEFINED 0

#define TRUE 1
#define FALSE 0

/*Main machine states*/
#define CLOSED 0
#define ESTABLISHED 1

/*Main machine transitions*/
#define OPEN 900
#define TEARDOWN 901
#define S_TEARDOWN 902

#define PACKET_DATA_SIZE 8

typedef struct rtp_
{
    unsigned char flags[4];     // [ACK|SYN|FIN|xxx]
    unsigned int ackseq;        // unique packet id
    unsigned int seq;           // window seq num
    unsigned int windowsize;    // number of slots in window
    unsigned int crc;           // Checksum
    char data[PACKET_DATA_SIZE];// Data to transfer
} rtp;

typedef int state;
typedef int event;
typedef int bool;
