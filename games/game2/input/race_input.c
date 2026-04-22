#include "race_input.h"
#include "../config/race_config.h"
#include "Joystick.h"

extern Joystick_cfg_t joystick_cfg;
extern Joystick_t joystick_data;
void RaceInput_Read(RaceInput *input) {
  if (input == NULL) {
    return;
  }

  input->move_x = 0.0f;
  input->move_y = 0.0f;

  Joystick_Read(&joystick_cfg, &joystick_data);

  switch (joystick_data.direction) {
  case E:
    input->move_x = RACE_PLAYER_SPEED;
    break;

  case W:
    input->move_x = -RACE_PLAYER_SPEED;
    break;

  case N:
    input->move_y = -RACE_PLAYER_SPEED;
    break;

  case S:
    input->move_y = RACE_PLAYER_SPEED;
    break;

  case NE:
    input->move_x = RACE_PLAYER_SPEED;
    input->move_y = -RACE_PLAYER_SPEED;
    break;

  case NW:
    input->move_x = -RACE_PLAYER_SPEED;
    input->move_y = -RACE_PLAYER_SPEED;
    break;

  case SE:
    input->move_x = RACE_PLAYER_SPEED;
    input->move_y = RACE_PLAYER_SPEED;
    break;

  case SW:
    input->move_x = -RACE_PLAYER_SPEED;
    input->move_y = RACE_PLAYER_SPEED;
    break;

  case CENTRE:
  default:
    break;
  }
}