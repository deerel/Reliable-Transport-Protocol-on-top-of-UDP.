/********************************************************************
 * FILENAME:
 *       s_connectionteardown.h
 * DESCRIPTION:
 *       Handles connection teardown for the sending side
 * AUTHOR:
 *       Rickard Lundberg & Pontus Wedén
 *******************************************************************/
#pragma once

#include "rtptypes.h"

/* Connection teardown states */
#define T_FIN_WAIT_1 20
#define T_FIN_WAIT_2 21
#define T_CLOSING 22
#define T_TIME_WAIT 23

/* Connection teardown transitions */
#define T_TIMEOUT 200
#define T_RECEIVE_FIN_WITH_ERROR 201
#define T_RECEIVE_ACK_WITH_ERROR 202
#define T_RECEIVE_FIN_ACK_WITH_ERROR 203
#define T_RECEIVE_ACK 204
#define T_RECEIVE_FIN 205
#define T_TIMER_EXP 206

/* Handles connection teardown  */
bool s_connectionTeardown();

/* Handles the first FIN wait state */
event st_stateFinWait1();
/* Handles the second fin wait state */
event st_stateFinWait2();
/* Handles the Closing state */
event st_stateClosing();
/* Handles the Time Wait state */
event st_stateTimeWait();

/* Sending ack to receiver */
void st_sendAck();
/* Sending fin to receiver */
void st_sendFin();

