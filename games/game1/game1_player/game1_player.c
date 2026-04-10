#include "game1_player.h"
#include "game1_world/game1_world.h"
#include "Joystick.h"

extern Joystick_cfg_t joystick_cfg;
extern Joystick_t joystick_data;

#define GAME1_SCREEN_WIDTH 240
#define GAME1_SCREEN_HEIGHT 240

static void Game1_Player_ClampToScreen(Game1_Player *player) {
  if (player->x < 0) {
    player->x = 0;
  }

  if (player->y < 0) {
    player->y = 0;
  }

  if (player->x + player->width > GAME1_SCREEN_WIDTH) {
    player->x = GAME1_SCREEN_WIDTH - player->width;
  }

  if (player->y + player->height > GAME1_SCREEN_HEIGHT) {
    player->y = GAME1_SCREEN_HEIGHT - player->height;
  }
}

static uint8_t Game1_Player_WouldCollideAt(const Game1_Player *player, int16_t test_x, int16_t test_y) {
  uint16_t left_tile = test_x / GAME1_TILE_SIZE;
  uint16_t right_tile = (test_x + player->width - 1) / GAME1_TILE_SIZE;
  uint16_t top_tile = test_y / GAME1_TILE_SIZE;
  uint16_t bottom_tile = (test_y + player->height - 1) / GAME1_TILE_SIZE;

  for (uint16_t tile_y = top_tile; tile_y <= bottom_tile; tile_y++) {
    for (uint16_t tile_x = left_tile; tile_x <= right_tile; tile_x++) {
      if (Game1_World_IsSolid(tile_x, tile_y)) {
        return 1;
      }
    }
  }

  return 0;
}

static void Game1_GetMovementFromInput(int16_t *dx, int16_t *dy, int16_t speed){
    *dx = 0;
    *dy = 0;

    switch (joystick_data.direction) {
    case N:  *dy = -speed; break;
    case S:  *dy =  speed; break;
    case E:  *dx =  speed; break;
    case W:  *dx = -speed; break;

    case NE: *dx =  speed; *dy = -speed; break;
    case NW: *dx = -speed; *dy = -speed; break;
    case SE: *dx =  speed; *dy =  speed; break;
    case SW: *dx = -speed; *dy =  speed; break;

    case CENTRE:
    default:
        break;
    }
}

void Game1_Player_Update(Game1_Player *player) {
  int16_t dx, dy;

  Joystick_Read(&joystick_cfg, &joystick_data);

  Game1_GetMovementFromInput(&dx, &dy, player->speed);

  // Horizontal
  if (!Game1_Player_WouldCollideAt(player, player->x + dx, player->y)) {
    player->x += dx;
  }

  // Vertical
  if (!Game1_Player_WouldCollideAt(player, player->x, player->y + dy)) {
    player->y += dy;
  }

  Game1_Player_ClampToScreen(player);
}

void Game1_Player_Init(Game1_Player *player) {
  player->x = 40;
  player->y = 40;
  player->width = 8;
  player->height = 8;
  player->speed = 2;
}