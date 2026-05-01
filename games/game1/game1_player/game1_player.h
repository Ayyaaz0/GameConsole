#ifndef GAME1_PLAYER_H
#define GAME1_PLAYER_H

#include <stdint.h>

typedef struct {
  int16_t x;
  int16_t y;
  int16_t vx;
  int16_t vy;

  uint8_t width;
  uint8_t height;

  int16_t move_speed;
  int16_t jump_strength;
  int16_t gravity;

  uint8_t grounded;
  uint8_t air_jumps_remaining;
  uint8_t coyote_timer;

  uint8_t has_key;
  uint8_t alive;
  uint16_t death_timer;
} Game1_Player;

void Game1_Player_Init(Game1_Player *player);
void Game1_Player_Update(Game1_Player *player, int16_t dx, uint8_t jump_pressed);

#endif