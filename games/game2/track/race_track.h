#ifndef RACE_TRACK_H
#define RACE_TRACK_H

#include <stdbool.h>
#include <stdint.h>

#include "../config/race_config.h"
#include "race_track_layout.h"

typedef struct {
  uint16_t screen_width;
  uint16_t screen_height;

  float camera_x;
  float camera_y;

  uint8_t current_lap;
  uint8_t total_laps;
  uint8_t next_checkpoint;
  uint8_t checkpoint_count;

  bool joker_lap_active;
  bool joker_lap_done;
  bool joker_lap_required;

  uint32_t race_start_ms;
  uint32_t elapsed_ms;

  bool race_finished;
  bool active;

  // Store tile IDs as uint8_t to save SRAM
  uint8_t tiles[RACE_WORLD_TILES_Y][RACE_WORLD_TILES_X];
} RaceTrack;

void RaceTrack_Init(RaceTrack *track, uint16_t screen_width,
                    uint16_t screen_height);
void RaceTrack_ResetToStart(RaceTrack *track);

RaceTileType RaceTrack_GetTileAt(const RaceTrack *track, int tile_x,
                                 int tile_y);
RaceTileType RaceTrack_GetTileAtWorld(const RaceTrack *track, float world_x,
                                      float world_y);

bool RaceTrack_PointIsDriveable(const RaceTrack *track, float world_x,
                                float world_y);
bool RaceTrack_CarIsDriveable(const RaceTrack *track, float car_x, float car_y,
                              uint16_t car_width, uint16_t car_height);

void RaceTrack_SetCamera(RaceTrack *track, float camera_x, float camera_y);
int16_t RaceTrack_WorldToScreenX(const RaceTrack *track, float world_x);
int16_t RaceTrack_WorldToScreenY(const RaceTrack *track, float world_y);

void RaceTrack_StartTimer(RaceTrack *track, uint32_t now_ms);
void RaceTrack_UpdateTimer(RaceTrack *track, uint32_t now_ms);

void RaceTrack_UpdateProgress(RaceTrack *track, float car_x, float car_y,
                              uint16_t car_width, uint16_t car_height);

uint8_t RaceTrack_GetCurrentLap(const RaceTrack *track);
uint8_t RaceTrack_GetTotalLaps(const RaceTrack *track);
uint8_t RaceTrack_GetNextCheckpoint(const RaceTrack *track);
uint8_t RaceTrack_GetCheckpointCount(const RaceTrack *track);
uint32_t RaceTrack_GetElapsedMs(const RaceTrack *track);
bool RaceTrack_IsFinished(const RaceTrack *track);

bool RaceTrack_IsJokerLapActive(const RaceTrack *track);
bool RaceTrack_IsJokerLapDone(const RaceTrack *track);
bool RaceTrack_IsJokerLapRequired(const RaceTrack *track);

RaceSector RaceTrack_GetCurrentSector(const RaceTrack *track);
const char *RaceTrack_GetCurrentCornerName(const RaceTrack *track);

RaceTriggerZone RaceTrack_GetStartFinishZone(void);
RaceTriggerZone RaceTrack_GetCheckpointZone(uint8_t checkpoint_number);
RaceTriggerZone RaceTrack_GetJokerStartZone(void);
RaceTriggerZone RaceTrack_GetJokerEndZone(void);

void RaceTrack_GoToNextScreen(RaceTrack *track);
const void *RaceTrack_GetCurrentScreen(const RaceTrack *track);
int RaceTrack_GetCurrentScreenIndex(const RaceTrack *track);
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