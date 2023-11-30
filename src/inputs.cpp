#include <Arduino.h>
#include "inputs.h"
#include <TCA9548.h>
#include <Wire.h>
#include "state.h"
#include <AS5600.h>
#include <MCP23017.h>
#include "joy.h"
#include "motors.h"

TwoWire Wire2(PB11, PB10);// Use STM32 I2C2
TCA9548 i2cMultiplexer(0x70);
AS5600 as5600; //  use default Wire
MCP23017 ioExpander;

int axisOffsets[NUMBER_OF_ANALOG_INPUTS] = { -200, 0, 0, 0, 0, 0, 0 };

bool shouldReadButtons = false;
bool initialRun = true;

void readButtons() {
    shouldReadButtons = true;
}

void setupInputs() {
    buttonState* buttons = getButtons();
    Wire.begin();
    if (i2cMultiplexer.begin()) {
        strcpy(getGlobalState()->i2cMultiplexerState, (char *) "OK");
    } else {
        strcpy(getGlobalState()->i2cMultiplexerState, (char *) "ERR");
    }
    as5600.begin(4);
    //ioExpander.begin(Wire2);
    ioExpander.begin();
    ioExpander.interruptSetup(1, 0, LOW); // mirror on, open drain off, polarity active low
    // set all buttons to input pullup
    for (int i = 0; i < NUMBER_OF_BUTTONS; i++) {
        ioExpander.pinMode(buttons[i].pin, INPUT_PULLUP);
        ioExpander.interruptPin(buttons[i].pin, CHANGE); // interrupt on pin 8, falling edge
    }
    pinMode(PA0, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PA0), readButtons, CHANGE); 

    // read all buttons once
    for (int i = 0; i < NUMBER_OF_BUTTONS; i++) {
        buttons[i].value = ioExpander.digitalRead(buttons[i].pin);
        buttons[i].changeCount++;
        buttons[i].lastChangeTime = millis();
    }

    // setup output pins
    for (size_t i = 0; i < NUMBER_OF_MOTORS; i++)
    {
        motorState *motor = getMotor(i);
        if (motor->stepPin != IND_STEP) {
            // for all motors except indicator enable is muxed
            ioExpander.pinMode(motor->enablePin, OUTPUT);
        } else {
            // for indicator enable is direct pin
            pinMode(motor->enablePin, OUTPUT);
        }
    }

    // setup park led pin
    pinMode(PIN_PARK_LED, OUTPUT);
}

void setMuxedOutputPin(uint8_t pin, uint8_t value) {
    ioExpander.digitalWrite(pin, value);
}

void readAxis() {
    for (int i = 0; i < NUMBER_OF_ANALOG_INPUTS; i++) {
        axisState *axis = getAxis(i);
        uint16_t lastValue = axis->value;
        uint16_t value = 0;
        uint16_t rawValue = 0;
        if (axis->pin != NOT_USED) {
            rawValue = analogRead(axis->pin);
        }
        if (axis->i2cChannel != NOT_USED) {
            i2cMultiplexer.selectChannel(axis->i2cChannel);
            rawValue = as5600.rawAngle();
        }
        // compenstate for axis offset, when movement goes over 0
        rawValue = (rawValue + AXIS_MAX + axisOffsets[i]) % AXIS_MAX;
        if (!axis->calibrating && (lastValue != value || initialRun)) {
            // because of some noise, never use value below minValue or above maxValue
            if (rawValue < axis->minValue) {
                rawValue = axis->minValue;
            }
            if (rawValue > axis->maxValue) {
                rawValue = axis->maxValue;
            }
            // recalculate value by calibrated axis range to have 0-4096 range
            value = map(rawValue, axis->minValue, axis->maxValue, 0, AXIS_MAX);
            setJoystickAxis(i, value);
        }
        setAnalogInputValue(i, value, rawValue);
    }
}

void manageButtons() {
    bool interruptSignalled = digitalRead(PA0) == LOW;
    if (shouldReadButtons || interruptSignalled) {
        buttonState* buttons = getButtons();
        shouldReadButtons = false;
        uint8_t pin = ioExpander.lastInterruptPin();
        uint8_t value = ioExpander.lastInterruptPinValue();
        uint32_t time = millis();
        for (int i = 0; i < NUMBER_OF_BUTTONS; i++) {
            if (buttons[i].pin == pin && buttons[i].value != value && time - buttons[i].lastChangeTime > 100) {
                buttons[i].value = value;
                buttons[i].changeCount++;
                buttons[i].lastChangeTime = millis();
                setJoystickButton(i, value == (buttons[i].reversed ? HIGH : LOW));
                if (buttons[i].pin == PIN_PARK_SWITCH) {
                    digitalWrite(PIN_PARK_LED, !value);
                }
                break;
            }
        }
    }
}

void initialReadButtons() {
    uint32_t time = millis();
    buttonState* buttons = getButtons();
    for (int i = 0; i < NUMBER_OF_BUTTONS; i++) {
        buttons[i].value = ioExpander.digitalRead(buttons[i].pin);
        buttons[i].changeCount++;
        buttons[i].lastChangeTime = millis();
        setJoystickButton(i, buttons[i].value == (buttons[i].reversed ? HIGH : LOW));
        if (buttons[i].pin == PIN_PARK_SWITCH) {
            digitalWrite(PIN_PARK_LED, !buttons[i].value);
        }
    }
}

void refreshInputs() {
    readAxis();
    if (initialRun) {
        initialReadButtons();
    } else { 
        manageButtons();
    }
    initialRun = false;
}

void scanI2C(int index, char screenBuffer[][21]) {
    TwoWire w = index == 0 ? Wire : Wire2;
    w.begin();
    int error;
    int nDevices;
    int i;
    nDevices = 0;
    for (i = 1; i < 127; i++) {
        w.beginTransmission(i);
        error = w.endTransmission();
        if (error == 0) {
            sprintf(screenBuffer[nDevices], "0x%x, ", i);
            nDevices++;
        }
    }
    if (nDevices == 0) {
        sprintf(screenBuffer[0], "No I2C devices found");
    }
}