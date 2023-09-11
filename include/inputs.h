#pragma once

#include "lcd.h"

void setupInputs();
void refreshInputs();
void scanI2C(int index, char screenBuffer[][LCD_COLS+1]);
