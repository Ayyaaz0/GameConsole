#include "race_camera.h"
#include "../config/race_config.h"
#include "../utils/race_math.h"

#include <stddef.h>

static float RaceCamera_GetDesiredX(const RaceCamera *camera,
                                    const RaceCar *player_car) {
  return player_car->x + ((float)player_car->width * 0.5f) -
         camera->target_screen_x;
}

static float RaceCamera_GetDesiredY(const RaceCamera *camera,
                                    const RaceCar *player_car) {
  return player_car->y + ((float)player_car->height * 0.5f) -
         camera->target_screen_y;
}

static void RaceCamera_ClampToWorld(RaceCamera *camera) {
  camera->x = RaceMath_ClampFloat(
      camera->x, 0.0f, (float)(RACE_WORLD_WIDTH_PX - RACE_SCREEN_WIDTH));
  camera->y = RaceMath_ClampFloat(
      camera->y, 0.0f, (float)(RACE_WORLD_HEIGHT_PX - RACE_SCREEN_HEIGHT));
}

void RaceCamera_Init(RaceCamera *camera) {
  if (camera == NULL) {
    return;
  }

  camera->x = 0.0f;
  camera->y = 0.0f;
  camera->target_screen_x = RACE_CAMERA_TARGET_X;
  camera->target_screen_y = RACE_CAMERA_TARGET_Y;
  camera->smoothing = RACE_CAMERA_SMOOTHING;
  camera->active = true;
}

void RaceCamera_Reset(RaceCamera *camera, const RaceCar *player_car) {
  if ((camera == NULL) || (player_car == NULL)) {
    return;
  }

  camera->x = RaceCamera_GetDesiredX(camera, player_car);
  camera->y = RaceCamera_GetDesiredY(camera, player_car);
  RaceCamera_ClampToWorld(camera);
}

void RaceCamera_Update(RaceCamera *camera, const RaceCar *player_car,
                       RaceTrack *track) {
  float desired_x = 0.0f;
  float desired_y = 0.0f;

  if ((camera == NULL) || (player_car == NULL) || (track == NULL) ||
      (camera->active == false)) {
    return;
  }

  desired_x = RaceCamera_GetDesiredX(camera, player_car);
  desired_y = RaceCamera_GetDesiredY(camera, player_car);

  camera->x += (desired_x - camera->x) * camera->smoothing;
  camera->y += (desired_y - camera->y) * camera->smoothing;

  RaceCamera_ClampToWorld(camera);
  RaceTrack_SetCamera(track, camera->x, camera->y);
}