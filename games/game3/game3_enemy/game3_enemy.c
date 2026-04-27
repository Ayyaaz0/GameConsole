#include "game3_enemy.h"
#include "game3_player.h"
#include "game3_world.h"
#include "stm32l4xx_hal.h"

#include <stdint.h> 

#define GAME3_ENEMY_KNOCKBACK_DURATION_MS 150
#define GAME3_ENEMY_HIT_FLASH_MS    200
#define GAME3_ENEMY_ATTACK_HIT_COOLDOWN_MS 250
#define GAME3_ATTACK_SIZE   8  
#define GAME3_ENEMY_START_HEALTH    3

// CHARGER DEFINE

#define GAME3_CHARGER_START_HEALTH  4
#define GAME3_CHARGER_IDLE_MS   900
#define GAME3_CHARGER_COOLDOWN_MS   700

#define GAME3_CHARGER_SPEED 7

#define GAME3_CHARGER_HIT_FLASH_MS  200
#define GAME3_CHARGER_ATTACK_COOLDOWN_MS    250

static int16_t Game3_Enemy_Get_Player_Centre_X(const Game3_Player *player) { 
    return player->x + (player->width / 2); 
}

/* 
static int16_t Game3_Enemy_Get_Player_Centre_Y(const Game3_Player *player) { 
    return player->y + (player->height / 2); 
}
*/ 
static int16_t Game3_Enemy_Get_Enemy_Centre_X(const Game3_Enemy *enemy) { 
    return enemy->x + (enemy->width / 2); 
}

/* 
static int16_t Game3_Enemy_Get_Enemy_Centre_Y(const Game3_Enemy *enemy) { 
    return enemy->y + (enemy->height / 2); 
}
*/ 

uint8_t Game3_Enemy_Is_Touching_Player(const Game3_Enemy *enemy, const Game3_Player *player) { 

    if (!enemy->is_alive) { 
        return 0;
    }

    if ((enemy->x + enemy->width) <= player->x) { 
        return 0; 
    }

    if (enemy->x > (player->x + player->width)) { 
        return 0; 
    }

    if ((enemy->y + enemy->height) <= player->y) { 
        return 0; 
    }

    if (enemy->y > (player->y + player->height)) { 
        return 0; 
    }

    return 1; 
}


// Prevents enemy from getting pushed OUTSIDE of the world width
static void Game3_Enemy_Clamp_To_World(Game3_Enemy *enemy) { 
    if (enemy->x < 0) { 
        enemy->x = 0; 
    }

    if (enemy->x > (GAME3_WORLD_WIDTH_PX - enemy->width)) { 
        enemy->x = GAME3_WORLD_WIDTH_PX - enemy->width; 
    }
}

void Game3_Enemy_Init(Game3_Enemy *enemy) { 
    enemy->width = 8; 
    enemy->height = 8; 
    enemy->move_speed = 1; 
    enemy->knockback_speed = 3;
    enemy->knockback_dx = 0;  

    enemy->x = 20 * GAME3_TILE_SIZE; 
    enemy->y = ((GAME3_ROOM_HEIGHT - 2) * GAME3_TILE_SIZE) - enemy->height; 

    enemy->is_in_knockback = 0; 
    enemy->knockback_end_time_ms = 0; 

    enemy->hit_flash_end_time_ms = 0; 
    enemy->last_attack_hit_time_ms = 0; 

    enemy->max_health = GAME3_ENEMY_START_HEALTH; 
    enemy->health = enemy->max_health; 
    enemy->is_alive = 1; 
}

