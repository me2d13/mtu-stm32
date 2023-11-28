#include "ui.h"
#include <MD_REncoder.h>
#include "lcd.h"
#include "menu.h"
#include "motors.h"
#include "joy.h"
#include "log.h"
#include "trim.h"

#define PIN_A            PB3  
#define PIN_B            PB4  
#define BUTTON           PA15  

u_int32_t lastButtonPress = 0;
u_int32_t enterEventCount = 0;
bool enterIsDown = false;
int currentLog = 0;

bool upPressed = false;
bool downPressed = false;
bool enterPressed = false;

void startPrintLog();
void printLog();

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

#define MENU_MONITOR_LOG 0
#define MENU_MONITOR_ALL 1
#define MENU_MONITOR_AXIS 2
#define MENU_MONITOR_BUTTONS 3

MenuItem monitorMenuItems[] = {
  MenuItem("Log", []() { startPrintLog(); }),
  MenuItem("Axis and buttons", []() { printAxisAndButtonsMonitor(); }),
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
  MenuItem("Calibrate axis", axisCalibrationMenuItems, 7),
  MenuItem("Trim indicators", []() { calibrateTrims(); }),
};

#define MENU_0_MOTORS 2
#define MENU_MONITOR 1
#define MENU_0_SETTINGS 3
#define MENU_1_CALIBRATE_AXIS 0

MenuItem menuItems[] = {
    MenuItem("About", printAbout),
    MenuItem("Monitor", monitorMenuItems, 7),
    MenuItem("Motors", motorsMenuItems, 6),
    MenuItem("Settings", settingsMenuItems, 2),
    MenuItem("Item2", NULL),
    MenuItem("Item3", NULL)
  };

void onDownInScreen(int menuLevel) {
  if (menuLevel == 1 && menu.getRowAtLevel(0) == 2) {
    motorTestDown(menu.getCurrentRow());
  } else if (menuLevel == 1 && menu.getRowAtLevel(0) == MENU_MONITOR && menu.getCurrentRow() == MENU_MONITOR_LOG) {
    currentLog++;
    if (currentLog >= getLogs().size()) {
      currentLog = getLogs().size() - 1;
    }
    printLog();
  } else {
    scrollDown();
  }
}  

void onUpInScreen(int menuLevel) {
  if (menuLevel == 1 && menu.getRowAtLevel(0) == 2) {
    motorTestUp(menu.getCurrentRow());
  } else if (menuLevel == 1 && menu.getRowAtLevel(0) == MENU_MONITOR && menu.getCurrentRow() == MENU_MONITOR_LOG) {
    currentLog--;
    if (currentLog < 0) {
      currentLog = 0;
    }
    printLog();
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
      } else if (menu.getCurrentLevel() == 1 && menu.getRowAtLevel(0) == MENU_0_MOTORS) {
        // at motors screen
        disableMotor(menu.getCurrentRow());
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

void printLog() {
  std::deque<std::string>& logs = getLogs();
  int totalLogs = logs.size();
  getLcd().clear();
  getLcd().setCursor(0, 0);
  getLcd().print("Log ");
  getLcd().print(currentLog + 1);
  getLcd().print("/");
  getLcd().print(totalLogs);
  getLcd().setCursor(0, 1);
  // logs are inverted in deque, so print them in reverse order
  int realIndex = totalLogs - currentLog - 1;
  // print first LCD_COLS chars
  getLcd().print(logs[realIndex].substr(0, LCD_COLS).c_str());
  getLcd().setCursor(0, 2);
  // print second LCD_COLS chars if available
  if (logs[realIndex].length() > LCD_COLS) {
    getLcd().print(logs[realIndex].substr(LCD_COLS, LCD_COLS).c_str());
  }
  getLcd().setCursor(0, 3);
  // print third LCD_COLS chars if available
  if (logs[realIndex].length() > LCD_COLS * 2) {
    getLcd().print(logs[realIndex].substr(LCD_COLS * 2, LCD_COLS).c_str());
  }
}

void startPrintLog() {
  currentLog = getLogs().size() - 1;
  printLog();
}

void refreshUi() {
  if (!menu.isVisible()) {
    // at some screen, not menu
    if (menu.getCurrentLevel() == 0 && menu.getCurrentRow() == 0) {
      // at about screen
      printAbout();
    } else if (menu.getCurrentLevel() == 2 && menu.getRowAtLevel(0) == MENU_MONITOR && menu.getRowAtLevel(1) == MENU_MONITOR_AXIS) {
      // at axis monitor screen
      printAxisMonitor(menu.getCurrentRow());
    } else if (menu.getCurrentLevel() == 1 && menu.getRowAtLevel(0) == MENU_MONITOR && menu.getCurrentRow() == MENU_MONITOR_BUTTONS) {
      // at axis monitor screen
      printButtonsMonitor();
    } else if (menu.getCurrentLevel() == 1 && menu.getRowAtLevel(0) == MENU_MONITOR && menu.getCurrentRow() == MENU_MONITOR_ALL) {
      printAxisAndButtonsMonitor();
    } else if (menu.getCurrentLevel() == 2 && menu.getRowAtLevel(0) == MENU_0_SETTINGS && menu.getRowAtLevel(1) == MENU_1_CALIBRATE_AXIS) {
      // at axis calibration screen
      printAxisCalibration();
    }
  }
}