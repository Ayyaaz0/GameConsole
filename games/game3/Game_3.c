#include "Game_3.h"

#include "InputHandler.h"
#include "LCD.h"
#include "stm32l4xx_hal.h"

#include <stdbool.h>
#include <stdio.h>

extern ST7789V2_cfg_t cfg0;

static bool game3_shutdown_requested = false;
static uint32_t frame_counter = 0;

static void game3_init(void) {
  game3_shutdown_requested = false;
  frame_counter = 0;
}

static void game3_update(void) {
  Input_Read();

  if (current_input.btn3_pressed) {
    game3_shutdown_requested = true;
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

static void game3_shutdown(void) { /* No cleanup needed yet */ }

bool Game3_ShouldExit(void) { return game3_shutdown_requested; }

const GameApi game3_api = {
    .name = "Game 3",
    .init = game3_init,
    .update = game3_update,
    .render = game3_render,
    .shutdown = game3_shutdown,
    .should_exit = Game3_ShouldExit,
};