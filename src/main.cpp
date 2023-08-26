#include <Arduino.h>
#include <AS5600.h>
#include "Joystick.h"
#include <TaskJockey.h>
#include "ui.h"
#include "usb_input.h"


#define ledPin PC13 //13

AS5600 as5600; //  use default Wire


uint16_t lastPos = 0;
uint8_t lastChar = ' ';
uint16_t seconds = 0;
uint32_t encReadMilis = 0;

TaskJockey jockey;


Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID,
                   JOYSTICK_TYPE_JOYSTICK, 0, 0,
                   true, false, false, false, false, false,
                   false, false, false, false, false);

void taskSecondsIncrease(taskId_t taskId) {
  seconds++;
  refreshUi();
}

void taskLedOff(taskId_t taskId) {
  digitalWrite(ledPin, HIGH);
}

void taskLedOn(taskId_t taskId) {
  digitalWrite(ledPin, LOW);
  jockey.addTask(taskLedOff, NULL, 50, 0, 1);
}



void setup() {
  pinMode(ledPin,OUTPUT);
  pinMode(PA4,OUTPUT);
  setupUsbIn();
  setupUi();
  
  Joystick.setXAxisRange(0, 4096);
  Joystick.begin();


  jockey.addTask(taskLedOn, NULL, 500);
  jockey.addTask(taskSecondsIncrease, NULL, 1000);

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
  void loopUsbIn();
  jockey.runTasks();
}