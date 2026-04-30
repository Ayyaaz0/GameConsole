#ifndef ROOM0_TILES_H
#define ROOM0_TILES_H

#include <stdint.h>

#define ROOM0_TILES_COUNT 25
#define ROOM0_TILES_ANIMATION_COUNT 4

typedef struct {
  const uint8_t *pixels;
  uint8_t width;
  uint8_t height;
} Game1_TileSprite;

const Game1_TileSprite *Game1_Tiles_Find(uint16_t tiled_id);
uint16_t Game1_Tiles_ResolveAnimation(uint16_t tiled_id, uint32_t frame_counter);

#endif
