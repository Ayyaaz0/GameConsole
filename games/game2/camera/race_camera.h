#ifndef RACE_CAMERA_H
#define RACE_CAMERA_H

#include "../car/race_car.h"
#include "../track/race_track.h"

#include <stdbool.h>

typedef struct {
  float x;
  float y;
  float target_screen_x;
  float target_screen_y;
  float smoothing;
  bool active;
} RaceCamera;

void RaceCamera_Init(RaceCamera *camera);
void RaceCamera_Reset(RaceCamera *camera, const RaceCar *player_car);
void RaceCamera_Update(RaceCamera *camera, const RaceCar *player_car,
                       RaceTrack *track);

#endif