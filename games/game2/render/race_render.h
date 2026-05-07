#ifndef RACE_RENDER_H
#define RACE_RENDER_H

#include "../boost/race_boost.h"
#include "../camera/race_camera.h"
#include "../car/race_car.h"
#include "../state/race_state.h"
#include "../track/race_track.h"

#include <stdint.h>

void RaceRender_DrawFrame(const RaceTrack *track, const RaceCar *player_car,
                          const RaceCamera *camera, const RaceState *race_state,
                          const char *flash_message, uint32_t best_lap_ms,
                          const RaceBoostState *boost_state);

#endif