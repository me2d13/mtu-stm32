#pragma once

#include <Arduino.h>
#include <AccelStepper.h>


#define SPEED_BRAKE_DIR PA6
#define SPEED_BRAKE_STEP PA7

#define THR1_DIR PC14
#define THR1_STEP PC15

#define NUMBER_OF_MOTORS 2

struct motorState {
    uint8_t dirPin;
    uint8_t stepPin;
    uint8_t enablePin;
    uint8_t axisIndex;
    uint16_t targetValue;
    bool traveling;
    uint32_t travelStartTime;
    AccelStepper stepper;
    long position; // temporary
};


void setupMotors();
void loopMotors();
void motorTestUp(uint8_t motorIndex);
void motorTestDown(uint8_t motorIndex);
void motorMoveRelation(uint8_t motorIndex, int diff);
motorState *getMotor(uint8_t index);
