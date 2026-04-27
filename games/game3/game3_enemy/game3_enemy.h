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

    uint8_t health; 
    uint8_t max_health; 
    uint8_t is_alive; 
} Game3_Enemy;  

typedef enum {
    GAME3_CHARGER_STATE_IDLE = 0, 
    GAME3_CHARGER_STATE_CHARGING, 
    GAME3_CHARGER_STATE_COOLDOWN
} Game3_ChargerEnemyState;

typedef struct { 
    int16_t x; 
    int16_t y; 
    int16_t charge_dx; 
    int16_t charge_target_x; 

    uint8_t width; 
    uint8_t height; 

    uint8_t health; 
    uint8_t max_health; 
    uint8_t is_alive; 

    Game3_ChargerEnemyState state; 
    uint32_t state_end_time_ms; 

    uint32_t hit_flash_end_time_ms; 
    uint32_t last_attack_hit_time_ms; 
} Game3_ChargerEnemy; 

// BASIC ENEMY
void Game3_Enemy_Init(Game3_Enemy *enemy); 
void Game3_Enemy_Update(Game3_Enemy *enemy, const Game3_Player *player); 
uint8_t Game3_Enemy_Is_Touching_Player(const Game3_Enemy *enemy, const Game3_Player *player); 
uint8_t Game3_Enemy_Start_Attack_Knockback(Game3_Enemy *enemy, const Game3_Player *player);
uint8_t Game3_Enemy_Is_Hit_Flashing(const Game3_Enemy *enemy);
uint8_t Game3_Enemy_Is_Touching_Player_Attack(const Game3_Enemy *enemy, const Game3_Player *player);

void Game3_Enemy_Take_Damage(Game3_Enemy *enemy, uint8_t amount);
uint8_t Game3_Enemy_Is_Alive(const Game3_Enemy *enemy);


// CHARGER ENEMY
void Game3_ChargerEnemy_Init(Game3_ChargerEnemy *enemy);
void Game3_ChargerEnemy_Update(Game3_ChargerEnemy *enemy, const Game3_Player *player);

uint8_t Game3_ChargerEnemy_Is_Touching_Player(const Game3_ChargerEnemy *enemy, const Game3_Player *player);
uint8_t Game3_ChargerEnemy_Is_Touching_Player_Attack(const Game3_ChargerEnemy *enemy, const Game3_Player *player);

uint8_t Game3_ChargerEnemy_Start_Attack_Hit(Game3_ChargerEnemy *enemy);
void Game3_ChargerEnemy_Take_Damage(Game3_ChargerEnemy *enemy, uint8_t amount);

uint8_t Game3_ChargerEnemy_Is_Alive(const Game3_ChargerEnemy *enemy);
uint8_t Game3_ChargerEnemy_Is_Hit_Flashing(const Game3_ChargerEnemy *enemy);

#endif