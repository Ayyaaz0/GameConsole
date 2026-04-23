#include "game3_enemy.h"
#include "game3_player.h"
#include "game3_world.h"

#include <stdint.h> 

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

void Game3_Enemy_Init(Game3_Enemy *enemy) { 
    enemy->width = 8; 
    enemy->height = 8; 
    enemy->move_speed = 1; 

    enemy->x = 20 * GAME3_TILE_SIZE; 
    enemy->y = ((GAME3_ROOM_HEIGHT - 2) * GAME3_TILE_SIZE) - enemy->height; 
}

void Game3_Enemy_Update(Game3_Enemy *enemy, const Game3_Player *player) { 
    int16_t enemy_centre_x = Game3_Enemy_Get_Enemy_Centre_X(enemy); 
    int16_t player_centre_x = Game3_Enemy_Get_Player_Centre_X(player);

    if (enemy_centre_x < player_centre_x) { 
        enemy->x += enemy->move_speed; 
    } else if (enemy_centre_x > player_centre_x) { 
        enemy->x -= enemy->move_speed; 
    }

    if (enemy->x < 0) { 
        enemy->x = 0; 
    }

    if (enemy->x > (GAME3_WORLD_WIDTH_PX - enemy->width)) { 
        enemy->x = GAME3_WORLD_WIDTH_PX - enemy->width; 
    }
}