#include "game1_world.h"

#include "game1_player.h"

#include "game1/room0.h"
#include "game1/room1.h"

#include <stdint.h>

#define ROOM0_START_TILE_X 0
#define ROOM0_START_TILE_Y 15

static uint8_t current_room = 0;

static uint8_t room_maps[GAME1_ROOM_COUNT][GAME1_ROOM_HEIGHT][GAME1_ROOM_WIDTH];
static uint16_t room_visuals[GAME1_ROOM_COUNT][GAME1_ROOM_HEIGHT][GAME1_ROOM_WIDTH];

static void Game1_World_ClearRoom(uint8_t room_index) {
  for (uint16_t y = 0; y < GAME1_ROOM_HEIGHT; y++) {
    for (uint16_t x = 0; x < GAME1_ROOM_WIDTH; x++) {
      room_maps[room_index][y][x] = TILE_EMPTY;
      room_visuals[room_index][y][x] = 0;
    }
  }
}

static uint8_t Game1_World_ConvertTiledTile(uint16_t tile) {
  /*
   * Visual tiles keep their original Tiled GIDs.
   * Collision tiles are reduced to simple engine tile types.
   */
  switch (tile) {
  case 20:
  case 116:
    return TILE_SOLID;

  default:
    return TILE_EMPTY;
  }
}

static void Game1_World_BuildRoom0(void) {
  for (uint16_t y = 0; y < ROOM0_HEIGHT && y < GAME1_ROOM_HEIGHT; y++) {
    for (uint16_t x = 0; x < ROOM0_WIDTH && x < GAME1_ROOM_WIDTH; x++) {
      uint16_t tiled = room0_data[y * ROOM0_WIDTH + x];

      room_visuals[0][y][x] = tiled;
      room_maps[0][y][x] = Game1_World_ConvertTiledTile(tiled);
    }
  }
}

static void Game1_World_BuildRoom1(void) {
  for (uint16_t y = 0; y < ROOM1_HEIGHT && y < GAME1_ROOM_HEIGHT; y++) {
    for (uint16_t x = 0; x < ROOM1_WIDTH && x < GAME1_ROOM_WIDTH; x++) {
      uint16_t tiled = room1_data[y * ROOM1_WIDTH + x];

      room_visuals[1][y][x] = tiled;
      room_maps[1][y][x] = Game1_World_ConvertTiledTile(tiled);
    }
  }
}

uint16_t Game1_World_GetCurrentRoomWidthTiles(void) {
  switch (current_room) {
  case 0:
    return ROOM0_WIDTH;

  case 1:
    return ROOM1_WIDTH;

  default:
    return GAME1_ROOM_WIDTH;
  }
}

uint16_t Game1_World_GetCurrentRoomHeightTiles(void) {
  switch (current_room) {
  case 0:
    return ROOM0_HEIGHT;

  case 1:
    return ROOM1_HEIGHT;

  default:
    return GAME1_ROOM_HEIGHT;
  }
}

uint16_t Game1_World_GetCurrentRoomWidthPx(void) {
  return Game1_World_GetCurrentRoomWidthTiles() * GAME1_TILE_SIZE;
}

uint16_t Game1_World_GetCurrentRoomHeightPx(void) {
  return Game1_World_GetCurrentRoomHeightTiles() * GAME1_TILE_SIZE;
}

void Game1_World_Init(void) {
  for (uint8_t room = 0; room < GAME1_ROOM_COUNT; room++) {
    Game1_World_ClearRoom(room);
  }

  Game1_World_BuildRoom0();
  Game1_World_BuildRoom1();

  current_room = 0;
}

void Game1_World_SpawnAtTile(Game1_Player *player, uint16_t tile_x, uint16_t tile_y) {
  player->x = (tile_x * GAME1_TILE_SIZE) + (GAME1_TILE_SIZE / 2) - (player->width / 2);
  player->y = (tile_y * GAME1_TILE_SIZE) + (GAME1_TILE_SIZE / 2) - (player->height / 2);
}

void Game1_World_SpawnAtStart(Game1_Player *player) {
  Game1_World_SpawnAtTile(player, ROOM0_START_TILE_X, ROOM0_START_TILE_Y);
}

uint8_t Game1_World_GetTile(uint16_t tile_x, uint16_t tile_y) {
  if (tile_x >= GAME1_ROOM_WIDTH || tile_y >= GAME1_ROOM_HEIGHT) {
    return TILE_SOLID;
  }

  return room_maps[current_room][tile_y][tile_x];
}

uint16_t Game1_World_GetVisualTile(uint16_t tile_x, uint16_t tile_y) {
  if (tile_x >= GAME1_ROOM_WIDTH || tile_y >= GAME1_ROOM_HEIGHT) {
    return 0;
  }

  return room_visuals[current_room][tile_y][tile_x];
}

void Game1_World_SetTile(uint16_t tile_x, uint16_t tile_y, uint8_t tile) {
  if (tile_x >= GAME1_ROOM_WIDTH || tile_y >= GAME1_ROOM_HEIGHT) {
    return;
  }

  room_maps[current_room][tile_y][tile_x] = tile;
}

uint8_t Game1_World_IsSolid(uint16_t tile_x, uint16_t tile_y) {
  return Game1_World_GetTile(tile_x, tile_y) == TILE_SOLID;
}

void Game1_World_SetCurrentRoom(uint8_t room_index) {
  if (room_index < GAME1_ROOM_COUNT) {
    current_room = room_index;
  }
}

uint8_t Game1_World_GetCurrentRoom(void) {
  return current_room;
}

uint8_t Game1_World_IsWater(uint16_t tile_x, uint16_t tile_y) {
  uint16_t tile = Game1_World_GetVisualTile(tile_x, tile_y);

  return tile == 1723 || tile == 1745 || tile == 1767 || tile == 1789;
}