#ifndef GAME3_WORLD_H
#define GAME3_WORLD_H 

#include <stdint.h> 

#define GAME3_TILE_SIZE     8 
#define GAME3_ROOM_WIDTH    30
#define GAME3_ROOM_HEIGHT   30
#define GAME3_ROOM_COUNT    1 

#define GAME3_WORLD_WIDTH_PX (GAME3_ROOM_WIDTH * GAME3_TILE_SIZE)
#define GAME3_WORLD_HEIGHT_PX (GAME3_ROOM_HEIGHT * GAME3_TILE_SIZE)

typedef enum { 
    GAME3_TILE_EMPTY = 0, 
    GAME3_TILE_SOLID = 1 
} Game3_Tile; 

void Game3_World_Init(void); 

uint8_t Game3_World_Get_Tile(uint16_t tile_x, uint16_t tile_y); 
void Game3_World_Set_Tile (uint16_t tile_x, uint16_t tile_y, uint8_t tile);

uint8_t Game3_World_Is_Solid(uint16_t tile_x, uint16_t tile_y);
 
#endif 