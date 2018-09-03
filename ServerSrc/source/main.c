#include "../headers/rtptypes.h"
#include "../headers/rtpglobal.h"
#include "../headers/r_connectionsetup.h"
#include "../headers/r_connectionteardown.h"
#include "../headers/s_connectionteardown.h"
#include "../headers/rtpdatatransfer.h"
#include "../headers/r_init.h"
#include "../headers/rtpwindow.h"

int main(void)
{

    /* Variables to handle states and transitions */
    state activeState = CLOSED;
    event transEvent = NOT_DEFINED;

    /* The program runs until closed, therefor it resides in an infinite loop */
    for(;;)
    {
        /* Switching on the only two states the program can be in, Closed or Established */
        switch(activeState)
        {
            case CLOSED:
                transEvent = init(); /* Wait for user input for server start */
                switch(transEvent)
                {
                case OPEN:
                    printf("The server is ready for a connection.\n");
                    if(r_connectionSetup()) /* Go to state machine receiver connection setup */
                    {
                        activeState = ESTABLISHED;
                    }
                    break;
                default:
                    printf("Undefined event.\n");
                    break;
                }
                break;
            case ESTABLISHED:
                printf("Connection ESTABLISHED\n");
                transEvent = rtpdataTransfer();
                switch(transEvent)
                {
                    case TEARDOWN:
                        r_connectionTeardown(getWinInPacketSlot()); /* Tear down connection, nicely or with timeout */
                        dispose(); /* Free allocated memory */
                        activeState = CLOSED;
                        printf("Disconnected\n");
                        continue;
                    case S_TEARDOWN:
                        s_connectionTeardown(); /* Tear down connection, nicely or with timeout */
                        dispose(); /* Free allocated memory */
                        activeState = CLOSED;
                        printf("Disconnected\n");
                        continue;
                    default:
                        printf("Undefined event.\n");
                        break;
                }
            default:
                printf("Unexpected state in program.\n");
                return 0;
        }
    }
}
