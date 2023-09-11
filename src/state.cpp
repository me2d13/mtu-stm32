#include "state.h"
#include <Arduino.h>
#include <string.h>
#include "joy.h"

char i2cMultiplexerState[COMMON_STATE_LENGTH] = "?";
uint16_t analogInputValues[NUMBER_OF_ANALOG_INPUTS];

buttonState buttons[] = {
    { 8, 0, 0, 0 }, // GPB0 AT disconnect 1
    { 9, 0, 0, 0 }, // GPB1 AT disconnect 2
    { 10, 0, 0, 0 }, // GPB2 TOGA 1
    { 11, 0, 0, 0 }, // GPB3 TOGA 2
    { 12, 0, 0, 0 }, // GPB4 stab trim normal
    { 13, 0, 0, 0 }, // GPB5 stab trim ap
    { 14, 0, 0, 0 }, // GPB6 start 1
    { 15, 0, 0, 0 }, // GPB7 start 2
    { 7, 0, 0, 0 }, // GPA7 AT cut off
    { 6, 0, 0, 0 }, // GPA6 trim indicator stop 1
    { 5, 0, 0, 0 }, // GPA5 trim indicator stop 2
    { 4, 0, 0, 0 } // GPA3 parking brake
};

buttonState* getButtons() {
    return buttons;
}

void setI2cMultiplexerState(char *state) {
    strcpy(i2cMultiplexerState, state);
}

char *getI2cMultiplexerState() {
    return i2cMultiplexerState;
}

void setAnalogInputValue(uint8_t index, uint16_t value) {
    if (analogInputValues[index] != value) {
        analogInputValues[index] = value;
        setJoystickAxis(index, value);
    }
}

uint16_t getAnalogInputValue(uint8_t index) {
    return analogInputValues[index];
}