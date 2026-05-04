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

  float heading_deg;
  float prev_heading_deg;

  uint16_t width;
  uint16_t height;

  uint8_t damage;
  uint8_t curb_damage_cooldown;


  bool active;
} RaceCar;

void RaceCar_Init(RaceCar *car);
void RaceCar_UpdatePhysics(RaceCar *car, const RaceInput *input);

void RaceCar_Move(RaceCar *car, float dx, float dy);
void RaceCar_RestorePreviousPosition(RaceCar *car);

void RaceCar_StopX(RaceCar *car);
void RaceCar_StopY(RaceCar *car);
void RaceCar_ApplyWallSlowdown(RaceCar *car);

void RaceCar_ClampToWorld(RaceCar *car, uint16_t world_width,
                          uint16_t world_height);

void RaceCar_ClampToScreen(RaceCar *car, uint16_t screen_width,
                           uint16_t screen_height);

void RaceCar_ClampToHorizontalRange(RaceCar *car, float min_x, float max_x);

void RaceCar_AddDamage(RaceCar *car, uint8_t amount);
uint8_t RaceCar_GetDamage(const RaceCar *car);


#endif