#ifndef GAME1_RENDER_H
#define GAME1_RENDER_H

#include "game1_camera/game1_camera.h"
#include "game1_player/game1_player.h"

void Game1_Render_DrawWorld(const Game1_Camera *camera);
void Game1_Render_DrawPlayer(const Game1_Player *player, const Game1_Camera *camera);

#endif