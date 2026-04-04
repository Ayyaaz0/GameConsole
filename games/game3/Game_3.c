#include "Game_3.h"

#include "InputHandler.h"
#include "LCD.h"
#include "Menu.h" // temporary bridge
#include "stm32l4xx_hal.h"

#include <stdbool.h>
#include <stdio.h>

extern ST7789V2_cfg_t cfg0;

// Game state variables
static bool game3_exit_requested = false;
static uint32_t frame_counter = 0;

// Frame timing target for Game 3 (30ms = ~33 FPS)
#define GAME3_FRAME_TIME_MS 30

// Internal game functions
static void game3_init(void) {
  game3_exit_requested = false;
  frame_counter = 0;
}

static void game3_update(void) {
  Input_Read();

  if (current_input.btn3_pressed) {
    game3_exit_requested = true;
    return;
  }

  frame_counter++;
}

static void game3_render(void) {
  char counter[32];

  LCD_Fill_Buffer(0);

  LCD_printString("GAME 3", 60, 10, 1, 3);
  LCD_printString("Placeholder Demo", 25, 80, 1, 2);

  sprintf(counter, "Frame: %lu", frame_counter);
  LCD_printString(counter, 50, 130, 1, 2);

  LCD_printString("Refactor in progress", 20, 180, 1, 1);
  LCD_printString("Press BT3 to", 40, 220, 1, 1);
  LCD_printString("Return to Menu", 40, 235, 1, 1);

  LCD_Refresh(&cfg0);
}

static void game3_exit(void) {
  // Any cleanup if necessary (currently none)
}

bool Game3_ShouldExit(void)
{
    return game3_exit_requested;
}

const GameApi game3_api = {
    .name = "Game 3",
    .init = game3_init,
    .update = game3_update,
    .render = game3_render,
    .exit = game3_exit,
};

MenuState Game3_Run(void) {
  game3_api.init();

  while (!Game3_ShouldExit()) {
    uint32_t frame_start = HAL_GetTick();

    game3_api.update();
    game3_api.render();

    uint32_t frame_time = HAL_GetTick() - frame_start;
    if (frame_time < GAME3_FRAME_TIME_MS) {
      HAL_Delay(GAME3_FRAME_TIME_MS - frame_time);
    }
  }

  game3_api.exit();

  return MENU_STATE_HOME;
}