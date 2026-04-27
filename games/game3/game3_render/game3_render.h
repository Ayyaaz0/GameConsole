#ifndef GAME3_RENDER_H
#define GAME3_RENDER_H 

#include "game3_attacks/game3_attacks.h"
#include "game3_player.h"
#include "game3_enemy.h" 
#include "game3_attacks.h"

void Game3_Render_Draw_World(void); 
void Game3_Render_Draw_Player(const Game3_Player *player);
void Game3_Render_Draw_Enemy(const Game3_Enemy *enemy);
void Game3_Render_Draw_Player_Attack(const Game3_Player *player);
void Game3_Render_Draw_Projectile(const Game3_Projectile *projectile);
void Game3_Render_Draw_Armour_Pack(int16_t x, int16_t y, uint8_t is_active);

#endif 

