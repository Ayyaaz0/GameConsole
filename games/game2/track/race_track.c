#include "race_track.h"
#include "../config/race_config.h"
#include <stddef.h> 
// Keep offset inside the repeating dash pattern.
static uint16_t RaceTrack_WrapOffset(uint16_t offset, uint16_t period) {
  if (period == 0U) {
    return 0U;
  }

  return (uint16_t)(offset % period);
}

void RaceTrack_Init(RaceTrack *track, uint16_t screen_width,
                    uint16_t screen_height) {
  if (track == NULL) {
    return;
  }

  track->screen_width = screen_width;
  track->screen_height = screen_height;

  track->road_center_x = (int16_t)(screen_width / 2U);
  track->road_width = RACE_TRACK_WIDTH;

  track->top_margin = RACE_TRACK_TOP_Y;
  track->bottom_margin = RACE_TRACK_BOTTOM_Y;

  track->lane_dash_gap = RACE_TRACK_LANE_GAP;
  track->lane_dash_offset = 0U;

  track->active = true;
}

void RaceTrack_Update(RaceTrack *track, uint16_t scroll_step) {
  if ((track == NULL) || (track->active == false)) {
    return;
  }

  track->lane_dash_offset = RaceTrack_WrapOffset(
      (uint16_t)(track->lane_dash_offset + scroll_step), track->lane_dash_gap);
}

int16_t RaceTrack_GetLeftEdgeX(const RaceTrack *track) {
  if (track == NULL) {
    return 0;
  }

  return (int16_t)(track->road_center_x - (track->road_width / 2U));
}

int16_t RaceTrack_GetRightEdgeX(const RaceTrack *track) {
  if (track == NULL) {
    return 0;
  }

  return (int16_t)(track->road_center_x + (track->road_width / 2U));
}

int16_t RaceTrack_GetCenterX(const RaceTrack *track) {
  if (track == NULL) {
    return 0;
  }

  return track->road_center_x;
}

void RaceTrack_GetDriveBounds(const RaceTrack *track, int16_t *min_x,
                              int16_t *max_x) {
  int16_t left_edge = 0;
  int16_t right_edge = 0;

  if ((track == NULL) || (min_x == NULL) || (max_x == NULL)) {
    return;
  }

  left_edge = RaceTrack_GetLeftEdgeX(track);
  right_edge = RaceTrack_GetRightEdgeX(track);

  *min_x = (int16_t)(left_edge + RACE_TRACK_EDGE_PADDING);
  *max_x = (int16_t)(right_edge - RACE_TRACK_EDGE_PADDING);
}