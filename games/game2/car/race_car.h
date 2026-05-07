#ifndef RACE_CAR_H
#define RACE_CAR_H

#include "../input/race_input.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct {
  float x;
  float y;
  float prev_x;
  float prev_y;
  float vx;
  float vy;
  float speed;
  uint16_t width;
  uint16_t height;
  bool active;
} RaceCar;

void RaceCar_Init(RaceCar *car);
void RaceCar_UpdatePhysics(RaceCar *car, const RaceInput *input);
void RaceCar_RestorePreviousPosition(RaceCar *car);
void RaceCar_ClampToWorld(RaceCar *car);
void RaceCar_ApplySpeedMultiplier(RaceCar *car, float multiplier);

#endif