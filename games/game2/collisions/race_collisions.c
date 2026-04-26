#include "race_collisions.h"
#include "../config/race_config.h"

#include <stddef.h>

void RaceCollision_Reset(RaceCollisionState *collision) {
  if (collision == NULL) {
    return;
  }

  collision->hit_left_edge = false;
  collision->hit_right_edge = false;
  collision->hit_any_edge = false;
}

static void RaceCollision_ApplyEdgePenalty(RaceCar *player_car) {
  if (player_car == NULL) {
    return;
  }

  // Hitting the edge should punish speed but not instantly crash yet
  player_car->speed *= RACE_COLLISION_EDGE_SPEED_MULTIPLIER;

  // Reduce heading so the car stops scraping along the wall forever
  player_car->heading_deg *= RACE_COLLISION_EDGE_HEADING_MULTIPLIER;
}

void RaceCollision_HandleRoadEdges(RaceCollisionState *collision,
                                   RaceCar *player_car,
                                   const RaceTrack *track) {
  int16_t min_x = 0;
  int16_t max_x = 0;
  float min_car_x = 0.0f;
  float max_car_x = 0.0f;

  if ((collision == NULL) || (player_car == NULL) || (track == NULL)) {
    return;
  }

  RaceCollision_Reset(collision);
  RaceTrack_GetDriveBounds(track, &min_x, &max_x);

  min_car_x = (float)min_x;
  max_car_x = (float)(max_x - (int16_t)player_car->width);

  if (player_car->x < min_car_x) {
    player_car->x = min_car_x + 3.0f;
    collision->hit_left_edge = true;
    collision->hit_any_edge = true;
    RaceCollision_ApplyEdgePenalty(player_car);
  }

  if (player_car->x > max_car_x) {
    player_car->x = max_car_x - 3.0f;
    collision->hit_right_edge = true;
    collision->hit_any_edge = true;
    RaceCollision_ApplyEdgePenalty(player_car);
  }
}