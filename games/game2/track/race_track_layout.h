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
  RACE_TRACK_STRAIGHT_VERTICAL = 0,
  RACE_TRACK_STRAIGHT_HORIZONTAL,
  RACE_TRACK_TURN_UP_RIGHT,
  RACE_TRACK_TURN_LEFT_UP,
  RACE_TRACK_TURN_UP_LEFT,
  RACE_TRACK_TURN_RIGHT_UP
} RaceTrackTileType;

typedef enum {
  RACE_EDGE_TOP = 0,
  RACE_EDGE_RIGHT,
  RACE_EDGE_BOTTOM,
  RACE_EDGE_LEFT
} RaceTrackEdge;

typedef enum {
  RACE_CURB_NONE = 0,
  RACE_CURB_LEFT,
  RACE_CURB_RIGHT,
  RACE_CURB_BOTH
} RaceCurbPlacement;

#define RACE_CURB_LEFT_APEX RACE_CURB_LEFT
#define RACE_CURB_RIGHT_APEX RACE_CURB_RIGHT
#define RACE_CURB_BOTH_APEX RACE_CURB_BOTH

typedef struct {
  int16_t x;
  int16_t y;
  uint16_t width;
  uint16_t height;
} RaceRect;

typedef struct {
  RaceTrackTileType tile_type;
  RaceSector sector;
  const char *corner_name;

  RaceTrackEdge entry_edge;
  RaceTrackEdge exit_edge;

  float spawn_x;
  float spawn_y;
  float spawn_heading_deg;

  RaceRect road_rects[3];
  uint8_t road_rect_count;

  RaceCurbPlacement curb_side;
  RaceRect curb_rect;

  bool has_start_finish;
} RaceTrackScreen;

int RaceTrackLayout_GetScreenCount(void);
const RaceTrackScreen *RaceTrackLayout_GetScreenByIndex(int index);

#endif