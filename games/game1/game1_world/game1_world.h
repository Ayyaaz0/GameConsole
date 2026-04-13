#ifndef GAME1_WORLD_H
#define GAME1_WORLD_H

#include <stdint.h>

#define GAME1_TILE_SIZE 8
#define GAME1_MAP_WIDTH 60
#define GAME1_MAP_HEIGHT 30

#define GAME1_WORLD_WIDTH_PX  (GAME1_MAP_WIDTH * GAME1_TILE_SIZE)
#define GAME1_WORLD_HEIGHT_PX (GAME1_MAP_HEIGHT * GAME1_TILE_SIZE)

typedef enum {
    TILE_EMPTY = 0,
    TILE_SOLID = 1
} Game1_Tile;

void Game1_World_Init(void);
uint8_t Game1_World_GetTile(uint16_t tile_x, uint16_t tile_y);
uint8_t Game1_World_IsSolid(uint16_t tile_x, uint16_t tile_y);

#endif