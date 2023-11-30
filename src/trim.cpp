#include "trim.h"
#include "lcd.h"
#include "state.h"
#include "log.h"
#include <AceRoutine.h>
#include "motors.h"

#define STEPS_FOR_TRIM_5 185

using namespace ace_routine;

enum trimCalibrationState { inactive, willStart, started, working, done };

trimCalibrationState leftTrimState = inactive;
trimCalibrationState rightTrimState = inactive;

class TrimCalibrationCoroutine : public Coroutine {
    private:
      u_int8_t buttonIndex;
      u_int8_t motorIndex;
      trimCalibrationState *trimState;
      trimCalibrationState *followingTrimState;
      u_int16_t switchPosition = 0;
  public:
    TrimCalibrationCoroutine(
            u_int8_t buttonIndex, 
            u_int8_t motorIndex, 
            trimCalibrationState *trimState, 
            u_int16_t switchPosition,
            trimCalibrationState *followingTrimState
            ) {
        this->buttonIndex = buttonIndex;
        this->motorIndex = motorIndex;
        this->trimState = trimState;
        this->switchPosition = switchPosition;
        this->followingTrimState = followingTrimState;
    }

    int runCoroutine() override {
        COROUTINE_LOOP() {
            if (*trimState == started) {
                if (getButtons()[buttonIndex].value) {
                    printString("Trim in 1st part\nSeeking second part margin");
                    *trimState = working;
                    motorState* motor = getMotor(motorIndex);
                    motor->stepper.setCurrentPosition(0);
                    enableMotor(motorIndex);
                    motor->stepper.move(400);
                    COROUTINE_AWAIT(!getMotor(motorIndex)->stepper.isRunning() || !getButtons()[buttonIndex].value);
                    getMotor(motorIndex)->stepper.stop();
                    getMotor(motorIndex)->stepper.setCurrentPosition(switchPosition);
                    // print current position 
                    printString("Second part margin\n found after steps: " + String(getMotor(motorIndex)->stepper.currentPosition()) + "\nHave trim 5");
                    getMotor(motorIndex)->stepper.moveTo(STEPS_FOR_TRIM_5);
                    COROUTINE_DELAY_SECONDS(5);
                    getMotor(motorIndex)->stepper.moveTo(2*STEPS_FOR_TRIM_5);
                    COROUTINE_AWAIT(!getMotor(motorIndex)->stepper.isRunning());
                    printString("Moved to trim 10");
                    COROUTINE_DELAY_SECONDS(5);
                    getMotor(motorIndex)->stepper.moveTo(3*STEPS_FOR_TRIM_5);
                    COROUTINE_AWAIT(!getMotor(motorIndex)->stepper.isRunning());
                    printString("Moved to trim 15");
                    COROUTINE_DELAY_SECONDS(5);
                    getMotor(motorIndex)->stepper.moveTo(0);
                    COROUTINE_AWAIT(!getMotor(motorIndex)->stepper.isRunning());
                    printString("Moved to trim 0");
                    disableMotor(motorIndex);
                    getMotor(motorIndex)->stepper.setCurrentPosition(0);
                    *trimState = done;
                    if (followingTrimState != NULL) {
                        *followingTrimState = started;
                    } else {
                        printString("Trim calibration done");
                        getGlobalState()->trimCalibrated = true;
                    }
                } else {
                    printString("Trim in 2nd part\nSeeking first part margin");
                    *trimState = working;
                    motorState* motor = getMotor(motorIndex);
                    motor->stepper.setCurrentPosition(0);
                    enableMotor(motorIndex);
                    motor->stepper.move(-400);
                    COROUTINE_AWAIT(!getMotor(motorIndex)->stepper.isRunning() || getButtons()[buttonIndex].value);
                    getMotor(motorIndex)->stepper.stop();
                    if (getButtons()[buttonIndex].value) {
                        // print current position
                        printString("First part margin\n found after steps: " + String(-getMotor(motorIndex)->stepper.currentPosition()));
                        *trimState = started; // now find second part
                    } else {
                        printString("First part margin\n not found");
                        *trimState = done;
                    }
                    disableMotor(motorIndex);
                }
            } else if (*trimState == willStart) {
                COROUTINE_DELAY_SECONDS(3);
                *trimState = started;
            } else {
                COROUTINE_YIELD();
            }
        }
    }
};

TrimCalibrationCoroutine leftCoroutine(BUTTON_INDEX_TRIM_INDICATOR_STOP_1, 4, &leftTrimState, 193, &rightTrimState);
TrimCalibrationCoroutine rightCoroutine(BUTTON_INDEX_TRIM_INDICATOR_STOP_2, 5, &rightTrimState, 180, NULL);

void calibrateTrims() {
    printString("Calibrating trims");
    if (!getGlobalState()->motorVoltagePresent) {
        log("WARN: Starting trim calibration without motor voltage");
    }
    leftTrimState = started;
}

void setupTrims() {
    leftTrimState = inactive;
    rightTrimState = inactive;
    addStateCallback(motorVoltageChanged, [](int param) {
        if (param == 1) {
            leftTrimState = willStart;
            rightTrimState = inactive;
        }
    });
}


void loopTrims() {
    if (leftTrimState != inactive) {
        leftCoroutine.runCoroutine();
    }
    if (rightTrimState != inactive) {
        rightCoroutine.runCoroutine();
    }
}