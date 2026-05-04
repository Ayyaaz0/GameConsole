#include "race_collisions.h"

#include <stddef.h>

void RaceCollision_Reset(RaceCollisionState *collision) {
  if (collision == NULL) {
    return;
  }

  collision->hit_left_edge = false;
  collision->hit_right_edge = false;
  collision->hit_any_edge = false;
}

static bool RaceCollision_PositionIsDriveable(const RaceTrack *track,
                                              const RaceCar *car,
                                              float x,
                                              float y) {
  if ((track == NULL) || (car == NULL)) {
    return false;
  }

  return RaceTrack_CarIsDriveable(track, x, y, car->width, car->height);
}

static bool RaceCollision_TileIsCurb(RaceTileType tile) {
  return ((tile == RACE_TILE_CURB_RED) || (tile == RACE_TILE_CURB_WHITE));
}

static void RaceCollision_HandleCurbDamage(RaceCar *player_car,
                                           const RaceTrack *track) {
  float center_x = 0.0f;
  float center_y = 0.0f;
  RaceTileType tile = RACE_TILE_ROAD;

  if ((player_car == NULL) || (track == NULL)) {
    return;
  }

  if (player_car->speed < RACE_CURB_DAMAGE_SPEED_THRESHOLD) {
    return;
  }

  if (player_car->curb_damage_cooldown > 0U) {
    return;
  }

  center_x = player_car->x + ((float)player_car->width * 0.5f);
  center_y = player_car->y + ((float)player_car->height * 0.5f);

  tile = RaceTrack_GetTileAtWorld(track, center_x, center_y);

  if (RaceCollision_TileIsCurb(tile)) {
    RaceCar_AddDamage(player_car, RACE_CURB_DAMAGE_AMOUNT);
    player_car->curb_damage_cooldown = RACE_CURB_DAMAGE_COOLDOWN_FRAMES;
  }
}

void RaceCollision_HandleRoadEdges(RaceCollisionState *collision,
                                   RaceCar *player_car,
                                   const RaceTrack *track) {
  bool full_position_ok = false;
  bool x_only_ok = false;
  bool y_only_ok = false;

  if ((collision == NULL) || (player_car == NULL) || (track == NULL)) {
    return;
  }

  RaceCollision_Reset(collision);

  RaceCollision_HandleCurbDamage(player_car, track);


  full_position_ok =
      RaceCollision_PositionIsDriveable(track, player_car,
                                        player_car->x,
                                        player_car->y);

  if (full_position_ok == true) {
    return;
  }

  x_only_ok =
      RaceCollision_PositionIsDriveable(track, player_car,
                                        player_car->x,
                                        player_car->prev_y);

  y_only_ok =
      RaceCollision_PositionIsDriveable(track, player_car,
                                        player_car->prev_x,
                                        player_car->y);

  collision->hit_any_edge = true;
  collision->hit_left_edge = true;
  collision->hit_right_edge = true;

  if ((x_only_ok == true) && (y_only_ok == false)) {
    RaceCar_StopY(player_car);
    RaceCar_ApplyWallSlowdown(player_car);
    return;
  }

  if ((y_only_ok == true) && (x_only_ok == false)) {
    RaceCar_StopX(player_car);
    RaceCar_ApplyWallSlowdown(player_car);
    return;
  }

  if ((x_only_ok == true) && (y_only_ok == true)) {
    if ((player_car->vx > player_car->vy) ||
        ((-player_car->vx) > player_car->vy)) {
      RaceCar_StopY(player_car);
    } else {
      RaceCar_StopX(player_car);
    }

    RaceCar_ApplyWallSlowdown(player_car);
    return;
  }

  RaceCar_RestorePreviousPosition(player_car);
  RaceCar_ApplyWallSlowdown(player_car);
}