#include "ui.h"
#include <MD_REncoder.h>
#include "lcd.h"

#define PIN_A            PB3  
#define PIN_B            PB4  
#define BUTTON           PA15  

MD_REncoder encoder = MD_REncoder(PIN_A, PIN_B);

void encoderISR()
{
  uint8_t x = encoder.read();
  if (x) {
    //lastPos += (x == DIR_CW) ? 1 : -1;
    //printLcd(lastPos, lastChar, seconds);
  }
}

void setupUi() {
  initLcd();
  encoder.begin();
  attachInterrupt(digitalPinToInterrupt(PIN_A),  encoderISR,       CHANGE);  //call encoderISR()    every high->low or low->high changes
  attachInterrupt(digitalPinToInterrupt(PIN_B),  encoderISR,       CHANGE);  //call encoderISR()    every high->low or low->high changes
  //attachInterrupt(digitalPinToInterrupt(BUTTON), encoderButtonISR, FALLING); //call pushButtonISR() every high->low              changes
}

void refreshUi() {
  printAbout();
}