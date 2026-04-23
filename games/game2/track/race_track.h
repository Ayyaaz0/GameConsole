#ifndef RACE_TRACK_H
#define RACE_TRACK_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
  uint16_t screen_width;
  uint16_t screen_height;

  int16_t road_center_x;
  uint16_t road_width;

  // World-space Y currently at the top of the visible screen
  int32_t world_top_y;

  // World-space Y position of the start/finish line
  int32_t start_finish_world_y;

  uint16_t lane_dash_length;
  uint16_t lane_dash_gap;

  bool active;
} RaceTrack;

// Initialise a straight track in world space
void RaceTrack_Init(RaceTrack *track, uint16_t screen_width,
                    uint16_t screen_height);

// Move the visible road window forward or backward in world space
void RaceTrack_Advance(RaceTrack *track, int32_t delta_y);

// Directly set the top visible world-space Y position
void RaceTrack_SetWorldTopY(RaceTrack *track, int32_t world_top_y);

// Get the visible world-space top Y position
int32_t RaceTrack_GetWorldTopY(const RaceTrack *track);

// Get the visible world-space bottom Y position
int32_t RaceTrack_GetWorldBottomY(const RaceTrack *track);

// Get the horizontal centre of the road
int16_t RaceTrack_GetCenterX(const RaceTrack *track);

// Get the left road edge in screen coordinates
int16_t RaceTrack_GetLeftEdgeX(const RaceTrack *track);

// Get the right road edge in screen coordinates
int16_t RaceTrack_GetRightEdgeX(const RaceTrack *track);

// Get the drivable horizontal bounds inside the road edges
void RaceTrack_GetDriveBounds(const RaceTrack *track, int16_t *min_x,
                              int16_t *max_x);

// Return true if a given world-space Y is currently visible on screen
bool RaceTrack_IsWorldYVisible(const RaceTrack *track, int32_t world_y);

// Convert a world-space Y into a screen-space Y
int16_t RaceTrack_WorldToScreenY(const RaceTrack *track, int32_t world_y);

// Return the start/finish line world-space position
int32_t RaceTrack_GetStartFinishWorldY(const RaceTrack *track);

#endif