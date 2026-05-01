#include "game1_entities.h"

#include "game1_coin.h"
#include "game1_door.h"
#include "game1_key.h"
#include "game1_world/game1_world.h"

#include "room0_entities.h"

#include <stdint.h>

extern const Game1_Entity room1_entities[];
extern const uint16_t room1_entity_count;

static int16_t spawn_x = 0;
static int16_t spawn_y = 0;
static uint8_t has_spawn = 0;

static void Game1_Entities_LoadSpawn(const Game1_Entity *entity) {
  spawn_x = entity->x;
  spawn_y = entity->y;
  has_spawn = 1;
}

static const Game1_Entity *Game1_Entities_GetCurrentRoomEntities(
    uint16_t *count) {
  switch (Game1_World_GetCurrentRoom()) {
  case 0:
    *count = room0_entity_count;
    return room0_entities;

  case 1:
    *count = room1_entity_count;
    return room1_entities;

  default:
    *count = 0;
    return 0;
  }
}

void Game1_Entities_Init(void) {
  has_spawn = 0;

  /*
   * Each room owns its own active entity set.
   * Reset module state before loading the current room's generated objects.
   */
  Game1_Key_Reset();
  Game1_Door_Reset();
  Game1_Coin_Reset();

  uint16_t entity_count = 0;
  const Game1_Entity *entities =
      Game1_Entities_GetCurrentRoomEntities(&entity_count);

  if (entities == 0) {
    return;
  }

  for (uint16_t i = 0; i < entity_count; i++) {
    const Game1_Entity *entity = &entities[i];

    if (entity->type == ENTITY_SPAWN) {
      Game1_Entities_LoadSpawn(entity);
    } else if (entity->type == ENTITY_KEY) {
      Game1_Key_Load(entity);
    } else if (entity->type == ENTITY_DOOR) {
      Game1_Door_Load(entity);
    } else if (entity->type == ENTITY_COIN) {
      Game1_Coin_Load(entity);
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
  Game1_Key_UpdateAll(player);
  Game1_Door_UpdateAll(player, interact_pressed);
  Game1_Coin_UpdateAll(player);
}

void Game1_Entities_Render(const Game1_Camera *camera) {
  Game1_Key_RenderAll(camera);
  Game1_Door_RenderAll(camera);
  Game1_Coin_RenderAll(camera);
}