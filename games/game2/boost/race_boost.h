#ifndef RACE_BOOST_H
#define RACE_BOOST_H

#include "../car/race_car.h"
#include "../input/race_input.h"
#include "../track/race_track.h"

#include <stdbool.h>
#include <stdint.h>

#define RACE_BOOST_PICKUP_COUNT 3U

typedef struct {
  float x;
  float y;
  bool collected;
} RaceBoostPickup;

typedef struct {
  RaceBoostPickup pickups[RACE_BOOST_PICKUP_COUNT];

  uint8_t charges;
  bool active;
  uint32_t active_until_ms;
  uint32_t message_until_ms;

  // Used to respawn a random set of pickups once per lap
  uint8_t last_spawn_lap;
  uint8_t spawned_this_lap;

  // Small software random generator state
  uint32_t rng_state;
} RaceBoostState;

void RaceBoost_Init(RaceBoostState *boost);

void RaceBoost_Update(RaceBoostState *boost, RaceCar *car,
                      const RaceInput *input, const RaceTrack *track,
                      uint32_t now_ms);

uint8_t RaceBoost_GetCharges(const RaceBoostState *boost);
bool RaceBoost_IsActive(const RaceBoostState *boost, uint32_t now_ms);
bool RaceBoost_ShouldShowMessage(const RaceBoostState *boost, uint32_t now_ms);
const RaceBoostPickup *RaceBoost_GetPickups(const RaceBoostState *boost);
uint8_t RaceBoost_GetSpawnedThisLap(const RaceBoostState *boost);

#endif