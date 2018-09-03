/********************************************************************
 * FILENAME:
 *       rtputilities.h
 * DESCRIPTION:
 *       Generic helper function
 * AUTHOR:
 *       Rickard Lundberg & Pontus Wedén
 *******************************************************************/
#pragma once

#include "rtptypes.h"

void resetPacket(rtp * pkt);

void getTimestamp(char * timestamp);

char * getPacketType(rtp * pkt);

int causeErrors();
