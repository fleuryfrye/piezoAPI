#include "piezoAPI.h"

int main(void)
{


    uint8_t transferMsg[MESSAGE_LEN];


    //Serialize a message to generate a rectangle wave with an amplitude of 3000 and a frequency of 100 Hz.
    serializeMessage(transferMsg, MESSAGE_LEN, RECTANGLE, 3000, 100);

    /*
    
    Transfer 4 byte message over SPI.

    ex. 

    for(int i = 0; i < MESSAGE_LEN; i++)
    {
        SPI.transfer(transferMsg[i]);
    }

    */

    return 0;
}