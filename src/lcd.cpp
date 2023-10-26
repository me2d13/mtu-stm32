#include "lcd.h"
#include <LiquidCrystal_I2C.h>
#include "SerialUSB.h"
#include "usb_input.h"
#include "state.h"
#include "inputs.h"
#include "ui.h"

#define BUFFER_ROWS 20

uint8_t bufferTopRow = 0;
bool bufferredScreen = false;

LiquidCrystal_I2C lcd(0x27, LCD_COLS, LCD_ROWS);

char screenBuffer[BUFFER_ROWS][LCD_COLS+1];

char* btnNames[NUMBER_OF_BUTTONS] = {
    (char*) "AT-DIS1",
    (char*) "AT-DIS2",
    (char*) "TOGA1",
    (char*) "TOGA2",
    (char*) "TR-NORM",
    (char*) "TR-AP",
    (char*) "START1",
    (char*) "START2",
    (char*) "AT-CUT",
    (char*) "TR-IND-1",
    (char*) "TR-IND-2",
    (char*) "PARK-BRK"
};

char *axisNames[NUMBER_OF_ANALOG_INPUTS] = {
    (char *) "SPD-BRK",
    (char *) "THR1",
    (char *) "THR2",
    (char *) "FLAPS",
    (char *) "REV1",
    (char *) "REV2",
    (char *) "TRIM"
};

void initLcd() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 1);
  lcd.print("MTU poc starting...");
  byte char2[8] = {0b00000,0b00100,0b00100,0b00100,0b10101,0b01110,0b00100,0b00000};
  byte char1[8] = {0b00000,0b00100,0b01110,0b10101,0b00100,0b00100,0b00100,0b00000};
  lcd.createChar(1, char1);
  lcd.createChar(2, char2);
}

LiquidCrystal_I2C& getLcd() {
  return lcd;
}

void printAbout()
{
  bufferredScreen = false;
  lcd.setCursor(0, 0);
  lcd.print("Uptime: ");
  // print milis as hours, minutes and seconds
  uint32_t milis = millis();
  uint32_t hours = milis / 3600000;
  lcd.print(hours);
  lcd.print(":");
  uint32_t minutes = (milis % 3600000) / 60000;
  if (minutes < 10) {
    lcd.print("0");
  }
  lcd.print(minutes);
  lcd.print(":");
  uint32_t seconds = (milis % 60000) / 1000;
  if (seconds < 10) {
    lcd.print("0");
  }
  lcd.print(seconds);
  lcd.print("  ");
  lcd.setCursor(0, 1);
  lcd.print("USB: ");
  if (USB_Running()) {
    lcd.print("Y");
  }
  else {
    lcd.print("N");
  }
  lcd.print(", serial: ");
  if (usbSerialConnected()) {
    lcd.print("Y");
  }
  else {
    lcd.print("N");
  }
  lcd.setCursor(0, 2);
  lcd.print("I2c multiplex: ");
  lcd.print(getI2cMultiplexerState());
  lcd.setCursor(0, 3);
  lcd.print("Enter event#: ");
  lcd.print(getEnterEventCount());
}

void printAxisMonitor(int axisIndex) {
  bufferredScreen = false;
  lcd.setCursor(0, 0);
  lcd.print(axisNames[axisIndex]);
  lcd.print(axisIndex);
  lcd.print("): ");
  // get axis data
  axisState *axis = getAxis(axisIndex);
  lcd.print(axis->value);
  lcd.print("    ");
  lcd.setCursor(0, 1);
  lcd.print("Min: ");
  lcd.print(axis->minValue);
  lcd.print("    ");
  lcd.setCursor(0, 2);
  lcd.print("Max: ");
  lcd.print(axis->maxValue);
  lcd.print("    ");
}

void printMotors(int index) {
  bufferredScreen = false;
  lcd.setCursor(0, 0);
  lcd.print("Turn to move");
}

void clearScreenBuffer() {
  bufferredScreen = true;
  for (int i = 0; i < BUFFER_ROWS; i++) {
    for (int j = 0; j < LCD_COLS; j++) {
      screenBuffer[i][j] = ' ';
    }
    screenBuffer[i][LCD_COLS] = '\0';
  }
}

void printString(String message) {
  bufferTopRow = 0;
  clearScreenBuffer();
  int row = 0;
  int col = 0;
  // iterate message char by char
  for (int i = 0; i < message.length(); i++) {
    char c = message.charAt(i);
    if (c == '\n') {
      screenBuffer[row][col] = 0;
      row++;
      col = 0;
    }
    else {
      screenBuffer[row][col] = c;
      col++;
      if (col >= LCD_COLS) {
        row++;
        screenBuffer[row][col] = 0;
        col = 0;
      }
    }
    if (row >= BUFFER_ROWS) {
      break;
    }
  }
  if (row < BUFFER_ROWS) {
    screenBuffer[row][col] = 0;
  }
  printBuffer();
}

void printLastUsbMessage() {
  printString(getLastMessage());
}

void printButtonsMonitor() {
  clearScreenBuffer();
  for (int i = 0; i < NUMBER_OF_BUTTONS; i++) {
    buttonState* buttons = getButtons();
    sprintf(screenBuffer[i], "%s: %d, ch# %d", btnNames[i], buttons[i].value, buttons[i].changeCount);
  }
  printBuffer();
}

void printI2C(int index) {
  clearScreenBuffer();
  scanI2C(index, screenBuffer);
  if (index == 0) {
    lcd.init();
  }
  printBuffer();
}

void printBuffer() {
  lcd.setCursor(0, 0);
  lcd.print(screenBuffer[bufferTopRow]);
  lcd.setCursor(0, 1);
  lcd.print(screenBuffer[bufferTopRow + 1]);
  lcd.setCursor(0, 2);
  lcd.print(screenBuffer[bufferTopRow + 2]);
  lcd.setCursor(0, 3);
  lcd.print(screenBuffer[bufferTopRow + 3]);
  // print arrow up if we can scroll up
  if (bufferTopRow > 0) {
    lcd.setCursor(19, 0);
    lcd.write((byte)1);
  }
  // print arrow down if we can scroll down
  if (bufferTopRow < BUFFER_ROWS - LCD_ROWS) {
    lcd.setCursor(19, LCD_ROWS - 1);
    lcd.write((byte)2);
  }
}

void scrollUp() {
  if (bufferredScreen && bufferTopRow > 0) {
    bufferTopRow--;
    lcd.clear();
    printBuffer();
  }
}

void scrollDown() {
  if (bufferredScreen && bufferTopRow < BUFFER_ROWS - LCD_ROWS) {
    bufferTopRow++;
    lcd.clear();
    printBuffer();
  }
}