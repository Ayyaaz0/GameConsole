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

typedef struct {
    int16_t x;          /* top-left pixel position */
    int16_t y;          /* top-left pixel position (tile aligned) */
    uint8_t width;      /* pixel width */
    uint8_t height;     /* pixel height */
    int16_t min_x;      /* leftmost x */
    int16_t max_x;      /* rightmost x */
    int8_t  direction;  /* +1 = moving right, -1 = moving left */
    int16_t dx;         /* delta x applied this frame (for player carry) */
} Game3_MovingPlatform;

void Game3_World_Init(void);
void Game3_World_Update(void);

uint8_t Game3_World_Get_Tile(uint16_t tile_x, uint16_t tile_y);
void Game3_World_Set_Tile (uint16_t tile_x, uint16_t tile_y, uint8_t tile);

uint8_t Game3_World_Is_Solid(uint16_t tile_x, uint16_t tile_y);

const Game3_MovingPlatform* Game3_World_Get_Moving_Platform(void);
uint8_t Game3_World_Pixel_Hits_Moving_Platform(int16_t px, int16_t py);
uint8_t Game3_World_Box_Is_On_Moving_Platform(int16_t x, int16_t y, uint8_t width, uint8_t height);

#endif
