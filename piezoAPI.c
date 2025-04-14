/**
 * This module allows the construction and decoding of waveform control messages,
 * which are serialized into a 4-byte (uint8_t[4]) format for communication
 * over SPI to the Piezo Pusher.
 *
 * Each message includes:
 *  - A waveform type (e.g., DC voltage, sine wave)
 *  - Amplitude (13-bit max). Must be within 0–5000
 *  - Frequency (15-bit max). Must be within 0–20,000
 *
 * The message is structured into two 16-bit fields:
 *   - The first word (START word) includes the waveform type and amplitude
 *   - The second word (STOP word) includes the frequency
 *
 * Both 16-bit words have their MSB (bit 15) set as framing bits (START/STOP).
 * These two 16-bit words are then split into four 8-bit bytes for transmission.
 *
 * Final Byte Layout (uint8_t[4]):
 *   Byte 0: START_WORD high byte
 *   Byte 1: START_WORD low byte
 *   Byte 2: STOP_WORD  high byte
 *   Byte 3: STOP_WORD  low byte
 *
 * Message Bit Format:
 *  -------------------------------------------------
 *  | Word 0 | 15      | 14-13     | 12 - 0         |
 *  |        | START=1 | WAVEFORM  | AMPLITUDE      |
 *  -------------------------------------------------
 *  | Word 1 | 15      | 14 - 0                      |
 *  |        | STOP=1  | FREQUENCY                   |
 *  -------------------------------------------------
 *
 * Example Input Strings for Parsing:
 *  - "VOLT:5000"            → DC voltage, amplitude only
 *  - "SIN:3000:100"         → Sine wave, amplitude + frequency
 *  - "RCT:4000:200"         → Rectangle wave, amplitude + frequency
 *  - "OFF"                  → Turns output off (amplitude = 0, frequency = 0)
 */


#include <string.h>         // for strtok, strcmp
#include <stdint.h>
#include "piezoAPI.h"

/**
 * @brief Serialize a waveform request into a 4-byte (uint8_t) message.
 *
 * This function encodes waveform parameters into a 4-byte message array.
 * The first 2 bytes represent the START word (waveform + amplitude),
 * and the next 2 bytes represent the STOP word (frequency).
 *
 * Message format:
 *   Byte 0: START_WORD >> 8
 *   Byte 1: START_WORD & 0xFF
 *   Byte 2: STOP_WORD >> 8
 *   Byte 3: STOP_WORD & 0xFF
 *
 * @param[out] message      Pointer to a 4-byte buffer (uint8_t[4])
 * @param[in]  messageLength Must be 4
 * @param[in]  waveform     Waveform type (VOLT, SINE, RECTANGLE)
 * @param[in]  amplitude    Amplitude (13-bit max)
 * @param[in]  frequency    Frequency (15-bit max)
 * @return 0 on success, -1 on failure
 */
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


/**
 * @brief Parse a colon-delimited string into a waveform request struct.
 *
 * Accepts human-readable strings and decodes them into `request_t` structs for later use
 * in waveform generation or message transmission.
 * 
 * @param[in]  msg            Input string (e.g., "SIN:3000:100")
 * @param[out] messageRequest Pointer to output request structure with waveform, amplitude, and frequency filled in to be passed to serializeMessage()
 * 
 * @return 0 on success, -1 on failure (e.g., malformed string)
 */
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


//ATOI algorithm created by ChatGPT. Stdlib atoi() was not working correctly.
//OpenAI, "Custom C implementation of the atoi algorithm," OpenAI ChatGPT, 2024. [Online]. Available: https://www.openai.com/chatgpt.
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

