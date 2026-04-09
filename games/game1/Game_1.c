#include "Game_1.h"

#include "InputHandler.h"
#include "LCD.h"
#include "game1_player/game1_player.h"
#include "game1_render/game1_render.h"
#include "game1_world/game1_world.h"

#include <stdbool.h>

extern ST7789V2_cfg_t cfg0;

static Game1_Player player;
static bool game1_shutdown_requested = false;

static void game1_init(void) {
  game1_shutdown_requested = false;

  Game1_World_Init();
  Game1_Player_Init(&player);

  LCD_Fill_Buffer(0);
  LCD_Refresh(&cfg0);
}

static void game1_update(void) {
  Input_Read();
  
  if (current_input.btn3_pressed) {
    game1_shutdown_requested = true;
    return;
  }
  
  Game1_Player_Update(&player);
}

static void game1_render(void) {
  LCD_Fill_Buffer(0);

  Game1_Render_DrawWorld();
  Game1_Render_DrawPlayer(&player);

  LCD_Refresh(&cfg0);
}

static void game1_shutdown(void) {
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