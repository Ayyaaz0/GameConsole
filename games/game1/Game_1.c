#include "Game_1.h"

#include "Buzzer.h"
#include "InputHandler.h"
#include "LCD.h"
#include "PWM.h"
#include "stm32l4xx_hal.h"

#include <stdbool.h>
#include <stdio.h>

extern ST7789V2_cfg_t cfg0;
extern PWM_cfg_t pwm_cfg;
extern Buzzer_cfg_t buzzer_cfg;

static uint32_t animation_counter = 0;
static int16_t moving_x = 0;
static int8_t move_direction = 1;
static bool game1_shutdown_requested = false;

static void game1_init(void) {
    animation_counter = 0;
    moving_x = 0;
    move_direction = 1;
    game1_shutdown_requested = false; 

    buzzer_tone(&buzzer_cfg, 1000, 30); // Play a tone to indicate game start
    HAL_Delay(50);
    buzzer_off(&buzzer_cfg);
}

static void game1_update(void) {
    Input_Read();
    if (current_input.btn3_pressed) {
        PWM_SetDuty(&pwm_cfg,50); 
        game1_shutdown_requested = true;
        return;
    }
    animation_counter++;

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

static void game1_shutdown(void) { PWM_SetDuty(&pwm_cfg, 50); }

bool Game1_ShouldExit(void) { return game1_shutdown_requested; }

const GameApi game1_api = {
    .name = "Game 1",
    .init = game1_init,
    .update = game1_update,
    .render = game1_render,
    .shutdown = game1_shutdown,
    .should_exit = Game1_ShouldExit,
};