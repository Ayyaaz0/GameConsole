#include "Game_1.h"

#include "InputHandler.h"
#include "LCD.h"
#include "game1_camera/game1_camera.h"
#include "game1_input/game1_input.h"
#include "game1_player/game1_player.h"
#include "game1_render/game1_render.h"
#include "game1_world/game1_world.h"

#include <stdbool.h>
#include <stdint.h>

extern ST7789V2_cfg_t cfg0;

static Game1_Player player;
static Game1_Camera camera;
static bool game1_shutdown_requested = false;

static void game1_init(void) {
  game1_shutdown_requested = false;

  LCD_Set_Palette(PALETTE_CHROMA);

  Game1_World_Init();
  Game1_Player_Init(&player);
  Game1_Camera_Init(&camera, 240, 240);
  Game1_World_SpawnAtStart(&player);

  LCD_Fill_Buffer(0);
  LCD_Refresh(&cfg0);
}

static void game1_update(void) {
  Game1_Input input = {0};

  Game1_Input_Read(&input);
  Game1_Player_Update(&player, input.dx, input.jump_pressed);

  Game1_World_HandleTransition(&player, input.interact_pressed);

  Game1_Camera_Update(&camera, player.x + (player.width / 2),
                      player.y + (player.height / 2), GAME1_WORLD_WIDTH_PX,
                      GAME1_WORLD_HEIGHT_PX);
  
  if (Game1_World_PlayerTouchesKey(&player)) {
    uint16_t tile_x = (player.x + (player.width / 2)) / GAME1_TILE_SIZE;
    uint16_t tile_y = (player.y + (player.height / 2)) / GAME1_TILE_SIZE;
    // remove from world.
    player.has_key = 1;
    Game1_World_SetTile(tile_x, tile_y, TILE_EMPTY);
  }
}

static void game1_render(void) {
  LCD_Fill_Buffer(0);

  Game1_Render_DrawWorld(&camera);
  Game1_Render_DrawPlayer(&player, &camera);

  LCD_Refresh(&cfg0);
}

static void game1_shutdown(void) {
  LCD_Fill_Buffer(0);
  LCD_Set_Palette(PALETTE_DEFAULT);
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