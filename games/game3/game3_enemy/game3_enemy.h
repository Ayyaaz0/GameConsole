#ifndef GAME3_ENEMY_H
#define GAME3_ENEMY_H

#include <stdint.h> 
#include "game3_player.h"

typedef struct { 
    int16_t x; 
    int16_t y; 
    int16_t move_speed; 
    int16_t knockback_speed; 
    int8_t knockback_dx; 

    uint8_t width; 
    uint8_t height; 

    uint8_t is_in_knockback; 
    uint32_t knockback_end_time_ms; 

    uint32_t hit_flash_end_time_ms; 
    uint32_t last_attack_hit_time_ms; 
} Game3_Enemy;  

void Game3_Enemy_Init(Game3_Enemy *enemy); 
void Game3_Enemy_Update(Game3_Enemy *enemy, const Game3_Player *player); 
uint8_t Game3_Enemy_Is_Touching_Player(const Game3_Enemy *enemy, const Game3_Player *player); 
void Game3_Enemy_Start_Attack_Knockback(Game3_Enemy *enemy, const Game3_Player *player);
uint8_t Game3_Enemy_Is_Hit_Flashing(const Game3_Enemy *enemy);
uint8_t Game3_Enemy_Is_Touching_Player_Attack(const Game3_Enemy *enemy, const Game3_Player *player);

#endif