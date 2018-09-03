#include "../headers/s_connectionsetup.h"
#include "../headers/rtpglobal.h"

#define MAX_RETRIES 5 /* The max retry time will be MAX_RETRIES * socket timeout time */

/* Setup connection  */
bool s_connectionSetup()
{
    state activeState;
    event transEvent;
    int synSentTimeout = 0, synReceivedTimeout = 0;

    ss_sendSyn(); /* Send syn to server */
    activeState = SYN_SENT;

    for(;;)
    {
        switch(activeState)
        {
            case SYN_SENT:

                /* If this condition is true it means that either the connection
                 * has timed out, or that we've received too many errors. */
                if(synSentTimeout == MAX_RETRIES)
                {
                    printf("Max retry timeout\n");
                    return FALSE;
                }

                transEvent = ss_stateSynSent();

                switch(transEvent)
                {
                    case RECEIVE_SYN_ACK:
                        ss_sendAck();
                        activeState = SYN_RECIEVED;
                        break;
                    case RECEIVE_SYN_ACK_WITH_ERROR:
                    case S_TIMEOUT:
                        synSentTimeout++;
                        ss_sendSyn();
                        break;
                    default:
                        break;
                }
                break;
            case SYN_RECIEVED:
                transEvent = ss_stateSynReceived();

                /* synReceivedTimeout get updated if transEvent == S_TIMEOUT. If we
                * match on MAX_RETRIES, we exit the loop and consider the connection
                * as established */
                if(synReceivedTimeout == MAX_RETRIES)
                {
                    return TRUE;
                }

                switch(transEvent)
                {

                    case S_TIMEOUT:
                        synReceivedTimeout++;
                        ss_sendAck();
                        break;
                    default:
                        break;
                }
                break;
        }
    }
}

/* Send syn request to the server */
void ss_sendSyn()
{
    setNegPacketToSyn();
    sendPacket(getNegPacket());
}

/* Send message received acknowledgement */
void ss_sendAck()
{
    setNegPacketToAck();
    sendPacket(getNegPacket());
}

/* State loop for syn sent state */
event ss_stateSynSent()
{
    resetNegPacket();

    if(receivePacket(getNegPacket()) == -1)
    {
        return S_TIMEOUT;
    }

    if(!validateNegPacket())
    {
        return RECEIVE_SYN_ACK_WITH_ERROR;
    }

    if(isCorrectNegSynAck())
    {

        return RECEIVE_SYN_ACK;
    }

    return NOT_DEFINED;
}

/* State loop for syn received state */
event ss_stateSynReceived()
{
    resetNegPacket();

    if(receivePacket(getNegPacket()) == -1)
    {
        return S_TIMEOUT;
    }

    if(isCorrectNegSynAck())
    {
        ss_sendAck();
    }

    return S_TIMER_EXP;
}
