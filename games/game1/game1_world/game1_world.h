#ifndef GAME1_WORLD_H
#define GAME1_WORLD_H

#include <stdint.h>
#include "game1_player.h"

#define GAME1_TILE_SIZE 8
#define GAME1_ROOM_WIDTH 30
#define GAME1_ROOM_HEIGHT 30
#define GAME1_ROOM_COUNT 3

#define GAME1_WORLD_WIDTH_PX  (GAME1_ROOM_WIDTH * GAME1_TILE_SIZE)
#define GAME1_WORLD_HEIGHT_PX (GAME1_ROOM_HEIGHT * GAME1_TILE_SIZE)

typedef enum {
  TILE_EMPTY = 0,
  TILE_SOLID = 1,
  TILE_DOOR = 2,
  TILE_DOOR_LOCKED = 3,
  TILE_KEY = 4
} Game1_Tile;

void Game1_World_Init(void);
uint8_t Game1_World_GetTile(uint16_t tile_x, uint16_t tile_y);
void Game1_World_SetTile(uint16_t tile_x, uint16_t tile_y, uint8_t tile);
uint8_t Game1_World_IsSolid(uint16_t tile_x, uint16_t tile_y);

void Game1_World_SetCurrentRoom(uint8_t room_index);
uint8_t Game1_World_GetCurrentRoom(void);

uint8_t Game1_World_PlayerTouchesKey(Game1_Player *player);
void Game1_World_HandleTransition(Game1_Player *player, uint8_t interact_pressed);

#endif