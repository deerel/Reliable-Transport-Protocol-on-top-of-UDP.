/********************************************************************
 * FILENAME:
 *       rtpsocket.h
 * DESCRIPTION:
 *       Handles UDP socket utilization for rtp packets
 * AUTHOR:
 *       Rickard Lundberg & Pontus Wedén
 *******************************************************************/
#pragma once

#include "rtptypes.h"

#define SOCKET_TIMEOUT 1

extern struct sockaddr_in addrIn, addrOther;
extern int sock;

/* Initializes the socket that will be used for inbound communication */
void initServerSocket(uint16_t port);
/* Initializes the socket used for outbound communication */
void initClientSocket(uint16_t port, char * hostName);

/* Receives packet sent to the program and prints a timestamp */
int receivePacket(rtp * packet);
/* Sends a packet sent to it and prints a timestamp for the operation*/
int sendPacket(rtp * packet);

/* Sets the timeout option for the server socket */
void setSocketTimeout(int sec);
/* Sets the socket timeout to a default value defined by SOCKET_TIMEOUT */
void setDefaultSocketTimeout();

/* Returns the server socket to those who ask for it */
int getSocket();

