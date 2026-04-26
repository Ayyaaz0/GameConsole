#ifndef RACE_COLLISION_H
#define RACE_COLLISION_H

#include <stdbool.h>

#include "../car/race_car.h"
#include "../track/race_track.h"

typedef struct {
  bool hit_left_edge;
  bool hit_right_edge;
  bool hit_any_edge;
} RaceCollisionState;

// Reset collision flags
void RaceCollision_Reset(RaceCollisionState *collision);

// Check and resolve player collision with the road edges
void RaceCollision_HandleRoadEdges(RaceCollisionState *collision,
                                   RaceCar *player_car, const RaceTrack *track);

#endif