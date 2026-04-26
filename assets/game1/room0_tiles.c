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

static const uint8_t t_1[ROOM0_TILES_WIDTH * ROOM0_TILES_HEIGHT] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 0, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 0, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 0, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 0, 1, 1, 1, 1, 1,
};

static const uint8_t t_2[ROOM0_TILES_WIDTH * ROOM0_TILES_HEIGHT] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    14, 14, 14, 14, 0, 0, 0, 0,
    0, 0, 0, 0, 14, 14, 14, 14,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
};

static const uint8_t t_3[ROOM0_TILES_WIDTH * ROOM0_TILES_HEIGHT] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 3, 3, 3, 3, 0, 0,
    0, 3, 0, 0, 0, 0, 3, 0,
    3, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 3,
};

static const uint8_t t_4[ROOM0_TILES_WIDTH * ROOM0_TILES_HEIGHT] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 14, 14, 14, 14, 0, 0,
    14, 14, 0, 0, 0, 0, 14, 14,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
};

static const uint8_t t_5[ROOM0_TILES_WIDTH * ROOM0_TILES_HEIGHT] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 14, 14, 14, 14,
    14, 14, 14, 14, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
};

static const uint8_t t_6[ROOM0_TILES_WIDTH * ROOM0_TILES_HEIGHT] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    14, 14, 0, 0, 0, 0, 14, 14,
    0, 0, 14, 14, 14, 14, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
};

static const uint8_t t_7[ROOM0_TILES_WIDTH * ROOM0_TILES_HEIGHT] = {
    0, 0, 14, 14, 14, 14, 0, 0,
    0, 0, 0, 14, 14, 0, 0, 0,
    0, 0, 0, 14, 14, 0, 0, 0,
    0, 0, 0, 14, 14, 0, 0, 0,
    0, 0, 0, 14, 14, 0, 0, 0,
    0, 0, 0, 14, 14, 0, 0, 0,
    0, 0, 0, 14, 14, 0, 0, 0,
    0, 0, 0, 14, 14, 0, 0, 0,
};

static const uint8_t t_8[ROOM0_TILES_WIDTH * ROOM0_TILES_HEIGHT] = {
    0, 0, 0, 14, 14, 0, 0, 0,
    0, 0, 0, 14, 14, 0, 0, 0,
    0, 0, 0, 14, 14, 0, 0, 0,
    0, 0, 0, 14, 14, 0, 0, 0,
    0, 0, 0, 14, 14, 0, 0, 0,
    0, 0, 0, 14, 14, 0, 0, 0,
    0, 0, 0, 14, 14, 0, 0, 0,
    0, 0, 0, 14, 14, 0, 0, 0,
};

static const uint8_t t_9[ROOM0_TILES_WIDTH * ROOM0_TILES_HEIGHT] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 9, 0, 0, 0, 0, 0,
    0, 9, 9, 0, 0, 0, 0, 0,
    0, 9, 0, 0, 0, 0, 9, 0,
    0, 9, 9, 0, 0, 9, 9, 0,
    0, 0, 9, 9, 0, 9, 0, 0,
};

static const uint8_t t_10[ROOM0_TILES_WIDTH * ROOM0_TILES_HEIGHT] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    9, 0, 0, 0, 0, 0, 0, 0,
    9, 9, 0, 0, 0, 0, 0, 0,
    0, 9, 9, 0, 0, 9, 9, 0,
    0, 0, 9, 0, 0, 9, 0, 0,
    0, 0, 9, 9, 0, 9, 0, 0,
};

static const uint8_t t_11[ROOM0_TILES_WIDTH * ROOM0_TILES_HEIGHT] = {
    7, 0, 7, 0, 0, 0, 0, 7,
    7, 0, 7, 0, 0, 0, 0, 7,
    7, 0, 0, 7, 7, 7, 7, 0,
    7, 0, 0, 0, 0, 0, 7, 0,
    7, 0, 0, 0, 0, 0, 7, 0,
    0, 7, 0, 0, 0, 7, 0, 0,
    0, 0, 7, 7, 7, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
};

static const uint8_t t_12[ROOM0_TILES_WIDTH * ROOM0_TILES_HEIGHT] = {
    7, 0, 0, 7, 0, 0, 0, 0,
    7, 0, 0, 7, 0, 0, 0, 0,
    7, 0, 0, 7, 0, 0, 0, 0,
    0, 0, 0, 7, 0, 0, 0, 0,
    0, 0, 0, 0, 7, 0, 0, 0,
    0, 0, 0, 0, 0, 7, 7, 7,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
};

static const uint8_t t_13[ROOM0_TILES_WIDTH * ROOM0_TILES_HEIGHT] = {
    0, 0, 0, 0, 7, 0, 7, 7,
    0, 0, 0, 0, 7, 0, 7, 0,
    0, 0, 0, 0, 7, 0, 7, 0,
    0, 0, 0, 0, 7, 0, 7, 0,
    0, 0, 0, 7, 0, 0, 7, 0,
    7, 7, 7, 0, 0, 0, 7, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
};

