/********************************************************************
 * FILENAME:
 *       crc.h
 * DESCRIPTION:
 *       Handles checksum calculation and comparison
 * AUTHOR:
 *       Rickard Lundberg & Pontus Wedén
 *******************************************************************/
#pragma once

#include "../headers/rtptypes.h"

/* Returns 1 or 0 depending on even or odd 1's (bits) in x */
int intToParityAsInt(unsigned int x);

/* Returns CRC as int */
unsigned int getCRC(rtp *packet);

/* Check if CRC in packet and calculated CRC is equal */
bool crcCompare(rtp * packet);
