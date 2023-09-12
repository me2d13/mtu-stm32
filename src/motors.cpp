#include <Arduino.h>
#include "motors.h"
#include <AccelStepper.h>

AccelStepper stepper = AccelStepper(1, SPEED_BRAKE_STEP, SPEED_BRAKE_DIR);

int pos = 0;

void setupMotors() {
    pinMode(SPEED_BRAKE_DIR, OUTPUT); 
    pinMode(SPEED_BRAKE_STEP,OUTPUT);
    stepper.setPinsInverted(false, false, true);
    stepper.setMaxSpeed(1000);
    stepper.setAcceleration(500);
    stepper.setCurrentPosition(pos);
}

void motorTestUp(uint8_t motorIndex) {
    pos += 10;
    stepper.moveTo(pos);
    //stepper.runToPosition();
}

void motorTestDown(uint8_t motorIndex) {
    pos -= 10;
    stepper.moveTo(pos);
}

void loopMotors() {
    stepper.run();
}