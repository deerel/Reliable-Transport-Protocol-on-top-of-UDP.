#include "crc.h"

/* Returns 1 or 0 depending on even or odd 1's (bits) in x */
int intToParityAsInt(unsigned int x)
{
    unsigned int count = 0, i, b = 1;

    /* Iterate through every bit of the int */
    for(i = 0; i < 32; i++)
    {
        /* Check if bit i in x is a 1 */
        if( x & (b << i) ){count++;}
    }

    /* If number of ones (1) _not_ is even return 0 */
    if( (count % 2) ){return 0;}

    return 1;
}

/* Returns CRC as int */
unsigned int getCRC(rtp *packet)
{

    int crc[32] = {0}; /* Array for parity bits */
    unsigned int crc_int = 0; /* Return value container */
    unsigned int flags_int, crc_i; /* Temporary storage */
    unsigned int data_int;

    /* Flag bit */
    flags_int = packet->flags[0] + packet->flags[1] + packet->flags[2] + packet->flags[3];
    crc[0] = intToParityAsInt(flags_int);

    /* Id bit */
    crc[1] = intToParityAsInt(packet->ackseq);

    /* Seq bit */
    crc[2] = intToParityAsInt(packet->seq);

    /* Windowsize bit */
    crc[3] = intToParityAsInt(packet->windowsize);

    /* Crc field - is always 0 when calculating parity bits */
    crc[4] = 0;

    /* Looping through data field */
    crc_i = 5;
    for(int i = 0; i < sizeof(packet->data);)
    {
        /* Take set of four chars and make int to utilize intToParityAsInt with*/
        data_int = ((unsigned int)packet->data[i]   << 24) |
                   ((unsigned int)packet->data[i+1] << 16) |
                   ((unsigned int)packet->data[i+2] << 8)  |
                   (unsigned int)packet->data[i+3];

        /* Check the parity bit and store it */
        crc[crc_i++] = intToParityAsInt(data_int);

        /* Jump 4 steps forward */
        i += 4;
    }

    /* Creating int from set of ones and zeros */
    for(int i = 0; i < 32; i++)
    {
        crc_int = crc_int << 1; /* Right shift one step*/

        if(crc[i] == 1)
        {
            crc_int |= 1;       /* Add 1 to left most bit */
        }
    }

    return crc_int;
}

/* Check if CRC in packet and calculated CRC is equal */
bool crcCompare(rtp * packet)
{
    if(packet->crc == getCRC(packet))
    {
        return TRUE;
    }

    return FALSE;
}
