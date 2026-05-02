#ifndef RACE_CAMERA_H
#define RACE_CAMERA_H

#include <stdbool.h>
#include <stdint.h>

#include "../car/race_car.h"
#include "../track/race_track.h"

typedef struct {
  float target_screen_y;
  float smoothing;
  int32_t world_top_y;
  bool active;
} RaceCamera;

void RaceCamera_Init(RaceCamera *camera);
void RaceCamera_Update(RaceCamera *camera, const RaceCar *player_car,
                       RaceTrack *track);

#endif