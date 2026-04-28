#ifndef GAME3_RENDER_H
#define GAME3_RENDER_H 

#include "game3_attacks/game3_attacks.h"
#include "game3_player.h"
#include "game3_enemy.h" 
#include "game3_attacks.h"
#include "game3_camera.h"

void Game3_Render_Draw_World(const Game3_Camera *camera); 
void Game3_Render_Draw_Player(const Game3_Player *player, const Game3_Camera *camera);
void Game3_Render_Draw_Enemy(const Game3_Enemy *enemy, const Game3_Camera *camera);
void Game3_Render_Draw_Player_Attack(const Game3_Player *player, const Game3_Camera *camera);
void Game3_Render_Draw_Projectile(const Game3_Projectile *projectile, const Game3_Camera *camera);
void Game3_Render_Draw_Armour_Pack(int16_t x, int16_t y, uint8_t is_active, const Game3_Camera *camera);
void Game3_Render_Draw_ChargerEnemy(const Game3_ChargerEnemy *enemy, const Game3_Camera *camera); 

#endif 

