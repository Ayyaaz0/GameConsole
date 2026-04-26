#ifndef RACE_STATE_H
#define RACE_STATE_H

#include <stdbool.h>

typedef enum {
  RACE_STATE_RUNNING = 0,
  RACE_STATE_CRASHED
} RaceGameState;

typedef struct {
  RaceGameState state;
  bool restart_requested;
} RaceState;

void RaceState_Init(RaceState *race_state);
void RaceState_SetCrashed(RaceState *race_state);
bool RaceState_IsCrashed(const RaceState *race_state);
void RaceState_RequestRestart(RaceState *race_state);
bool RaceState_ShouldRestart(const RaceState *race_state);
void RaceState_ClearRestart(RaceState *race_state);

#endif