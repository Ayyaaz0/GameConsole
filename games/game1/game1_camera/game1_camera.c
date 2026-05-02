#include "game1_camera.h"

#include "game1_world/game1_world.h"

#define GAME1_SCREEN_WIDTH 240
#define GAME1_SCREEN_HEIGHT 240

static int16_t Game1_Camera_Clamp(int16_t value, int16_t min, int16_t max) {
  if (value < min) {
    return min;
  }

  if (value > max) {
    return max;
  }

  return value;
}

void Game1_Camera_Init(Game1_Camera *camera, uint16_t width, uint16_t height) {
  camera->x = 0;
  camera->y = 0;
  camera->width = width;
  camera->height = height;
}

void Game1_Camera_Update(Game1_Camera *camera, const Game1_Player *player) {
  int16_t max_camera_x =
      Game1_World_GetCurrentRoomWidthPx() - GAME1_SCREEN_WIDTH;

  if (max_camera_x < 0) {
    max_camera_x = 0;
  }

  camera->x = player->x - (GAME1_SCREEN_WIDTH / 2);
  camera->x = Game1_Camera_Clamp(camera->x, 0, max_camera_x);

  camera->y = 0;
}