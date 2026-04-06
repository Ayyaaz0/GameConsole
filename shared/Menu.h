#ifndef MENU_H
#define MENU_H

#include <stdint.h>

typedef struct {
    uint8_t selected_option; // Index of the currently selected menu option
} MenuSystem;

void Menu_Init(MenuSystem *menu); // Initialize menu system
int Menu_Run(MenuSystem *menu); // Returns the selected game state

#endif 
