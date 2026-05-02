#ifndef GAME1_CAMERA_H
#define GAME1_CAMERA_H

#include "game1_player.h"

#include <stdint.h>

typedef struct {
  int16_t x;
  int16_t y;
  uint16_t width;
  uint16_t height;
} Game1_Camera;

void Game1_Camera_Init(Game1_Camera *camera, uint16_t width, uint16_t height);
void Game1_Camera_Update(Game1_Camera *camera, const Game1_Player *player);

#endif