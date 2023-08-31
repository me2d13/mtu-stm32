#include "menu.h"
#include <LiquidCrystal_I2C.h>

MenuItem::MenuItem(const char* name, void (*action)()) : MenuItem(name, NULL, 0, action) {}

MenuItem::MenuItem(const char* name, MenuItem items[], int itemsCount) : MenuItem(name, items, itemsCount, NULL) {}

MenuItem::MenuItem(const char* name, MenuItem items[], int itemsCount, void (*action)()) {
  this->name = name;
  this->items = items;
  this->itemsCount = itemsCount;
  this->action = action;
}

const char* MenuItem::getName() {
  return this->name;
}

void (*MenuItem::getAction())() {
  return this->action;
}

MenuItem* MenuItem::getItems() {
  return this->items;
}

int MenuItem::getItemsCount() {
  return this->itemsCount;
}

void Menu::addItems(MenuItem items[], int itemsCount) {
  this->rootItems = items;
  this->rootItemsCount = itemsCount;
  this->currentItems = this->rootItems;
  this->currentItemsCount = this->rootItemsCount;
}

Menu::Menu(LiquidCrystal_I2C& lcd) : 
  rootItems(NULL), 
  lcd(lcd), 
  rootItemsCount(0), 
  visible(false),
  currentLevel(0),
  rows(4),
  cols(20),
  currentRow(0),
  firstRowIndex(0),
  currentItems(NULL),
  currentItemsCount(0) {
  }

void Menu::show() {
  this->visible = true;
  this->lcd.clear();
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
  if (this->currentRow < this->currentItemsCount - 1) {
    this->currentRow += 1;
  }
  this->printMenu();
}

void Menu::enter() {
  // first handle back item
  if (this->currentLevel > 0 && this->currentRow == this->currentItemsCount - 1) {
    this->currentLevel -= 1;
    this->currentRow = this->pathCurrentIndexes[this->currentLevel];
    this->firstRowIndex = 0;
    this->currentItems = this->pathItems[this->currentLevel];
    this->currentItemsCount = this->pathItemsCount[this->currentLevel];
    this->lcd.clear();
    this->printMenu();
    return;
  }
  // handle regular items
  MenuItem item = this->currentItems[this->currentRow];
  if (item.getAction() != NULL) {
    this->visible = false;
    this->lcd.clear();
    item.getAction()();
  } 
  if (item.getItems() != NULL) {
    // store current path for return
    this->pathItems[this->currentLevel] = this->currentItems;
    this->pathCurrentIndexes[this->currentLevel] = this->currentRow;
    this->pathItemsCount[this->currentLevel] = this->currentItemsCount;
    this->currentLevel += 1;
    this->currentRow = 0;
    this->firstRowIndex = 0;
    this->currentItems = item.getItems();
    this->currentItemsCount = item.getItemsCount() + 1; // for back item
    this->lcd.clear();
    this->printMenu();
  }
}

void Menu::printMenu() {
  this->lcd.setCursor(0, 0);
  int linesToPrint = this->currentItemsCount > this->rows ? this->rows : this->currentItemsCount;
  // check if we need to scroll
  if (this->currentRow >= this->firstRowIndex + linesToPrint) {
    // scroll down
    this->firstRowIndex = this->currentRow - linesToPrint + 1;
    this->lcd.clear();
  } else if (this->currentRow < this->firstRowIndex) {
    // scroll up
    this->firstRowIndex = this->currentRow;
    this->lcd.clear();
  }
  for (int i = 0; i < linesToPrint; i++) {
    this->lcd.setCursor(0, i);
    int itemIndex = i + this->firstRowIndex;
    this->lcd.print((itemIndex == this->currentRow) ? ">" : " ");
    if (itemIndex == this->currentItemsCount - 1 && this->currentLevel > 0) {
      // last item, print back
      this->lcd.print("Back");
    } else {
      // print item name
      this->lcd.print(this->currentItems[itemIndex].getName());
      // print submenu mark for nested menu
      if (this->currentItems[itemIndex].getItems() != NULL) {
        this->lcd.print(" ...");
      }
    }
  }
  if (this->firstRowIndex > 0) {
    this->lcd.setCursor(19, 0);
    this->lcd.write((byte)1);
  }
  if (this->firstRowIndex + linesToPrint < this->currentItemsCount) {
    this->lcd.setCursor(19, linesToPrint - 1);
    this->lcd.write((byte)2);
  }
}