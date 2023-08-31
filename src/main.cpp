#include <Arduino.h>
#include "Joystick.h"
#include "ui.h"
#include "usb_input.h"
#include <AceRoutine.h>
#include "inputs.h"


#define ledPin PC13 //13



uint16_t lastPos = 0;
uint8_t lastChar = ' ';
uint16_t seconds = 0;
uint32_t encReadMilis = 0;


Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID,
                   JOYSTICK_TYPE_JOYSTICK, 0, 0,
                   true, false, false, false, false, false,
                   false, false, false, false, false);



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
  setupUsbIn();
  setupUi();
  setupInputs();
  
  Joystick.setXAxisRange(0, 4096);
  Joystick.begin();
}

void loop() {
  //loopUsbIn();
  blinkLed.runCoroutine();
  refreshUiCoRoutine.runCoroutine();
  loopUi();
  refreshInputs();
}