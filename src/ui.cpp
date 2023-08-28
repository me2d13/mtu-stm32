#include "ui.h"
#include <MD_REncoder.h>
#include "lcd.h"
#include "menu.h"

#define PIN_A            PB3  
#define PIN_B            PB4  
#define BUTTON           PA15  

u_int32_t lastButtonPress = 0;

MD_REncoder encoder = MD_REncoder(PIN_A, PIN_B);
Menu menu = Menu(getLcd());

MenuItem menuItems[3] = {
    MenuItem("About", printAbout),
    MenuItem("Monitor axis", NULL),
    MenuItem("Settings", NULL)
  };

void encoderISR()
{
  uint8_t x = encoder.read();
  if (x && menu.isVisible()) {
    if (x == DIR_CCW) {
      menu.up();
    } else {
      menu.down();
    }
  }
}

void encoderButtonISR()
{
  if (millis() - lastButtonPress < 100) {
    return;
  }
  lastButtonPress = millis();
  if (menu.isVisible()) {
    menu.enter();
  } else {
    getLcd().clear();
    menu.show();
  }
}

void setupMenu() {
  menu.addItems(menuItems, 3);
}

void setupUi() {
  initLcd();
  encoder.begin();
  attachInterrupt(digitalPinToInterrupt(PIN_A),  encoderISR,       CHANGE);  //call encoderISR()    every high->low or low->high changes
  attachInterrupt(digitalPinToInterrupt(PIN_B),  encoderISR,       CHANGE);  //call encoderISR()    every high->low or low->high changes
  attachInterrupt(digitalPinToInterrupt(BUTTON), encoderButtonISR, FALLING); //call pushButtonISR() every high->low              changes
  setupMenu();
  menu.show();
}

void refreshUi() {
  if (!menu.isVisible()) {
    // at some screen, not menu
    if (menu.getCurrentLevel() == 0 && menu.getCurrentRow() == 0) {
      // at about screen
      printAbout();
    }
  }
}