#include <Arduino.h>
#include "motors.h"
#include <AccelStepper.h>
#include "inputs.h"

motorState motorStates[NUMBER_OF_MOTORS] = {
    {SPEED_BRAKE_DIR, SPEED_BRAKE_STEP, 3, 0, 0, false, 0, NULL, 0}, // GPA3 speed brake enable
    {THR1_DIR, THR1_STEP, 0, 0, 0, false, 0, NULL, 0} // GPA0 throttle enable
};

// function to return motor state
motorState *getMotor(uint8_t index) {
    return &motorStates[index];
}

void setupMotors() {
    //pinMode(SPEED_BRAKE_DIR, OUTPUT); 
    //pinMode(SPEED_BRAKE_STEP,OUTPUT);
    // itterate motorStates
    for (size_t i = 0; i < NUMBER_OF_MOTORS; i++)
    {
        motorState *motor = &motorStates[i];
        motor->stepper = AccelStepper(1, motor->stepPin, motor->dirPin);
        motor->stepper.setPinsInverted(false, false, true);
        motor->stepper.setMaxSpeed(500);
        motor->stepper.setAcceleration(100);
        motor->stepper.setCurrentPosition(0);
        setMuxedOutputPin(motor->enablePin, LOW);
    }
}

void motorTestUp(uint8_t motorIndex) {
    motorState *motor = &motorStates[motorIndex];
    motor->position += 10;
    motor->stepper.moveTo(motor->position);
    //stepper.runToPosition();
}

void motorTestDown(uint8_t motorIndex) {
    motorState *motor = &motorStates[motorIndex];
    motor->position -= 10;
    motor->stepper.moveTo(motor->position);
}

void loopMotors() {
    for (size_t i = 0; i < NUMBER_OF_MOTORS; i++)
    {
        motorState *motor = &motorStates[i];
        motor->stepper.run();
    }
}

void motorMoveRelation(uint8_t motorIndex, int diff) {
    motorState *motor = &motorStates[motorIndex];
    motor->position += diff;
    motor->stepper.moveTo(motor->position);
}

void motorMoveToSensorValue(uint8_t motorIndex, uint8_t axisIndex, uint16_t value) {

}