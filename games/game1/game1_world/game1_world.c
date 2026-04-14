#include "game1_world.h"

static uint8_t current_room = 0;

static uint8_t room_maps[GAME1_ROOM_COUNT][GAME1_ROOM_HEIGHT][GAME1_ROOM_WIDTH];

static void Game1_World_ClearRoom(uint8_t room_index) {
  for (uint16_t y = 0; y < GAME1_ROOM_HEIGHT; y++) {
    for (uint16_t x = 0; x < GAME1_ROOM_WIDTH; x++) {
      room_maps[room_index][y][x] = TILE_EMPTY;
    }
  }
}

static void Game1_World_BuildRoom0(void) {
  // Floor
  for (uint16_t y = GAME1_ROOM_HEIGHT - 3; y < GAME1_ROOM_HEIGHT; y++) {
    for (uint16_t x = 0; x < GAME1_ROOM_WIDTH; x++) {
      room_maps[0][y][x] = TILE_SOLID;
    }
  }

  // Test wall / platform area
  for (uint16_t y = 23; y <= 26; y++) {
    room_maps[0][y][3] = TILE_SOLID;
    room_maps[0][y][4] = TILE_SOLID;
  }

  for (uint16_t x = 12; x <= 18; x++) {
    room_maps[0][20][x] = TILE_SOLID;
  }

  // Locked door near right side
  room_maps[0][24][26] = TILE_DOOR_LOCKED;
  room_maps[0][25][26] = TILE_DOOR_LOCKED;
  room_maps[0][26][26] = TILE_DOOR_LOCKED;
}

static void Game1_World_BuildRoom1(void) {
  // Floor
  for (uint16_t y = GAME1_ROOM_HEIGHT - 3; y < GAME1_ROOM_HEIGHT; y++) {
    for (uint16_t x = 0; x < GAME1_ROOM_WIDTH; x++) {
      room_maps[1][y][x] = TILE_SOLID;
    }
  }

  // Platforms
  for (uint16_t x = 6; x <= 12; x++) {
    room_maps[1][22][x] = TILE_SOLID;
  }

  for (uint16_t x = 16; x <= 22; x++) {
    room_maps[1][18][x] = TILE_SOLID;
  }

  // Key pickup
  room_maps[1][17][20] = TILE_KEY;
}

void Game1_World_Init(void) {
  for (uint8_t room = 0; room < GAME1_ROOM_COUNT; room++) {
    Game1_World_ClearRoom(room);
  }

  Game1_World_BuildRoom0();
  Game1_World_BuildRoom1();

  current_room = 0;
}

uint8_t Game1_World_GetTile(uint16_t tile_x, uint16_t tile_y) {
  if (tile_x >= GAME1_ROOM_WIDTH || tile_y >= GAME1_ROOM_HEIGHT) {
    return TILE_SOLID;
  }

  return room_maps[current_room][tile_y][tile_x];
}

void Game1_World_SetTile(uint16_t tile_x, uint16_t tile_y, uint8_t tile) {
  if (tile_x >= GAME1_ROOM_WIDTH || tile_y >= GAME1_ROOM_HEIGHT) {
    return;
  }

  room_maps[current_room][tile_y][tile_x] = tile;
}

uint8_t Game1_World_IsSolid(uint16_t tile_x, uint16_t tile_y) {
  uint8_t tile = Game1_World_GetTile(tile_x, tile_y);

  return (tile == TILE_SOLID || tile == TILE_DOOR_LOCKED);
}

void Game1_World_SetCurrentRoom(uint8_t room_index) {
  if (room_index < GAME1_ROOM_COUNT) {
    current_room = room_index;
  }
}

uint8_t Game1_World_GetCurrentRoom(void) { return current_room; }