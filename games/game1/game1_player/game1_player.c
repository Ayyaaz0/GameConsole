#include "game1_player.h"

#include "InputHandler.h"
#include "game1_world/game1_world.h"
#include "Joystick.h"

extern Joystick_cfg_t joystick_cfg;
extern Joystick_t joystick_data;

#define GAME1_SCREEN_WIDTH 240
#define GAME1_SCREEN_HEIGHT 240
#define GAME1_MAX_FALL_SPEED 6

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

static int16_t Game1_Player_GetHorizontalInput(const Game1_Player *player) {
  (void)player;

  switch (joystick_data.direction) {
  case W:
  case NW:
  case SW:
    return -1;

  case E:
  case NE:
  case SE:
    return 1;

  case N:
  case S:
  case CENTRE:
  default:
    return 0;
  }
}

void Game1_Player_Update(Game1_Player *player) {
  Joystick_Read(&joystick_cfg, &joystick_data);

  int16_t move_input = Game1_Player_GetHorizontalInput(player);
  player->vx = move_input * player->move_speed;

  if (current_input.btn2_pressed && player->grounded) {
    player->vy = -player->jump_strength;
    player->grounded = 0;
  }

  player->vy += player->gravity;
  if (player->vy > GAME1_MAX_FALL_SPEED) {
    player->vy = GAME1_MAX_FALL_SPEED;
  }

  // Horizontal movement first
  if (!Game1_Player_WouldCollideAt(player, player->x + player->vx, player->y)) {
    player->x += player->vx;
  } else {
    player->vx = 0;
  }

  // Assume airborne unless there's a collision below
  player->grounded = 0;

  // Vertical movement second
  if (!Game1_Player_WouldCollideAt(player, player->x, player->y + player->vy)) {
    player->y += player->vy;
  } else {
    if (player->vy > 0) {
      // Falling onto ground
      while (!Game1_Player_WouldCollideAt(player, player->x, player->y + 1)) {
        player->y += 1;
      }
      player->grounded = 1;
    } else if (player->vy < 0) {
      // Moving upward into ceiling
      while (!Game1_Player_WouldCollideAt(player, player->x, player->y - 1)) {
        player->y -= 1;
      }
    }

    player->vy = 0;
  }

  Game1_Player_ClampToScreen(player);
}

void Game1_Player_Init(Game1_Player *player) {
  player->x = 40;
  player->y = 40;
  player->vx = 0;
  player->vy = 0;
  player->width = 8;
  player->height = 8;
  player->move_speed = 2;
  player->jump_strength = 8;
  player->gravity = 1;
  player->grounded = 0;
}
