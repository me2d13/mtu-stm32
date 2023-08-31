#include "inputs.h"
#include <TCA9548.h>
#include <Wire.h>
#include "state.h"
#include <AS5600.h>


TCA9548 i2cMultiplexer(0x70);
AS5600 as5600; //  use default Wire

void setupInputs() {
    Wire.begin();
    if (i2cMultiplexer.begin()) {
        setI2cMultiplexerState((char *) "OK");
    } else {
        setI2cMultiplexerState((char *) "ERR");
    }
    as5600.begin(4);
}

void refreshInputs() {
    // throttle 1 is on channel 1
    i2cMultiplexer.selectChannel(1);
    setAnalogInputValue(0, as5600.rawAngle());
}