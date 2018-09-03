#include <string.h>
#include "../headers/r_connectionteardown.h"
#include "../headers/rtpglobal.h"

/* Handles connection teardown */
bool r_connectionTeardown(rtp *pkt)
{
    /* Variables to handle states and transitions */
    state activeState = T_CLOSE_WAIT;
    event transEvent = NOT_DEFINED;

    /* Copy FIN packet content to get current seq num */
    resetNegPacket();
    rtp * tmpPkt = getNegPacket();
    memcpy(tmpPkt, pkt, sizeof(rtp));

    /* Need to acknowledge that teardown request is received */
    rt_sendAck();

    /* Infinite loop, number of iterations depends on events inside loop */
    for(;;)
    {
        switch(activeState)
        {
        case T_CLOSE_WAIT:
            rt_sendFin();
            activeState = T_LAST_ACK;
            break;
        case T_LAST_ACK:
            transEvent = rt_stateLastAck();
            switch(transEvent)
            {
            case RT_RECEIVE_ACK:
            case RT_TIMEOUT:
                return TRUE;
            case RT_RECEIVE_ACK_WITH_ERROR:
                rt_sendFin();
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
    }

    return FALSE;
}

/* Handles state Last Ack */
event rt_stateLastAck()
{
    resetNegPacket();

    if((receivePacket(getNegPacket()))== -1)
    {
        return RT_TIMEOUT;
    }

    if(!validateNegPacket())
    {
        printf("CRC check error!\n");
        return RT_RECEIVE_ACK_WITH_ERROR;
    }

    if(isCorrectNegAck())
    {
        return RT_RECEIVE_ACK;
    }


    return NOT_DEFINED;
}

/* Sending Ack */
void rt_sendAck()
{
    setNegPacketToAck();

    sendPacket(getNegPacket());
}

/* Sending Fin */
void rt_sendFin()
{

    setNegPacketToFin();

    sendPacket(getNegPacket());

}
