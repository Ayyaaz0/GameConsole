#include "race_boost.h"

#include "../config/race_config.h"

#include <stddef.h>

#define RACE_BOOST_MAX_CHARGES 3U
#define RACE_BOOST_DURATION_MS 2000U
#define RACE_BOOST_MESSAGE_MS 650U
#define RACE_BOOST_PICKUP_RADIUS_PX 13.0f

// The car physics clamps normal speed to RACE_CAR_MAX_SPEED, so the boost
// adds extra movement after normal physics to make it actually feel faster
#define RACE_BOOST_EXTRA_MOVE_PX 2.50f
#define RACE_BOOST_MIN_SPEED_WHILE_ACTIVE RACE_CAR_MAX_SPEED

// Random amount of pickups per lap
#define RACE_BOOST_MIN_PICKUPS_PER_LAP 3U
#define RACE_BOOST_MAX_PICKUPS_PER_LAP 3U

// Pickups are 10x10 sprites
#define RACE_BOOST_PICKUP_HALF_SIZE_PX 5

// Try several random world positions until a road-only location is found
#define RACE_BOOST_SPAWN_ATTEMPTS 300U

// Prevent pickups from spawning too close together
#define RACE_BOOST_MIN_PICKUP_GAP_PX 32.0f

#ifndef RACE_WORLD_WIDTH_PX
#define RACE_WORLD_WIDTH_PX 240
#endif

#ifndef RACE_WORLD_HEIGHT_PX
#define RACE_WORLD_HEIGHT_PX 960
#endif

#ifndef RACE_TRACK_START_FINISH_Y
#define RACE_TRACK_START_FINISH_Y 80U
#endif

static float RaceBoost_AbsFloat(float value) {
  if (value < 0.0f) {
    return -value;
  }

  return value;
}

static uint32_t RaceBoost_NextRandom(RaceBoostState *boost) {
  if (boost == NULL) {
    return 0U;
  }

  // LCG random generator
  boost->rng_state = (boost->rng_state * 1664525UL) + 1013904223UL;

  return boost->rng_state;
}

static uint32_t RaceBoost_RandomRange(RaceBoostState *boost, uint32_t min_value,
                                      uint32_t max_value) {
  uint32_t span = 0U;

  if (max_value <= min_value) {
    return min_value;
  }

  span = (max_value - min_value) + 1U;

  return min_value + (RaceBoost_NextRandom(boost) % span);
}

static bool RaceBoost_IsRoadOnlyTile(RaceTileType tile) {

  // This deliberately rejects grass, curbs, start line, checkpoint tiles,
  // joker tiles, sand, tyre barriers, buildings and scenery
  return ((tile == RACE_TILE_ROAD) || (tile == RACE_TILE_ROAD_DARK));
}

static bool RaceBoost_WorldPointIsRoadOnly(const RaceTrack *track,
                                           float world_x, float world_y) {
  int tile_x = 0;
  int tile_y = 0;
  RaceTileType tile = RACE_TILE_GRASS;

  if (track == NULL) {
    return false;
  }

  if ((world_x < 0.0f) || (world_y < 0.0f)) {
    return false;
  }

  tile_x = (int)(world_x / (float)RACE_TILE_SIZE);
  tile_y = (int)(world_y / (float)RACE_TILE_SIZE);

  tile = RaceTrack_GetTileAt(track, tile_x, tile_y);

  return RaceBoost_IsRoadOnlyTile(tile);
}

static bool RaceBoost_PickupAreaIsRoadOnly(const RaceTrack *track,
                                           float centre_x, float centre_y) {
  int16_t offset_x = 0;
  int16_t offset_y = 0;

  if (track == NULL) {
    return false;
  }

  // Check the whole 10x10 sprite area, not just the centre
  // This stops lightning bolts sitting half on road and half on curb/grass
  for (offset_y = -RACE_BOOST_PICKUP_HALF_SIZE_PX;
       offset_y <= RACE_BOOST_PICKUP_HALF_SIZE_PX; offset_y += 2) {
    for (offset_x = -RACE_BOOST_PICKUP_HALF_SIZE_PX;
         offset_x <= RACE_BOOST_PICKUP_HALF_SIZE_PX; offset_x += 2) {
      if (RaceBoost_WorldPointIsRoadOnly(track, centre_x + (float)offset_x,
                                         centre_y + (float)offset_y) == false) {
        return false;
      }
    }
  }

  return true;
}

