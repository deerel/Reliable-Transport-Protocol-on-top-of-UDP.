/********************************************************************
 * FILENAME:
 *       rtpdatatranfer.h
 * DESCRIPTION:
 *       Handles the data transfer state
 * AUTHOR:
 *       Rickard Lundberg & Pontus Wedén
 *******************************************************************/
#pragma once

#include "rtptypes.h"

#define STDIN 0
#define MESSAGE_LENGTH 256
#define BUFFER_SIZE 512

/* Handles data transfer on established connection */
event rtpdataTransfer();