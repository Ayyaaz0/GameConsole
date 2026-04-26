#include "race_state.h"

#include <stddef.h>

void RaceState_Init(RaceState *race_state) {
  if (race_state == NULL) {
    return;
  }

  race_state->state = RACE_STATE_RUNNING;
  race_state->restart_requested = false;
}

void RaceState_SetCrashed(RaceState *race_state) {
  if (race_state == NULL) {
    return;
  }

  race_state->state = RACE_STATE_CRASHED;
}

bool RaceState_IsCrashed(const RaceState *race_state) {
  if (race_state == NULL) {
    return false;
  }

  return race_state->state == RACE_STATE_CRASHED;
}

void RaceState_RequestRestart(RaceState *race_state) {
  if (race_state == NULL) {
    return;
  }

  race_state->restart_requested = true;
}

bool RaceState_ShouldRestart(const RaceState *race_state) {
  if (race_state == NULL) {
    return false;
  }

  return race_state->restart_requested;
}

void RaceState_ClearRestart(RaceState *race_state) {
  if (race_state == NULL) {
    return;
  }

  race_state->restart_requested = false;
}