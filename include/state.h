#pragma once

#include <Arduino.h>

#define COMMON_STATE_LENGTH 20
#define NUMBER_OF_ANALOG_INPUTS 7

#define NUMBER_OF_BUTTONS 12

#define PIN_PARK_SWITCH 4
#define PIN_PARK_LED PA3

#define NOT_USED 253

#define BUTTON_INDEX_TRIM_INDICATOR_STOP_1 9
#define BUTTON_INDEX_TRIM_INDICATOR_STOP_2 10

enum callbackEventType { motorVoltageChanged };

struct globalState {
    char i2cMultiplexerState[COMMON_STATE_LENGTH] = "?";
    bool motorVoltagePresent = false;
    bool strong5VoltagePresent = false;
    bool trimCalibrated = false;
};

struct buttonState {
    uint8_t pin;
    uint8_t value;
    uint8_t changeCount;
    uint32_t lastChangeTime;
    bool reversed;
};

struct axisState {
    uint8_t pin;
    uint8_t i2cChannel;
    uint16_t value; // 0-4096
    uint16_t rawValue; // from sensor, just shifted with offset and reversed
    uint16_t minValue;
    uint16_t maxValue;
    bool calibrating;
    bool reversed;
};

buttonState* getButtons();

void setI2cMultiplexerState(char *state);
char *getI2cMultiplexerState();
void setAnalogInputValue(uint8_t index, uint16_t value, uint16_t rawValue);
uint16_t getAnalogInputValue(uint8_t index);
uint16_t getAnalogInputRawValue(uint8_t index);
axisState *getAxis(uint8_t index);

void setLastMessage(String message);
String getLastMessage();

void addStateCallback(callbackEventType eventType, void (*callback)(int param));
void setMotorVoltagePresent(bool value);
globalState *getGlobalState();