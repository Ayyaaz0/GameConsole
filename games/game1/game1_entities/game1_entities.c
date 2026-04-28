#include "game1_entities.h"
#include "LCD.h"
#include "game1_world/game1_world.h"
#include "room0_entities.h"
#include "room0_tiles.h"

#include <stdint.h>

#define GAME1_MAX_DOORS 4
#define GAME1_MAX_KEYS 4
#define GAME1_DOOR_OPEN_TIME 8

#define GAME1_ROOM0_INDEX 0

// Target room settings
#define GAME1_ROOM0_DOOR_TARGET_ROOM 1
#define GAME1_ROOM1_SPAWN_TILE_X 2
#define GAME1_ROOM1_SPAWN_TILE_Y 20

static int16_t spawn_x = 0;
static int16_t spawn_y = 0;
static uint8_t has_spawn = 0;

typedef enum {
  DOOR_LOCKED,
  DOOR_CLOSED,
  DOOR_OPENING,
  DOOR_OPEN
} Game1_DoorState;

typedef struct {
  int16_t x;
  int16_t y;
  uint8_t w;
  uint8_t h;
  uint8_t key_id;
  Game1_DoorState state;
  uint8_t animation_timer;

  uint16_t closed_gid;
  uint16_t opening_gid;
  uint16_t open_gid;
} Game1_Door;

typedef struct {
  int16_t x;
  int16_t y;
  uint8_t w;
  uint8_t h;
  uint8_t key_id;
  uint8_t active;

  uint16_t sprite_gid;
} Game1_Key;

static Game1_Door doors[GAME1_MAX_DOORS];
static uint8_t door_count = 0;

static Game1_Key keys[GAME1_MAX_KEYS];
static uint8_t key_count = 0;

static uint8_t Game1_Entities_IsRoom0Active(void) {
  return Game1_World_GetCurrentRoom() == GAME1_ROOM0_INDEX;
}

static uint8_t Game1_Entities_OverlapsPlayer(const Game1_Player *player, int16_t x, int16_t y, uint8_t w, uint8_t h) {
  return player->x < x + w && player->x + player->width > x &&
         player->y < y + h && player->y + player->height > y;
}

static void Game1_Entities_LoadSpawn(const Game1_Entity *entity) {
  spawn_x = entity->x;
  spawn_y = entity->y;
  has_spawn = 1;
}

static void Game1_Entities_LoadDoor(const Game1_Entity *entity) {
  if (door_count >= GAME1_MAX_DOORS) return;
  Game1_Door *door = &doors[door_count++];
  door->x = entity->x;
  door->y = entity->y;
  door->w = entity->w;
  door->h = entity->h;
  door->key_id = entity->key_id;
  door->state = entity->locked ? DOOR_LOCKED : DOOR_CLOSED;
  door->animation_timer = 0;
  door->closed_gid = entity->closed_gid;
  door->opening_gid = entity->opening_gid;
  door->open_gid = entity->open_gid;
}

static void Game1_Entities_LoadKey(const Game1_Entity *entity) {
  if (key_count >= GAME1_MAX_KEYS) return;
  Game1_Key *key = &keys[key_count++];
  key->x = entity->x;
  key->y = entity->y;
  key->w = entity->w;
  key->h = entity->h;
  key->key_id = entity->key_id;
  key->active = 1;
  key->sprite_gid = entity->sprite_gid;
}

static void Game1_Entities_UpdateKeys(Game1_Player *player) {
  for (uint8_t i = 0; i < key_count; i++) {
    Game1_Key *key = &keys[i];
    if (!key->active) continue;
    if (Game1_Entities_OverlapsPlayer(player, key->x, key->y, key->w, key->h)) {
      player->has_key = 1;
      key->active = 0;
    }
  }
}

// Logic for changing the room
static void Game1_Entities_EnterDoor(Game1_Player *player) {
  Game1_World_SetCurrentRoom(GAME1_ROOM0_DOOR_TARGET_ROOM);
  Game1_World_SpawnAtTile(player, GAME1_ROOM1_SPAWN_TILE_X, GAME1_ROOM1_SPAWN_TILE_Y);
}

