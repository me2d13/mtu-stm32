#include "lcd.h"
#include <LiquidCrystal_I2C.h>
#include "SerialUSB.h"
#include "usb_input.h"
#include "state.h"
#include "inputs.h"

#define BUFFER_ROWS 20

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
}

void printAxisMonitor(int axisIndex) {
  lcd.setCursor(0, 0);
  lcd.print("SpdBrake (");
  lcd.print(axisIndex);
  lcd.print("): ");
  lcd.print(getAnalogInputValue(axisIndex));
  lcd.print("    ");
  lcd.setCursor(0, 1);
  lcd.print("Thr1 (");
  lcd.print(axisIndex + 1);
  lcd.print("): ");
  lcd.print(getAnalogInputValue(axisIndex + 1));
  lcd.print("    ");
  lcd.setCursor(0, 2);
  lcd.print("Axis ");
  lcd.print(axisIndex + 2);
  lcd.print(": ");
  lcd.print(getAnalogInputValue(axisIndex + 2));
  lcd.print("    ");
  lcd.setCursor(0, 3);
  lcd.print("Axis ");
  lcd.print(axisIndex + 3);
  lcd.print(": ");
  lcd.print(getAnalogInputValue(axisIndex + 3));
  lcd.print("    ");
}

void clearScreenBuffer() {
  for (int i = 0; i < BUFFER_ROWS; i++) {
    for (int j = 0; j < LCD_COLS; j++) {
      screenBuffer[i][j] = ' ';
    }
    screenBuffer[i][LCD_COLS] = '\0';
  }
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
  lcd.print(screenBuffer[0]);
  lcd.setCursor(0, 1);
  lcd.print(screenBuffer[1]);
  lcd.setCursor(0, 2);
  lcd.print(screenBuffer[2]);
  lcd.setCursor(0, 3);
  lcd.print(screenBuffer[3]);
}