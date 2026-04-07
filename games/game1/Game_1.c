#include "Game_1.h"

#include "InputHandler.h"
#include "LCD.h"
#include "stm32l4xx_hal.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

extern ST7789V2_cfg_t cfg0;

#define GAME1_BOX_WIDTH 20
#define GAME1_BOX_HEIGHT 20
#define GAME1_BOX_Y 100
#define GAME1_BOX_MIN_X 0
#define GAME1_BOX_MAX_X 200
#define GAME1_BOX_STEP 3

static uint32_t animation_counter = 0;
static int16_t moving_x = 0;
static int16_t prev_x = 0; // NEW: track previous position
static int8_t move_direction = 1;
static bool game1_shutdown_requested = false;

static void game1_init(void) {
    animation_counter = 0;
    moving_x = 0;
    prev_x = 0;
    move_direction = 1;
    game1_shutdown_requested = false; 

    // Draw static UI once
    LCD_Fill_Buffer(0); // Clear screen with black
    LCD_printString("Game 1: Moving Box", 20, 20, 1, 2);

    // Draw initial box
    LCD_Draw_Rect(20 + moving_x, GAME1_BOX_Y, GAME1_BOX_WIDTH, GAME1_BOX_HEIGHT, 3, 1);

    LCD_Refresh(&cfg0);
}

static void game1_update(void) {
    Input_Read();

    if (current_input.btn3_pressed) {
        game1_shutdown_requested = true;
        return;
    }

    animation_counter++;

    // Store previous position before moving
    prev_x = moving_x;

    moving_x += move_direction * GAME1_BOX_STEP;    

    if (moving_x >= GAME1_BOX_MAX_X || moving_x <= GAME1_BOX_MIN_X) {
        move_direction *= -1; // Reverse direction
    }
}

static void game1_render(void)
{
    uint32_t start = HAL_GetTick();
    static uint32_t total_render_time = 0;
    static uint32_t sample_count = 0;

    LCD_Fill_Buffer(0);
    LCD_printString("Game 1: Moving Box", 20, 20, 1, 2);

    LCD_Draw_Rect(20 + moving_x,
                  GAME1_BOX_Y,
                  GAME1_BOX_WIDTH,
                  GAME1_BOX_HEIGHT,
                  1,
                  1);

    LCD_Refresh(&cfg0);

    uint32_t render_time = HAL_GetTick() - start;
    total_render_time += render_time;
    sample_count++;

    if (sample_count == 60) {
        printf("Average render time: %lu ms\n", total_render_time / 60);
        total_render_time = 0;
        sample_count = 0;
    }
}

static void game1_shutdown(void) {
    // Optional: clear screen when exiting
    LCD_Fill_Buffer(0);
    LCD_Refresh(&cfg0);
}

bool Game1_ShouldExit(void) { return game1_shutdown_requested; }

const GameApi game1_api = {
    .name = "Game 1",
    .init = game1_init,
    .update = game1_update,
    .render = game1_render,
    .shutdown = game1_shutdown,
    .should_exit = Game1_ShouldExit,
};