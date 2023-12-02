#include <Arduino.h>
#include "motors.h"
#include <AccelStepper.h>
#include "inputs.h"
#include "state.h"
#include "log.h"

// keep the motor spinning until we have ar least minimal axis change per given miliseconds
#define MIN_AXIS_CHANGE 10
#define MIN_AXIS_CHANGE_TIME 500

#define NO_TARGET 5000

#define MOTOR_CONFIG(x, enablePin, reversed) {x##_DIR, x##_STEP, enablePin, NO_TARGET, NULL, reversed, false}

motorState motorStates[NUMBER_OF_MOTORS] = {
    MOTOR_CONFIG(SPEED_BRAKE, 3, true),
    MOTOR_CONFIG(THR1, 0, false),
    MOTOR_CONFIG(THR2, 0, true),
    MOTOR_CONFIG(TRIM, 2, false),
    MOTOR_CONFIG(IND, PA8, false), 
    MOTOR_CONFIG(IND, PB12, false)
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
    motor->enabled = value == LOW;
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
        motor->stepper.setPinsInverted(motor->invertedDirection, false, true);
        motor->stepper.setMaxSpeed(500);
        motor->stepper.setAcceleration(100);
        motor->stepper.setCurrentPosition(0);
        disableMotor(i);
    }
    // stepper voltage detect PIN
    pinMode(PB15, INPUT);
    // strong 5v voltage detect PIN
    pinMode(PB14, INPUT_PULLUP);
}

void handleSpeedByTagertValue(uint8_t motorIndex) {
    motorState *motor = &motorStates[motorIndex];
    if (motor->targetValue == NO_TARGET) {
        return;
    }
    u_int16_t currentValue = getAxis(motorIndex)->value;
    // if motor is locked (e.g. by hand or end stop) and value has not changed, stop it
    uint32_t currentTime = millis();
    if (currentTime - motor->lastRecordedTime > MIN_AXIS_CHANGE_TIME) {
        int travelDistance = abs(currentValue - motor->lastRecordedValue);
        if (travelDistance < MIN_AXIS_CHANGE) {
            logi("Motor locked, distance only", travelDistance);
            logi("Locked time", currentTime);
            motor->stepper.setSpeed(0);
            disableMotor(motorIndex);
            motor->targetValue = NO_TARGET;
            return;
        } else {
            motor->lastRecordedTime = currentTime;
            motor->lastRecordedValue = currentValue;
        }
    }
    // if position is close to target, stop motor if it runs
    if (abs(currentValue - motor->targetValue) < 1) {
        motor->stepper.setSpeed(0);
        disableMotor(motorIndex);
        motor->targetValue = NO_TARGET;
        return;
    }
    // otherwise set motor speed based on difference between axis position and target
    float speed = 300;
    if (currentValue > motor->targetValue) {
        speed = -speed;
    }
    enableMotor(motorIndex);
    motor->stepper.setSpeed(speed);
}

void loopMotors() {
    for (size_t i = 0; i < NUMBER_OF_MOTORS - 1; i++) // -1 because indicator 2 is slave of indicator 1
    {
        motorState *motor = &motorStates[i];
        // motors 0-2 run to speed, others run to position
        if (i < 3) {
            handleSpeedByTagertValue(i);
            motor->stepper.runSpeed();
        } else {
            motor->stepper.run();
        }
    }
    // read stepper voltage detect PIN and set its state if value changed
    bool stepperVoltageDetected = digitalRead(PB15) == LOW;
    if (stepperVoltageDetected != getGlobalState()->motorVoltagePresent) {
        setMotorVoltagePresent(stepperVoltageDetected);
    }
    // read strong 5v voltage detect PIN and set its state if value changed
    bool strong5vVoltageDetected = digitalRead(PB14) == LOW;
    if (strong5vVoltageDetected != getGlobalState()->strong5VoltagePresent) {
        getGlobalState()->strong5VoltagePresent = strong5vVoltageDetected;
    }
}

void motorMoveToSensorValue(uint8_t motorIndex, uint8_t axisIndex, float value) {
    // get current axis position as percentage of axis range 0-4096
    float axisPosition = getAxis(axisIndex)->value / 4096.0 * 100;
    logff("Moving to value with current axis position", value, axisPosition);
    motorStates[motorIndex].lastRecordedTime = millis();
    motorStates[motorIndex].lastRecordedValue = getAxis(axisIndex)->value;
    motorStates[motorIndex].targetValue = value*4096.0/100.0;
    logi("Start time", motorStates[motorIndex].lastRecordedTime);
}

void moveMotorBySensor(uint8_t motorIndex, float position) {
    if (motorIndex >= 0 && motorIndex <= 2) {
        // for speed brake and throttles, indexes are the same as axis indexes
        motorMoveToSensorValue(motorIndex, motorIndex, position);
    }
}