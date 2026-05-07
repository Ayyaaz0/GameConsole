#include "race_collisions.h"
#include "../config/race_config.h"
#include "../track/race_track_layout.h"

#include <stddef.h>

void RaceCollision_Reset(RaceCollisionState *collision) {
  if (collision == NULL) {
    return;
  }

  collision->hit_left_edge = false;
  collision->hit_right_edge = false;
  collision->hit_any_edge = false;
}

static void RaceCollision_ApplyPenalty(RaceCar *player_car) {
  RaceCar_RestorePreviousPosition(player_car);
  RaceCar_ApplySpeedMultiplier(player_car,
                               RACE_COLLISION_EDGE_SPEED_MULTIPLIER);
}

static void RaceCollision_ApplySurfaceEffects(RaceCar *player_car,
                                              const RaceTrack *track) {
  float center_x = 0.0f;
  float center_y = 0.0f;
  RaceTileType tile = RACE_TILE_GRASS;

  if ((player_car == NULL) || (track == NULL)) {
    return;
  }

  center_x = player_car->x + ((float)player_car->width * 0.5f);
  center_y = player_car->y + ((float)player_car->height * 0.5f);
  tile = RaceTrack_GetTileAtWorld(track, center_x, center_y);

  if (tile == RACE_TILE_SAND) {
    RaceCar_ApplySpeedMultiplier(player_car,
                                 RACE_SURFACE_SAND_SPEED_MULTIPLIER);
  }

  if ((tile == RACE_TILE_CURB_RED) || (tile == RACE_TILE_CURB_WHITE)) {
    RaceCar_ApplySpeedMultiplier(player_car,
                                 RACE_SURFACE_CURB_SPEED_MULTIPLIER);
  }
}

void RaceCollision_HandleRoadEdges(RaceCollisionState *collision,
                                   RaceCar *player_car,
                                   const RaceTrack *track) {
  if ((collision == NULL) || (player_car == NULL) || (track == NULL)) {
    return;
  }

  RaceCollision_Reset(collision);

  if (RaceTrack_CarIsDriveable(track, player_car->x, player_car->y,
                               player_car->width,
                               player_car->height) == false) {
    collision->hit_any_edge = true;
    RaceCollision_ApplyPenalty(player_car);
    return;
  }

  RaceCollision_ApplySurfaceEffects(player_car, track);
}