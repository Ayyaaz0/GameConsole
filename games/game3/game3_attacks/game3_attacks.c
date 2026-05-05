#include "game3_attacks.h"
#include "game3_enemy.h"
#include "game3_world.h"

// TUNING

#define GAME3_PROJECTILE_SPEED  4
#define GAME3_PROJECTILE_SIZE   6

// LIFECYCLE

void Game3_Projectile_Init(Game3_Projectile *projectile) {
    projectile->x = 0; 
    projectile->y = 0; 
    projectile->vx = 0; 
    projectile->width = GAME3_PROJECTILE_SIZE; 
    projectile->height = GAME3_PROJECTILE_SIZE; 
    projectile->is_active = 0; 
}

// Spawns the projectile at the player's mid-height, going in the facing direction
void Game3_Projectile_Fire(Game3_Projectile *projectile, const Game3_Player *player) {
    projectile->is_active = 1;
    projectile->width = GAME3_PROJECTILE_SIZE;
    projectile->height = GAME3_PROJECTILE_SIZE;

    projectile->y = player->y + (player->height / 2) - (projectile->height / 2);

    if (player->facing_dx < 0) {
        projectile->x = player->x - projectile->width;
        projectile->vx = -GAME3_PROJECTILE_SPEED;
    } else {
        projectile->x = player->x + player->width;
        projectile->vx = GAME3_PROJECTILE_SPEED;
    }
}

// PER FRAME

void Game3_Projectile_Update(Game3_Projectile *projectile) {
    if (!projectile->is_active) { 
        return; 
    }

    projectile->x += projectile->vx; 

    if (projectile->x < 0 || projectile->x > GAME3_WORLD_WIDTH_PX) { 
        projectile->is_active = 0; 
    }
}

// COLLISION

// AABB overlap between projectile and enemy hitbox
uint8_t Game3_Projectile_Is_Touching_Enemy(const Game3_Projectile *projectile, const Game3_Enemy *enemy) {
    if (!projectile->is_active || !Game3_Enemy_Is_Alive(enemy)) { 
        return 0; 
    }

    if ((projectile->x + projectile->width) <= enemy->x) return 0; 
    if (projectile->x >= (enemy->x + enemy->width)) return 0; 
    if ((projectile->y + projectile->height) <= enemy->y) return 0; 
    if (projectile->y >= (enemy->y + enemy->height)) return 0; 

    return 1; 
}