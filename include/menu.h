#pragma once

#include <LiquidCrystal_I2C.h>

class MenuItem {
    public:
        MenuItem(const char* name, void (*action)());
        MenuItem(const char* name, MenuItem items[], int itemCount);
        const char* getName();
        void (*getAction())();
    private:
        const char* name;
        void (*action)();
        MenuItem* items;
        int itemCount;
};

class Menu {
    public:
        Menu(LiquidCrystal_I2C &lcd);
        void addItems(MenuItem items[], int itemCount);
        void show();
        void hide();
        void up();
        void down();
        void enter();
        bool isVisible();
        int getCurrentLevel();
        int getCurrentRow();
    private:
        int rootItemCount;
        MenuItem* rootItems;
        bool visible;
        LiquidCrystal_I2C &lcd;
        int currentLevel;
        int rows, cols;
        int currentRow;
        int firstRowIndex;
        void printMenu();
};