#include "piezoAPI.h"


int main(void)
{

    char* userInput = "SIN:5000:1000";
    request_t requestedWaveform;

    processMessage(userInput, &requestedWaveform);

    uint8_t transferMsg[MESSAGE_LEN];

    serializeMessage(transferMsg, MESSAGE_LEN, requestedWaveform.waveform, requestedWaveform.amplitude, requestedWaveform.frequency);

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