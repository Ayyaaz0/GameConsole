#ifndef GAME1_ENTITY_COMMON_H
#define GAME1_ENTITY_COMMON_H

#include "game1_player.h"
#include "game1_tiles.h"

#include <stdint.h>

static inline uint8_t Game1_Entity_OverlapsPlayer(const Game1_Player *player,
                                                  int16_t x, int16_t y,
                                                  uint8_t w, uint8_t h) {
  return player->x < x + w && player->x + player->width > x &&
         player->y < y + h && player->y + player->height > y;
}

void Game1_Entity_DrawSprite(int16_t screen_x, int16_t screen_y,
                             const Game1_TileSprite *sprite);

void Game1_Entity_DrawSprite_Flipped(int16_t screen_x, int16_t screen_y,
                                     const Game1_TileSprite *sprite,
                                     uint8_t flip_x);

#endif