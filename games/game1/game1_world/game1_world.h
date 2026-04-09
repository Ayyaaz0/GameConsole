#ifndef GAME1_WORLD_H
#define GAME1_WORLD_H

#include <stdint.h>

#define GAME1_TILE_SIZE 6
#define GAME1_MAP_WIDTH 30
#define GAME1_MAP_HEIGHT 30

typedef enum{
    TILE_EMPTY = 0,
    TILE_SOLID = 1
} Game1_Tile;

void Game1_World_Init(void);
uint8_t Game1_World_GetTile(uint16_t tile_x, uint16_t tile_y);
uint8_t Game1_World_IsSolid(uint16_t tile_x, uint16_t tile_y);

#endif