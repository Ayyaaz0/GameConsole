#ifndef RACE_INPUT_H
#define RACE_INPUT_H

typedef struct {
  float throttle;
  float brake;
  float steering;
  float move_x;
  float move_y;
} RaceInput;

void RaceInput_Read(RaceInput *input);

#endif