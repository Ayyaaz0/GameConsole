#include "race_car.h"
#include "../config/race_config.h"
#include "../utils/race_math.h"
#include <stddef.h>

static void RaceCar_ApplyThrottleAndBrake(RaceCar *car,
                                          const RaceInput *input) {
  if (input->throttle > 0.0f) {
    car->speed += RACE_CAR_ACCELERATION * input->throttle;
  }

  if (input->brake > 0.0f) {
    // If moving forward, brake hard.
    // If already stopped/slow, allow reverse.
    if (car->speed > 0.0f) {
      car->speed -= RACE_CAR_BRAKE_FORCE * input->brake;
    } else {
      car->speed -= (RACE_CAR_BRAKE_FORCE * 0.55f) * input->brake;
    }
  }
}

static void RaceCar_ApplyDrag(RaceCar *car) {
  if (car->speed > 0.0f) {
    car->speed -= RACE_CAR_DRAG;

    if (car->speed < 0.0f) {
      car->speed = 0.0f;
    }
  } else if (car->speed < 0.0f) {
    car->speed += RACE_CAR_DRAG;

    if (car->speed > 0.0f) {
      car->speed = 0.0f;
    }
  }
}

static void RaceCar_ApplySteering(RaceCar *car, const RaceInput *input) {
  float speed_abs = 0.0f;
  float speed_ratio = 0.0f;
  float steering_strength = 0.0f;

  if ((car->speed < RACE_CAR_MIN_TURN_SPEED) &&
      (car->speed > -RACE_CAR_MIN_TURN_SPEED)) {
    return;
  }

  speed_abs = car->speed;

  if (speed_abs < 0.0f) {
    speed_abs = -speed_abs;
  }

  // 0.0 = stopped, 1.0 = max speed
  speed_ratio = speed_abs / RACE_CAR_MAX_SPEED;

  if (speed_ratio > 1.0f) {
    speed_ratio = 1.0f;
  }

  // More speed now gives more steering response.
  steering_strength = RACE_CAR_TURN_RATE *
                      (1.0f + (speed_ratio * RACE_CAR_HIGH_SPEED_STEER_BOOST));

  car->heading_deg += input->steering * steering_strength;
}

static void RaceCar_ApplyForwardMovement(RaceCar *car) {
  float speed_abs = 0.0f;
  float speed_ratio = 0.0f;
  float lateral_move = 0.0f;

  speed_abs = car->speed;

  if (speed_abs < 0.0f) {
    speed_abs = -speed_abs;
  }

  speed_ratio = speed_abs / RACE_CAR_MAX_SPEED;

  if (speed_ratio > 1.0f) {
    speed_ratio = 1.0f;
  }

  lateral_move = car->heading_deg * RACE_CAR_LATERAL_GRIP *
                 (0.018f + (speed_ratio * 0.020f));

  car->x += lateral_move;
  car->y -= car->speed;

  car->heading_deg *= (0.65f + (speed_ratio * 0.18f));
}

void RaceCar_Init(RaceCar *car) {
  if (car == NULL) {
    return;
  }

  car->x = RACE_PLAYER_START_X;
  car->y = RACE_PLAYER_START_Y;
  car->speed = 0.0f;
  car->heading_deg = 0.0f;
  car->width = RACE_PLAYER_WIDTH;
  car->height = RACE_PLAYER_HEIGHT;
  car->active = true;
}

void RaceCar_UpdatePhysics(RaceCar *car, const RaceInput *input) {
  if ((car == NULL) || (input == NULL) || (car->active == false)) {
    return;
  }

  RaceCar_ApplyThrottleAndBrake(car, input);
  RaceCar_ApplyDrag(car);

  car->speed =
      RaceMath_ClampFloat(car->speed, RACE_CAR_MIN_SPEED, RACE_CAR_MAX_SPEED);

  RaceCar_ApplySteering(car, input);
  RaceCar_ApplyForwardMovement(car);
}

void RaceCar_Move(RaceCar *car, float dx, float dy) {
  if ((car == NULL) || (car->active == false)) {
    return;
  }

  car->x += dx;
  car->y += dy;
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