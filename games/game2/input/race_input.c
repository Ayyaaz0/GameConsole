#include "race_input.h"

#include "Joystick.h"

#include <stddef.h>

extern Joystick_cfg_t joystick_cfg;
extern Joystick_t joystick_data;

static void RaceInput_Reset(RaceInput *input) {
  input->throttle = 0.0f;
  input->brake = 0.0f;
  input->steering = 0.0f;
  input->move_x = 0.0f;
  input->move_y = 0.0f;
}

void RaceInput_Read(RaceInput *input) {
  if (input == NULL) {
    return;
  }

  RaceInput_Reset(input);

  Joystick_Read(&joystick_cfg, &joystick_data);

  switch (joystick_data.direction) {
  case N:
    input->throttle = 1.0f;
    input->move_y = -1.0f;
    break;

  case NE:
    input->throttle = 1.0f;
    input->steering = 1.0f;
    input->move_x = 1.0f;
    input->move_y = -1.0f;
    break;

  case E:
    input->throttle = 1.0f;
    input->steering = 1.0f;
    input->move_x = 1.0f;
    break;

  case SE:
    input->throttle = 1.0f;
    input->brake = 1.0f;
    input->steering = 1.0f;
    input->move_x = 1.0f;
    input->move_y = 1.0f;
    break;

  case S:
    input->brake = 1.0f;
    input->move_y = 1.0f;
    break;

  case SW:
    input->throttle = 1.0f;
    input->brake = 1.0f;
    input->steering = -1.0f;
    input->move_x = -1.0f;
    input->move_y = 1.0f;
    break;

  case W:
    input->throttle = 1.0f;
    input->steering = -1.0f;
    input->move_x = -1.0f;
    break;

  case NW:
    input->throttle = 1.0f;
    input->steering = -1.0f;
    input->move_x = -1.0f;
    input->move_y = -1.0f;
    break;

  case CENTRE:
  default:
    break;
  }
}