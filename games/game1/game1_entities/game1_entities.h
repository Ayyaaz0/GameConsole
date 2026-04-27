#ifndef GAME1_ENTITIES_H
#define GAME1_ENTITIES_H

#include "game1_player.h"
#include "game1_camera.h"
#include <stdint.h>

void Game1_Entities_Init(void);
void Game1_Entities_Update(Game1_Player *player, uint8_t interact_pressed);
void Game1_Entities_Render(const Game1_Camera *camera);

#endif