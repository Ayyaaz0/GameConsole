#ifndef GAME3_RENDER_H
#define GAME3_RENDER_H 

#include "game3_player.h"
#include "game3_enemy.h" 

void Game3_Render_Draw_World(void); 
void Game3_Render_Draw_Player(const Game3_Player *player);
void Game3_Render_Draw_Enemy(const Game3_Enemy *enemy);

#endif 

