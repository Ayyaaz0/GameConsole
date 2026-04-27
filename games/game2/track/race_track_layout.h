#ifndef RACE_TRACK_LAYOUT_H
#define RACE_TRACK_LAYOUT_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
  RACE_SECTOR_1 = 1,
  RACE_SECTOR_2 = 2,
  RACE_SECTOR_3 = 3
} RaceSector;

typedef struct {
  int32_t start_y;
  int32_t end_y;

  int16_t road_center_x;
  uint16_t road_width;

  RaceSector sector;
  const char *corner_name;

  bool left_curb;
  bool right_curb;
} RaceTrackSegment;

const RaceTrackSegment *RaceTrackLayout_GetSegmentAtY(int32_t world_y);
int RaceTrackLayout_GetSegmentCount(void);
const RaceTrackSegment *RaceTrackLayout_GetSegmentByIndex(int index);

#endif