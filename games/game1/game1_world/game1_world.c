#include "game1_world.h"

#include "game1_player.h"

#include <stdint.h>

#define GAME1_TRANSITION_COOLDOWN_FRAMES 10
#define GAME1_DOOR_HEIGHT 3

static uint8_t current_room = 0;
static uint8_t transition_cooldown = 0;

static uint8_t room_maps[GAME1_ROOM_COUNT][GAME1_ROOM_HEIGHT][GAME1_ROOM_WIDTH];

#define ROOM0_TO_ROOM1_DOOR_X 26
#define ROOM0_TO_ROOM1_DOOR_Y 24

#define ROOM1_TO_ROOM0_DOOR_X 1
#define ROOM1_TO_ROOM0_DOOR_Y 24

#define ROOM0_TO_ROOM2_DOOR_X 20
#define ROOM0_TO_ROOM2_DOOR_Y 24

#define ROOM2_TO_ROOM0_DOOR_X 1
#define ROOM2_TO_ROOM0_DOOR_Y 24

static void Game1_World_ClearRoom(uint8_t room_index) {
  for (uint16_t y = 0; y < GAME1_ROOM_HEIGHT; y++) {
    for (uint16_t x = 0; x < GAME1_ROOM_WIDTH; x++) {
      room_maps[room_index][y][x] = TILE_EMPTY;
    }
  }
}

static void Game1_World_SetDoorColumn(uint8_t room_index, uint16_t tile_x, uint16_t tile_y_top, uint8_t tile_type) {
  for (uint16_t y = tile_y_top; y < tile_y_top + GAME1_DOOR_HEIGHT; y++) {
    room_maps[room_index][y][tile_x] = tile_type;
  }
}

static void Game1_World_SpawnAtDoor(Game1_Player *player, uint16_t tile_x,uint16_t tile_y_middle) {
  player->x = (tile_x * GAME1_TILE_SIZE) + (GAME1_TILE_SIZE / 2) - (player->width / 2);
  player->y = (tile_y_middle * GAME1_TILE_SIZE) + (GAME1_TILE_SIZE / 2) - (player->height / 2);
}

static void Game1_World_BuildRoom0(void) {
  // Floor
  for (uint16_t y = GAME1_ROOM_HEIGHT - 3; y < GAME1_ROOM_HEIGHT; y++) {
    for (uint16_t x = 0; x < GAME1_ROOM_WIDTH; x++) {
      room_maps[0][y][x] = TILE_SOLID;
    }
  }

  // Left test wall
  for (uint16_t y = 23; y <= 26; y++) {
    room_maps[0][y][3] = TILE_SOLID;
    room_maps[0][y][4] = TILE_SOLID;
  }

  // Mid platform
  for (uint16_t x = 12; x <= 18; x++) {
    room_maps[0][20][x] = TILE_SOLID;
  }

  // Open door to Room 1
  Game1_World_SetDoorColumn(0, ROOM0_TO_ROOM1_DOOR_X, ROOM0_TO_ROOM1_DOOR_Y, TILE_DOOR);

  // Locked door to Room 2
  Game1_World_SetDoorColumn(0, ROOM0_TO_ROOM2_DOOR_X, ROOM0_TO_ROOM2_DOOR_Y, TILE_DOOR_LOCKED);
}

static void Game1_World_BuildRoom1(void) {
  // Floor
  for (uint16_t y = GAME1_ROOM_HEIGHT - 3; y < GAME1_ROOM_HEIGHT; y++) {
    for (uint16_t x = 0; x < GAME1_ROOM_WIDTH; x++) {
      room_maps[1][y][x] = TILE_SOLID;
    }
  }

  // Lower platform
  for (uint16_t x = 6; x <= 12; x++) {
    room_maps[1][22][x] = TILE_SOLID;
  }

  // Upper platform
  for (uint16_t x = 16; x <= 22; x++) {
    room_maps[1][18][x] = TILE_SOLID;
  }

  // Door back to Room 0
  Game1_World_SetDoorColumn(1, ROOM1_TO_ROOM0_DOOR_X, ROOM1_TO_ROOM0_DOOR_Y, TILE_DOOR);

  // Key pickup
  room_maps[1][17][20] = TILE_KEY;
}

static void Game1_World_BuildRoom2(void) {
  // Floor
  for (uint16_t y = GAME1_ROOM_HEIGHT - 3; y < GAME1_ROOM_HEIGHT; y++) {
    for (uint16_t x = 0; x < GAME1_ROOM_WIDTH; x++) {
      room_maps[2][y][x] = TILE_SOLID;
    }
  }
  // Door back to Room 0
  Game1_World_SetDoorColumn(2, ROOM2_TO_ROOM0_DOOR_X, ROOM2_TO_ROOM0_DOOR_Y, TILE_DOOR);
}

