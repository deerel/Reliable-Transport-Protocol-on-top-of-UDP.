/********************************************************************
 * FILENAME:
 *       s_connectionsetup.h
 * DESCRIPTION:
 *       Handles connection setup for the sending side
 * AUTHOR:
 *       Rickard Lundberg & Pontus Wedén
 *******************************************************************/
#pragma once

#include "rtptypes.h"

/* Connection setup machine states */
#define SYN_SENT 10
#define SYN_RECIEVED 11

/* Connection setup machine transitions */
#define MAX_RETRY_TIMEOUT 100
#define RECEIVE_SYN_ACK 101
#define RECEIVE_SYN_ACK_WITH_ERROR 102
#define S_TIMEOUT 103
#define S_TIMER_EXP 104

/* Setup connection  */
bool s_connectionSetup();

/* Send syn request to the server */
void ss_sendSyn();
/* Send message received acknowledgement */
void ss_sendAck();

/* State loop for syn sent state */
event ss_stateSynSent();
/* State loop for syn received state */
event ss_stateSynReceived();
