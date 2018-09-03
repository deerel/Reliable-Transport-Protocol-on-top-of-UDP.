/********************************************************************
 * FILENAME:
 *       rtptypes.h
 * DESCRIPTION:
 *       Definition of global constants, types and structs.
 * AUTHOR:
 *       Rickard Lundberg & Pontus Wedén
 *******************************************************************/
#pragma once

#define TRUE 1
#define FALSE 0

#define CLOSED 0
#define ESTABLISHED 1

#define NOT_DEFINED 0;

#define OPEN 900
#define TEARDOWN 901
#define S_TEARDOWN 902

#define PACKET_DATA_SIZE 8 /* Should be a number n where n%4 = 0*/

/* Defining custom types */
typedef int state;
typedef int event;
typedef int bool;

/* Struct for protocol packets */
typedef struct rtp_
{
    unsigned char flags[4];          // [ACK|SYN|FIN|xxx]
    unsigned int ackseq;             // unique packet id
    unsigned int seq;                // window seq num
    unsigned int windowsize;         // number of slots in window
    unsigned int crc;                // Checksum
    char data[PACKET_DATA_SIZE];     // Data to transfer
} rtp;


