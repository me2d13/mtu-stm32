#pragma once

#include <LiquidCrystal_I2C.h>

void initLcd();
void printAbout();
void printAxisMonitor(int axisIndex);
LiquidCrystal_I2C& getLcd();