#include "game1_door.h"

#include "game1_animation.h"
#include "game1_entities.h"
#include "game1_entity_common.h"
#include "game1_tiles.h"
#include "game1_world/game1_world.h"
#include "game1_audio/game1_sfx.h"

#define GAME1_ROOM0_DOOR_TARGET_ROOM 1
#define GAME1_ROOM1_SPAWN_TILE_X 2
#define GAME1_ROOM1_SPAWN_TILE_Y 20

#define GAME1_DOOR_FRAME_COUNT 3
#define GAME1_DOOR_FRAME_SPEED 4

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

/*
 * Used to stop the door update loop immediately after changing rooms.
 * This avoids continuing to iterate over a door array that has just been reset.
 */
static uint8_t room_changed_this_frame = 0;

void Game1_Door_Reset(void) {
  door_count = 0;
  room_changed_this_frame = 0;
}

void Game1_Door_Load(const Game1_Entity *entity) {
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

static void enter_door(Game1_Player *player) {
  Game1_World_SetCurrentRoom(GAME1_ROOM0_DOOR_TARGET_ROOM);

  /*
   * Reload keys, doors, coins, and spawn data for the new room.
   * This keeps one entity system instead of duplicating logic per room.
   */
  Game1_Entities_Init();

  Game1_World_SpawnAtTile(player, GAME1_ROOM1_SPAWN_TILE_X, GAME1_ROOM1_SPAWN_TILE_Y);

  room_changed_this_frame = 1;
}

static void start_opening(Game1_Door *door) {
  door->state = DOOR_OPENING;
  Game1_Animation_Init(&door->animation_state, &door->open_animation);
}

static void update_opening(Game1_Door *door) {
  if (door->state != DOOR_OPENING) {
    return;
  }

  Game1_Animation_Update(&door->animation_state);

  if (door->animation_state.timer >=
      door->open_animation.frame_count * door->open_animation.speed) {
    door->state = DOOR_OPEN;
  }
}

static void interact_door(Game1_Door *door, Game1_Player *player) {
  if (door->state == DOOR_LOCKED) {
    if (!player->has_key) {
      Game1_Sfx_PlayDoorLocked();
      return;
    }

    player->has_key = 0;
    start_opening(door);
    Game1_Sfx_PlayDoorOpen();
    return;
  }

  if (door->state == DOOR_CLOSED) {
    start_opening(door);
    Game1_Sfx_PlayDoorOpen();
    return;
  }

  if (door->state == DOOR_OPEN) {
    Game1_Sfx_PlayDoorEnter();
    enter_door(player);
  }
}

void Game1_Door_UpdateAll(Game1_Player *player, uint8_t interact_pressed) {
  room_changed_this_frame = 0;

  for (uint8_t i = 0; i < door_count; i++) {
    Game1_Door *door = &doors[i];

    update_opening(door);

    if (!interact_pressed) {
      continue;
    }

    if (Game1_Entity_OverlapsPlayer(player, door->x, door->y, door->w, door->h)) {
      interact_door(door, player);

      if (room_changed_this_frame) {
        return;
      }
    }
  }
}

static uint16_t get_door_gid(const Game1_Door *door) {
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

void Game1_Door_RenderAll(const Game1_Camera *camera) {
  for (uint8_t i = 0; i < door_count; i++) {
    const Game1_Door *door = &doors[i];

    int16_t screen_x = door->x - camera->x;
    int16_t screen_y = door->y - camera->y;

    if (!Game1_Entity_IsVisibleOnScreen(screen_x, screen_y, door->w, door->h)) {
      continue;
    }

    uint16_t door_gid = get_door_gid(door);

    const Game1_TileSprite *sprite = Game1_Tiles_Find(door_gid);

    if (sprite == 0) {
      continue;
    }

    Game1_Entity_DrawSprite(screen_x, screen_y, sprite);
  }
}