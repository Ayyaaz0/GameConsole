#ifndef GAME1_KEY_H
#define GAME1_KEY_H

#include "game1_camera.h"
#include "game1_player.h"
#include "room0_entities.h"

#include <stdint.h>

#define GAME1_MAX_KEYS 4

typedef struct {
  int16_t x;
  int16_t y;
  uint8_t w;
  uint8_t h;
  uint8_t key_id;
  uint8_t active;
  uint16_t sprite_gid;
} Game1_Key;

void Game1_Key_Reset(void);
void Game1_Key_Load(const Game1_Entity *entity);
void Game1_Key_UpdateAll(Game1_Player *player);
void Game1_Key_RenderAll(const Game1_Camera *camera);

#endif