void Game3_Enemy_Update(Game3_Enemy *enemy, const Game3_Player *player) { 

    if (!enemy->is_alive) { 
        return;   
    }

    uint32_t now = HAL_GetTick(); 

    if (enemy->is_in_knockback) { 
        if (now >= enemy->knockback_end_time_ms) { 
            enemy->is_in_knockback = 0; 
        } else { 
            enemy->x += enemy->knockback_dx * enemy->knockback_speed; 
            Game3_Enemy_Clamp_To_World(enemy);
            return; 
        }
    }

    if (Game3_Enemy_Is_Touching_Player(enemy, player)) { 
        int16_t enemy_centre_x = Game3_Enemy_Get_Enemy_Centre_X(enemy);
        int16_t player_centre_x = Game3_Enemy_Get_Player_Centre_X(player); 

        enemy->is_in_knockback = 1; 
        enemy->knockback_end_time_ms = now + GAME3_ENEMY_KNOCKBACK_DURATION_MS; 

        if (enemy_centre_x < player_centre_x) { 
            enemy->knockback_dx = -2; // Left
        } else { 
            enemy->knockback_dx = 2; // Right
        }

        enemy->x += enemy->knockback_dx * enemy->knockback_speed; 
        Game3_Enemy_Clamp_To_World(enemy);
        return; 
    }
    
    int16_t enemy_centre_x = Game3_Enemy_Get_Enemy_Centre_X(enemy); 
    int16_t player_centre_x = Game3_Enemy_Get_Player_Centre_X(player);

    if (enemy_centre_x < player_centre_x) { 
        enemy->x += enemy->move_speed; 
    } else if (enemy_centre_x > player_centre_x) { 
        enemy->x -= enemy->move_speed; 
    }

    Game3_Enemy_Clamp_To_World(enemy);
}

uint8_t Game3_Enemy_Is_Touching_Player_Attack(const Game3_Enemy *enemy, const Game3_Player *player) { 

    if (!enemy->is_alive) { 
        return 0; 
    }

    if (!Game3_Player_Is_Attacking(player)) { 
        return 0; 
    }

    int16_t attack_x; 

    if (player->facing_dx < 0) { 
        attack_x = player->x - GAME3_ATTACK_SIZE; 
    } else { 
        attack_x = player->x + player->width; 
    }

    int16_t attack_y = player->y; 

    if (attack_x < 0) { 
        attack_x = 0; 
    }

    if (attack_x > (GAME3_WORLD_WIDTH_PX - GAME3_ATTACK_SIZE)) { 
        attack_x = GAME3_WORLD_WIDTH_PX - GAME3_ATTACK_SIZE; 
    }   

    if ((enemy->x + enemy->width) <= attack_x) {
        return 0; 
    }

    if (enemy->x >= (attack_x + GAME3_ATTACK_SIZE)) { 
        return 0; 
    }

    if ((enemy->y + enemy->height) <= attack_y) { 
        return 0; 
    }

    if (enemy->y >= (attack_y + GAME3_ATTACK_SIZE)) {
        return 0; 
    }

    return 1; 
}

uint8_t Game3_Enemy_Start_Attack_Knockback(Game3_Enemy *enemy, const Game3_Player *player) { 
    uint32_t now = HAL_GetTick(); 

    if (!enemy->is_alive) {
        return 0;
    }
    
    if (enemy->last_attack_hit_time_ms != 0 && (now - enemy->last_attack_hit_time_ms) < GAME3_ENEMY_ATTACK_HIT_COOLDOWN_MS) { 
        return 0; 
    }   
    
    enemy->last_attack_hit_time_ms = now;  

    Game3_Enemy_Take_Damage(enemy, 1);

    enemy->hit_flash_end_time_ms = now + GAME3_ENEMY_HIT_FLASH_MS;

    if (!enemy->is_alive) {
        return 1;
    }

    enemy->is_in_knockback = 1; 
    enemy->knockback_end_time_ms = now + GAME3_ENEMY_KNOCKBACK_DURATION_MS; 

    if (player->facing_dx < 0) { 
        enemy->knockback_dx = -2; 
    } else { 
        enemy->knockback_dx = 2; 
    }

    enemy->x += enemy->knockback_dx * enemy->knockback_speed; 
    Game3_Enemy_Clamp_To_World(enemy);

    return 1;
}

