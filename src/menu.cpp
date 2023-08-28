#include "menu.h"
#include <LiquidCrystal_I2C.h>

MenuItem::MenuItem(const char* name, void (*action)()) {
  this->name = name;
  this->action = action;
  this->items = NULL;
  this->itemCount = 0;
}

MenuItem::MenuItem(const char* name, MenuItem items[], int itemCount) {
  this->name = name;
  this->items = items;
  this->itemCount = itemCount;
  this->action = NULL;
}

const char* MenuItem::getName() {
  return this->name;
}

void (*MenuItem::getAction())() {
  return this->action;
}

void Menu::addItems(MenuItem items[], int itemCount) {
  this->rootItems = items;
  this->rootItemCount = itemCount;
}

Menu::Menu(LiquidCrystal_I2C& lcd) : 
  rootItems(NULL), 
  lcd(lcd), 
  rootItemCount(0), 
  visible(false),
  currentLevel(0),
  rows(4),
  cols(20),
  currentRow(0),
  firstRowIndex(0) {
  }

void Menu::show() {
  this->visible = true;
  this->printMenu();
}

void Menu::hide() {
  this->visible = false;
  this->lcd.clear();
}

bool Menu::isVisible() {
  return this->visible;
}

int Menu::getCurrentLevel() {
  return this->currentLevel;
}

int Menu::getCurrentRow() {
  return this->currentRow;
}

void Menu::up() {
  if (this->currentRow > 0) {
    this->currentRow -= 1;
  }
  this->printMenu();
}

void Menu::down() {
  if (this->currentRow < this->rootItemCount - 1) {
    this->currentRow += 1;
  }
  this->printMenu();
}

void Menu::enter() {
  MenuItem item = this->rootItems[this->currentRow];
  if (item.getAction() != NULL) {
    this->visible = false;
    this->lcd.clear();
    item.getAction()();
  }
}

void Menu::printMenu() {
  // this->lcd.clear(); only needed on scroll
  this->lcd.setCursor(0, 0);
  int linesToPrint = this->rootItemCount > this->rows ? this->rows : this->rootItemCount;
  for (int i = 0; i < linesToPrint; i++) {
    this->lcd.setCursor(0, i);
    int itemIndex = i + this->firstRowIndex;
    this->lcd.print((itemIndex == this->currentRow) ? ">" : " ");
    this->lcd.print(this->rootItems[itemIndex].getName());
    // print submenu mark for nested menu
    if (this->rootItems[itemIndex].getAction() == NULL) {
      this->lcd.print(" ...");
    }
  }
}