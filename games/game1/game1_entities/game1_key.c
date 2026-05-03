#include "game1_key.h"

#include "game1_entity_common.h"
#include "game1_tiles.h"

static Game1_Key keys[GAME1_MAX_KEYS];
static uint8_t key_count = 0;

void Game1_Key_Reset(void) { key_count = 0; }

void Game1_Key_Load(const Game1_Entity *entity) {
  if (key_count >= GAME1_MAX_KEYS) {
    return;
  }

  Game1_Key *key = &keys[key_count++];

  key->x = entity->x;
  key->y = entity->y;
  key->w = entity->w;
  key->h = entity->h;
  key->key_id = entity->key_id;
  key->active = 1;
  key->sprite_gid = entity->sprite_gid;
}

void Game1_Key_UpdateAll(Game1_Player *player) {
  for (uint8_t i = 0; i < key_count; i++) {
    Game1_Key *key = &keys[i];

    if (!key->active) {
      continue;
    }

    if (Game1_Entity_OverlapsPlayer(player, key->x, key->y, key->w, key->h)) {
      player->has_key = 1;
      key->active = 0;
    }
  }
}

void Game1_Key_RenderAll(const Game1_Camera *camera) {
  for (uint8_t i = 0; i < key_count; i++) {
    const Game1_Key *key = &keys[i];

    if (!key->active) {
      continue;
    }

    int16_t screen_x = key->x - camera->x;
    int16_t screen_y = key->y - camera->y;

    if (!Game1_Entity_IsVisibleOnScreen(screen_x, screen_y, key->w, key->h)) {
      continue;
    }
    
    const Game1_TileSprite *sprite = Game1_Tiles_Find(key->sprite_gid);
    Game1_Entity_DrawSprite(screen_x, screen_y, sprite);
  }
}