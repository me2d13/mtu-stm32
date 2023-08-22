#include <Arduino.h>
#include <AS5600.h>
#include "Joystick.h"
#include "SerialUSB.h"
#include "lcd.h"

#define ledPin PC13 //13

AS5600 as5600; //  use default Wire
SerialUSB_ SerialUSB;

uint16_t lastPos = 0;
uint8_t lastChar = ' ';

Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID,
                   JOYSTICK_TYPE_JOYSTICK, 0, 0,
                   true, false, false, false, false, false,
                   false, false, false, false, false);

void setup() {
  pinMode(ledPin,OUTPUT);
  pinMode(PA4,OUTPUT);
  SerialUSB.begin(115200);

  USB_Begin();

  while (!USB_Running()){
    //wait until usb connected
    delay(50);
  }
  //while (!SerialUSB){
    //wait until Serial port is connected
    //delay(50);
  //}
  
  Joystick.setXAxisRange(0, 4096);
  Joystick.begin();

  initLcd();

  as5600.begin(4);
}

void loop() {
  digitalWrite(ledPin, HIGH);
  delay(50);
  digitalWrite(ledPin, LOW);
  delay(50);

  uint16_t pos = as5600.rawAngle();
  if (pos != lastPos)
  {
    lastPos = pos;
    printLcd(pos, lastChar);
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

  if (SerialUSB.available()){
    lastChar = SerialUSB.read();
    printLcd(lastPos, lastChar);
  }
}