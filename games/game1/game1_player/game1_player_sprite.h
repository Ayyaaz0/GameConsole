#ifndef GAME1_PLAYER_SPRITE_H
#define GAME1_PLAYER_SPRITE_H

#include "game1_camera.h"
#include "game1_player.h"

void Game1_PlayerSprite_Init(void);
void Game1_PlayerSprite_Render(const Game1_Player *player, const Game1_Camera *camera);

#endif