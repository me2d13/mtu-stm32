#include <Arduino.h>
#include "ui.h"
#include "usb_input.h"
#include <AceRoutine.h>
#include "inputs.h"
#include "joy.h"
#include "motors.h"
#include "eeprom.h"
#include "log.h"
#include "trim.h"

#define ledPin PC13 //13



uint16_t lastPos = 0;
uint8_t lastChar = ' ';
uint16_t seconds = 0;
uint32_t encReadMilis = 0;


COROUTINE(blinkLed) {
  COROUTINE_LOOP() {
    digitalWrite(ledPin, LOW);
    COROUTINE_DELAY(100);
    digitalWrite(ledPin, HIGH);
    COROUTINE_DELAY(500);
  }
}

COROUTINE(refreshUiCoRoutine) {
  COROUTINE_LOOP() {
    // TODO: enabling following line break the ui - on race condition on lcd
    refreshUi();
    COROUTINE_DELAY(1000);
  }
}


void setup() {
  pinMode(ledPin,OUTPUT);
  pinMode(PA4,OUTPUT);
  setupEeprom();
  setupUsbIn();
  setupUi();
  setupInputs();
  setupJoystick();
  setupMotors();
  readCalibrationFromEeprom();
  setupTrims();
  log("MTU started...");
}

void loop() {
  loopUsbIn();
  blinkLed.runCoroutine();
  refreshUiCoRoutine.runCoroutine();
  loopUi();
  refreshInputs();
  loopMotors();
  loopTrims();
}