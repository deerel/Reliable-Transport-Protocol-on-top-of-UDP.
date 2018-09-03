/********************************************************************
 * FILENAME:
 *       r_connectionteardown.h
 * DESCRIPTION:
 *       Handles connection teardown for the receiving side
 * AUTHOR:
 *       Rickard Lundberg & Pontus Wedén
 *******************************************************************/
#pragma once

#include "rtptypes.h"

#define T_CLOSE_WAIT 40
#define T_LAST_ACK 41

#define RT_TIMEOUT 400
#define RT_RECEIVE_ACK 401
#define RT_RECEIVE_ACK_WITH_ERROR 402

/* Handles connection teardown */
bool r_connectionTeardown(rtp *pkt);

/* Handles state Last Ack */
event rt_stateLastAck();

/* Sending Ack */
void rt_sendAck();

/* Sending Fin */
void rt_sendFin();
