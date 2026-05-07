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
}

static void RaceCar_ApplyInputVelocity(RaceCar *car, const RaceInput *input) {
  float input_x = 0.0f;
  float input_y = 0.0f;
  float diagonal_scale = 1.0f;

  if ((car == NULL) || (input == NULL)) {
    return;
  }

  // X axis:
  // steering < 0 = move west / left
  // steering > 0 = move east / right
  input_x = input->steering;

  // Y axis:
  // throttle = joystick up / north
  // brake    = joystick down / south
  //
  // Screen/world coordinates increase downward, so north is negative Y
  input_y = input->brake - input->throttle;

  // Prevent diagonals from being unfairly faster
  if ((input_x != 0.0f) && (input_y != 0.0f)) {
    diagonal_scale = 0.7071f;
  }

  car->vx += input_x * RACE_CAR_TURN_SPEED * diagonal_scale;
  car->vy += input_y * RACE_CAR_ACCELERATION * diagonal_scale;
}

static void RaceCar_ApplyDrag(RaceCar *car) {
  if (car == NULL) {
    return;
  }

  car->vx *= RACE_CAR_DRAG_MULTIPLIER;
  car->vy *= RACE_CAR_DRAG_MULTIPLIER;

  // Remove tiny drift so the car does not slowly slide forever
  if (RaceCar_AbsFloat(car->vx) < 0.03f) {
    car->vx = 0.0f;
  }

  if (RaceCar_AbsFloat(car->vy) < 0.03f) {
    car->vy = 0.0f;
  }
}

static void RaceCar_LimitSpeed(RaceCar *car) {
  float speed_sq = 0.0f;
  float max_speed_sq = 0.0f;
  float speed = 0.0f;
  float scale = 1.0f;

  if (car == NULL) {
    return;
  }

  speed_sq = (car->vx * car->vx) + (car->vy * car->vy);
  max_speed_sq = RACE_CAR_MAX_SPEED * RACE_CAR_MAX_SPEED;

  if (speed_sq <= max_speed_sq) {
    return;
  }

  speed = sqrtf(speed_sq);

  if (speed <= 0.0f) {
    return;
  }

  scale = RACE_CAR_MAX_SPEED / speed;

  car->vx *= scale;
  car->vy *= scale;
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
  car->width = RACE_PLAYER_WIDTH;
  car->height = RACE_PLAYER_HEIGHT;
  car->active = true;
}

void RaceCar_UpdatePhysics(RaceCar *car, const RaceInput *input) {
  if ((car == NULL) || (input == NULL) || (car->active == false)) {
    return;
  }

  RaceCar_SavePreviousPosition(car);
  RaceCar_ApplyInputVelocity(car, input);
  RaceCar_ApplyDrag(car);
  RaceCar_LimitSpeed(car);

  car->x += car->vx;
  car->y += car->vy;

  // Speed is the true movement magnitude, not just vertical velocity
  car->speed = sqrtf((car->vx * car->vx) + (car->vy * car->vy));
}

void RaceCar_RestorePreviousPosition(RaceCar *car) {
  if ((car == NULL) || (car->active == false)) {
    return;
  }

  car->x = car->prev_x;
  car->y = car->prev_y;
}

void RaceCar_ClampToWorld(RaceCar *car) {
  if ((car == NULL) || (car->active == false)) {
    return;
  }

  if (car->x < 0.0f) {
    car->x = 0.0f;
    car->vx *= RACE_WALL_BOUNCE_MULTIPLIER;
  }

  if (car->y < 0.0f) {
    car->y = 0.0f;
    car->vy *= RACE_WALL_BOUNCE_MULTIPLIER;
  }

  if ((car->x + car->width) > RACE_WORLD_WIDTH_PX) {
    car->x = (float)(RACE_WORLD_WIDTH_PX - car->width);
    car->vx *= RACE_WALL_BOUNCE_MULTIPLIER;
  }

  if ((car->y + car->height) > RACE_WORLD_HEIGHT_PX) {
    car->y = (float)(RACE_WORLD_HEIGHT_PX - car->height);
    car->vy *= RACE_WALL_BOUNCE_MULTIPLIER;
  }
}

void RaceCar_ApplySpeedMultiplier(RaceCar *car, float multiplier) {
  if ((car == NULL) || (car->active == false)) {
    return;
  }

  car->vx *= multiplier;
  car->vy *= multiplier;

  car->speed = sqrtf((car->vx * car->vx) + (car->vy * car->vy));
}