#include <Arduino.h>
#include <I2C_eeprom.h>
#include "state.h"
#include "eeprom.h"

I2C_eeprom ee(0x50, I2C_DEVICESIZE_24LC128);

void setupEeprom() {
    ee.begin();
    //uint32_t eepromSize = ee.determineSize();
    //setLastMessage("EEPROM size " + String(eepromSize));
}

void writeWord(uint16_t address, uint16_t value) {
    ee.writeByte(address, value >> 8);
    ee.writeByte(address + 1, value & 0xFF);
}

uint16_t readWord(uint16_t address) {
    uint16_t value = ee.readByte(address) << 8;
    value |= ee.readByte(address + 1);
    return value;
}

void writeCalibrationToEeprom() {
    if (!ee.isConnected()) {
        setLastMessage("EEPROM not ready");
        return;
    }
    int res = ee.writeByte(0, 0x13); // magic
    ee.writeByte(1, 0x1); // version

    for (int i = 0; i < NUMBER_OF_ANALOG_INPUTS; i++) {
        axisState *axis = getAxis(i);
        writeWord(2 + i * 4, axis->minValue);
        writeWord(4 + i * 4, axis->maxValue);
    }
    setLastMessage("EEPROM updated " + String(res));
}

void readCalibrationFromEeprom() {
    if (!ee.isConnected()) {
        setLastMessage("EEPROM not ready");
        return;
    }
    uint8_t magic = ee.readByte(0);
    uint8_t version = ee.readByte(1);
    if (magic != 0x13 || version != 0x1) {
        setLastMessage("EEPROM not calibrated " + String(magic, HEX) + " " + String(version, HEX));
        return;
    }
    for (int i = 0; i < NUMBER_OF_ANALOG_INPUTS; i++) {
        axisState *axis = getAxis(i);
        axis->minValue = readWord(2 + i * 4);
        axis->maxValue = readWord(4 + i * 4);
    }
}