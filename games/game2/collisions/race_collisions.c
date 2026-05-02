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

  player_car->speed *= RACE_COLLISION_EDGE_SPEED_MULTIPLIER;
}

static bool RaceCollision_CarIsStillOnRoad(const RaceTrack *track,
                                           const RaceCar *car) {
  float left = 0.0f;
  float right = 0.0f;
  float top = 0.0f;
  float bottom = 0.0f;
  float centre_x = 0.0f;
  float centre_y = 0.0f;
  int points_on_road = 0;

  if ((track == NULL) || (car == NULL)) {
    return false;
  }

  left = car->x + 2.0f;
  right = car->x + (float)car->width - 2.0f;
  top = car->y + 2.0f;
  bottom = car->y + (float)car->height - 2.0f;

  centre_x = car->x + ((float)car->width * 0.5f);
  centre_y = car->y + ((float)car->height * 0.5f);

  if (RaceTrack_PointIsOnRoad(track, centre_x, centre_y)) {
    points_on_road++;
  }

  if (RaceTrack_PointIsOnRoad(track, left, top)) {
    points_on_road++;
  }

  if (RaceTrack_PointIsOnRoad(track, right, top)) {
    points_on_road++;
  }

  if (RaceTrack_PointIsOnRoad(track, left, bottom)) {
    points_on_road++;
  }

  if (RaceTrack_PointIsOnRoad(track, right, bottom)) {
    points_on_road++;
  }

  return points_on_road >= 2;
}

void RaceCollision_HandleRoadEdges(RaceCollisionState *collision,
                                   RaceCar *player_car,
                                   const RaceTrack *track) {
  if ((collision == NULL) || (player_car == NULL) || (track == NULL)) {
    return;
  }

  RaceCollision_Reset(collision);

  if (RaceCollision_CarIsStillOnRoad(track, player_car) == false) {
    RaceCar_RestorePreviousPosition(player_car);

    collision->hit_left_edge = true;
    collision->hit_right_edge = true;
    collision->hit_any_edge = true;

    RaceCollision_ApplyEdgePenalty(player_car);
  }
}