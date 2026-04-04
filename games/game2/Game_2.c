#include "Game_2.h"
#include "Buzzer.h"
#include "InputHandler.h"
#include "LCD.h"
#include "stm32l4xx_hal.h"
#include <stdbool.h>
#include <stdio.h>

// Temporary bridge
#include "Menu.h"

extern ST7789V2_cfg_t cfg0;
extern Buzzer_cfg_t buzzer_cfg;

/*
 * Game 2 state
 */
static uint32_t animation_counter = 0;
static int16_t moving_y = 0;
static int8_t move_direction = 1;
static bool game2_exit_requested = false;

/*
 * Frame timing target for Game 2
 */
#define GAME2_FRAME_TIME_MS 50

/*
 * Internal game functions
 */
static void game2_init(void) {
  animation_counter = 0;
  moving_y = 0;
  move_direction = 1;
  game2_exit_requested = false;

  buzzer_tone(&buzzer_cfg, 1200, 30);
  HAL_Delay(50);
  buzzer_off(&buzzer_cfg);
}

static void game2_update(void) {
  Input_Read();

  if (current_input.btn3_pressed) {
    game2_exit_requested = true;
    return;
  }

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

static void game2_exit(void) {
  // No special cleanup needed yet
}

bool Game2_ShouldExit(void) { return game2_exit_requested; }

const GameApi game2_api = {
    .name = "Game 2",
    .init = game2_init,
    .update = game2_update,
    .render = game2_render,
    .exit = game2_exit
};

MenuState Game2_Run(void) {
    game2_api.init();

    while (!Game2_ShouldExit()) {
        uint32_t frame_start = HAL_GetTick();

        game2_api.update();
        game2_api.render();

        uint32_t frame_time = HAL_GetTick() - frame_start;
        if (frame_time < GAME2_FRAME_TIME_MS) {
            HAL_Delay(GAME2_FRAME_TIME_MS - frame_time);
        }
    }

    game2_api.exit();
    return MENU_STATE_HOME;
}