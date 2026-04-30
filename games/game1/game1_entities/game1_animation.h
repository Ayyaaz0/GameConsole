#ifndef GAME1_ANIMATION_H
#define GAME1_ANIMATION_H

#include <stdint.h>

typedef struct {
  const uint16_t *frames;   // array of GIDs
  uint8_t frame_count;
  uint8_t speed;            // frames per step
} Game1_Animation;

typedef struct {
  const Game1_Animation *anim;
  uint16_t timer;
} Game1_AnimationState;

void Game1_Animation_Init(Game1_AnimationState *state, const Game1_Animation *anim);

uint16_t Game1_Animation_Update(Game1_AnimationState *state);

#endif