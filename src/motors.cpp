#include <Arduino.h>
#include "motors.h"
#include <AccelStepper.h>
#include "inputs.h"

motorState motorStates[NUMBER_OF_MOTORS] = {
    {SPEED_BRAKE_DIR, SPEED_BRAKE_STEP, 3, 0, 0, false, 0, NULL, 0}, // GPA3 speed brake enable
    {THR1_DIR, THR1_STEP, 0, 0, 0, false, 0, NULL, 0}, // GPA0 throttle enable
    {THR2_DIR, THR2_STEP, 0, 0, 0, false, 0, NULL, 0}, // GPA0 throttle enable
    {TRIM_DIR, TRIM_STEP, 2, 0, 0, false, 0, NULL, 0}, // GPA2 trim enable
    {IND_DIR, IND_STEP, PA8, 0, 0, false, 0, NULL, 0}, 
    {IND_DIR, IND_STEP, PB12, 0, 0, false, 0, NULL, 0}
};

// function to return motor state
motorState *getMotor(uint8_t index) {
    return &motorStates[index];
}

void setEnabledMotor(uint8_t index, uint8_t value) {
    motorState *motor = &motorStates[index];
    if (motor->stepPin != IND_STEP) {
        // for all motors except indicator enable is muxed
        setMuxedOutputPin(motor->enablePin, value);
    } else {
        // for indicator enable is direct pin
        digitalWrite(motor->enablePin, value);
    }
}

void enableMotor(uint8_t index) {
    setEnabledMotor(index, LOW);
}

void disableMotor(uint8_t index) {
    setEnabledMotor(index, HIGH);
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
        disableMotor(i);
    }
}

void motorTestUp(uint8_t motorIndex) {
    enableMotor(motorIndex);
    motorState *motor = &motorStates[motorIndex];
    motor->position += 10;
    motor->stepper.moveTo(motor->position);
    //stepper.runToPosition();
}

void motorTestDown(uint8_t motorIndex) {
    enableMotor(motorIndex);
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