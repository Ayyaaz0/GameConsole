#include "race_car.h"
#include "../config/race_config.h"
#include <stddef.h>
void RaceCar_Init(RaceCar *car) {
  if (car == NULL) {
    return;
  }

  car->x = RACE_PLAYER_START_X;
  car->y = RACE_PLAYER_START_Y;
  car->width = RACE_PLAYER_WIDTH;
  car->height = RACE_PLAYER_HEIGHT;
  car->active = true;
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