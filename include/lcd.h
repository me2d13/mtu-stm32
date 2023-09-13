#pragma once

#include <LiquidCrystal_I2C.h>

void initLcd();
void printAbout();
void printAxisMonitor(int axisIndex);
void printI2C(int index);
LiquidCrystal_I2C& getLcd();
void printButtonsMonitor();
void printBuffer();
void printMotors(int index);
void printLastUsbMessage();
void scrollUp();
void scrollDown();

#define LCD_COLS 20
#define LCD_ROWS 4