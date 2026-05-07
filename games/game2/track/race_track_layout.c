#include "race_track_layout.h"

bool RaceTrackLayout_IsDriveableTile(RaceTileType tile) {
  return ((tile == RACE_TILE_ROAD) || (tile == RACE_TILE_ROAD_DARK) ||
          (tile == RACE_TILE_START) || (tile == RACE_TILE_CHECKPOINT) ||
          (tile == RACE_TILE_JOKER));
}

bool RaceTrackLayout_IsSlowTile(RaceTileType tile) {
  (void)tile;
  return false;
}