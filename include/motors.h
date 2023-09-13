#pragma once

#include <Arduino.h>


#define SPEED_BRAKE_DIR PA6
#define SPEED_BRAKE_STEP PA7

void setupMotors();
void loopMotors();
void motorTestUp(uint8_t motorIndex);
void motorTestDown(uint8_t motorIndex);
void motorMoveRelation(uint8_t motorIndex, int diff);
