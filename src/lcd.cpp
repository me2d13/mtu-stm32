#include "lcd.h"
#include <LiquidCrystal_I2C.h>
#include "SerialUSB.h"
#include "usb_input.h"

LiquidCrystal_I2C lcd(0x27, 20, 4);

void initLcd() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(3, 0);
  //lcd.print("MTU poc starting...");
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
}