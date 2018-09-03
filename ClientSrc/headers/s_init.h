/********************************************************************
 * FILENAME:
 *       r_init.h
 * DESCRIPTION:
 *       Handles application initialization for the receiving side.
 * AUTHOR:
 *       Rickard Lundberg & Pontus Wedén
 *******************************************************************/
#pragma once

#include "rtptypes.h"
#include "rtpglobal.h"

/* Initializes connection to receiver */
event init(char * hostName);

void dispose();