void Game3_Enemy_Take_Damage(Game3_Enemy *enemy, uint8_t amount) { 
    if (!enemy->is_alive) { 
        return; 
    }

    if (amount >= enemy->health) { 
        enemy->health = 0; 
        enemy->is_alive = 0; 

        enemy->is_in_knockback = 0; 
        enemy->knockback_dx = 0; 
        enemy->hit_flash_end_time_ms = 0; 
        return; 
    }

    enemy->health -= amount; 
}

uint8_t Game3_Enemy_Is_Alive(const Game3_Enemy *enemy) { 
    return enemy->is_alive;
}

uint8_t Game3_Enemy_Is_Hit_Flashing(const Game3_Enemy *enemy) { 
    return HAL_GetTick() < enemy->hit_flash_end_time_ms; 
}

// CHARGER ENEMY

static int16_t Game3_ChargerEnemy_Get_Player_Centre_X(const Game3_Player *player) { 
    return player->x + (player->width / 2); 
}

static int16_t Game3_ChargerEnemy_Get_Centre_X(const Game3_ChargerEnemy *enemy) { 
    return enemy->x + (enemy->width / 2); 
}

static void Game3_ChargerEnemy_Clamp_To_World(Game3_ChargerEnemy *enemy) { 
    if (enemy->x < 0) { 
        enemy->x = 0; 
    }
    
    if (enemy->x > (GAME3_WORLD_WIDTH_PX - enemy->width)) { 
        enemy->x = GAME3_WORLD_WIDTH_PX - enemy->width; 
    }
}

void Game3_ChargerEnemy_Init(Game3_ChargerEnemy *enemy) { 
    enemy->width = 10; 
    enemy->height = 10; 

    enemy->x = 10 * GAME3_TILE_SIZE; 
    enemy->y = ((GAME3_ROOM_HEIGHT - 2) * GAME3_TILE_SIZE) - enemy->height; 

    enemy->charge_dx = 1; 
    enemy->charge_target_x = enemy->x; 

    enemy->max_health = GAME3_CHARGER_START_HEALTH; 
    enemy->health = enemy->max_health; 
    enemy->is_alive = 1;

    enemy->state = GAME3_CHARGER_STATE_IDLE;
    enemy->state_end_time_ms = HAL_GetTick() + GAME3_CHARGER_IDLE_MS; 

    enemy->hit_flash_end_time_ms = 0; 
    enemy->last_attack_hit_time_ms = 0; 
}

void Game3_ChargerEnemy_Update(Game3_ChargerEnemy *enemy, const Game3_Player *player) { 
    if (!enemy->is_alive) { 
        return; 
    }

    uint32_t now = HAL_GetTick(); 

    if (enemy->state == GAME3_CHARGER_STATE_IDLE) { 
        if (now >= enemy->state_end_time_ms) { 
            int16_t enemy_centre_x = Game3_ChargerEnemy_Get_Centre_X(enemy); 
            int16_t player_centre_x = Game3_ChargerEnemy_Get_Player_Centre_X(player);

            enemy->charge_target_x = player_centre_x; 

            if (enemy_centre_x < player_centre_x) { 
                enemy->charge_dx = 1; 
            } else { 
                enemy->charge_dx = -1; 
            }

            enemy->state = GAME3_CHARGER_STATE_CHARGING; 
        }

        return; 
    }

    if (enemy->state == GAME3_CHARGER_STATE_CHARGING) { 
        enemy->x += enemy->charge_dx * GAME3_CHARGER_SPEED; 
        Game3_ChargerEnemy_Clamp_To_World(enemy);

        int16_t enemy_centre_x = Game3_ChargerEnemy_Get_Centre_X(enemy);

        uint8_t reached_target = 0; 

        if (enemy->charge_dx > 0 && enemy_centre_x >= enemy->charge_target_x) { 
            reached_target = 1; 
        } else if (enemy->charge_dx < 0 && enemy_centre_x <= enemy->charge_target_x) { 
            reached_target = 1; 
        }

        if (reached_target || Game3_ChargerEnemy_Is_Touching_Player(enemy, player)) { 
            enemy->state = GAME3_CHARGER_STATE_COOLDOWN; 
            enemy->state_end_time_ms = now + GAME3_CHARGER_COOLDOWN_MS; 
        }

        return; 
    }

    if (enemy->state == GAME3_CHARGER_STATE_COOLDOWN) { 
        if (now >= enemy->state_end_time_ms) { 
            enemy->state = GAME3_CHARGER_STATE_IDLE; 
            enemy->state_end_time_ms = now + GAME3_CHARGER_IDLE_MS; 
        }

        return; 
    }
}

