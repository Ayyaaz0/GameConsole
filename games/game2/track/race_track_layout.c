#include "race_track_layout.h"

bool RaceTrackLayout_IsDriveableTile(RaceTileType tile) {
  return ((tile == RACE_TILE_ROAD) || (tile == RACE_TILE_ROAD_EDGE) ||
          (tile == RACE_TILE_CURB_RED) || (tile == RACE_TILE_CURB_WHITE) ||
          (tile == RACE_TILE_START) || (tile == RACE_TILE_CHECKPOINT) ||
          (tile == RACE_TILE_JOKER));
}