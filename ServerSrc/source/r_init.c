#include <time.h>
#include <stdlib.h>
#include "../headers/r_init.h"
#include "../headers/rtpglobal.h"

/* Setting up socket to listen on */
event init()
{

    srand((unsigned int)(time(NULL)));

    /* Waiting for user to confirm start */
    printf("Press ENTER to start the server.");
    getc(stdin);

    printf("Initializing server\n");
    /* Initializing a packet for negotiation (setup and teardown)  */
    initNegPacket();
    /* Initializing socket for server to listen on */
    initServerSocket(PORT);

    return OPEN;
}

/* Free allocated memory */
void dispose()
{
    disposeNegPacket();
}

