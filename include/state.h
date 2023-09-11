#pragma once

#include <Arduino.h>

#define COMMON_STATE_LENGTH 20
#define NUMBER_OF_ANALOG_INPUTS 6

#define NUMBER_OF_BUTTONS 12

struct buttonState {
    uint8_t pin;
    uint8_t value;
    uint8_t changeCount;
    uint32_t lastChangeTime;
};

buttonState* getButtons();

void setI2cMultiplexerState(char *state);
char *getI2cMultiplexerState();
void setAnalogInputValue(uint8_t index, uint16_t value);
uint16_t getAnalogInputValue(uint8_t index);