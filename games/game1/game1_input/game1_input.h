#ifndef GAME1_INPUT_H
#define GAME1_INPUT_H

#include <stdint.h>

typedef struct {
  int16_t dx;
  uint8_t jump_pressed;
  uint8_t interact_pressed;
} Game1_Input;

void Game1_Input_Read(Game1_Input *input);

#endif