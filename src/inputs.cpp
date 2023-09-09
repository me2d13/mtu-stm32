#include <Arduino.h>
#include "inputs.h"
#include <TCA9548.h>
#include <Wire.h>
#include "state.h"
#include <AS5600.h>
#include <MCP23017.h>


TwoWire Wire2(PB11, PB10);// Use STM32 I2C2
TCA9548 i2cMultiplexer(0x70);
AS5600 as5600; //  use default Wire
MCP23017 ioExpander;

bool shouldReadButtons = false;

buttonState buttons[] = {
    { 8, 0, 0, 0 }, // GPB0 AT disconnect 1
    { 9, 0, 0, 0 }, // GPB1 AT disconnect 2
    { 10, 0, 0, 0 }, // GPB2 TOGA 1
    { 11, 0, 0, 0 }, // GPB3 TOGA 2
    { 12, 0, 0, 0 }, // GPB4 stab trim normal
    { 13, 0, 0, 0 }, // GPB5 stab trim ap
    { 14, 0, 0, 0 }, // GPB6 start 1
    { 15, 0, 0, 0 }, // GPB7 start 2
    { 7, 0, 0, 0 }, // GPA7 AT cut off
    { 6, 0, 0, 0 }, // GPA6 trim indicator stop 1
    { 5, 0, 0, 0 }, // GPA5 trim indicator stop 2
    { 4, 0, 0, 0 } // GPA3 parking brake
};

buttonState* getButtons() {
    return buttons;
}

void readButtons() {
    shouldReadButtons = true;
}

void setupInputs() {
    Wire.begin();
    if (i2cMultiplexer.begin()) {
        setI2cMultiplexerState((char *) "OK");
    } else {
        setI2cMultiplexerState((char *) "ERR");
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
}

void refreshInputs() {
    // speed brake is on channel 0
    i2cMultiplexer.selectChannel(0);
    setAnalogInputValue(0, as5600.rawAngle());
    // throttle 1 is on channel 1
    i2cMultiplexer.selectChannel(1);
    setAnalogInputValue(1, as5600.rawAngle());
    bool interruptSignalled = digitalRead(PA0) == LOW;
    if (shouldReadButtons || interruptSignalled) {
        shouldReadButtons = false;
        uint8_t pin = ioExpander.lastInterruptPin();
        uint8_t value = ioExpander.lastInterruptPinValue();
        uint32_t time = millis();
        for (int i = 0; i < NUMBER_OF_BUTTONS; i++) {
            if (buttons[i].pin == pin && buttons[i].value != value && time - buttons[i].lastChangeTime > 100) {
                buttons[i].value = value;
                buttons[i].changeCount++;
                buttons[i].lastChangeTime = millis();
                break;
            }
        }
    }
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