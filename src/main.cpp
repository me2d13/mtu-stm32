#include <Arduino.h>
#include <AS5600.h>
#include "Joystick.h"
#include "ui.h"
#include "usb_input.h"
#include <AceRoutine.h>
using namespace ace_routine;


#define ledPin PC13 //13

AS5600 as5600; //  use default Wire


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
    refreshUi();
    COROUTINE_DELAY(1000);
  }
}


void setup() {
  pinMode(ledPin,OUTPUT);
  pinMode(PA4,OUTPUT);
  setupUsbIn();
  setupUi();
  
  Joystick.setXAxisRange(0, 4096);
  Joystick.begin();


  as5600.begin(4);
}

void loop() {
  //uint16_t pos = as5600.rawAngle();
  int16_t pos = lastPos; //encoder.getPosition();
  if (pos != lastPos)
  {
    lastPos = pos;
    Joystick.setXAxis(pos);
    if (pos > 2048)
    {
      digitalWrite(PA4, HIGH);
    }
    else
    {
      digitalWrite(PA4, LOW);
    }
  }
  //loopUsbIn();
  blinkLed.runCoroutine();
  refreshUiCoRoutine.runCoroutine();
}