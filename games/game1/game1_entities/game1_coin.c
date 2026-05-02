#include "game1_coin.h"

#include "game1_entity_common.h"
#include "room0_tiles.h"

#define GAME1_COIN_ANIMATION_SPEED 3

typedef struct {
  int16_t x;
  int16_t y;
  uint8_t w;
  uint8_t h;
  uint8_t active;
  uint8_t value;
  uint16_t sprite_gid;
} Game1_Coin;

static Game1_Coin coins[GAME1_MAX_COINS];
static uint8_t coin_count = 0;

 //Score is global and should NOT reset when changing rooms.
static uint16_t score = 0;

static uint32_t animation_counter = 0;

/*
 * Reset coin instances (called on room load)
 * DOES NOT reset score.
 */
void Game1_Coin_Reset(void) {
  coin_count = 0;
  animation_counter = 0;
}
 
void Game1_Coin_ResetScore(void) {
  score = 0;
}

void Game1_Coin_Load(const Game1_Entity *entity) {
  if (coin_count >= GAME1_MAX_COINS) {
    return;
  }

  Game1_Coin *coin = &coins[coin_count++];

  coin->x = entity->x;
  coin->y = entity->y;
  coin->w = entity->w;
  coin->h = entity->h;
  coin->active = 1;
  coin->value = entity->value;
  coin->sprite_gid = entity->sprite_gid;
}

void Game1_Coin_UpdateAll(Game1_Player *player) {
  for (uint8_t i = 0; i < coin_count; i++) {
    Game1_Coin *coin = &coins[i];

    if (!coin->active) {
      continue;
    }

    if (Game1_Entity_OverlapsPlayer(player, coin->x, coin->y, coin->w, coin->h)) {
      coin->active = 0;
      score += coin->value;
    }
  }

  animation_counter++;
}

void Game1_Coin_RenderAll(const Game1_Camera *camera) {
  uint32_t animation_frame =
      animation_counter / GAME1_COIN_ANIMATION_SPEED;

  for (uint8_t i = 0; i < coin_count; i++) {
    const Game1_Coin *coin = &coins[i];

    if (!coin->active) {
      continue;
    }

    uint16_t draw_gid = Game1_Tiles_ResolveAnimation(coin->sprite_gid, animation_frame);
    
    const Game1_TileSprite *sprite = Game1_Tiles_Find(draw_gid);

    Game1_Entity_DrawSprite(coin->x - camera->x, coin->y - camera->y, sprite);
  }
}

uint16_t Game1_Coin_GetScore(void) {
  return score;
}