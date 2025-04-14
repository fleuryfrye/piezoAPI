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



#ifndef __PIEZO_API_H
#define __PIEZO_API_H

#include <stdint.h>
#include <stdio.h>
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


uint8_t serializeMessage(uint8_t* message, uint8_t messageLength, wavetype_t waveform, uint16_t amplitude, uint16_t frequency);

int8_t processMessage(char* msg, request_t* messageRequest);

int my_atoi(const char *str);



#endif