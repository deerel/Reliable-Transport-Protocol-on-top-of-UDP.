#include "../headers/s_connectionteardown.h"
#include "../headers/rtpglobal.h"


#define MAX_RETRIES 5 /* Represents the max timeout value, which is MAX_RETRIES * socket timeout setting */

/* Handles connection teardown */
bool s_connectionTeardown()
{
    int stTimeWaitTimer = 0;
    state activeState;
    event transEvent;

    st_sendFin();
    activeState = T_FIN_WAIT_1;

    for(;;)
    {
        switch(activeState)
        {
            case T_FIN_WAIT_1:

                transEvent = st_stateFinWait1();
                switch(transEvent)
                {
                    case T_RECEIVE_ACK:

                        activeState = T_FIN_WAIT_2;
                        break;
                    case T_RECEIVE_FIN:

                        st_sendAck();
                        activeState = T_CLOSING;
                        break;
                    case T_TIMER_EXP:
                        return TRUE;
                    case T_RECEIVE_FIN_ACK_WITH_ERROR:
                        st_sendFin();
                        break;
                    default:
                        break;
                }
                break;
            case T_FIN_WAIT_2:

                transEvent = st_stateFinWait2();

                switch(transEvent)
                {
                    case T_RECEIVE_FIN:
                        st_sendAck();
                        activeState = T_TIME_WAIT;
                        break;
                    case T_RECEIVE_FIN_WITH_ERROR:
                        continue;
                    case T_TIMEOUT:
                        return TRUE;
                    default:
                        break;
                }
                break;
            case T_CLOSING:

                transEvent = st_stateClosing();

                switch(transEvent)
                {
                    case T_RECEIVE_ACK:
                        activeState = T_TIME_WAIT;
                        break;
                    case T_TIMER_EXP:
                        return TRUE;
                    case T_RECEIVE_ACK_WITH_ERROR:
                        st_sendAck();
                        break;
                    default:
                        break;
                }
                break;
            case T_TIME_WAIT:

                transEvent = st_stateTimeWait();

                if(stTimeWaitTimer == MAX_RETRIES)
                {
                    return TRUE;
                }

                switch(transEvent)
                {
                    case T_TIMEOUT:
                        stTimeWaitTimer++;
                        continue;
                    case T_TIMER_EXP:
                        return TRUE;
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }
}

/* Handles the first FIN wait state */
event st_stateFinWait1()
{
    int i;

    for (i = 0; i < MAX_RETRIES; i++)
    {
        resetNegPacket();

        if(receivePacket(getNegPacket()) == -1)
        {
            st_sendFin();
            continue;
        }

        if(!validateNegPacket())
        {
            return T_RECEIVE_FIN_ACK_WITH_ERROR;
        }

        if(isCorrectNegAck())
        {
            return T_RECEIVE_ACK;
        }

        if(isCorrectNegFin())
        {
            return T_RECEIVE_FIN;
        }
    }

    return T_TIMER_EXP;
}

/* Handles the second fin wait state */
event st_stateFinWait2()
{
    resetNegPacket();

    if(receivePacket(getNegPacket()) == -1)
    {
        return T_TIMEOUT;
    }

    if(!validateNegPacket())
    {
        return T_RECEIVE_FIN_WITH_ERROR;
    }

    if(isCorrectNegFin())
    {
        return T_RECEIVE_FIN;
    }

    return NOT_DEFINED;
}

/* Handles the Closing state */
event st_stateClosing()
{
    int i;
    resetNegPacket();

    /* Every loop, we will stop at receivePacket and wait for timeout, so
     * the max retry time will be the loop length * the timeout time. If
     * we get no timeout, and receive and ack instead we can change state*/
    for (i = 0; i < MAX_RETRIES; ++i)
    {
        if(receivePacket(getNegPacket()) == -1)
        {
            st_sendAck();
        }

        if(!validateNegPacket())
        {
            return T_RECEIVE_ACK_WITH_ERROR;
        }

        if(isCorrectNegAck())
        {
            return T_RECEIVE_ACK;
        }
    }

    return T_TIMER_EXP;
}

/* Handles the Time Wait state */
event st_stateTimeWait()
{
    int i;

    resetNegPacket();

    /* Return if we receive nothing, which will increment a timer
     * until we've finally gotten "nothing" enough times to assume
     * that we can close the application safely */
    if((receivePacket(getNegPacket()) == -1))
    {
        return T_TIMEOUT;
    }

    /* If we get another FIN we re-ack */
    if(isCorrectNegFin() && validateNegPacket())
    {
        st_sendAck();
    }

    return T_TIMER_EXP;
}

/* Sending ack to receiver */
void st_sendAck()
{
    setNegPacketToAck();

    sendPacket(getNegPacket());
}

/* Sending fin to receiver */
void st_sendFin()
{
    setNegPacketToFin();

    sendPacket(getNegPacket());
}
