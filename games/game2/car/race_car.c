#include "race_car.h"

#include "../config/race_config.h"
#include "../utils/race_math.h"

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

static float RaceCar_GetArcadeHeading(float move_x, float move_y,
                                      float current_heading) {
  if ((move_x == 0.0f) && (move_y == 0.0f)) {
    return current_heading;
  }

  if ((move_x == 0.0f) && (move_y < 0.0f)) {
    return 0.0f;
  }

  if ((move_x > 0.0f) && (move_y < 0.0f)) {
    return 45.0f;
  }

  if ((move_x > 0.0f) && (move_y == 0.0f)) {
    return 90.0f;
  }

  if ((move_x > 0.0f) && (move_y > 0.0f)) {
    return 135.0f;
  }

  if ((move_x == 0.0f) && (move_y > 0.0f)) {
    return 180.0f;
  }

  if ((move_x < 0.0f) && (move_y > 0.0f)) {
    return 225.0f;
  }

  if ((move_x < 0.0f) && (move_y == 0.0f)) {
    return 270.0f;
  }

  return 315.0f;
}

static void RaceCar_ApplyArcadeSpeed(RaceCar *car, const RaceInput *input) {
  if ((input->move_x != 0.0f) || (input->move_y != 0.0f)) {
    car->speed += RACE_CAR_ACCELERATION;
  } else {
    car->speed -= RACE_CAR_DRAG;
  }

  car->speed = RaceMath_ClampFloat(car->speed, 0.0f, RACE_CAR_MAX_SPEED);
}

static void RaceCar_ApplyArcadeMovement(RaceCar *car, const RaceInput *input) {
  float move_x = input->move_x;
  float move_y = input->move_y;
  float diagonal_multiplier = 1.0f;

  if ((move_x == 0.0f) && (move_y == 0.0f)) {
    return;
  }

  if ((move_x != 0.0f) && (move_y != 0.0f)) {
    diagonal_multiplier = 0.7071f;
  }

  car->heading_deg = RaceCar_GetArcadeHeading(move_x, move_y, car->heading_deg);

  car->x += move_x * car->speed * diagonal_multiplier;
  car->y += move_y * car->speed * diagonal_multiplier;
}

void RaceCar_Init(RaceCar *car) {
  if (car == NULL) {
    return;
  }

  car->x = RACE_PLAYER_START_X;
  car->y = RACE_PLAYER_START_Y;
  car->prev_x = car->x;
  car->prev_y = car->y;
  car->speed = 0.0f;
  car->heading_deg = 0.0f;
  car->prev_heading_deg = 0.0f;
  car->width = RACE_PLAYER_WIDTH;
  car->height = RACE_PLAYER_HEIGHT;
  car->active = true;
}

void RaceCar_UpdatePhysics(RaceCar *car, const RaceInput *input) {
  if ((car == NULL) || (input == NULL) || (car->active == false)) {
    return;
  }

  RaceCar_SavePreviousPosition(car);
  RaceCar_ApplyArcadeSpeed(car, input);
  RaceCar_ApplyArcadeMovement(car, input);
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

void RaceCar_ClampToScreen(RaceCar *car, uint16_t screen_width,
                           uint16_t screen_height) {
  if ((car == NULL) || (car->active == false)) {
    return;
  }

  if (car->x < 0.0f) {
    car->x = 0.0f;
  }

  if (car->y < 0.0f) {
    car->y = 0.0f;
  }

  if ((car->x + car->width) > screen_width) {
    car->x = (float)(screen_width - car->width);
  }

  if ((car->y + car->height) > screen_height) {
    car->y = (float)(screen_height - car->height);
  }
}

void RaceCar_ClampToHorizontalRange(RaceCar *car, float min_x, float max_x) {
  if ((car == NULL) || (car->active == false)) {
    return;
  }

  if (car->x < min_x) {
    car->x = min_x;
  }

  if (car->x > max_x) {
    car->x = max_x;
  }
}