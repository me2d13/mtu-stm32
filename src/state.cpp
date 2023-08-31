#include "state.h"
#include <string.h>

char i2cMultiplexerState[COMMON_STATE_LENGTH] = "?";
int analogInputValues[NUMBER_OF_ANALOG_INPUTS];

void setI2cMultiplexerState(char *state) {
    strcpy(i2cMultiplexerState, state);
}

char *getI2cMultiplexerState() {
    return i2cMultiplexerState;
}

void setAnalogInputValue(int index, int value) {
    analogInputValues[index] = value;
}

int getAnalogInputValue(int index) {
    return analogInputValues[index];
}