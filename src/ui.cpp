#include "ui.h"
#include <MD_REncoder.h>
#include "lcd.h"
#include "menu.h"
#include "motors.h"
#include "joy.h"

#define PIN_A            PB3  
#define PIN_B            PB4  
#define BUTTON           PA15  

u_int32_t lastButtonPress = 0;
u_int32_t enterEventCount = 0;
bool enterIsDown = false;

bool upPressed = false;
bool downPressed = false;
bool enterPressed = false;

MD_REncoder encoder = MD_REncoder(PIN_A, PIN_B);
Menu menu = Menu(getLcd());

MenuItem axisMenuItems[] = {
  MenuItem("Speed brake", []() { printAxisMonitor(0); }),
  MenuItem("Throttle 1", []() { printAxisMonitor(1); }),
  MenuItem("Throttle 2", []() { printAxisMonitor(2); }),
  MenuItem("Flaps", []() { printAxisMonitor(3); }),
  MenuItem("Reverse 1", []() { printAxisMonitor(4); }),
  MenuItem("Reverse 2", []() { printAxisMonitor(5); }),
  MenuItem("Trim", []() { printAxisMonitor(6); })
};

MenuItem monitorMenuItems[] = {
  MenuItem("Axis", axisMenuItems, 7),
  MenuItem("Buttons", []() { printButtonsMonitor(); }),
  MenuItem("Usb In", []() { printLastUsbMessage(); }),
  MenuItem("I2C1", []() { printI2C(0); }),
  MenuItem("I2C2", []() { printI2C(1); })
};

MenuItem motorsMenuItems[] = {
  MenuItem("Speed brake", []() { printMotors(0); }),
  MenuItem("Throttle 1", []() { printMotors(1); }),
  MenuItem("Throttle 2", []() { printMotors(2); }),
  MenuItem("Trim", []() { printMotors(3); }),
  MenuItem("Trim Ind 1", []() { printMotors(4); }),
  MenuItem("Trim Ind 2", []() { printMotors(5); })
};

void startCalibrateAxis(uint8_t axis) {
  setCalibrateAxis(axis, true);
  printAxisCalibration();
}

MenuItem axisCalibrationMenuItems[] = {
  MenuItem("All together", []() { startCalibrateAxis(ALL_AXIS); }),
  MenuItem("Speed brake", []() { startCalibrateAxis(0); }),
  MenuItem("Throttle 1", []() { startCalibrateAxis(1); }),
  MenuItem("Throttle 2", []() { startCalibrateAxis(2); }),
  MenuItem("Flaps", []() { startCalibrateAxis(3); }),
  MenuItem("Reverse 1", []() { startCalibrateAxis(4); }),
  MenuItem("Reverse 2", []() { startCalibrateAxis(5); })
};

MenuItem settingsMenuItems[] = {
  MenuItem("Calibrate axis", axisCalibrationMenuItems, 7)
};

#define MENU_0_SETTINGS 3
#define MENU_1_CALIBRATE_AXIS 0

MenuItem menuItems[] = {
    MenuItem("About", printAbout),
    MenuItem("Monitor", monitorMenuItems, 5),
    MenuItem("Motors", motorsMenuItems, 6),
    MenuItem("Settings", settingsMenuItems, 1),
    MenuItem("Item2", NULL),
    MenuItem("Item3", NULL)
  };

void onDownInScreen(int menuLevel) {
  if (menuLevel == 1 && menu.getRowAtLevel(0) == 2) {
    motorTestDown(menu.getCurrentRow());
  } else {
    scrollDown();
  }
}  

void onUpInScreen(int menuLevel) {
  if (menuLevel == 1 && menu.getRowAtLevel(0) == 2) {
    motorTestUp(menu.getCurrentRow());
  } else {
    scrollUp();
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
      if (menu.getCurrentLevel() == 2 && menu.getRowAtLevel(0) == MENU_0_SETTINGS && menu.getRowAtLevel(1) == MENU_1_CALIBRATE_AXIS) {
        // at axis calibration screen
        setCalibrateAxis(ALL_AXIS, false);
      }
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
  if (enterPressed) {
    // previous press was not handled yet
    return;
  }
  uint8_t buttonValue = digitalRead(BUTTON);
  // on button going LOW start measuring time
  if (buttonValue == LOW) {
    enterEventCount++;
    lastButtonPress = millis();
    enterIsDown = true;
    return;
  }
  // on button going HIGH check if it was pressed for more than 20ms
  if (enterIsDown && buttonValue == HIGH && millis() - lastButtonPress > 20) {
    enterPressed = true;
    enterIsDown = false;
  }
  // otherwise it was just a short press, ignore
}

int getEnterEventCount() {
  return enterEventCount;
}

void setupMenu() {
  menu.addItems(menuItems, 6);
}

void setupUi() {
  pinMode(BUTTON, INPUT_PULLUP);
  initLcd();
  encoder.begin();
  attachInterrupt(digitalPinToInterrupt(PIN_A),  encoderISR,       CHANGE);  //call encoderISR()    every high->low or low->high changes
  attachInterrupt(digitalPinToInterrupt(PIN_B),  encoderISR,       CHANGE);  //call encoderISR()    every high->low or low->high changes
  attachInterrupt(digitalPinToInterrupt(BUTTON), encoderButtonISR, CHANGE); //call pushButtonISR() every high->low              changes
  setupMenu();
  menu.show();
}

void refreshUi() {
  if (!menu.isVisible()) {
    // at some screen, not menu
    if (menu.getCurrentLevel() == 0 && menu.getCurrentRow() == 0) {
      // at about screen
      printAbout();
    } else if (menu.getCurrentLevel() == 2 && menu.getRowAtLevel(0) == 1 && menu.getRowAtLevel(1) == 0) {
      // at axis monitor screen
      printAxisMonitor(menu.getCurrentRow());
    } else if (menu.getCurrentLevel() == 1 && menu.getRowAtLevel(0) == 1 && menu.getCurrentRow() == 1) {
      // at axis monitor screen
      printButtonsMonitor();
    } else if (menu.getCurrentLevel() == 2 && menu.getRowAtLevel(0) == MENU_0_SETTINGS && menu.getRowAtLevel(1) == MENU_1_CALIBRATE_AXIS) {
      // at axis calibration screen
      printAxisCalibration();
    }
  }
}