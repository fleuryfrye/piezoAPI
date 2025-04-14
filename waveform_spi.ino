#include <SPI.h>
#include "piezoAPI.h"

/*

    Slave select: 10
    CLK: Pin 13
    MOSI: Pin 11


 * Example Input Strings for Parsing:
 *  - "VOLT:5000"            → DC voltage, amplitude only
 *  - "SIN:3000:100"         → Sine wave, amplitude + frequency
 *  - "RCT:4000:200"         → Rectangle wave, amplitude + frequency
 *  - "OFF"                  → Turns output off (amplitude = 0, frequency = 0)
 
*/




// Pin for the slave select (SS) line
const int slaveSelectPin = 10;

// Message to send
const char* message = "RCT:1000:1000";


char msg[50];


#include <string.h>

#define START_BIT (1 << 15) //bit 15
#define STOP_BIT (1 << 15) // bit 15
#define WAVEFORM_BITS (13) //bits [14:13]

#define MESSAGE_LEN 4

typedef enum {VOLT, SINE, RECTANGLE, PULSE} wavetype_t;

typedef struct request
{
  wavetype_t waveform;
  uint16_t amplitude;
  uint16_t frequency;
} request_t;


uint8_t serializeMessage(uint8_t* message, uint8_t messageLength, wavetype_t waveform, uint16_t amplitude, uint16_t frequency)
{
    if (messageLength != 4) return -1;

    uint16_t word0 = START_BIT | (waveform << WAVEFORM_BITS) | (amplitude & 0x1FFF); // 13-bit amplitude
    uint16_t word1 = STOP_BIT | (frequency & 0x7FFF);                                // 15-bit frequency

    message[0] = (word0 >> 8) & 0xFF;
    message[1] = word0 & 0xFF;
    message[2] = (word1 >> 8) & 0xFF;
    message[3] = word1 & 0xFF;

    return 0;
}


int my_atoi(const char *str) {
    int result = 0;      // To store the final integer value
    int sign = 1;        // To handle negative numbers
    int i = 0;           // Iterator index

    // Step 1: Skip leading whitespace
    while (str[i] == ' ' || str[i] == '\t' || str[i] == '\n' ||
           str[i] == '\v' || str[i] == '\f' || str[i] == '\r') {
        i++;
    }

    // Step 2: Check for optional '+' or '-' sign
    if (str[i] == '-') {
        sign = -1;
        i++;
    } else if (str[i] == '+') {
        i++;
    }

    // Step 3: Convert digits to integer
    while (str[i] >= '0' && str[i] <= '9') {
        result = result * 10 + (str[i] - '0');  // Accumulate the digit
        i++;
    }

    // Step 4: Return the result with the correct sign
    return result * sign;
}



int8_t processMessage(char* msg, request_t* messageRequest)
{
    int8_t ret = -1;
    char* tokenMsg = msg;
    char* token = strtok(tokenMsg, ":");

    if(token != NULL)
    {
        if(strcmp(token, "VOLT") == 0)
        {
            messageRequest->waveform = VOLT;
            token = strtok(NULL, ":");
            if(token != NULL)
            {
                messageRequest->amplitude = my_atoi(token);
                
                ret = 0;
            }
        }

        else if(strcmp(token, "SIN") == 0)
        {
            messageRequest->waveform = SINE;
            token = strtok(NULL, ":");
            if(token != NULL)
            {
                messageRequest->amplitude = my_atoi(token);
                token = strtok(NULL, ":");
                if(token != NULL)
                {
                    messageRequest->frequency = my_atoi(token);
                    ret = 0;
                }
            }
        }

        else if(strcmp(token, "RCT") == 0)
        {
            messageRequest->waveform = RECTANGLE;
            token = strtok(NULL, ":");
            if(token != NULL)
            {
                messageRequest->amplitude = my_atoi(token);
                token = strtok(NULL, ":");
                if(token != NULL)
                {
                    messageRequest->frequency = my_atoi(token);
                    ret = 0;
                }
            }
        }

        else if(strcmp(token, "OFF") == 0)
        {
            messageRequest->waveform = VOLT;
            messageRequest->amplitude = 0;
            messageRequest->frequency = 0;
            ret = 0;
        }
    }

    return ret;
}







void setup() {
  // Set up the serial monitor
  Serial.begin(9600);

  // Set the slave select pin as output
  pinMode(slaveSelectPin, OUTPUT);

  // Ensure the slave select pin is HIGH initially (deselect the slave)
  digitalWrite(slaveSelectPin, HIGH);

  // Initialize SPI as master
  SPI.begin();

  // Configure SPI settings
  SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0)); // 1 MHz, MSB first, SPI mode 0

  Serial.println("SPI Master Initialized");
}

void loop() {
  static int msgLength = 0;
  
  while (Serial.available()) {
    char c = Serial.read();

  
    if(c == '\n')
    {

    msg[msgLength] = '\0';

    uint8_t SPI_TX_Buffer[MESSAGE_LEN];

    request_t waveformRequest;

    //processMessage(msg, &waveformRequest);

      Serial.print("Received: ");
      Serial.println(msg);

      if (processMessage(msg, &waveformRequest) != 0) {
      Serial.println("Invalid message format.");
      }

      else
      {

      


        serializeMessage(SPI_TX_Buffer, MESSAGE_LEN, waveformRequest.waveform, waveformRequest.amplitude, waveformRequest.frequency);


        digitalWrite(slaveSelectPin, LOW);


        uint16_t firstByteSent = ((SPI_TX_Buffer[0] << 8) | SPI_TX_Buffer[1]);
        uint16_t secondByteSent = ((SPI_TX_Buffer[2] << 8) | SPI_TX_Buffer[3]);


        Serial.print("Sent start, waveform, and amplitude: ");
        Serial.println(firstByteSent, HEX);
        Serial.print("Sent stop and frequency: ");
        Serial.println(secondByteSent, HEX);


        Serial.println();


        for(int i = 0; i < MESSAGE_LEN; i++)
        {
          SPI.transfer(SPI_TX_Buffer[i]);
          //Serial.println(SPI_TX_Buffer[i], HEX);
        }

        digitalWrite(slaveSelectPin, HIGH);

      }


      msgLength = 0;

    }

    else
    {
      msg[msgLength++] = c;
      //Serial.println(msg);
    }

    }


}
