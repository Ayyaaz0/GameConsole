#include "race_camera.h"
#include "../config/race_config.h"
#include "../utils/race_math.h"

#include <stddef.h>

void RaceCamera_Init(RaceCamera *camera) {
  if (camera == NULL) {
    return;
  }

  camera->target_screen_y = RACE_CAMERA_TARGET_Y;
  camera->smoothing = RACE_CAMERA_SMOOTHING;
  camera->world_top_y = 0;
  camera->active = true;
}

void RaceCamera_Update(RaceCamera *camera, const RaceCar *player_car,
                       RaceTrack *track) {
  float desired_world_top = 0.0f;
  float current_world_top = 0.0f;
  float smoothed_world_top = 0.0f;

  if ((camera == NULL) || (player_car == NULL) || (track == NULL) ||
      (camera->active == false)) {
    return;
  }

  // Convert player screen position into a desired world-top position
  desired_world_top = player_car->y - camera->target_screen_y;

  // Camera should not scroll above the start of the world
  if (desired_world_top < 0.0f) {
    desired_world_top = 0.0f;
  }

  current_world_top = (float)RaceTrack_GetWorldTopY(track);

  // Smooth camera movement so it follows without snapping too harshly
  smoothed_world_top =
      current_world_top +
      ((desired_world_top - current_world_top) * camera->smoothing);

  camera->world_top_y = (int32_t)smoothed_world_top;

  RaceTrack_SetWorldTopY(track, camera->world_top_y);
}