static bool RaceBoost_IsFarFromExistingPickups(const RaceBoostState *boost,
                                               float x, float y,
                                               uint8_t used_count) {
  uint8_t i = 0U;

  if (boost == NULL) {
    return false;
  }

  for (i = 0U; i < used_count; i++) {
    float dx = RaceBoost_AbsFloat(boost->pickups[i].x - x);
    float dy = RaceBoost_AbsFloat(boost->pickups[i].y - y);

    if ((dx < RACE_BOOST_MIN_PICKUP_GAP_PX) &&
        (dy < RACE_BOOST_MIN_PICKUP_GAP_PX)) {
      return false;
    }
  }

  return true;
}

static void RaceBoost_ClearPickups(RaceBoostState *boost) {
  uint8_t i = 0U;

  if (boost == NULL) {
    return;
  }

  for (i = 0U; i < RACE_BOOST_PICKUP_COUNT; i++) {
    boost->pickups[i].x = -100.0f;
    boost->pickups[i].y = -100.0f;
    boost->pickups[i].collected = true;
  }

  boost->spawned_this_lap = 0U;
}

static bool RaceBoost_TryFindPickupPosition(RaceBoostState *boost,
                                            const RaceTrack *track,
                                            float *out_x, float *out_y,
                                            uint8_t used_count) {
  uint16_t attempt = 0U;

  if ((boost == NULL) || (track == NULL) || (out_x == NULL) ||
      (out_y == NULL)) {
    return false;
  }

  for (attempt = 0U; attempt < RACE_BOOST_SPAWN_ATTEMPTS; attempt++) {
    float x = 0.0f;
    float y = 0.0f;

    // Keep away from the outer world boundary
    x = (float)RaceBoost_RandomRange(boost, 16U,
                                     (uint32_t)(RACE_WORLD_WIDTH_PX - 16U));

    // Avoid the immediate start/finish area and avoid very bottom edge
    y = (float)RaceBoost_RandomRange(
        boost, (uint32_t)(RACE_TRACK_START_FINISH_Y + 60U),
        (uint32_t)(RACE_WORLD_HEIGHT_PX - 40U));

    if (RaceBoost_PickupAreaIsRoadOnly(track, x, y) == false) {
      continue;
    }

    if (RaceBoost_IsFarFromExistingPickups(boost, x, y, used_count) == false) {
      continue;
    }

    *out_x = x;
    *out_y = y;
    return true;
  }

  return false;
}

static void RaceBoost_SpawnPickupsForLap(RaceBoostState *boost,
                                         const RaceTrack *track, uint8_t lap,
                                         uint32_t now_ms) {
  uint8_t target_count = 0U;
  uint8_t placed_count = 0U;
  uint8_t i = 0U;

  if ((boost == NULL) || (track == NULL)) {
    return;
  }

  if (boost->rng_state == 0U) {
    boost->rng_state = now_ms ^ 0xA5A55A5AU ^ ((uint32_t)lap * 2654435761UL);
  }

  RaceBoost_ClearPickups(boost);

  target_count = (uint8_t)RaceBoost_RandomRange(
      boost, RACE_BOOST_MIN_PICKUPS_PER_LAP, RACE_BOOST_MAX_PICKUPS_PER_LAP);

  if (target_count > RACE_BOOST_PICKUP_COUNT) {
    target_count = RACE_BOOST_PICKUP_COUNT;
  }

  for (i = 0U; i < target_count; i++) {
    float x = 0.0f;
    float y = 0.0f;

    if (RaceBoost_TryFindPickupPosition(boost, track, &x, &y, placed_count) ==
        true) {
      boost->pickups[placed_count].x = x;
      boost->pickups[placed_count].y = y;
      boost->pickups[placed_count].collected = false;
      placed_count++;
    }
  }

  boost->last_spawn_lap = lap;
  boost->spawned_this_lap = placed_count;
}

static float RaceBoost_GetCarCentreX(const RaceCar *car) {
  if (car == NULL) {
    return 0.0f;
  }

  return car->x + ((float)car->width * 0.5f);
}

static float RaceBoost_GetCarCentreY(const RaceCar *car) {
  if (car == NULL) {
    return 0.0f;
  }

  return car->y + ((float)car->height * 0.5f);
}

static bool RaceBoost_CarTouchesPickup(const RaceCar *car,
                                       const RaceBoostPickup *pickup) {
  float dx = 0.0f;
  float dy = 0.0f;

  if ((car == NULL) || (pickup == NULL)) {
    return false;
  }

  dx = RaceBoost_AbsFloat(RaceBoost_GetCarCentreX(car) - pickup->x);
  dy = RaceBoost_AbsFloat(RaceBoost_GetCarCentreY(car) - pickup->y);

  return ((dx <= RACE_BOOST_PICKUP_RADIUS_PX) &&
          (dy <= RACE_BOOST_PICKUP_RADIUS_PX));
}

