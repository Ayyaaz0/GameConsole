#include "Game_1.h"
#include "Buzzer.h"
#include "InputHandler.h"
#include "LCD.h"
#include "PWM.h"
#include "stm32l4xx_hal.h"
#include <stdbool.h>
#include <stdio.h>

// Temporary include
#include "Menu.h"

extern ST7789V2_cfg_t cfg0;
extern PWM_cfg_t pwm_cfg;
extern Buzzer_cfg_t buzzer_cfg;

/*
 * Game 1 state
 */
static uint32_t animation_counter = 0;
static int16_t moving_x = 0;
static int8_t move_direction = 1;
static bool game1_exit_requested = false;

/*
 * Frame timing target for Game 1
 */
#define GAME1_FRAME_TIME_MS 30

/*
 * Internal game functions
 */
static void game1_init(void) {
  animation_counter = 0;
  moving_x = 0;
  move_direction = 1;
  game1_exit_requested = false;

  buzzer_tone(&buzzer_cfg, 1000, 30);
  HAL_Delay(50);
  buzzer_off(&buzzer_cfg);
}

static void game1_update(void) {
  Input_Read();

  if (current_input.btn3_pressed) {
    PWM_SetDuty(&pwm_cfg, 50);
    game1_exit_requested = true;
    return;
  }

  animation_counter++;

  moving_x += move_direction * 3;
  if (moving_x >= 200 || moving_x <= 0) {
    move_direction *= -1;
  }

  uint8_t brightness = (moving_x * 100) / 200;
  PWM_SetDuty(&pwm_cfg, brightness);
}

static void game1_render(void) {
  char counter[32];
  char pwm_str[32];
  uint8_t brightness = (moving_x * 100) / 200;

  LCD_Fill_Buffer(0);

  LCD_printString("GAME 1", 60, 10, 1, 3);
  LCD_printString("[*]", 20 + moving_x, 100, 1, 3);

  sprintf(counter, "Frame: %lu", animation_counter);
  LCD_printString(counter, 50, 150, 1, 2);

  LCD_printString("LED: PWM Demo", 30, 180, 1, 1);
  sprintf(pwm_str, "Brightness: %d%%", brightness);
  LCD_printString(pwm_str, 30, 195, 1, 1);

  LCD_printString("Press BT3 to", 40, 220, 1, 1);
  LCD_printString("Return to Menu", 40, 235, 1, 1);

  LCD_Refresh(&cfg0);
}

static void game1_exit(void) { PWM_SetDuty(&pwm_cfg, 50); }

bool Game1_ShouldExit(void) {
    return game1_exit_requested;
}

const GameApi game1_api = {
    .name = "Game 1",
    .init = game1_init,
    .update = game1_update,
    .render = game1_render,
    .exit = game1_exit
};



MenuState Game1_Run(void)
{
    game1_api.init();

    while (!Game1_ShouldExit())
    {
        game1_api.update();
        game1_api.render();
    }

    game1_api.exit();

    return MENU_STATE_HOME;
}