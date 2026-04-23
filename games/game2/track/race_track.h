#ifndef RACE_TRACK_H
#define RACE_TRACK_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
  uint16_t screen_width;
  uint16_t screen_height;

  int16_t road_center_x;
  uint16_t road_width;

  uint16_t top_margin;
  uint16_t bottom_margin;

  uint16_t lane_dash_gap;
  uint16_t lane_dash_offset;

  bool active;
} RaceTrack;

// Set up a simple straight track.
void RaceTrack_Init(RaceTrack *track, uint16_t screen_width,
                    uint16_t screen_height);

// Move the lane marker animation forward.
void RaceTrack_Update(RaceTrack *track, uint16_t scroll_step);

// Helper queries used by Game_2 and rendering.
int16_t RaceTrack_GetLeftEdgeX(const RaceTrack *track);
int16_t RaceTrack_GetRightEdgeX(const RaceTrack *track);
int16_t RaceTrack_GetCenterX(const RaceTrack *track);

// Returns the usable horizontal driving space inside the track edges.
void RaceTrack_GetDriveBounds(const RaceTrack *track, int16_t *min_x,
                              int16_t *max_x);

#endif