static void RaceBoost_TryCollectPickups(RaceBoostState *boost,
                                        const RaceCar *car, uint32_t now_ms) {
  uint8_t i = 0U;

  if ((boost == NULL) || (car == NULL)) {
    return;
  }

  for (i = 0U; i < RACE_BOOST_PICKUP_COUNT; i++) {
    if (boost->pickups[i].collected == true) {
      continue;
    }

    if (RaceBoost_CarTouchesPickup(car, &boost->pickups[i]) == true) {
      boost->pickups[i].collected = true;

      if (boost->charges < RACE_BOOST_MAX_CHARGES) {
        boost->charges++;
      }

      boost->message_until_ms = now_ms + RACE_BOOST_MESSAGE_MS;
    }
  }
}

static void RaceBoost_TryActivate(RaceBoostState *boost, RaceCar *car,
                                  const RaceInput *input, uint32_t now_ms) {
  if ((boost == NULL) || (car == NULL) || (input == NULL)) {
    return;
  }

  if (input->boost_pressed == false) {
    return;
  }

  if (boost->charges == 0U) {
    return;
  }

  boost->charges--;
  boost->active = true;
  boost->active_until_ms = now_ms + RACE_BOOST_DURATION_MS;
  boost->message_until_ms = now_ms + RACE_BOOST_MESSAGE_MS;

  // Immediate full-speed kick so the boost is obvious
  if (car->speed < RACE_CAR_MAX_SPEED) {
    car->speed = RACE_CAR_MAX_SPEED;
  }
}

static void RaceBoost_ApplyActiveBoost(RaceBoostState *boost, RaceCar *car,
                                       uint32_t now_ms) {
  if ((boost == NULL) || (car == NULL)) {
    return;
  }

  if (boost->active == false) {
    return;
  }

  if (now_ms >= boost->active_until_ms) {
    boost->active = false;
    return;
  }

  // Do not boost while reversing
  if (car->speed < 0.0f) {
    return;
  }

  // Keep the car at full normal speed while boost is active
  if (car->speed < RACE_BOOST_MIN_SPEED_WHILE_ACTIVE) {
    car->speed = RACE_BOOST_MIN_SPEED_WHILE_ACTIVE;
  }

  // Extra forward movement after normal physics
  car->y -= RACE_BOOST_EXTRA_MOVE_PX;
}

void RaceBoost_Init(RaceBoostState *boost) {
  if (boost == NULL) {
    return;
  }

  boost->charges = 0U;
  boost->active = false;
  boost->active_until_ms = 0U;
  boost->message_until_ms = 0U;
  boost->last_spawn_lap = 255U;
  boost->spawned_this_lap = 0U;
  boost->rng_state = 0U;

  RaceBoost_ClearPickups(boost);
}

void RaceBoost_Update(RaceBoostState *boost, RaceCar *car,
                      const RaceInput *input, const RaceTrack *track,
                      uint32_t now_ms) {
  uint8_t current_lap = 0U;

  if ((boost == NULL) || (car == NULL) || (input == NULL)) {
    return;
  }

  if (track != NULL) {
    current_lap = RaceTrack_GetCurrentLap(track);

    // Spawn a new random set once per lap
    if (current_lap != boost->last_spawn_lap) {
      RaceBoost_SpawnPickupsForLap(boost, track, current_lap, now_ms);
    }
  }

  RaceBoost_TryCollectPickups(boost, car, now_ms);
  RaceBoost_TryActivate(boost, car, input, now_ms);
  RaceBoost_ApplyActiveBoost(boost, car, now_ms);
}

uint8_t RaceBoost_GetCharges(const RaceBoostState *boost) {
  if (boost == NULL) {
    return 0U;
  }

  return boost->charges;
}

bool RaceBoost_IsActive(const RaceBoostState *boost, uint32_t now_ms) {
  if (boost == NULL) {
    return false;
  }

  return ((boost->active == true) && (now_ms < boost->active_until_ms));
}

bool RaceBoost_ShouldShowMessage(const RaceBoostState *boost, uint32_t now_ms) {
  if (boost == NULL) {
    return false;
  }

  return (now_ms < boost->message_until_ms);
}

const RaceBoostPickup *RaceBoost_GetPickups(const RaceBoostState *boost) {
  if (boost == NULL) {
    return NULL;
  }

  return boost->pickups;
}

uint8_t RaceBoost_GetSpawnedThisLap(const RaceBoostState *boost) {
  if (boost == NULL) {
    return 0U;
  }

  return boost->spawned_this_lap;
}