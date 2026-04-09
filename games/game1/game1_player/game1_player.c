#include "game1_player.h"

#include "Joystick.h"

extern Joystick_cfg_t joystick_cfg;
extern Joystick_t joystick_data;

#define GAME1_SCREEN_WIDTH 240
#define GAME1_SCREEN_HEIGHT 240

static void Game1_Player_ClampToScreen(Game1_Player *player) {
  if (player->x < 0) {
    player->x = 0;
  }

  if (player->y < 0) {
    player->y = 0;
  }

  if (player->x + player->width > GAME1_SCREEN_WIDTH) {
    player->x = GAME1_SCREEN_WIDTH - player->width;
  }

  if (player->y + player->height > GAME1_SCREEN_HEIGHT) {
    player->y = GAME1_SCREEN_HEIGHT - player->height;
  }
}

void Game1_Player_Init(Game1_Player *player) {
  player->x = 40;
  player->y = 40;
  player->width = 8;
  player->height = 8;
  player->speed = 2;
}

void Game1_Player_Update(Game1_Player *player) {
  Joystick_Read(&joystick_cfg, &joystick_data);

  switch (joystick_data.direction) {
  case N:
    player->y -= player->speed;
    break;

  case E:
    player->x += player->speed;
    break;

  case S:
    player->y += player->speed;
    break;

  case W:
    player->x -= player->speed;
    break;

  case NW:
    player->x -= player->speed;
    player->y -= player->speed;
    break;

  case NE:
    player->x += player->speed;
    player->y -= player->speed;
    break;

  case SW:
    player->x -= player->speed;
    player->y += player->speed;
    break;

  case SE:
    player->x += player->speed;
    player->y += player->speed;
    break;

  case CENTRE:
  default:
    break;
  }

  Game1_Player_ClampToScreen(player);
}