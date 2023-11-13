#pragma once

#include <Arduino.h>

#define ALL_AXIS 100
#define AXIS_MAX 4096

void setupJoystick();
void loopJoystick();
void setJoystickAxis(uint8_t axis, uint16_t value);
void setJoystickButton(uint8_t button, uint8_t value);
void setCalibrateAxis(uint8_t axis, bool calibrate);