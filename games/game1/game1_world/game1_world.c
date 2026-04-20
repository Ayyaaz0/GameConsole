#include "game1_world.h"

#define GAME1_ROOM_ENTRY_OFFSET 8
#define GAME1_TRANSITION_COOLDOWN_FRAMES 10

static uint8_t current_room = 0;
static uint8_t transition_cooldown = 0;

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

  // Door on right side of room 0
  room_maps[0][24][26] = TILE_DOOR;
  room_maps[0][25][26] = TILE_DOOR;
  room_maps[0][26][26] = TILE_DOOR;
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

  // Door on left side of room 1
  room_maps[1][24][1] = TILE_DOOR;
  room_maps[1][25][1] = TILE_DOOR;
  room_maps[1][26][1] = TILE_DOOR;

  // Key tile for later
  room_maps[1][17][20] = TILE_KEY;
}

static uint8_t Game1_World_PlayerTouchesDoor(const Game1_Player *player) {
  uint16_t left_tile = player->x / GAME1_TILE_SIZE;
  uint16_t right_tile = (player->x + player->width - 1) / GAME1_TILE_SIZE;
  uint16_t top_tile = player->y / GAME1_TILE_SIZE;
  uint16_t bottom_tile = (player->y + player->height - 1) / GAME1_TILE_SIZE;

  for (uint16_t tile_y = top_tile; tile_y <= bottom_tile; tile_y++) {
    for (uint16_t tile_x = left_tile; tile_x <= right_tile; tile_x++) {
      uint8_t tile = Game1_World_GetTile(tile_x, tile_y);

      if (tile == TILE_DOOR) {
        return 1;
      }
    }
  }

  return 0;
}

void Game1_World_Init(void) {
  for (uint8_t room = 0; room < GAME1_ROOM_COUNT; room++) {
    Game1_World_ClearRoom(room);
  }

  Game1_World_BuildRoom0();
  Game1_World_BuildRoom1();

  current_room = 0;
  transition_cooldown = 0;
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

void Game1_World_HandleTransition(Game1_Player *player, uint8_t interact_pressed) {
  if (transition_cooldown > 0) {
    transition_cooldown--;
  }

  if (transition_cooldown == 0 && Game1_World_PlayerTouchesDoor(player) && interact_pressed) {
    uint8_t room = Game1_World_GetCurrentRoom();

    if (room == 0) {
      Game1_World_SetCurrentRoom(1);

      player->x = 2 * GAME1_TILE_SIZE + GAME1_ROOM_ENTRY_OFFSET;
      player->y = 24 * GAME1_TILE_SIZE;

      transition_cooldown = GAME1_TRANSITION_COOLDOWN_FRAMES;
    } else if (room == 1) {
      Game1_World_SetCurrentRoom(0);

      player->x = (26 * GAME1_TILE_SIZE) - player->width - GAME1_ROOM_ENTRY_OFFSET;
      player->y = 24 * GAME1_TILE_SIZE;

      transition_cooldown = GAME1_TRANSITION_COOLDOWN_FRAMES;
    }
  }
}