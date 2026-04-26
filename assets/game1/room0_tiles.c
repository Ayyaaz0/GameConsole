#include "room0_tiles.h"

static const uint8_t t_0[ROOM0_TILES_WIDTH * ROOM0_TILES_HEIGHT] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    2, 2, 2, 2, 2, 0, 2, 2,
    0, 0, 0, 0, 0, 0, 0, 0,
    2, 2, 0, 2, 2, 2, 2, 2,
    0, 0, 0, 0, 0, 0, 0, 0,
    2, 2, 2, 2, 2, 0, 2, 2,
    0, 0, 0, 0, 0, 0, 0, 0,
    2, 2, 0, 2, 2, 2, 2, 2,
};

typedef struct {
  uint16_t id;
  const uint8_t *pixels;
} TileEntry;

static const TileEntry lookup[ROOM0_TILES_COUNT] = {
  { 20, t_0 },
};

const uint8_t *Game1_Tiles_Find(uint16_t tiled_id) {
  for (uint16_t i = 0; i < ROOM0_TILES_COUNT; i++) {
    if (lookup[i].id == tiled_id) {
      return lookup[i].pixels;
    }
  }

  return 0;
}
