/********************************************************************
 * FILENAME:
 *       rtpglobal.h
 * DESCRIPTION:
 *       Defines values and data types that need to be accessed globally
 * AUTHOR:
 *       Rickard Lundberg & Pontus Wedén
 *******************************************************************/

#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include "rtptypes.h"
#include "rtpnpackets.h"
#include "rtpsocket.h"

#define PORT 9934
#define MAX_SEQ 100000

#define DEBUG TRUE /* TRUE or FALSE for debug or no debug */
#define ERROR_PROBABILITY 7 /* Set to above 4, higher = less errors */

#define RED "\x1B[31m"
#define MAGENTA "\x1B[35m"
#define GREY "\x1B[37m"

#define BOLD "\x1B[1m"
#define NORMAL "\x1B[0m"

/* Global variables to keep track of sequence numbers and agreed window size */
extern unsigned int ownSeq, otherSeq, windowSize;

