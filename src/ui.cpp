#include "ui.h"
#include <MD_REncoder.h>
#include "lcd.h"
#include "menu.h"
#include "motors.h"

#define PIN_A            PB3  
#define PIN_B            PB4  
#define BUTTON           PA15  

u_int32_t lastButtonPress = 0;

bool upPressed = false;
bool downPressed = false;
bool enterPressed = false;

MD_REncoder encoder = MD_REncoder(PIN_A, PIN_B);
Menu menu = Menu(getLcd());

MenuItem monitorMenuItems[] = {
  MenuItem("Axis", []() { printAxisMonitor(0); }),
  MenuItem("Buttons", []() { printButtonsMonitor(); }),
  MenuItem("I2C1", []() { printI2C(0); }),
  MenuItem("I2C2", []() { printI2C(1); })
};

MenuItem motorsMenuItems[] = {
  MenuItem("Speed brake", []() { printMotors(0); })
};

MenuItem menuItems[] = {
    MenuItem("About", printAbout),
    MenuItem("Monitor", monitorMenuItems, 4),
    MenuItem("Motors", motorsMenuItems, 1),
    MenuItem("Item1", NULL),
    MenuItem("Item2", NULL),
    MenuItem("Item3", NULL)
  };

void onDownInScreen(int menuLevel) {
  if (menuLevel == 1 && menu.getRowAtLevel(0) == 2) {
    motorTestDown(menu.getCurrentRow());
  }
}  

void onUpInScreen(int menuLevel) {
  if (menuLevel == 1 && menu.getRowAtLevel(0) == 2) {
    motorTestUp(menu.getCurrentRow());
  }
}

void loopUi() {
  if (downPressed) {
    if (menu.isVisible()) {
      menu.down();
    } else {
      onDownInScreen(menu.getCurrentLevel());
    }
    downPressed = false;
  }
  if (upPressed) {
    if (menu.isVisible()) {
      menu.up();
    } else {
      onUpInScreen(menu.getCurrentLevel());
    }
    upPressed = false;
  }
  if (enterPressed) {
    if (menu.isVisible()) {
      menu.enter();
    } else {
      getLcd().clear();
      menu.show();
    }
    enterPressed = false;
  }
}

void encoderISR()
{
  uint8_t x = encoder.read();
  if (x) {
    if (x == DIR_CCW) {
      upPressed = true;
    } else {
      downPressed = true;
    }
  }
}

void encoderButtonISR()
{
  if (millis() - lastButtonPress < 300) {
    return;
  }
  lastButtonPress = millis();
  enterPressed = true;
}

void setupMenu() {
  menu.addItems(menuItems, 6);
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
    } else if (menu.getCurrentLevel() == 1 && menu.getRowAtLevel(1) == 1 && menu.getCurrentRow() == 0) {
      // at axis monitor screen
      printAxisMonitor(menu.getCurrentRow());
    }
  }
}