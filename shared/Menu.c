#include "Menu.h"

#include "InputHandler.h"
#include "Joystick.h"
#include "LCD.h"
#include "game_registry.h"
#include "stm32l4xx_hal.h"

#include <stdint.h>

extern ST7789V2_cfg_t cfg0;
extern Joystick_cfg_t joystick_cfg;
extern Joystick_t joystick_data;

#define MENU_FRAME_TIME_MS 30

// Render the home menu using the registered game list
static void render_home_menu(MenuSystem *menu) {
  LCD_Fill_Buffer(0);
  LCD_printString("MAIN MENU", 50, 10, 1, 3);

  for (int i = 0; i < game_count; i++) {
    uint16_t y_pos = 70 + (i * 40);
    uint8_t text_size = 2;

    if (i == menu->selected_option) {
      LCD_printString(">", 40, y_pos, 1, text_size);
    }

    LCD_printString((char *)game_list[i]->name, 70, y_pos, 1, text_size);
  }

  LCD_printString("Press BT3", 50, 240, 1, 1);
  LCD_Refresh(&cfg0);
}

// Initialize menu system: set selected option to the first game
void Menu_Init(MenuSystem *menu) { menu->selected_option = 0; }

// Run the menu loop until the user selects a game, then return its index
int Menu_Run(MenuSystem *menu) {
  static Direction last_direction = CENTRE;
  int selected_game_index = 0;

  while (1) {
    uint32_t frame_start = HAL_GetTick();

    Input_Read();
    Joystick_Read(&joystick_cfg, &joystick_data);

    Direction current_direction = joystick_data.direction;

    if (current_direction == S && last_direction != S) {
      menu->selected_option++;
      if (menu->selected_option >= game_count) {
        menu->selected_option = 0;
      }
    } else if (current_direction == N && last_direction != N) {
      if (menu->selected_option == 0) {
        menu->selected_option = game_count - 1;
      } else {
        menu->selected_option--;
      }
    }

    last_direction = current_direction;

    if (current_input.btn3_pressed) {
      selected_game_index = menu->selected_option;
      break;
    }

    render_home_menu(menu);

    uint32_t frame_time = HAL_GetTick() - frame_start;
    if (frame_time < MENU_FRAME_TIME_MS) {
      HAL_Delay(MENU_FRAME_TIME_MS - frame_time);
    }
  }

  return selected_game_index;
}