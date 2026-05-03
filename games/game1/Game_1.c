#include "Game_1.h"

#include "InputHandler.h"
#include "LCD.h"
#include "game1_camera/game1_camera.h"
#include "game1_entities/game1_coin.h"
#include "game1_entities/game1_entities.h"
#include "game1_input/game1_input.h"
#include "game1_player/game1_player.h"
#include "game1_render/game1_render.h"
#include "game1_world/game1_world.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

//test
#include "stm32l4xx_hal.h"

#define GAME1_WIN_ROOM 1
#define GAME1_WIN_X 440

extern ST7789V2_cfg_t cfg0;

static Game1_Player player;
static Game1_Camera camera;

static bool game1_shutdown_requested = false;
static uint8_t game1_won = 0;

static void Game1_CheckWinCondition(const Game1_Player *player) {
  if (Game1_World_GetCurrentRoom() == GAME1_WIN_ROOM &&
      player->x > GAME1_WIN_X) {
    game1_won = 1;
  }
}

static void Game1_RenderWinScreen(void) {
  char score_text[20];

  snprintf(score_text, sizeof(score_text), "COINS: %u", Game1_Coin_GetScore());

  LCD_Fill_Buffer(0);
  LCD_printString("YOU WIN!", 70, 90, 5, 2);
  LCD_printString(score_text, 70, 115, 5, 1);
  LCD_Refresh(&cfg0);
}

static void game1_init(void) {
  game1_shutdown_requested = false;
  game1_won = 0;

  LCD_Set_Palette(PALETTE_CHROMA);

  Game1_World_Init();
  Game1_Entities_Init();

  Game1_Coin_ResetScore();
  Game1_Player_Init(&player);
  Game1_Entities_SpawnPlayer(&player);

  Game1_Camera_Init(&camera, 240, 240);

  LCD_Fill_Buffer(0);
  LCD_Refresh(&cfg0);
}

static void game1_update(void) {
  if (game1_won) {
    return;
  }

  Game1_Input input = {0};
  Game1_Input_Read(&input);

  Game1_Player_Update(&player, input.dx, input.jump_pressed);
  Game1_Entities_Update(&player, input.interact_pressed);

  Game1_CheckWinCondition(&player);
  Game1_Camera_Update(&camera, &player);
}

static void game1_render(void) {
  if (game1_won) {
    Game1_RenderWinScreen();
    return;
  }

  LCD_Fill_Buffer(0);

  Game1_Render_DrawWorld(&camera);
  Game1_Entities_Render(&camera);
  Game1_Render_DrawPlayer(&player, &camera);

  LCD_Refresh(&cfg0);
}

static void game1_shutdown(void) {
  LCD_Fill_Buffer(0);
  LCD_Set_Palette(PALETTE_DEFAULT);
  LCD_Refresh(&cfg0);
}

bool Game1_ShouldExit(void) {
  return game1_shutdown_requested;
}

const GameApi game1_api = {
    .name = "Game 1",
    .init = game1_init,
    .update = game1_update,
    .render = game1_render,
    .shutdown = game1_shutdown,
    .should_exit = Game1_ShouldExit,
};