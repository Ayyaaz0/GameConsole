#ifndef RACE_TRACK_LAYOUT_H
#define RACE_TRACK_LAYOUT_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
  RACE_SECTOR_1 = 1,
  RACE_SECTOR_2 = 2,
  RACE_SECTOR_3 = 3
} RaceSector;

typedef enum {
  RACE_TILE_GRASS = 0,
  RACE_TILE_GRASS_DARK,
  RACE_TILE_ROAD,
  RACE_TILE_ROAD_DARK,
  RACE_TILE_CURB_RED,
  RACE_TILE_CURB_WHITE,
  RACE_TILE_START,
  RACE_TILE_CHECKPOINT,
  RACE_TILE_JOKER,
  RACE_TILE_SAND,
  RACE_TILE_TYRE,
  RACE_TILE_BARRIER,
  RACE_TILE_BUILDING,
  RACE_TILE_TREE
} RaceTileType;

typedef struct {
  uint16_t x;
  uint16_t y;
  uint16_t width;
  uint16_t height;
} RaceTriggerZone;

bool RaceTrackLayout_IsDriveableTile(RaceTileType tile);
bool RaceTrackLayout_IsSlowTile(RaceTileType tile);

#endif