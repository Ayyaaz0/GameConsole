#include "game1_camera.h"

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

void Game1_Camera_Update(Game1_Camera *camera, int16_t target_x,
                         int16_t target_y, uint16_t world_width_px,
                         uint16_t world_height_px) {
  int16_t desired_x = target_x - (camera->width / 2);
  int16_t desired_y = target_y - (camera->height / 2);

  int16_t max_x = world_width_px - camera->width;
  int16_t max_y = world_height_px - camera->height;

  if (max_x < 0) {
    max_x = 0;
  }

  if (max_y < 0) {
    max_y = 0;
  }

  camera->x = Game1_Camera_Clamp(desired_x, 0, max_x);
  camera->y = Game1_Camera_Clamp(desired_y, 0, max_y);
}