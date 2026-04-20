#include "Game_2.h"

#include "Buzzer.h"
#include "InputHandler.h"
#include "LCD.h"
#include "stm32l4xx_hal.h"

#include <stdbool.h>
#include <stdio.h>

extern ST7789V2_cfg_t cfg0;
extern Buzzer_cfg_t buzzer_cfg;

static uint32_t animation_counter = 0;
static int16_t moving_y = 0;
static int8_t move_direction = 1;
static bool game2_shutdown_requested = false;

static void game2_init(void) {
  animation_counter = 0;
  moving_y = 0;
  move_direction = 1;
  game2_shutdown_requested = false;

  buzzer_tone(&buzzer_cfg, 1200, 30);
  HAL_Delay(50);
  buzzer_off(&buzzer_cfg);
}

static void game2_update(void) {
  animation_counter++;

  moving_y += move_direction * 2;
  if (moving_y >= 150 || moving_y <= 0) {
    move_direction *= -1;
  }
}

static void game2_render(void) {
  char counter[32];

  LCD_Fill_Buffer(0);

  LCD_printString("GAME 2", 60, 10, 1, 3);
  LCD_printString("[+]", 100, 60 + moving_y, 1, 3);

  sprintf(counter, "Frame: %lu", animation_counter);
  LCD_printString(counter, 50, 150, 1, 2);

  LCD_printString("Slower Demo", 20, 180, 1, 1);
  LCD_printString("20 FPS", 20, 195, 1, 1);

  LCD_printString("Press BT3 to", 40, 220, 1, 1);
  LCD_printString("Return to Menu", 40, 235, 1, 1);

  LCD_Refresh(&cfg0);
}

static void game2_shutdown(void) {}

bool Game2_ShouldExit(void) { return game2_shutdown_requested; }

const GameApi game2_api = {
    .name = "Game 2",
    .init = game2_init,
    .update = game2_update,
    .render = game2_render,
    .shutdown = game2_shutdown,
    .should_exit = Game2_ShouldExit,
};