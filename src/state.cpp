#include "state.h"
#include <Arduino.h>
#include <string.h>
#include "joy.h"

globalState global;

String lastMessage = "No message received yet.\nBut dont worry, it will come soon.\nOne day...\nMaybe...\nOr not...\nWho knows...\n";

axisState axis[NUMBER_OF_ANALOG_INPUTS] = {
    { NOT_USED, 0, 0, 0, 0, AXIS_MAX, false }, // Speed brake
    { NOT_USED, 1, 0, 0, 0, AXIS_MAX, false }, // Throttle 1
    { NOT_USED, 2, 0, 0, 0, AXIS_MAX, false }, // Throttle 2
    { NOT_USED, 3, 0, 0, 0, AXIS_MAX, false }, // Flaps
    { PA1, NOT_USED, 0, 0, 0, AXIS_MAX, false }, // Reverse 1
    { PA2, NOT_USED, 0, 0, 0, AXIS_MAX, false }, // Reverse 2
    { NOT_USED, 5, 0, 0, 0, AXIS_MAX, false }, // Trim
};

buttonState buttons[] = {
    { 10, 0, 0, 0, false }, // GPB0 AT disconnect 1
    { 9, 0, 0, 0, false }, // GPB1 AT disconnect 2
    { 8, 0, 0, 0, false }, // GPB2 TOGA 1
    { 11, 0, 0, 0, false }, // GPB3 TOGA 2
    { 12, 0, 0, 0, false }, // GPB4 stab trim normal
    { 13, 0, 0, 0, false }, // GPB5 stab trim ap
    { 14, 0, 0, 0, true }, // GPB6 start 1
    { 15, 0, 0, 0, true }, // GPB7 start 2
    { 7, 0, 0, 0, false }, // GPA7 AT cut off
    { 6, 0, 0, 0, false }, // GPA6 trim indicator stop 1
    { 5, 0, 0, 0, false }, // GPA5 trim indicator stop 2
    { PIN_PARK_SWITCH, 0, 0, 0 } // GPA3 parking brake
};

struct callbackRecord {
    callbackEventType eventType;
    void (*callback)(int param);
};

callbackRecord callbacks[10];
int callbackCount = 0;

buttonState* getButtons() {
    return buttons;
}


void setAnalogInputValue(uint8_t index, uint16_t value, uint16_t rawValue) {
    if (axis[index].value != value) {
        axis[index].value = value;
        // auto calibration, set min and max values
        if (axis[index].calibrating && axis[index].minValue > value) {
            axis[index].minValue = value;
        }
        if (axis[index].calibrating && axis[index].maxValue < value) {
            axis[index].maxValue = value;
        }
    }
    axis[index].rawValue = rawValue;
}

uint16_t getAnalogInputValue(uint8_t index) {
    return axis[index].value;
}

uint16_t getAnalogInputRawValue(uint8_t index) {
    return axis[index].rawValue;
}

axisState *getAxis(uint8_t index) {
    return &axis[index];
}

void setLastMessage(String message) {
    lastMessage = message;
}

String getLastMessage() {
    return lastMessage;
}

globalState *getGlobalState() {
    return &global;
}

void addStateCallback(callbackEventType eventType, void (*callback)(int param)) {
    callbacks[callbackCount].eventType = eventType;
    callbacks[callbackCount].callback = callback;
    callbackCount++;
}

void setMotorVoltagePresent(bool value) {
    if (global.motorVoltagePresent != value) {
        global.motorVoltagePresent = value;
        for (int i = 0; i < callbackCount; i++) {
            if (callbacks[i].eventType == motorVoltageChanged) {
                callbacks[i].callback(value);
            }
        }
    }
}