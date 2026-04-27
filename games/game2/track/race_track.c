#include "race_track.h"
#include "../config/race_config.h"
#include "../utils/race_math.h"
#include "race_track_layout.h"
#include <limits.h>
#include <stddef.h>

void RaceTrack_Init(RaceTrack *track, uint16_t screen_width,
                    uint16_t screen_height) {
  if (track == NULL) {
    return;
  }

  track->screen_width = screen_width;
  track->screen_height = screen_height;

  // Keep the road centred on screen for now
  track->road_center_x = (int16_t)(screen_width / 2U);
  track->road_width = RACE_TRACK_WIDTH;

  // The visible track starts at the beginning of the world
  track->world_top_y = RACE_TRACK_WORLD_START_Y;

  // The start/finish line is a fixed marker in world space
  track->start_finish_world_y = RACE_TRACK_START_FINISH_Y;

  track->lane_dash_length = RACE_TRACK_DASH_LENGTH;
  track->lane_dash_gap = RACE_TRACK_DASH_GAP;

  track->active = true;
}

void RaceTrack_Advance(RaceTrack *track, int32_t delta_y) {
  int32_t next_world_top = 0;

  if ((track == NULL) || (track->active == false)) {
    return;
  }

  next_world_top = track->world_top_y + delta_y;

  // Do not allow the visible road window to go above the start of the world
  track->world_top_y = RaceMath_ClampInt32(next_world_top, 0, INT32_MAX);
}

void RaceTrack_SetWorldTopY(RaceTrack *track, int32_t world_top_y) {
  if ((track == NULL) || (track->active == false)) {
    return;
  }

  track->world_top_y = RaceMath_ClampInt32(world_top_y, 0, INT32_MAX);
}

int32_t RaceTrack_GetWorldTopY(const RaceTrack *track) {
  if (track == NULL) {
    return 0;
  }

  return track->world_top_y;
}

int32_t RaceTrack_GetWorldBottomY(const RaceTrack *track) {
  if (track == NULL) {
    return 0;
  }

  return (track->world_top_y + (int32_t)track->screen_height - 1);
}

int16_t RaceTrack_GetCenterX(const RaceTrack *track) {
  const RaceTrackSegment *segment = NULL;

  if (track == NULL) {
    return 0;
  }

  segment = RaceTrackLayout_GetSegmentAtY(track->world_top_y + 120);

  return segment->road_center_x;
}

int16_t RaceTrack_GetLeftEdgeX(const RaceTrack *track) {
  const RaceTrackSegment *segment = NULL;

  if (track == NULL) {
    return 0;
  }

  segment = RaceTrackLayout_GetSegmentAtY(track->world_top_y + 120);

  return (int16_t)(segment->road_center_x -
                   (int16_t)(segment->road_width / 2U));
}

int16_t RaceTrack_GetRightEdgeX(const RaceTrack *track) {
  const RaceTrackSegment *segment = NULL;

  if (track == NULL) {
    return 0;
  }

  segment = RaceTrackLayout_GetSegmentAtY(track->world_top_y + 120);

  return (int16_t)(segment->road_center_x +
                   (int16_t)(segment->road_width / 2U));
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

  *min_x = (int16_t)(left_edge + (int16_t)RACE_TRACK_EDGE_PADDING);
  *max_x = (int16_t)(right_edge - (int16_t)RACE_TRACK_EDGE_PADDING);
}

bool RaceTrack_IsWorldYVisible(const RaceTrack *track, int32_t world_y) {
  int32_t world_top = 0;
  int32_t world_bottom = 0;

  if (track == NULL) {
    return false;
  }

  world_top = RaceTrack_GetWorldTopY(track);
  world_bottom = RaceTrack_GetWorldBottomY(track);

  return ((world_y >= world_top) && (world_y <= world_bottom));
}

int16_t RaceTrack_WorldToScreenY(const RaceTrack *track, int32_t world_y) {
  if (track == NULL) {
    return 0;
  }

  return (int16_t)(world_y - track->world_top_y);
}

int32_t RaceTrack_GetStartFinishWorldY(const RaceTrack *track) {
  if (track == NULL) {
    return 0;
  }

  return track->start_finish_world_y;
}

const RaceTrackSegment *RaceTrack_GetCurrentSegment(const RaceTrack *track) {
  if (track == NULL) {
    return NULL;
  }

  return RaceTrackLayout_GetSegmentAtY(track->world_top_y + 120);
}

const char *RaceTrack_GetCurrentCornerName(const RaceTrack *track) {
  const RaceTrackSegment *segment = RaceTrack_GetCurrentSegment(track);

  if (segment == NULL) {
    return "UNKNOWN";
  }

  return segment->corner_name;
}

RaceSector RaceTrack_GetCurrentSector(const RaceTrack *track) {
  const RaceTrackSegment *segment = RaceTrack_GetCurrentSegment(track);

  if (segment == NULL) {
    return RACE_SECTOR_1;
  }

  return segment->sector;
}

bool RaceTrack_HasLeftCurbAtY(int32_t world_y) {
  const RaceTrackSegment *segment = RaceTrackLayout_GetSegmentAtY(world_y);

  if (segment == NULL) {
    return false;
  }

  return segment->left_curb;
}

bool RaceTrack_HasRightCurbAtY(int32_t world_y) {
  const RaceTrackSegment *segment = RaceTrackLayout_GetSegmentAtY(world_y);

  if (segment == NULL) {
    return false;
  }

  return segment->right_curb;
}