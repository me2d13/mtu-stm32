#pragma once

#define COMMON_STATE_LENGTH 20
#define NUMBER_OF_ANALOG_INPUTS 6

void setI2cMultiplexerState(char *state);
char *getI2cMultiplexerState();
void setAnalogInputValue(int index, int value);
int getAnalogInputValue(int index);