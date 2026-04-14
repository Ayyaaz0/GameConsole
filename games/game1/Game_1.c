#include "Game_1.h"

#include "InputHandler.h"
#include "Joystick.h"
#include "LCD.h"
#include "game1_camera/game1_camera.h"
#include "game1_player/game1_player.h"
#include "game1_render/game1_render.h"
#include "game1_world/game1_world.h"

#include <stdbool.h>
#include <stdint.h>

extern ST7789V2_cfg_t cfg0;
extern Joystick_cfg_t joystick_cfg;
extern Joystick_t joystick_data;

static Game1_Player player;
static Game1_Camera camera;
static bool game1_shutdown_requested = false;

static void game1_get_player_input(int16_t *dx, uint8_t *jump_pressed) {
  *dx = 0;
  *jump_pressed = current_input.btn2_pressed;

  Joystick_Read(&joystick_cfg, &joystick_data);

  switch (joystick_data.direction) {
  case W:
  case NW:
  case SW:
    *dx = -1;
    break;

  case E:
  case NE:
  case SE:
    *dx = 1;
    break;

  case N:
  case S:
  case CENTRE:
  default:
    *dx = 0;
    break;
  }
}

static void game1_handle_room_transition(void) {
  uint8_t current_room = Game1_World_GetCurrentRoom();

  // Move from room 0 to room 1 at right edge
  if (current_room == 0 && (player.x + player.width) >= GAME1_WORLD_WIDTH_PX) {
    Game1_World_SetCurrentRoom(1);

    // Place player near left side of new room
    player.x = 8;
    if (player.y < 0) {
      player.y = 0;
    }

    Game1_Camera_Update(&camera, player.x + (player.width / 2),
                        player.y + (player.height / 2), GAME1_WORLD_WIDTH_PX,
                        GAME1_WORLD_HEIGHT_PX);
  }

  // Move from room 1 to room 0 at left edge
  else if (current_room == 1 && player.x <= 0) {
    Game1_World_SetCurrentRoom(0);

    // Place player near right side of previous room
    player.x = GAME1_WORLD_WIDTH_PX - player.width - 8;
    if (player.y < 0) {
      player.y = 0;
    }

    Game1_Camera_Update(&camera, player.x + (player.width / 2),
                        player.y + (player.height / 2), GAME1_WORLD_WIDTH_PX,
                        GAME1_WORLD_HEIGHT_PX);
  }
}

static void game1_init(void) {
  game1_shutdown_requested = false;

  Game1_World_Init();
  Game1_Player_Init(&player);
  Game1_Camera_Init(&camera, 240, 240);

  LCD_Fill_Buffer(0);
  LCD_Refresh(&cfg0);
}

static void game1_update(void) {
  int16_t dx = 0;
  uint8_t jump_pressed = 0;

  Input_Read();

  if (current_input.btn3_pressed) {
    game1_shutdown_requested = true;
    return;
  }

  game1_get_player_input(&dx, &jump_pressed);
  Game1_Player_Update(&player, dx, jump_pressed);

  game1_handle_room_transition();
  
  Game1_Camera_Update(&camera, player.x + (player.width / 2),
                      player.y + (player.height / 2), GAME1_WORLD_WIDTH_PX,
                      GAME1_WORLD_HEIGHT_PX);
}

static void game1_render(void) {
  LCD_Fill_Buffer(0);

  Game1_Render_DrawWorld(&camera);
  Game1_Render_DrawPlayer(&player, &camera);

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