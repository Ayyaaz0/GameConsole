#include "game1_input.h"

#include "InputHandler.h"
#include "Joystick.h"

extern Joystick_cfg_t joystick_cfg;
extern Joystick_t joystick_data;

void Game1_Input_Read(Game1_Input *input) {
  input->dx = 0;
  input->jump_pressed = current_input.btn2_pressed;

  Joystick_Read(&joystick_cfg, &joystick_data);

  switch (joystick_data.direction) {
  case W:
  case NW:
  case SW:
    input->dx = -1;
    break;

  case E:
  case NE:
  case SE:
    input->dx = 1;
    break;

  default:
    input->dx = 0;
    break;
  }
}