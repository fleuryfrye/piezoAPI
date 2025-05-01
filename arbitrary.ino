#include <SPI.h>

// Pin for the slave select (SS) line
const int slaveSelectPin = 10;


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

  //delay(5000);

}

void loop() {
  static int msgLength = 0;
  


    uint8_t SPI_TX_Buffer[MESSAGE_LEN];
    uint8_t SPI_TX_Buffer2[MESSAGE_LEN];
    uint8_t SPI_TX_Buffer3[MESSAGE_LEN];
    uint8_t SPI_TX_Buffer4[MESSAGE_LEN];
    uint8_t SPI_TX_Buffer5[MESSAGE_LEN];



    uint8_t* messageBuffers[5] = {SPI_TX_Buffer, SPI_TX_Buffer2, SPI_TX_Buffer3, SPI_TX_Buffer4, SPI_TX_Buffer5};


    serializeMessage(SPI_TX_Buffer, MESSAGE_LEN, VOLT, 1000, 0);
    serializeMessage(SPI_TX_Buffer2, MESSAGE_LEN, VOLT, 4000, 0);
    serializeMessage(SPI_TX_Buffer3, MESSAGE_LEN, VOLT, 1000, 0);
    serializeMessage(SPI_TX_Buffer4, MESSAGE_LEN, VOLT, 5000, 0);
    serializeMessage(SPI_TX_Buffer5, MESSAGE_LEN, VOLT, 2000, 0);





    for(int j = 0; j < 5; j++)
    {

      digitalWrite(slaveSelectPin, LOW);
    

    for(int i = 0; i < MESSAGE_LEN; i++)
    {
      SPI.transfer(messageBuffers[j][i]);
    }


      digitalWrite(slaveSelectPin, HIGH);

      delay(200);



    }


    //delay(1000);

}
