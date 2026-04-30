#include "game1_entities.h"
#include "game1_entity_common.h"

#include "game1_animation.h"

#include "game1_world/game1_world.h"

#include "game1_key.h"

#include "room0_entities.h"
#include "room0_tiles.h"

#include <stdint.h>

#define GAME1_MAX_DOORS 4

#define GAME1_ROOM0_INDEX 0

#define GAME1_ROOM0_DOOR_TARGET_ROOM 1
#define GAME1_ROOM1_SPAWN_TILE_X 2
#define GAME1_ROOM1_SPAWN_TILE_Y 20

#define GAME1_DOOR_FRAME_COUNT 3
#define GAME1_DOOR_FRAME_SPEED 4

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

  uint16_t closed_gid;
  uint16_t opening_gid;
  uint16_t open_gid;

  uint16_t open_frames[GAME1_DOOR_FRAME_COUNT];
  Game1_Animation open_animation;
  Game1_AnimationState animation_state;
} Game1_Door;

static Game1_Door doors[GAME1_MAX_DOORS];
static uint8_t door_count = 0;

static uint8_t Game1_Entities_IsRoom0Active(void) {
  return Game1_World_GetCurrentRoom() == GAME1_ROOM0_INDEX;
}

static void Game1_Entities_LoadSpawn(const Game1_Entity *entity) {
  spawn_x = entity->x;
  spawn_y = entity->y;
  has_spawn = 1;
}

static void Game1_Entities_LoadDoor(const Game1_Entity *entity) {
  if (door_count >= GAME1_MAX_DOORS) {
    return;
  }

  Game1_Door *door = &doors[door_count++];

  door->x = entity->x;
  door->y = entity->y;
  door->w = entity->w;
  door->h = entity->h;
  door->key_id = entity->key_id;

  door->closed_gid = entity->closed_gid;
  door->opening_gid = entity->opening_gid;
  door->open_gid = entity->open_gid;

  door->open_frames[0] = door->closed_gid;
  door->open_frames[1] = door->opening_gid;
  door->open_frames[2] = door->open_gid;

  door->open_animation.frames = door->open_frames;
  door->open_animation.frame_count = GAME1_DOOR_FRAME_COUNT;
  door->open_animation.speed = GAME1_DOOR_FRAME_SPEED;

  door->state = entity->locked ? DOOR_LOCKED : DOOR_CLOSED;
  Game1_Animation_Init(&door->animation_state, &door->open_animation);
}

static void Game1_Entities_EnterDoor(Game1_Player *player) {
  Game1_World_SetCurrentRoom(GAME1_ROOM0_DOOR_TARGET_ROOM);
  Game1_World_SpawnAtTile(player, GAME1_ROOM1_SPAWN_TILE_X, GAME1_ROOM1_SPAWN_TILE_Y);
}

static void Game1_Entities_StartDoorOpening(Game1_Door *door) {
  door->state = DOOR_OPENING;
  Game1_Animation_Init(&door->animation_state, &door->open_animation);
}

static void Game1_Entities_UpdateOpeningDoor(Game1_Door *door) {
  if (door->state != DOOR_OPENING) {
    return;
  }

  Game1_Animation_Update(&door->animation_state);

  if (door->animation_state.timer >=
      door->open_animation.frame_count * door->open_animation.speed) {
    door->state = DOOR_OPEN;
  }
}

static void Game1_Entities_InteractDoor(Game1_Door *door, Game1_Player *player) {
  if (door->state == DOOR_LOCKED) {
    if (!player->has_key) {
      return;
    }

    player->has_key = 0;
    Game1_Entities_StartDoorOpening(door);
    return;
  }

  if (door->state == DOOR_CLOSED) {
    Game1_Entities_StartDoorOpening(door);
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

    if (!interact_pressed) {
      continue;
    }

    if (Game1_Entity_OverlapsPlayer(player, door->x, door->y, door->w, door->h)) {
      Game1_Entities_InteractDoor(door, player);
    }
  }
}

static uint16_t Game1_Entities_GetDoorGid(const Game1_Door *door) {
  if (door->state == DOOR_OPENING) {
    uint8_t frame_index =
        door->animation_state.timer / door->open_animation.speed;

    if (frame_index >= door->open_animation.frame_count) {
      frame_index = door->open_animation.frame_count - 1;
    }

    return door->open_animation.frames[frame_index];
  }

  if (door->state == DOOR_OPEN) {
    return door->open_gid;
  }

  return door->closed_gid;
}

void Game1_Entities_Init(void) {
  has_spawn = 0;
  door_count = 0;
  Game1_Key_Reset();

  for (uint16_t i = 0; i < room0_entity_count; i++) {
    const Game1_Entity *entity = &room0_entities[i];

    if (entity->type == ENTITY_SPAWN) {
      Game1_Entities_LoadSpawn(entity);
    } else if (entity->type == ENTITY_DOOR) {
      Game1_Entities_LoadDoor(entity);
    } else if (entity->type == ENTITY_KEY) {
      Game1_Key_Load(entity);
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

  Game1_Key_UpdateAll(player);
  Game1_Entities_UpdateDoors(player, interact_pressed);
}

void Game1_Entities_Render(const Game1_Camera *camera) {
  if (!Game1_Entities_IsRoom0Active()) {
    return;
  }

  Game1_Key_RenderAll(camera);

  for (uint8_t i = 0; i < door_count; i++) {
    const Game1_Door *door = &doors[i];
    uint16_t door_gid = Game1_Entities_GetDoorGid(door);
    const Game1_TileSprite *sprite = Game1_Tiles_Find(door_gid);

    Game1_Entity_DrawSprite(door->x - camera->x, door->y - camera->y, sprite);
  }
}