#ifndef RACE_INPUT_H
#define RACE_INPUT_H

#include <stdbool.h>

typedef struct {
  float throttle;
  float brake;
  float steering;
  bool boost_pressed;
} RaceInput;

void RaceInput_Read(RaceInput *input);

#endif