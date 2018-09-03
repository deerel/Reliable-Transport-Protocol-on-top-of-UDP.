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

/* Setting up socket to listen on */
event init();

/* Free allocated memory */
void dispose();
