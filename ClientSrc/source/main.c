#include <stdio.h>
#include <memory.h>
#include "../headers/rtptypes.h"
#include "../headers/s_connectionsetup.h"
#include "../headers/s_connectionteardown.h"
#include "../headers/rtpdatatransfer.h"
#include "../headers/s_init.h"
#include "../headers/r_connectionteardown.h"
#include "../headers/rtpwindow.h"

#define HOST_NAME_LENGTH 256

int main(int argc, char *argv[])
{
    char hostName[HOST_NAME_LENGTH];

    /* Since the IDE doesn't let us set argv when running, we default it to localhost
     * if no other value is given */
    if(argv[1] == NULL) {
        printf("Please start with a hostname, setting to default value 'localhost'\n");
        argv[1] = "127.0.0.1";
    }
    strncpy(hostName, argv[1], HOST_NAME_LENGTH);
    hostName[HOST_NAME_LENGTH - 1] = '\0';


    state activeState = CLOSED;
    event transEvent;

    for(;;)
    {
        switch(activeState)
        {
            case CLOSED:
                printf("Press ENTER to connect to server.");
                getchar();
                transEvent = init(hostName);
                switch(transEvent)
                {
                    case OPEN:
                        if(s_connectionSetup())
                        {
                            activeState = ESTABLISHED;
                        }
                        else
                        {
                            printf("Couldn't connect to server\n");
                        }
                        break;
                    default:
                        break;
                }
                break;
            case ESTABLISHED:
                printf("Connection ESTABLISHED\n");

                transEvent = rtpdataTransfer();
                switch(transEvent)
                {
                    case S_TEARDOWN:
                        s_connectionTeardown();
                        dispose();

                        printf("Disconnected\n");
                        activeState = CLOSED;
                        break;
                    case TEARDOWN:
                        r_connectionTeardown(getWinInPacketSlot());
                        dispose();

                        printf("Disconnected\n");
                        activeState = CLOSED;
                        break;
                    default:
                        break;
                }
                break;
            default:
                return 0;
                break;
        }
    }
}
