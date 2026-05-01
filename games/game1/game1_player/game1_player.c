#include "game1_player.h"

#include "game1_world/game1_world.h"
#include "game1_entities/game1_entities.h"

#define GAME1_MAX_FALL_SPEED 6
#define GAME1_COYOTE_FRAMES 6
#define GAME1_MAX_AIR_JUMPS 1

static void Game1_Player_ClampToWorld(Game1_Player *player) {
  if (player->x < 0) {
    player->x = 0;
  }

  if (player->y < 0) {
    player->y = 0;
  }

  if (player->x + player->width > GAME1_WORLD_WIDTH_PX) {
    player->x = GAME1_WORLD_WIDTH_PX - player->width;
  }

  if (player->y + player->height > GAME1_WORLD_HEIGHT_PX) {
    player->y = GAME1_WORLD_HEIGHT_PX - player->height;
  }
}

static uint8_t Game1_Player_WouldCollideAt(const Game1_Player *player,
                                           int16_t test_x, int16_t test_y) {
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

static void Game1_Player_HandleJump(Game1_Player *player,
                                    uint8_t jump_pressed) {
  if (!jump_pressed) {
    return;
  }

  if (player->grounded || player->coyote_timer > 0) {
    player->vy = -player->jump_strength;
    player->grounded = 0;
    player->coyote_timer = 0;
  } else if (player->air_jumps_remaining > 0) {
    player->vy = -player->jump_strength;
    player->air_jumps_remaining--;
  }
}

static void Game1_Player_ApplyGravity(Game1_Player *player) {
  player->vy += player->gravity;

  if (player->vy > GAME1_MAX_FALL_SPEED) {
    player->vy = GAME1_MAX_FALL_SPEED;
  }
}

static void Game1_Player_MoveHorizontal(Game1_Player *player) {
  if (!Game1_Player_WouldCollideAt(player, player->x + player->vx, player->y)) {
    player->x += player->vx;
  } else {
    player->vx = 0;
  }
}

static void Game1_Player_UpdateAirState(Game1_Player *player) {
  if (player->grounded) {
    player->grounded = 0;
    player->coyote_timer = GAME1_COYOTE_FRAMES;
  } else if (player->coyote_timer > 0) {
    player->coyote_timer--;
  }
}

static void Game1_Player_MoveVertical(Game1_Player *player) {
  if (!Game1_Player_WouldCollideAt(player, player->x, player->y + player->vy)) {
    player->y += player->vy;
    return;
  }

  if (player->vy > 0) {
    while (!Game1_Player_WouldCollideAt(player, player->x, player->y + 1)) {
      player->y += 1;
    }

    player->grounded = 1;
    player->air_jumps_remaining = GAME1_MAX_AIR_JUMPS;
    player->coyote_timer = GAME1_COYOTE_FRAMES;
  } else if (player->vy < 0) {
    while (!Game1_Player_WouldCollideAt(player, player->x, player->y - 1)) {
      player->y -= 1;
    }
  }

  player->vy = 0;
}

static uint8_t Game1_Player_TouchingWater(const Game1_Player *player) {
  uint16_t left_tile = player->x / GAME1_TILE_SIZE;
  uint16_t right_tile = (player->x + player->width - 1) / GAME1_TILE_SIZE;
  uint16_t top_tile = player->y / GAME1_TILE_SIZE;
  uint16_t bottom_tile = (player->y + player->height - 1) / GAME1_TILE_SIZE;

  for (uint16_t ty = top_tile; ty <= bottom_tile; ty++) {
    for (uint16_t tx = left_tile; tx <= right_tile; tx++) {
      if (Game1_World_IsWater(tx, ty)) {
        return 1;
      }
    }
  }

  return 0;
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
  player->air_jumps_remaining = GAME1_MAX_AIR_JUMPS;
  player->coyote_timer = 0;

  player->has_key = 0;
  player->alive = 1;
  player->death_timer = 0;
}

void Game1_Player_Update(Game1_Player *player, int16_t dx, uint8_t jump_pressed) {
  if (!player->alive) {
    player->vx = 0;
    player->vy = 0;
    player->death_timer++;

    if (player->death_timer > 30) {
      Game1_Player_Init(player);
      Game1_Entities_SpawnPlayer(player);
    }

    return;
  }

  player->vx = dx * player->move_speed;

  Game1_Player_HandleJump(player, jump_pressed);
  Game1_Player_ApplyGravity(player);
  Game1_Player_MoveHorizontal(player);
  Game1_Player_UpdateAirState(player);
  Game1_Player_MoveVertical(player);

  Game1_Player_ClampToWorld(player);

  if (Game1_Player_TouchingWater(player)) {
    player->alive = 0;
    player->death_timer = 0;
  }
}