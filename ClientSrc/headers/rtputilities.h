/********************************************************************
 * FILENAME:
 *       rtputilities.h
 * DESCRIPTION:
 *       Generic helper functions
 * AUTHOR:
 *       Rickard Lundberg & Pontus Wedén
 *******************************************************************/
#pragma once

#include "rtptypes.h"

/* Returns the current time formatted as hh:mm:ss*/
void getTimestamp(char * timestamp);

/* Returns the type of the package sent to it */
char * getPacketType(rtp * pkt);

/* Determines the error frequency for the error generator */
int causeErrors();