#ifndef GAME3_ENEMY_H
#define GAME3_ENEMY_H

#include <stdint.h> 
#include "game3_player.h"

typedef struct { 
    int16_t x; 
    int16_t y; 
    int16_t move_speed; 
    uint8_t width; 
    uint8_t height; 
} Game3_Enemy; 

void Game3_Enemy_Init(Game3_Enemy *enemy); 
void Game3_Enemy_Update(Game3_Enemy *enemy, const Game3_Player *player); 

#endif