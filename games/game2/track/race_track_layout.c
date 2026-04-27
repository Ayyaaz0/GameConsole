#include "race_track_layout.h"

#include <stddef.h>

// Singapore-inspired simple street circuit layout.
// Each segment represents one visible chunk of track in world space.
static const RaceTrackSegment g_track_segments[] = {
    {0, 260, 120, 140, RACE_SECTOR_1, "START STRAIGHT", true, true},
    {260, 520, 112, 128, RACE_SECTOR_1, "MARINA ENTRY", true, false},
    {520, 780, 98, 120, RACE_SECTOR_1, "BAY LEFT", true, true},
    {780, 1040, 130, 118, RACE_SECTOR_1, "RAFFLES SWEEP", false, true},

    {1040, 1300, 145, 116, RACE_SECTOR_2, "HOTEL RIGHT", true, true},
    {1300, 1560, 118, 126, RACE_SECTOR_2, "BRIDGE RUN", false, true},
    {1560, 1820, 92, 112, RACE_SECTOR_2, "TUNNEL LEFT", true, false},
    {1820, 2080, 120, 132, RACE_SECTOR_2, "BAYFRONT DASH", true, true},

    {2080, 2340, 150, 116, RACE_SECTOR_3, "STADIUM RIGHT", false, true},
    {2340, 2600, 130, 118, RACE_SECTOR_3, "HARBOUR CUT", true, true},
    {2600, 2860, 105, 124, RACE_SECTOR_3, "FINAL LEFT", true, false},
    {2860, 3200, 120, 140, RACE_SECTOR_3, "FINISH RUN", true, true},
};

const RaceTrackSegment *RaceTrackLayout_GetSegmentAtY(int32_t world_y) {
  int i = 0;
  int count = RaceTrackLayout_GetSegmentCount();

  for (i = 0; i < count; i++) {
    if ((world_y >= g_track_segments[i].start_y) &&
        (world_y < g_track_segments[i].end_y)) {
      return &g_track_segments[i];
    }
  }

  return &g_track_segments[count - 1];
}

int RaceTrackLayout_GetSegmentCount(void) {
  return (int)(sizeof(g_track_segments) / sizeof(g_track_segments[0]));
}

const RaceTrackSegment *RaceTrackLayout_GetSegmentByIndex(int index) {
  int count = RaceTrackLayout_GetSegmentCount();

  if ((index < 0) || (index >= count)) {
    return NULL;
  }

  return &g_track_segments[index];
}