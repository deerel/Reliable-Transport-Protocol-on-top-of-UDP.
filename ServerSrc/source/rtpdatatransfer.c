#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <errno.h>

#include "../headers/rtpdatatransfer.h"
#include "../headers/rtpglobal.h"
#include "../headers/rtpwindow.h"


event rtpdataTransfer()
{
    int i;

    fd_set activeFdSet, readFdSet; /* File descriptor sets for select function */

    char messageString[MESSAGE_LENGTH]; /* Container for stdin */

    /* Variables for fragmenting too long messages */
    char stdinBuffer[BUFFER_SIZE];
    stdinBuffer[0] = '\0';
    int stdinBufferEnd = 0;
    int stdinMsgLength = 0;

    /* Timing variables for select and packet timeout */
    struct timeval tv, beforeTv, afterTv;
    long int elapsedTime, secDiff, usecDiff;

    rtp * pkt; /* Used for pointer to active window packet */

    initWinPacket(); /* Initializing sliding window */

    /* Setting up file descriptor set for select to trigger on */
    FD_ZERO(&activeFdSet);
    FD_SET(getSocket(), &activeFdSet);
    FD_SET(STDIN, &activeFdSet);

    /* Infinite loop that handles sending and receiving of messages */
    for(;;)
    {
        tv = getOldestPacketTimeout(); /* For select timeout */
        gettimeofday(&beforeTv, NULL); /* Timestamp to calculate elapsed time */

        readFdSet = activeFdSet;
        int selectReturn = select(FD_SETSIZE, &readFdSet, NULL, NULL, &tv);

        gettimeofday(&afterTv, NULL); /* Timestamp to calculate elapsed time */
        secDiff = (afterTv.tv_sec - beforeTv.tv_sec) * 1000000; /* Times 1 million to convert to usecs*/
        usecDiff = (afterTv.tv_usec - beforeTv.tv_usec);
        elapsedTime = secDiff + usecDiff;

        updatePacketsTimeoutTime(elapsedTime);

        if(selectReturn < 0) {
            printf("SELECT ERROR: %d, %s\n", selectReturn, strerror(errno));
        }

        /* Select timed out */
        if(selectReturn == 0)
        {
            resendNonAckedPackets();
        }

        /* Loop over the whole set of file descriptors */
        for(i = 0; i < FD_SETSIZE; ++i)
        {
            /* Test to see if file descriptor i is part of set returned from select() */
            if(FD_ISSET(i, &readFdSet))
            {

                /* Check user input */
                if(i == STDIN)
                {
                    fgets(messageString, MESSAGE_LENGTH, stdin);
                    messageString[MESSAGE_LENGTH - 1] = '\0';

                    if(strncmp(messageString,"FIN\n",MESSAGE_LENGTH) != 0)
                    {

                        stdinMsgLength = (int)strlen(messageString) + 1; /* +1 for \0 */

                        /* Determine if message must be fragmented or not */
                        if(stdinMsgLength <= PACKET_DATA_SIZE) /* Send all at once */
                        {
                            if(sendPacket(createWinPacket(messageString)) == -1) /* -1 means that internal buffer is full */
                            {
                                resetPacketsTimeoutTime();
                                printf("Buffer full, cannot send message.\n");

                            }
                            stdinBufferEnd = 0;
                        }
                        else /* Copy to stdinBuffer */
                        {
                            stdinBufferEnd = stdinMsgLength;
                            strncpy(stdinBuffer, messageString, (size_t)stdinMsgLength);
                        }
                    }
                    else
                    {
                        disposeWinPacket(); /* Free allocated memory before teardown */
                        return S_TEARDOWN;
                    }
                }

                /* Check Socket*/
                if(i == getSocket()) {
                    if((pkt = getWinInPacketSlot()) != NULL)
                    {
                        if(receivePacket(pkt) != -1 && validateWinPacket(pkt))  /* receive packet and validate CRC */
                        {
                            switch(getWinPacketType()) /* Get type of packet to determine what to to with it */
                            {
                                case PTYPE_MSG:
                                    winMsgReceived();
                                    break;
                                case PTYPE_ACK:
                                    winAckReceived();
                                    break;
                                case PTYPE_FIN:
                                    return TEARDOWN;
                                default:
                                    break;
                            }
                        }
                    }
                }

                /* Send packets from stdinBuffer if the buffer contains data */
                while(stdinBufferEnd > 0)
                {
                    char * tmp = malloc(PACKET_DATA_SIZE * sizeof(char)); /* temp buffer */

                    strncpy(tmp, stdinBuffer, PACKET_DATA_SIZE-1); /* Copy first part to temp buffer */

                    tmp[PACKET_DATA_SIZE-1] = '\0'; /* Null terminate temp buffer for correct string behaviour */

                    /* Check if stdinBuffer not is empty and packet is sent */
                    if(stdinBufferEnd >= 0 && (sendPacket(createWinPacket(tmp))) != -1)
                    {
                        /* Move unsent part of buffer to the beginning of the buffer */
                        strcpy(stdinBuffer, stdinBuffer + (PACKET_DATA_SIZE-1));
                    }
                    else
                    {
                        free(tmp);
                        break;
                    }

                    stdinBufferEnd -= (PACKET_DATA_SIZE-1); /* Move stdinBufferEnd to new end */

                    free(tmp);
                }
            }
        }
    }
}
