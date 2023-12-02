#pragma once

#include <Arduino.h>
#include <AccelStepper.h>


#define SPEED_BRAKE_DIR PA6
#define SPEED_BRAKE_STEP PA7

#define THR1_DIR PC14
#define THR1_STEP PC15

#define THR2_DIR PB0
#define THR2_STEP PB1

#define TRIM_DIR PA4
#define TRIM_STEP PA5

#define IND_DIR PB8
#define IND_STEP PB9

#define NUMBER_OF_MOTORS 6

struct motorState {
    uint8_t dirPin;
    uint8_t stepPin;
    uint8_t enablePin;
    uint16_t targetValue;
    AccelStepper stepper;
    bool invertedDirection;
    bool enabled;
    uint16_t lastRecordedValue;
    uint32_t lastRecordedTime;
};


void setupMotors();
void loopMotors();
void enableMotor(uint8_t index);
void disableMotor(uint8_t index);
motorState *getMotor(uint8_t index);
void moveMotorBySensor(uint8_t motorIndex, float position);