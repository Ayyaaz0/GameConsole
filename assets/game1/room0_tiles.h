#ifndef ROOM0_TILES_H
#define ROOM0_TILES_H

#include <stdint.h>

#define ROOM0_TILES_COUNT 21
#define ROOM0_TILES_ANIMATION_COUNT 1
#define ROOM0_TILES_WIDTH 8
#define ROOM0_TILES_HEIGHT 8

const uint8_t *Game1_Tiles_Find(uint16_t tiled_id);
uint16_t Game1_Tiles_ResolveAnimation(uint16_t tiled_id, uint32_t frame_counter);

#endif // ROOM0_TILES_H
