#include "game1_animation.h"

void Game1_Animation_Init(Game1_AnimationState *state, const Game1_Animation *anim) {
  state->anim = anim;
  state->timer = 0;
}

uint16_t Game1_Animation_Update(Game1_AnimationState *state) {
  if (!state->anim || state->anim->frame_count == 0) {
    return 0;
  }

  uint8_t frame = (state->timer / state->anim->speed) % state->anim->frame_count;

  state->timer++;

  return state->anim->frames[frame];
}