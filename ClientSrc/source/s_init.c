#include "../headers/s_init.h"
#include "../headers/rtpglobal.h"

event init(char * hostName)
{
    windowSize = WINDOW_SIZE;

    initNegPacket();
    initServerSocket(INPORT);
    initClientSocket(PORT, hostName);

    return OPEN;
}

/* Free allocated memory */
void dispose()
{
    disposeNegPacket();
}