static const uint8_t t_14[ROOM0_TILES_WIDTH * ROOM0_TILES_HEIGHT] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 3, 3, 3, 0, 0,
    0, 0, 3, 3, 3, 3, 3, 0,
};

static const uint8_t t_15[ROOM0_TILES_WIDTH * ROOM0_TILES_HEIGHT] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 3, 3, 3, 0, 0, 0,
    0, 3, 3, 3, 3, 3, 0, 0,
    3, 3, 3, 3, 3, 0, 3, 3,
    3, 3, 3, 3, 0, 3, 3, 3,
};

static const uint8_t t_16[ROOM0_TILES_WIDTH * ROOM0_TILES_HEIGHT] = {
    0, 0, 0, 3, 3, 0, 0, 0,
    0, 0, 0, 3, 3, 0, 0, 0,
    0, 0, 3, 0, 0, 3, 0, 0,
    0, 0, 3, 0, 0, 3, 0, 0,
    0, 0, 0, 3, 3, 0, 0, 0,
    0, 0, 0, 3, 3, 0, 0, 0,
    0, 0, 3, 0, 0, 3, 0, 0,
    0, 0, 3, 0, 0, 3, 0, 0,
};

static const uint8_t t_17[ROOM0_TILES_WIDTH * ROOM0_TILES_HEIGHT] = {
    0, 0, 0, 3, 3, 0, 0, 0,
    0, 0, 0, 3, 3, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
};

static const uint8_t t_18[ROOM0_TILES_WIDTH * ROOM0_TILES_HEIGHT] = {
    0, 1, 1, 1, 1, 1, 1, 0,
    0, 1, 1, 1, 1, 1, 1, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 1, 1, 1, 0, 0,
    0, 0, 1, 1, 1, 1, 0, 0,
    0, 0, 1, 1, 1, 1, 0, 0,
    0, 0, 1, 1, 1, 1, 0, 0,
    0, 0, 1, 1, 1, 1, 0, 0,
};

static const uint8_t t_19[ROOM0_TILES_WIDTH * ROOM0_TILES_HEIGHT] = {
    0, 0, 1, 1, 1, 1, 0, 0,
    0, 0, 1, 1, 1, 1, 0, 0,
    0, 0, 1, 1, 1, 1, 0, 0,
    0, 0, 1, 1, 1, 1, 0, 0,
    0, 0, 1, 1, 1, 1, 0, 0,
    0, 0, 1, 1, 1, 1, 0, 0,
    0, 0, 1, 1, 1, 1, 0, 0,
    0, 0, 1, 1, 1, 1, 0, 0,
};

static const uint8_t t_20[ROOM0_TILES_WIDTH * ROOM0_TILES_HEIGHT] = {
    0, 0, 1, 1, 1, 1, 0, 0,
    0, 0, 1, 1, 1, 1, 0, 0,
    0, 0, 1, 1, 1, 1, 0, 0,
    0, 0, 1, 1, 1, 1, 0, 0,
    0, 0, 1, 1, 1, 1, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 1, 1, 1, 1, 1, 1, 0,
    0, 1, 1, 1, 1, 1, 1, 0,
};

typedef struct {
  uint16_t id;
  const uint8_t *pixels;
} TileEntry;

static const TileEntry lookup[ROOM0_TILES_COUNT] = {
  { 20, t_0 },
  { 116, t_1 },
  { 1723, t_2 },
  { 1733, t_3 },
  { 1745, t_4 },
  { 1767, t_5 },
  { 1789, t_6 },
  { 1833, t_7 },
  { 1855, t_8 },
  { 1887, t_9 },
  { 1909, t_10 },
  { 2694, t_11 },
  { 2698, t_12 },
  { 2699, t_13 },
  { 28927, t_14 },
  { 28980, t_15 },
  { 28985, t_16 },
  { 29011, t_17 },
  { 29188, t_18 },
  { 29214, t_19 },
  { 29266, t_20 },
};

const uint8_t *Game1_Tiles_Find(uint16_t tiled_id) {
  for (uint16_t i = 0; i < ROOM0_TILES_COUNT; i++) {
    if (lookup[i].id == tiled_id) {
      return lookup[i].pixels;
    }
  }

  return 0;
}

typedef struct {
  uint16_t id;
  uint8_t frame_count;
  const uint16_t *frames;
} AnimationEntry;

static const uint16_t animation_0_frames[] = { 1723, 1745, 1767, 1789 };

static const AnimationEntry animations[ROOM0_TILES_ANIMATION_COUNT] = {
  { 1789, 4, animation_0_frames },
};

uint16_t Game1_Tiles_ResolveAnimation(uint16_t tiled_id, uint32_t frame_counter) {
  for (uint16_t i = 0; i < ROOM0_TILES_ANIMATION_COUNT; i++) {
    if (animations[i].id == tiled_id) {
      uint8_t frame_index = frame_counter % animations[i].frame_count;
      return animations[i].frames[frame_index];
    }
  }

  return tiled_id;
}

