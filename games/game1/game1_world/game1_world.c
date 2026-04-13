#include "game1_world.h"

static uint8_t game1_map[GAME1_MAP_HEIGHT][GAME1_MAP_WIDTH];

void Game1_World_Init(void) {
  // Clear whole map
  for (uint16_t y = 0; y < GAME1_MAP_HEIGHT; y++) {
    for (uint16_t x = 0; x < GAME1_MAP_WIDTH; x++) {
      game1_map[y][x] = TILE_EMPTY;
    }
  }

  // Floor: bottom 3 rows across full map width
  for (uint16_t y = GAME1_MAP_HEIGHT - 3; y < GAME1_MAP_HEIGHT; y++) {
    for (uint16_t x = 0; x < GAME1_MAP_WIDTH; x++) {
      game1_map[y][x] = TILE_SOLID;
    }
  }

  // Test vertical wall
  for (uint16_t y = 23; y <= 26; y++) {
    game1_map[y][3] = TILE_SOLID;
    game1_map[y][4] = TILE_SOLID;
  }

  // Test platform in the middle-left
  for (uint16_t x = 12; x <= 18; x++) {
    game1_map[20][x] = TILE_SOLID;
  }

  // Test platform further right for camera scrolling
  for (uint16_t x = 32; x <= 40; x++) {
    game1_map[18][x] = TILE_SOLID;
  }

  // Another raised platform near far right
  for (uint16_t x = 48; x <= 55; x++) {
    game1_map[15][x] = TILE_SOLID;
  }
}

uint8_t Game1_World_GetTile(uint16_t tile_x, uint16_t tile_y) {
  if (tile_x >= GAME1_MAP_WIDTH || tile_y >= GAME1_MAP_HEIGHT) {
    return TILE_SOLID;
  }

  return game1_map[tile_y][tile_x];
}

uint8_t Game1_World_IsSolid(uint16_t tile_x, uint16_t tile_y) {
  return Game1_World_GetTile(tile_x, tile_y) == TILE_SOLID;
}