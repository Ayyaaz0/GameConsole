#include "Game_3.h"

#include "InputHandler.h"
#include "LCD.h"
#include "game3_enemy/game3_enemy.h"
#include "game3_input.h"
#include "game3_enemy.h"
#include "game3_player.h"
#include "game3_render.h"
#include "game3_ui/game3_ui.h"
#include "game3_world.h"
#include "game3_ui.h"
#include "stm32l4xx_hal.h"

#include <stdbool.h>
#include <stdio.h>

extern ST7789V2_cfg_t cfg0;

static bool game3_shutdown_requested = false;
static Game3_Player player; 
static Game3_Enemy enemy; 
static Game3_Hud hud; 

static void game3_init(void) {
  game3_shutdown_requested = false;
  
  Game3_World_Init();
  Game3_Player_Init(&player);
  Game3_Enemy_Init(&enemy);

  hud.max_health = 3; 
  hud.health = 3; 
  hud.max_armour = 3; 
  hud.armour = 3; 
  hud.start_time_ms = HAL_GetTick(); 

  LCD_Fill_Buffer(0);
  LCD_Refresh(&cfg0);
}

static void game3_update(void) {
  Game3_Input input = {0}; 

  Game3_Input_Read(&input);
  Game3_Player_Update(&player, input.dx, input.jump_pressed, input.dash_pressed, input.dash_dx);
  Game3_Enemy_Update(&enemy, &player);
}

static void game3_render(void) {
  LCD_Fill_Buffer(0);

  Game3_Render_Draw_World();
  Game3_Render_Draw_Player(&player);
  Game3_Render_Draw_Enemy(&enemy);
  Game3_UI_Draw(&hud);

  LCD_Refresh(&cfg0);
}

static void game3_shutdown(void) { 
  LCD_Fill_Buffer(0);
  LCD_Refresh(&cfg0);
 }

bool Game3_ShouldExit(void) { return game3_shutdown_requested; }

const GameApi game3_api = {
    .name = "Game 3",
    .init = game3_init,
    .update = game3_update,
    .render = game3_render,
    .shutdown = game3_shutdown,
    .should_exit = Game3_ShouldExit,
};