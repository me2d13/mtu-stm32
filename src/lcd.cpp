#include "lcd.h"
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

void initLcd() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(3, 0);
  lcd.print("MTU poc");
}

void printLcd(uint16_t thr1, uint8_t inChar)
{
  lcd.setCursor(3, 0);
  lcd.print("MTU poc");
  lcd.setCursor(0, 1);
  lcd.print("Thr1: ");
  lcd.print(thr1);
  lcd.setCursor(0, 2);
  lcd.print("USB read: ");
  lcd.print(inChar);
  lcd.print("  ");
}