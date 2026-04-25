#include "game3_enemy.h"
#include "game3_player.h"
#include "game3_world.h"
#include "stm32l4xx_hal.h"

#include <stdint.h> 

#define GAME3_ENEMY_KNOCKBACK_DURATION_MS 150

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
}

void Game3_Enemy_Update(Game3_Enemy *enemy, const Game3_Player *player) { 
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