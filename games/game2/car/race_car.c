#include "race_car.h"

#include "../config/race_config.h"
#include "../utils/race_math.h"

#include <math.h>
#include <stddef.h>

static float RaceCar_AbsFloat(float value) {
  if (value < 0.0f) {
    return -value;
  }

  return value;
}

static void RaceCar_SavePreviousPosition(RaceCar *car) {
  car->prev_x = car->x;
  car->prev_y = car->y;
  car->prev_heading_deg = car->heading_deg;
}

static float RaceCar_GetInputLength(float x, float y) {
  return sqrtf((x * x) + (y * y));
}

static void RaceCar_NormaliseInput(float *x, float *y) {
  float length = 0.0f;

  if ((x == NULL) || (y == NULL)) {
    return;
  }

  length = RaceCar_GetInputLength(*x, *y);

  if (length <= 1.0f) {
    return;
  }

  *x = *x / length;
  *y = *y / length;
}

static float RaceCar_GetHeadingFromVelocity(float vx, float vy,
                                            float fallback_heading) {
  float heading = 0.0f;

  if ((RaceCar_AbsFloat(vx) < 0.03f) && (RaceCar_AbsFloat(vy) < 0.03f)) {
    return fallback_heading;
  }

  heading = atan2f(vx, -vy) * 57.2957795f;

  if (heading < 0.0f) {
    heading += 360.0f;
  }

  return heading;
}

static void RaceCar_ApplyIdleDrag(RaceCar *car) {
  car->vx *= RACE_CAR_DRAG_MULTIPLIER;
  car->vy *= RACE_CAR_DRAG_MULTIPLIER;

  if (RaceCar_AbsFloat(car->vx) < 0.02f) {
    car->vx = 0.0f;
  }

  if (RaceCar_AbsFloat(car->vy) < 0.02f) {
    car->vy = 0.0f;
  }
}

static void RaceCar_ApproachVelocity(RaceCar *car, const RaceInput *input) {
  float move_x = input->move_x;
  float move_y = input->move_y;
  float target_vx = 0.0f;
  float target_vy = 0.0f;
  float response = RACE_CAR_ACCELERATION_RESPONSE;

  RaceCar_NormaliseInput(&move_x, &move_y);

  if ((RaceCar_AbsFloat(move_x) < 0.05f) &&
      (RaceCar_AbsFloat(move_y) < 0.05f)) {
    RaceCar_ApplyIdleDrag(car);
    return;
  }

  target_vx = move_x * RACE_CAR_MAX_SPEED;
  target_vy = move_y * RACE_CAR_MAX_SPEED;

  if (input->brake > 0.0f) {
    response = RACE_CAR_DECELERATION_RESPONSE;
  }

  car->vx += (target_vx - car->vx) * response;
  car->vy += (target_vy - car->vy) * response;
}

void RaceCar_Init(RaceCar *car) {
  if (car == NULL) {
    return;
  }

  car->x = RACE_PLAYER_START_X;
  car->y = RACE_PLAYER_START_Y;

  car->prev_x = car->x;
  car->prev_y = car->y;

  car->vx = 0.0f;
  car->vy = 0.0f;
  car->speed = 0.0f;

  car->heading_deg = 0.0f;
  car->prev_heading_deg = 0.0f;

  car->damage = 0U;
  car->curb_damage_cooldown = 0U;


  car->width = RACE_PLAYER_WIDTH;
  car->height = RACE_PLAYER_HEIGHT;

  car->active = true;
}

void RaceCar_UpdatePhysics(RaceCar *car, const RaceInput *input) {
  if ((car == NULL) || (input == NULL) || (car->active == false)) {
    return;
  }

  RaceCar_SavePreviousPosition(car);

    if (car->curb_damage_cooldown > 0U) {
    car->curb_damage_cooldown--;
  }


  RaceCar_ApproachVelocity(car, input);

  car->vx =
      RaceMath_ClampFloat(car->vx, -RACE_CAR_MAX_SPEED, RACE_CAR_MAX_SPEED);
  car->vy =
      RaceMath_ClampFloat(car->vy, -RACE_CAR_MAX_SPEED, RACE_CAR_MAX_SPEED);

  car->x += car->vx;
  car->y += car->vy;

  car->speed = sqrtf((car->vx * car->vx) + (car->vy * car->vy));

  car->heading_deg =
      RaceCar_GetHeadingFromVelocity(car->vx, car->vy, car->heading_deg);
}

void RaceCar_Move(RaceCar *car, float dx, float dy) {
  if ((car == NULL) || (car->active == false)) {
    return;
  }

  car->x += dx;
  car->y += dy;
}

void RaceCar_RestorePreviousPosition(RaceCar *car) {
  if ((car == NULL) || (car->active == false)) {
    return;
  }

  car->x = car->prev_x;
  car->y = car->prev_y;
  car->heading_deg = car->prev_heading_deg;
}

void RaceCar_StopX(RaceCar *car) {
  if (car == NULL) {
    return;
  }

  car->x = car->prev_x;
  car->vx = 0.0f;
}

void RaceCar_StopY(RaceCar *car) {
  if (car == NULL) {
    return;
  }

  car->y = car->prev_y;
  car->vy = 0.0f;
}

void RaceCar_ApplyWallSlowdown(RaceCar *car) {
  if (car == NULL) {
    return;
  }

  car->vx *= RACE_CAR_WALL_BOUNCE_MULTIPLIER;
  car->vy *= RACE_CAR_WALL_BOUNCE_MULTIPLIER;
  car->speed *= RACE_COLLISION_EDGE_SPEED_MULTIPLIER;
}

void RaceCar_ClampToWorld(RaceCar *car, uint16_t world_width,
                          uint16_t world_height) {
  if ((car == NULL) || (car->active == false)) {
    return;
  }

  if (car->x < 0.0f) {
    car->x = 0.0f;
    car->vx = 0.0f;
  }

  if (car->y < 0.0f) {
    car->y = 0.0f;
    car->vy = 0.0f;
  }

  if ((car->x + (float)car->width) > (float)world_width) {
    car->x = (float)(world_width - car->width);
    car->vx = 0.0f;
  }

  if ((car->y + (float)car->height) > (float)world_height) {
    car->y = (float)(world_height - car->height);
    car->vy = 0.0f;
  }
}

void RaceCar_ClampToScreen(RaceCar *car, uint16_t screen_width,
                           uint16_t screen_height) {
  RaceCar_ClampToWorld(car, screen_width, screen_height);
}

void RaceCar_ClampToHorizontalRange(RaceCar *car, float min_x, float max_x) {
  if ((car == NULL) || (car->active == false)) {
    return;
  }

  if (car->x < min_x) {
    car->x = min_x;
    car->vx = 0.0f;
  }

  if (car->x > max_x) {
    car->x = max_x;
    car->vx = 0.0f;
  }
}

void RaceCar_AddDamage(RaceCar *car, uint8_t amount) {
  uint16_t new_damage = 0U;

  if (car == NULL) {
    return;
  }

  new_damage = (uint16_t)car->damage + (uint16_t)amount;

  if (new_damage > RACE_MAX_DAMAGE) {
    new_damage = RACE_MAX_DAMAGE;
  }

  car->damage = (uint8_t)new_damage;
}

uint8_t RaceCar_GetDamage(const RaceCar *car) {
  if (car == NULL) {
    return 0U;
  }

  return car->damage;
}
