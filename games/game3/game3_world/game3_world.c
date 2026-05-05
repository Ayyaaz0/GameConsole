#include "game3_world.h"
#include <stdint.h>

// MOVING PLATFORM TUNING

#define GAME3_MOVING_PLATFORM_WIDTH       32
#define GAME3_MOVING_PLATFORM_HEIGHT      GAME3_TILE_SIZE
#define GAME3_MOVING_PLATFORM_SPEED       1
#define GAME3_MOVING_PLATFORM_ROW         19
#define GAME3_MOVING_PLATFORM_MIN_TILE_X  10
#define GAME3_MOVING_PLATFORM_MAX_TILE_X  20    // rightmost column the platform's left edge can reach

// 2D grid of tile types, plus the single moving platform
static uint8_t room_map[GAME3_ROOM_HEIGHT][GAME3_ROOM_WIDTH];
static Game3_MovingPlatform moving_platform;

// ROOM SETUP

static void Game3_World_ClearRoom(void) {
    for (uint16_t y = 0; y < GAME3_ROOM_HEIGHT; y++) {
        for (uint16_t x = 0; x < GAME3_ROOM_WIDTH; x++) {
            room_map[y][x] = GAME3_TILE_EMPTY;
        }
    }
}

static void Game3_World_Build_Arena(void) {
    /* Floor (rows 28-29) */
    for (uint16_t y = GAME3_ROOM_HEIGHT - 2; y < GAME3_ROOM_HEIGHT; y++) {
        for (uint16_t x = 0; x < GAME3_ROOM_WIDTH; x++) {
            room_map[y][x] = GAME3_TILE_SOLID;
        }
    }

    /* Platform A: low-left stepping stone (row 23) */
    for (uint16_t x = 2; x <= 8; x++) {
        room_map[23][x] = GAME3_TILE_SOLID;
    }

    /* Platform B: mid-right (row 15) */
    for (uint16_t x = 22; x <= 28; x++) {
        room_map[15][x] = GAME3_TILE_SOLID;
    }

    /* Platform C: high-centre, above the moving platform (row 11) */
    for (uint16_t x = 12; x <= 18; x++) {
        room_map[11][x] = GAME3_TILE_SOLID;
    }
}

static void Game3_World_Init_MovingPlatform(void) {
    moving_platform.width  = GAME3_MOVING_PLATFORM_WIDTH;
    moving_platform.height = GAME3_MOVING_PLATFORM_HEIGHT;
    moving_platform.y      = GAME3_MOVING_PLATFORM_ROW * GAME3_TILE_SIZE;
    moving_platform.min_x  = GAME3_MOVING_PLATFORM_MIN_TILE_X * GAME3_TILE_SIZE;
    moving_platform.max_x  = GAME3_MOVING_PLATFORM_MAX_TILE_X * GAME3_TILE_SIZE;
    moving_platform.x      = moving_platform.min_x;
    moving_platform.direction = 1;
    moving_platform.dx     = 0;
}

void Game3_World_Init(void) {
    Game3_World_ClearRoom();
    Game3_World_Build_Arena();
    Game3_World_Init_MovingPlatform();
}

// PER FRAME

void Game3_World_Update(void) {
    int16_t old_x = moving_platform.x;

    moving_platform.x += (int16_t)moving_platform.direction * GAME3_MOVING_PLATFORM_SPEED;

    // Bounce at either end and flip direction
    if (moving_platform.x >= moving_platform.max_x) {
        moving_platform.x = moving_platform.max_x;
        moving_platform.direction = -1;
    } else if (moving_platform.x <= moving_platform.min_x) {
        moving_platform.x = moving_platform.min_x;
        moving_platform.direction = 1;
    }

    // dx is how far the platform moved this frame; the player uses it to ride along
    moving_platform.dx = moving_platform.x - old_x;
}

// TILE QUERIES

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

// MOVING PLATFORM QUERIES

const Game3_MovingPlatform* Game3_World_Get_Moving_Platform(void) {
    return &moving_platform;
}

uint8_t Game3_World_Pixel_Hits_Moving_Platform(int16_t px, int16_t py) {
    if (px < moving_platform.x) return 0;
    if (px >= (int16_t)(moving_platform.x + moving_platform.width)) return 0;
    if (py < moving_platform.y) return 0;
    if (py >= (int16_t)(moving_platform.y + moving_platform.height)) return 0;
    return 1;
}

// True if the box's bottom edge sits exactly on the platform top AND overlaps in x
uint8_t Game3_World_Box_Is_On_Moving_Platform(int16_t x, int16_t y, uint8_t width, uint8_t height) {
    if ((int16_t)(y + height) != moving_platform.y) return 0;

    int16_t left  = x;
    int16_t right = x + (int16_t)width - 1;
    int16_t plat_left  = moving_platform.x;
    int16_t plat_right = moving_platform.x + (int16_t)moving_platform.width - 1;

    if (right < plat_left || left > plat_right) return 0;
    return 1;
}
