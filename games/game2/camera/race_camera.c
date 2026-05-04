#include "race_camera.h"

#include "../config/race_config.h"
#include "../utils/race_math.h"

#include <stddef.h>

static float RaceCamera_GetClampedX(float camera_x) {
  return RaceMath_ClampFloat(camera_x, 0.0f,
                             (float)(RACE_WORLD_WIDTH_PX - RACE_VIEW_WIDTH_PX));
}

static float RaceCamera_GetClampedY(float camera_y) {
  return RaceMath_ClampFloat(
      camera_y, 0.0f, (float)(RACE_WORLD_HEIGHT_PX - RACE_VIEW_HEIGHT_PX));
}

static float RaceCamera_GetCarCentreX(const RaceCar *player_car) {
  return player_car->x + ((float)player_car->width * 0.5f);
}

static float RaceCamera_GetCarCentreY(const RaceCar *player_car) {
  return player_car->y + ((float)player_car->height * 0.5f);
}

void RaceCamera_Init(RaceCamera *camera) {
  if (camera == NULL) {
    return;
  }

  camera->x = 0.0f;
  camera->y = 0.0f;
  camera->target_x = 0.0f;
  camera->target_y = 0.0f;
  camera->smoothing = RACE_CAMERA_SMOOTHING;
  camera->active = true;
}

void RaceCamera_Reset(RaceCamera *camera, const RaceCar *player_car) {
  if ((camera == NULL) || (player_car == NULL)) {
    return;
  }

  camera->target_x = RaceCamera_GetClampedX(
      RaceCamera_GetCarCentreX(player_car) - RACE_CAMERA_TARGET_X);

  camera->target_y = RaceCamera_GetClampedY(
      RaceCamera_GetCarCentreY(player_car) - RACE_CAMERA_TARGET_Y);

  camera->x = camera->target_x;
  camera->y = camera->target_y;
}

void RaceCamera_Update(RaceCamera *camera, const RaceCar *player_car,
                       RaceTrack *track) {
  float dx = 0.0f;
  float dy = 0.0f;

  if ((camera == NULL) || (player_car == NULL) || (track == NULL) ||
      (camera->active == false)) {
    return;
  }

  camera->target_x = RaceCamera_GetClampedX(
      RaceCamera_GetCarCentreX(player_car) - RACE_CAMERA_TARGET_X);

  camera->target_y = RaceCamera_GetClampedY(
      RaceCamera_GetCarCentreY(player_car) - RACE_CAMERA_TARGET_Y);

  dx = camera->target_x - camera->x;
  dy = camera->target_y - camera->y;

  // Smooth follow.
  camera->x += dx * camera->smoothing;
  camera->y += dy * camera->smoothing;

  // Snap tiny sub-pixel camera errors to avoid micro-jitter.
  if ((dx > -0.05f) && (dx < 0.05f)) {
    camera->x = camera->target_x;
  }

  if ((dy > -0.05f) && (dy < 0.05f)) {
    camera->y = camera->target_y;
  }

  RaceTrack_SetCamera(track, camera->x, camera->y);
}