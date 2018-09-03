#include <stdio.h>

#include "../headers/r_connectionsetup.h"
#include "../headers/rtpglobal.h"

/* Handles three way handshake */
bool r_connectionSetup()
{
    /* Variables to handle states and transitions */
    state activeState = S_LISTEN;
    event transEvent = NOT_DEFINED;

    /* Variables to keep track of connection retries after SYN is received */
    int max_retries = 5;
    int retries = 0;

    /* Infinite loop, number of iterations depends on events inside loop */
    for(;;)
    {
        switch(activeState)
        {
        case S_LISTEN:
            retries = 0;
            transEvent = stateListen();
            switch(transEvent)
            {
            case S_RECEIVE_SYN:
                sendSynAck();
                activeState = S_SYN_RECEIVED;
                break;
            case S_RECEIVE_SYN_WITH_ERROR:
                continue;
                break;
            default:
                break;
            }
            break;
        case S_SYN_RECEIVED:
            if(retries == max_retries+1)
            {
                printf("Max connection retries.\n");
                return FALSE;
            }
            transEvent = stateSynReceive();
            switch(transEvent)
            {
                case S_RECEIVE_ACK:
                case S_RECEIVE_PACKET:
                    return TRUE;
                    break;
                case S_RECEIVE_PKT_WITH_ERROR:
                case S_RECEIVE_UNEXPECTED:
                case S_TIMEOUT:
                    retries++;
                    sendSynAck();
                    continue;
                    break;
                default:
                    break;
            }
            break;
        default:
            printf("Unexpected state in connection setup.\n");
            break;
        }
    }


    return FALSE;
}

/* Handles state Listen */
event stateListen()
{

    /* Need to clean packet for correct crc check */
    resetNegPacket();

    /* Change timeout for the state listen */
    setSocketTimeout(300);

    if(receivePacket(getNegPacket()) == -1)
    {
        return NOT_DEFINED;
    }

    if(!validateNegPacket())
    {
        return S_RECEIVE_SYN_WITH_ERROR;
    }

    /* Revert to default timeout */
    setDefaultSocketTimeout();

    if(isCorrectNegSyn())
    {
        return S_RECEIVE_SYN;
    }

    return NOT_DEFINED;


 }


/* Handles state Syn Received */
event stateSynReceive()
{

    /* Need to clean packet for correct crc check */
    resetNegPacket();

    if(receivePacket(getNegPacket()) == -1)
    {
        return NOT_DEFINED;
    }

    if(!validateNegPacket())
    {
        return S_RECEIVE_PKT_WITH_ERROR;
    }

    if(isCorrectNegAck())
    {
        return S_RECEIVE_ACK;
    }

    if(isCorrectWinPkt())
    {
        return S_RECEIVE_PACKET;
    }

    return S_RECEIVE_UNEXPECTED;

}

/* Send packet with SYN ACK */
void sendSynAck()
{
    setNegPacketToSynAck();

    sendPacket(getNegPacket());
}

