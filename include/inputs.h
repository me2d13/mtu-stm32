#pragma once

#include "lcd.h"

#define NUMBER_OF_BUTTONS 12

struct buttonState {
    uint8_t pin;
    uint8_t value;
    uint8_t changeCount;
    uint32_t lastChangeTime;
};


void setupInputs();
void refreshInputs();
void scanI2C(int index, char screenBuffer[][LCD_COLS+1]);
buttonState* getButtons();