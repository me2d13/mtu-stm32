#pragma once

#include <LiquidCrystal_I2C.h>

#define MAX_PATH_LENGTH 5

class MenuItem {
    public:
        MenuItem(const char* name, void (*action)());
        MenuItem(const char* name, MenuItem items[], int itemsCount);
        MenuItem(const char* name, MenuItem items[], int itemsCount, void (*action)());
        const char* getName();
        MenuItem* getItems();
        int getItemsCount();
        void (*getAction())();
    private:
        const char* name;
        void (*action)();
        MenuItem* items;
        int itemsCount;
};

class Menu {
    public:
        Menu(LiquidCrystal_I2C &lcd);
        void addItems(MenuItem items[], int itemCsount);
        void show();
        void hide();
        void up();
        void down();
        void enter();
        bool isVisible();
        int getCurrentLevel();
        int getCurrentRow();
    private:
        int rootItemsCount;
        MenuItem* rootItems;
        bool visible;
        LiquidCrystal_I2C &lcd;
        int currentLevel;
        int rows, cols;
        int currentRow;
        int firstRowIndex;
        MenuItem* pathItems[MAX_PATH_LENGTH];
        int pathItemsCount[MAX_PATH_LENGTH];
        int pathCurrentIndexes[MAX_PATH_LENGTH];
        MenuItem* currentItems;
        int currentItemsCount;
        void printMenu();
};