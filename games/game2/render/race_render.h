#ifndef RACE_RENDER_H
#define RACE_RENDER_H

#include "../camera/race_camera.h"
#include "../car/race_car.h"
#include "../track/race_track.h"

void RaceRender_DrawFrame(const RaceTrack *track, const RaceCar *player_car,
                          const RaceCamera *camera);

#endif