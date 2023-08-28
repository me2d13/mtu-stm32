#pragma once

#include <LiquidCrystal_I2C.h>

void initLcd();
void printAbout();
LiquidCrystal_I2C& getLcd();