static void Game1_Entities_UpdateOpeningDoor(Game1_Door *door) {
  if (door->state != DOOR_OPENING) {
    return;
  }

  door->animation_timer++;

  if (door->animation_timer >= GAME1_DOOR_OPEN_TIME) {
    door->state = DOOR_OPEN;
  }
}

static void Game1_Entities_InteractDoor(Game1_Door *door, Game1_Player *player) {
  if (door->state == DOOR_LOCKED) {
    if (!player->has_key) return;
    player->has_key = 0;
    door->state = DOOR_OPENING;
    door->animation_timer = 0;
    return;
  }

  if (door->state == DOOR_CLOSED) {
    door->state = DOOR_OPENING;
    door->animation_timer = 0;
    return;
  }

  if (door->state == DOOR_OPEN) {
    Game1_Entities_EnterDoor(player);
  }
}

static void Game1_Entities_UpdateDoors(Game1_Player *player, uint8_t interact_pressed) {
  for (uint8_t i = 0; i < door_count; i++) {
    Game1_Door *door = &doors[i];

    Game1_Entities_UpdateOpeningDoor(door);

    if (!interact_pressed) continue;

    if (Game1_Entities_OverlapsPlayer(player, door->x, door->y, door->w, door->h)) {
      Game1_Entities_InteractDoor(door, player);
    }
  }
}

static uint16_t Game1_Entities_GetDoorGid(const Game1_Door *door) {
  switch (door->state) {
  case DOOR_LOCKED:
  case DOOR_CLOSED:
    return door->closed_gid;

  case DOOR_OPENING:
    return door->opening_gid;

  case DOOR_OPEN:
    return door->open_gid;

  default:
    return door->closed_gid;
  }
}

static void Game1_Entities_DrawSprite(int16_t screen_x, int16_t screen_y,
                                      const Game1_TileSprite *sprite) {
  if (sprite == 0 || sprite->pixels == 0) {
    return;
  }

  for (uint8_t y = 0; y < sprite->height; y++) {
    for (uint8_t x = 0; x < sprite->width; x++) {
      uint8_t colour = sprite->pixels[y * sprite->width + x];

      if (colour == 0) {
        continue;
      }

      LCD_Draw_Rect(screen_x + x, screen_y + y, 1, 1, colour, 1);
    }
  }
}

void Game1_Entities_Init(void) {
  has_spawn = 0;
  door_count = 0;
  key_count = 0;

  for (uint16_t i = 0; i < room0_entity_count; i++) {
    const Game1_Entity *entity = &room0_entities[i];

    if (entity->type == ENTITY_SPAWN) {
      Game1_Entities_LoadSpawn(entity);
    } else if (entity->type == ENTITY_DOOR) {
      Game1_Entities_LoadDoor(entity);
    } else if (entity->type == ENTITY_KEY) {
      Game1_Entities_LoadKey(entity);
    }
  }
}

void Game1_Entities_SpawnPlayer(Game1_Player *player) {
  if (!has_spawn) {
    return;
  }

  player->x = spawn_x;
  player->y = spawn_y;
}

void Game1_Entities_Update(Game1_Player *player, uint8_t interact_pressed) {
  if (!Game1_Entities_IsRoom0Active()) {
    return;
  }

  Game1_Entities_UpdateKeys(player);
  Game1_Entities_UpdateDoors(player, interact_pressed);
}

void Game1_Entities_Render(const Game1_Camera *camera) {
  if (!Game1_Entities_IsRoom0Active()) {
    return;
  }

  for (uint8_t i = 0; i < key_count; i++) {
    const Game1_Key *key = &keys[i];

    if (!key->active) {
      continue;
    }

    int16_t screen_x = key->x - camera->x;
    int16_t screen_y = key->y - camera->y;

    const Game1_TileSprite *sprite = Game1_Tiles_Find(key->sprite_gid);
    Game1_Entities_DrawSprite(screen_x, screen_y, sprite);
  }

  for (uint8_t i = 0; i < door_count; i++) {
    const Game1_Door *door = &doors[i];

    int16_t screen_x = door->x - camera->x;
    int16_t screen_y = door->y - camera->y;

    uint16_t door_gid = Game1_Entities_GetDoorGid(door);
    const Game1_TileSprite *sprite = Game1_Tiles_Find(door_gid);

    Game1_Entities_DrawSprite(screen_x, screen_y, sprite);
  }
}