static uint8_t Game1_World_PlayerTouchesDoor(const Game1_Player *player) {
  uint16_t left_tile = player->x / GAME1_TILE_SIZE;
  uint16_t right_tile = (player->x + player->width - 1) / GAME1_TILE_SIZE;
  uint16_t top_tile = player->y / GAME1_TILE_SIZE;
  uint16_t bottom_tile = (player->y + player->height - 1) / GAME1_TILE_SIZE;

  for (uint16_t tile_y = top_tile; tile_y <= bottom_tile; tile_y++) {
    for (uint16_t tile_x = left_tile; tile_x <= right_tile; tile_x++) {
      uint8_t tile = Game1_World_GetTile(tile_x, tile_y);
      if (tile == TILE_DOOR || tile == TILE_DOOR_LOCKED) {
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
  Game1_World_BuildRoom2();

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

  // Locked doors are NOT solid now; they are interactable triggers.
  return (tile == TILE_SOLID);
}

void Game1_World_SetCurrentRoom(uint8_t room_index) {
  if (room_index < GAME1_ROOM_COUNT) {
    current_room = room_index;
  }
}

uint8_t Game1_World_GetCurrentRoom(void) { return current_room; }

uint8_t Game1_World_PlayerTouchesKey(Game1_Player *player) {
  uint16_t tile_x = (player->x + (player->width / 2)) / GAME1_TILE_SIZE;
  uint16_t tile_y = (player->y + (player->height / 2)) / GAME1_TILE_SIZE;

  return Game1_World_GetTile(tile_x, tile_y) == TILE_KEY;
}

void Game1_World_HandleTransition(Game1_Player *player, uint8_t interact_pressed) {
  if (transition_cooldown > 0) {
    transition_cooldown--;
  }

  if (!(transition_cooldown == 0 && interact_pressed &&
        Game1_World_PlayerTouchesDoor(player))) {
    return;
  }

  uint16_t tile_x = (player->x + (player->width / 2)) / GAME1_TILE_SIZE;
  uint16_t tile_y = (player->y + (player->height / 2)) / GAME1_TILE_SIZE;
  uint8_t tile = Game1_World_GetTile(tile_x, tile_y);

  // Interact with locked door: unlock it, consume key, do not transition yet
  if (tile == TILE_DOOR_LOCKED) {
    if (!player->has_key) {
      return;
    }

    Game1_World_SetDoorColumn(current_room, tile_x, ROOM0_TO_ROOM2_DOOR_Y, TILE_DOOR);
    player->has_key = 0;
    transition_cooldown = GAME1_TRANSITION_COOLDOWN_FRAMES;
    return;
  }

  if (tile != TILE_DOOR) {
    return;
  }

  // Open door transitions
  if (current_room == 0 && tile_x == ROOM0_TO_ROOM1_DOOR_X) {
    Game1_World_SetCurrentRoom(1);
    Game1_World_SpawnAtDoor(player, ROOM1_TO_ROOM0_DOOR_X, ROOM1_TO_ROOM0_DOOR_Y + 1);
    transition_cooldown = GAME1_TRANSITION_COOLDOWN_FRAMES;
  } else if (current_room == 1 && tile_x == ROOM1_TO_ROOM0_DOOR_X) {
    Game1_World_SetCurrentRoom(0);
    Game1_World_SpawnAtDoor(player, ROOM0_TO_ROOM1_DOOR_X, ROOM0_TO_ROOM1_DOOR_Y + 1);
    transition_cooldown = GAME1_TRANSITION_COOLDOWN_FRAMES;
  } else if (current_room == 0 && tile_x == ROOM0_TO_ROOM2_DOOR_X) {
    Game1_World_SetCurrentRoom(2);
    Game1_World_SpawnAtDoor(player, ROOM2_TO_ROOM0_DOOR_X, ROOM2_TO_ROOM0_DOOR_Y + 1);
    transition_cooldown = GAME1_TRANSITION_COOLDOWN_FRAMES;
  } else if (current_room == 2 && tile_x == ROOM2_TO_ROOM0_DOOR_X) {
    Game1_World_SetCurrentRoom(0);
    Game1_World_SpawnAtDoor(player, ROOM0_TO_ROOM2_DOOR_X, ROOM0_TO_ROOM2_DOOR_Y + 1);
    transition_cooldown = GAME1_TRANSITION_COOLDOWN_FRAMES;
  }
}