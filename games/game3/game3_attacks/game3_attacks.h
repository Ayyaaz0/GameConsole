#ifndef GAME3_ATTACKS_H
#define GAME3_ATTACKS_H

#include <stdint.h> 
#include "game3_enemy.h"
#include "game3_player.h"

typedef struct { 
    int16_t x;
    int16_t y; 
    int16_t vx; 
    uint8_t width; 
    uint8_t height; 
    uint8_t is_active; 
} Game3_Projectile; 

void Game3_Projectile_Init(Game3_Projectile *projectile); 
void Game3_Projectile_Fire(Game3_Projectile *projectile, const Game3_Player *player); 
void Game3_Projectile_Update(Game3_Projectile *projectile); 
uint8_t Game3_Projectile_Is_Touching_Enemy(const Game3_Projectile *projectile, const Game3_Enemy *enemy);

#endif