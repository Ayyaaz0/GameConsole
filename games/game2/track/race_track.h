#ifndef RACE_TRACK_H
#define RACE_TRACK_H

#include <stdbool.h>
#include <stdint.h>

#include "race_track_layout.h"

typedef struct {
  uint16_t screen_width;
  uint16_t screen_height;
  int current_screen_index;
  bool active;
} RaceTrack;

void RaceTrack_Init(RaceTrack *track, uint16_t screen_width,
                    uint16_t screen_height);
void RaceTrack_GoToNextScreen(RaceTrack *track);
void RaceTrack_ResetToStart(RaceTrack *track);

const RaceTrackScreen *RaceTrack_GetCurrentScreen(const RaceTrack *track);
int RaceTrack_GetCurrentScreenIndex(const RaceTrack *track);

RaceTrackEdge RaceTrack_GetCurrentExitEdge(const RaceTrack *track);

float RaceTrack_GetSpawnX(const RaceTrack *track);
float RaceTrack_GetSpawnY(const RaceTrack *track);
float RaceTrack_GetSpawnHeadingDeg(const RaceTrack *track);

RaceSector RaceTrack_GetCurrentSector(const RaceTrack *track);
const char *RaceTrack_GetCurrentCornerName(const RaceTrack *track);

bool RaceTrack_CurrentScreenHasStartFinish(const RaceTrack *track);

bool RaceTrack_PointIsOnRoad(const RaceTrack *track, float x, float y);
bool RaceTrack_CarCentreIsOnRoad(const RaceTrack *track, float car_x,
                                 float car_y, uint16_t car_width,
                                 uint16_t car_height);

const RaceRect *RaceTrack_GetRoadRects(const RaceTrack *track,
                                       uint8_t *rect_count);
RaceRect RaceTrack_GetCurbRect(const RaceTrack *track);
RaceCurbPlacement RaceTrack_GetCurbPlacement(const RaceTrack *track);

// Compatibility functions used by older render/collision code.
int16_t RaceTrack_GetCenterX(const RaceTrack *track);
int16_t RaceTrack_GetLeftEdgeX(const RaceTrack *track);
int16_t RaceTrack_GetRightEdgeX(const RaceTrack *track);
int16_t RaceTrack_GetCenterXAtScreenY(const RaceTrack *track,
                                      uint16_t screen_y);
uint16_t RaceTrack_GetWidthAtScreenY(const RaceTrack *track, uint16_t screen_y);
int16_t RaceTrack_GetLeftEdgeXAtScreenY(const RaceTrack *track,
                                        uint16_t screen_y);
int16_t RaceTrack_GetRightEdgeXAtScreenY(const RaceTrack *track,
                                         uint16_t screen_y);
void RaceTrack_GetDriveBounds(const RaceTrack *track, int16_t *min_x,
                              int16_t *max_x);
bool RaceTrack_HasLeftCurbAtScreenY(const RaceTrack *track, uint16_t screen_y);
bool RaceTrack_HasRightCurbAtScreenY(const RaceTrack *track, uint16_t screen_y);
float RaceTrack_GetNextSpawnX(const RaceTrack *track);

#endif