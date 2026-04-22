#include "game3_world.h"
#include <stdint.h>

static uint8_t room_map[GAME3_ROOM_HEIGHT][GAME3_ROOM_WIDTH]; 

static void Game3_World_ClearRoom(void) { 
    for (uint16_t y = 0; y < GAME3_ROOM_HEIGHT; y++) { 
        for (uint16_t x = 0; x < GAME3_ROOM_WIDTH; x++) { 
            room_map[y][x] = GAME3_TILE_EMPTY;
        }
    }
}

static void Game3_World_Build_Arena(void) { 
    for (uint16_t y = GAME3_ROOM_HEIGHT - 2; y < GAME3_ROOM_HEIGHT; y++) { 
        for (uint16_t x = 0; x < GAME3_ROOM_WIDTH; x++) { 
            room_map[y][x] = GAME3_TILE_SOLID; 
        }
    }

    for (uint16_t x = 4; x <= 10; x++) { 
        room_map[18][x] = GAME3_TILE_SOLID; 
    }

    for (uint16_t x = 17; x <= 23; x++) { 
        room_map[14][x] = GAME3_TILE_SOLID; 
    }
}

void Game3_World_Init(void) { 
    Game3_World_ClearRoom(); 
    Game3_World_Build_Arena();
}

uint8_t Game3_World_Get_Tile(uint16_t tile_x, uint16_t tile_y) { 
    if (tile_x >= GAME3_ROOM_WIDTH || tile_y >= GAME3_ROOM_HEIGHT) { 
        return GAME3_TILE_SOLID; 
    }

    return room_map[tile_y][tile_x]; 
}

void Game3_World_Set_Tile(uint16_t tile_x, uint16_t tile_y, uint8_t tile) { 
    if (tile_x >= GAME3_ROOM_WIDTH || tile_y >= GAME3_ROOM_HEIGHT) { 
        return; 
    }

    room_map[tile_y][tile_x] = tile; 
}

uint8_t Game3_World_Is_Solid(uint16_t tile_x, uint16_t tile_y) { 
    return Game3_World_Get_Tile(tile_x, tile_y) == GAME3_TILE_SOLID; 
}