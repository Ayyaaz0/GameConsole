#include "race_camera.h"

#include "../config/race_config.h"

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
  (void)player_car;
  (void)track;

  if ((camera == NULL) || (camera->active == false)) {
    return;
  }

  camera->world_top_y = 0;
}