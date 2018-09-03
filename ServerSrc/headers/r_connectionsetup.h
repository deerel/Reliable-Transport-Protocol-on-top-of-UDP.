/********************************************************************
 * FILENAME:
 *       r_connectionsetup.h
 * DESCRIPTION:
 *       Handles connection setup for the receiving side
 * AUTHOR:
 *       Rickard Lundberg & Pontus Wedén
 *******************************************************************/
#pragma once

#include "rtptypes.h"

#define S_LISTEN 30
#define S_SYN_RECEIVED 31

#define S_TIMEOUT 300
#define S_RECEIVE_SYN 301
#define S_RECEIVE_ACK 302
#define S_RECEIVE_PACKET 303
#define S_RECEIVE_SYN_WITH_ERROR 304
#define S_RECEIVE_PKT_WITH_ERROR 305
#define S_RECEIVE_UNEXPECTED 306

/* Handles three way handshake */
bool r_connectionSetup();
/* Handles state Listen */
event stateListen();
/* Handles state Syn Received */
event stateSynReceive();

/* Send packet with SYN ACK */
void sendSynAck();

