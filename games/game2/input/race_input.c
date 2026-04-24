#include "race_input.h"
#include "../config/race_config.h"
#include "Joystick.h"

extern Joystick_cfg_t joystick_cfg;
extern Joystick_t joystick_data;
void RaceInput_Read(RaceInput *input) {
  if (input == NULL) {
    return;
  }

  input->throttle = 0.0f;
  input->brake = 0.0f;
  input->steering = 0.0f;

  Joystick_Read(&joystick_cfg, &joystick_data);

  switch (joystick_data.direction) {
  case N:
    input->throttle = 1.0f;
    break;

  case S:
    input->brake = 1.0f;
    break;

  case W:
    input->steering = -1.0f;
    break;

  case E:
    input->steering = 1.0f;
    break;

  case NW:
    input->throttle = 1.0f;
    input->steering = -1.0f;
    break;

  case NE:
    input->throttle = 1.0f;
    input->steering = 1.0f;
    break;

  case SW:
    input->brake = 1.0f;
    input->steering = -1.0f;
    break;

  case SE:
    input->brake = 1.0f;
    input->steering = 1.0f;
    break;

  case CENTRE:
  default:
    break;
  }
}