uint8_t Game3_ChargerEnemy_Is_Touching_Player(const Game3_ChargerEnemy *enemy, const Game3_Player *player) { 
    if (!enemy->is_alive) { 
        return 0; 
    }

    if ((enemy->x + enemy->width) <= player->x) { 
        return 0; 
    }

    if (enemy->x >= (player->x + player->width)) { 
        return 0; 
    }

    if ((enemy->y + enemy->height) <= player->y) { 
        return 0; 
    }

    if (enemy->y >= (player->y + player->height)) { 
        return 0; 
    }
    
    return 1; 
}

uint8_t Game3_ChargerEnemy_Is_Touching_Player_Attack(const Game3_ChargerEnemy *enemy, const Game3_Player *player) { 
    if (!enemy->is_alive) {
        return 0; 
    }

    if (!Game3_Player_Is_Attacking(player)) { 
        return 0; 
    }

    int16_t attack_x; 

    if (player->facing_dx < 0) { 
        attack_x = player->x - GAME3_ATTACK_SIZE; 
    } else { 
        attack_x = player->x + player->width; 
    }

    int16_t attack_y = player->y; 

    if (attack_x < 0) { 
        attack_x = 0; 
    }

    if (attack_x > (GAME3_WORLD_WIDTH_PX - GAME3_ATTACK_SIZE)) { 
        attack_x = GAME3_WORLD_WIDTH_PX - GAME3_ATTACK_SIZE; 
    }

    if ((enemy->x + enemy->width) <= attack_x) { 
        return 0;
    }

    if (enemy->x >= (attack_x + GAME3_ATTACK_SIZE)) { 
        return 0; 
    }

    if ((enemy->y + enemy->height) <= attack_y) { 
        return 0; 
    }

    if (enemy->y >= (attack_y + GAME3_ATTACK_SIZE)) { 
        return 0;
    }

    return 1; 
}

uint8_t Game3_ChargerEnemy_Start_Attack_Hit(Game3_ChargerEnemy *enemy) { 
    uint32_t now = HAL_GetTick(); 

    if (!enemy->is_alive) { 
        return 0; 
    }

    if (enemy->last_attack_hit_time_ms != 0 && (now -  enemy->last_attack_hit_time_ms) < GAME3_CHARGER_ATTACK_COOLDOWN_MS) { 
        return 0; 
    }

    enemy->last_attack_hit_time_ms = now; 
    enemy->hit_flash_end_time_ms = now + GAME3_CHARGER_HIT_FLASH_MS; 

    return 1; 
}

void Game3_ChargerEnemy_Take_Damage(Game3_ChargerEnemy *enemy, uint8_t amount) { 
    if (!enemy->is_alive) { 
        return; 
    }

    if (amount >= enemy->health) { 
        enemy->health = 0;
        enemy->is_alive = 0;
        return;  
    }

    enemy->health -= amount; 
}

uint8_t Game3_ChargerEnemy_Is_Alive(const Game3_ChargerEnemy *enemy) { 
    return enemy->is_alive; 
}

uint8_t Game3_ChargerEnemy_Is_Hit_Flashing(const Game3_ChargerEnemy *enemy) { 
    return HAL_GetTick() < enemy->hit_flash_end_time_ms; 
}