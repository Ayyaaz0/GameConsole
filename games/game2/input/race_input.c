#include "race_input.h"

#include "Joystick.h"

#include <stddef.h>

extern Joystick_cfg_t joystick_cfg;
extern Joystick_t joystick_data;

static float g_smoothed_move_x = 0.0f;
static float g_smoothed_move_y = 0.0f;

static void RaceInput_Reset(RaceInput *input) {
  input->throttle = 0.0f;
  input->brake = 0.0f;
  input->steering = 0.0f;
  input->move_x = 0.0f;
  input->move_y = 0.0f;
}

static float RaceInput_ClampUnit(float value) {
  if (value > 1.0f) {
    return 1.0f;
  }

  if (value < -1.0f) {
    return -1.0f;
  }

  return value;
}

static float RaceInput_ApplyDeadzone(float value, float deadzone) {
  float sign = 1.0f;
  float magnitude = value;

  if (magnitude < 0.0f) {
    sign = -1.0f;
    magnitude = -magnitude;
  }

  if (magnitude < deadzone) {
    return 0.0f;
  }

  // Re-scale after the deadzone so movement still reaches full strength.
  magnitude = (magnitude - deadzone) / (1.0f - deadzone);

  return sign * RaceInput_ClampUnit(magnitude);
}

void RaceInput_Read(RaceInput *input) {
  float raw_move_x = 0.0f;
  float raw_move_y = 0.0f;

  const float deadzone = 0.14f;
  const float input_smoothing = 0.32f;

  if (input == NULL) {
    return;
  }

  RaceInput_Reset(input);

  Joystick_Read(&joystick_cfg, &joystick_data);

  raw_move_x = joystick_data.coord_mapped.x;
  raw_move_y = -joystick_data.coord_mapped.y;

  raw_move_x = RaceInput_ApplyDeadzone(raw_move_x, deadzone);
  raw_move_y = RaceInput_ApplyDeadzone(raw_move_y, deadzone);

  // Smooth the joystick so small ADC noise does not make the car jitter.

  g_smoothed_move_x += (raw_move_x - g_smoothed_move_x) * input_smoothing;
  g_smoothed_move_y += (raw_move_y - g_smoothed_move_y) * input_smoothing;

  if ((g_smoothed_move_x > -0.03f) && (g_smoothed_move_x < 0.03f)) {
    g_smoothed_move_x = 0.0f;
  }

  if ((g_smoothed_move_y > -0.03f) && (g_smoothed_move_y < 0.03f)) {
    g_smoothed_move_y = 0.0f;
  }

  input->move_x = RaceInput_ClampUnit(g_smoothed_move_x);
  input->move_y = RaceInput_ClampUnit(g_smoothed_move_y);

  input->steering = input->move_x;

  if ((input->move_x != 0.0f) || (input->move_y != 0.0f)) {
    input->throttle = 1.0f;
  }

  if (input->move_y > 0.65f) {
    input->brake = 1.0f;
  }
}