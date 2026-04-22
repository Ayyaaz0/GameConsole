#ifndef RACE_CAR_H
#define RACE_CAR_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
  float x;
  float y;
  uint16_t width;
  uint16_t height;
  bool active;
} RaceCar;

void RaceCar_Init(RaceCar *car);

void RaceCar_Move(RaceCar *car, float dx, float dy);

void RaceCar_ClampToScreen(RaceCar *car, uint16_t screen_width, uint16_t screen_height);

#endif 