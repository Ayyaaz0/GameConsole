#include "game3_render.h"

#include "LCD.h"
#include "game3_player.h"
#include "game3_world.h"
#include "game3_enemy.h"
#include "game3_camera.h"
#include <stdint.h>

#define GAME3_SCREEN_WIDTH  240
#define GAME3_SCREEN_HEIGHT 240 

#define GAME3_ATTACK_SIZE   8
#define GAME3_ATTACK_COLOUR 6

#define GAME3_ENEMY_HEALTH_BAR_WIDTH    12
#define GAME3_ENEMY_HEALTH_BAR_HEIGHT   3
#define GAME3_ENEMY_HEALTH_BAR_GAP  3

#define GAME3_ENEMY_HEALTH_BAR_BG_COLOUR    13
#define GAME3_ENEMY_HEALTH_BAR_FILL_COLOUR  2

#define GAME3_ARMOUR_PACK_SIZE  6
#define GAME3_ARMOUR_PACK_COLOUR  4

static void Game3_Render_Draw_Rect_Camera(int16_t world_x, int16_t world_y, uint8_t width, uint8_t height, uint8_t colour, uint8_t fill, const Game3_Camera *camera) { 
    int16_t screen_x = world_x - camera->x; 
    int16_t screen_y = world_y - camera->y; 

    if (screen_x >= GAME3_SCREEN_WIDTH || screen_y >= GAME3_SCREEN_HEIGHT) { 
        return; 
    }

    if ((screen_x + width) <= 0 || (screen_y + height) <= 0) { 
        return; 
    } 

    if (screen_x < 0) { 
        width += screen_x; 
        screen_x = 0; 
    }

    if (screen_y < 0) { 
        height += screen_y; 
        screen_y = 0; 
    }

    if ((screen_x + width) > GAME3_SCREEN_WIDTH) { 
        width = GAME3_SCREEN_WIDTH - screen_x; 
    }

    if ((screen_y + height) > GAME3_SCREEN_HEIGHT) { 
        height = GAME3_SCREEN_HEIGHT - screen_y;
    }

    if (width == 0 || height == 0) { 
        return; 
    }

    LCD_Draw_Rect((uint16_t)screen_x, (uint16_t)screen_y, width, height, colour, fill);
}

void Game3_Render_Draw_World(const Game3_Camera *camera) { 
    for (uint16_t tile_y = 0; tile_y < GAME3_ROOM_HEIGHT; tile_y++) {
         for (uint16_t tile_x = 0; tile_x < GAME3_ROOM_WIDTH; tile_x++) { 
            uint8_t tile = Game3_World_Get_Tile(tile_x, tile_y); 

            if (tile == GAME3_TILE_EMPTY) { 
                continue; 
            }

            uint16_t world_x = tile_x * GAME3_TILE_SIZE; 
            uint16_t world_y = tile_y * GAME3_TILE_SIZE; 

            Game3_Render_Draw_Rect_Camera(world_x, world_y, GAME3_TILE_SIZE, GAME3_TILE_SIZE, 2, 1, camera);  
         }
    }
}

void Game3_Render_Draw_Player (const Game3_Player *player, const Game3_Camera *camera) { 
    uint8_t colour = Game3_Player_Is_Damage_Flashing(player) ? 2 : 1;
    Game3_Render_Draw_Rect_Camera(player->x, player->y, player->width, player->height, colour, 1, camera);  
}

static void Game3_Render_Draw_Enemy_Health_Bar(const Game3_Enemy *enemy, const Game3_Camera *camera) { 
    if (!Game3_Enemy_Is_Alive(enemy)) {
        return; 
    }

    if (enemy->max_health == 0) { 
        return; 
    }

    int16_t bar_x = enemy->x + (enemy->width/2) - (GAME3_ENEMY_HEALTH_BAR_WIDTH / 2);
    int16_t bar_y = enemy->y - GAME3_ENEMY_HEALTH_BAR_GAP - GAME3_ENEMY_HEALTH_BAR_HEIGHT; 

    if (bar_x < 0) { 
        bar_x = 0; 
    }

    if (bar_x > (GAME3_SCREEN_WIDTH - GAME3_ENEMY_HEALTH_BAR_WIDTH)) { 
        bar_x = GAME3_SCREEN_WIDTH - GAME3_ENEMY_HEALTH_BAR_WIDTH; 
    }

    if (bar_y < 0) { 
        bar_y = 0; 
    }

    uint8_t filled_width = (enemy->health * GAME3_ENEMY_HEALTH_BAR_WIDTH) / enemy->max_health; 

    Game3_Render_Draw_Rect_Camera(bar_x, bar_y, GAME3_ENEMY_HEALTH_BAR_WIDTH, GAME3_ENEMY_HEALTH_BAR_HEIGHT, GAME3_ENEMY_HEALTH_BAR_BG_COLOUR, 1, camera);  

    if (filled_width > 0) { 
        Game3_Render_Draw_Rect_Camera(bar_x, bar_y, filled_width, GAME3_ENEMY_HEALTH_BAR_HEIGHT, GAME3_ENEMY_HEALTH_BAR_FILL_COLOUR, 1, camera);  
    }
}

void Game3_Render_Draw_Enemy(const Game3_Enemy *enemy, const Game3_Camera *camera) { 
    if (!Game3_Enemy_Is_Alive(enemy)) { 
        return; 
    }

    uint8_t colour = Game3_Enemy_Is_Hit_Flashing(enemy) ? 2 : 5; 
    Game3_Render_Draw_Rect_Camera(enemy->x, enemy->y, enemy->width, enemy->height, colour, 1, camera);  
    Game3_Render_Draw_Enemy_Health_Bar(enemy, camera);
}

void Game3_Render_Draw_Player_Attack(const Game3_Player *player, const Game3_Camera *camera) { 
    if (!Game3_Player_Is_Attacking(player)) { 
        return; 
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

    Game3_Render_Draw_Rect_Camera(attack_x, attack_y, GAME3_ATTACK_SIZE, GAME3_ATTACK_SIZE, GAME3_ATTACK_COLOUR, 1, camera);  
}

void Game3_Render_Draw_Projectile(const Game3_Projectile *projectile, const Game3_Camera *camera) { 
    if (!projectile->is_active) { 
        return; 
    }

    Game3_Render_Draw_Rect_Camera(projectile->x, projectile->y, projectile->width, projectile->height, 14, 1, camera);  
}

void Game3_Render_Draw_Armour_Pack(int16_t x, int16_t y, uint8_t is_active, const Game3_Camera *camera) { 
    if (!is_active) { 
        return; 
    }

    Game3_Render_Draw_Rect_Camera(x, y, GAME3_ARMOUR_PACK_SIZE, GAME3_ARMOUR_PACK_SIZE, GAME3_ARMOUR_PACK_COLOUR, 1, camera);  
}

static void Game3_Render_Draw_ChargerEnemy_Health_Bar(const Game3_ChargerEnemy *enemy, const Game3_Camera *camera) { 
    if (!Game3_ChargerEnemy_Is_Alive(enemy)) { 
        return; 
    }

    if (enemy->max_health == 0) { 
        return; 
    }

    int16_t bar_x = enemy->x + (enemy->width/2) - (GAME3_ENEMY_HEALTH_BAR_WIDTH / 2);
    int16_t bar_y = enemy->y - GAME3_ENEMY_HEALTH_BAR_GAP - GAME3_ENEMY_HEALTH_BAR_HEIGHT; 

    if (bar_x < 0) { 
        bar_x = 0; 
    }

    if (bar_x > (GAME3_SCREEN_WIDTH - GAME3_ENEMY_HEALTH_BAR_WIDTH)) { 
        bar_x = GAME3_SCREEN_WIDTH - GAME3_ENEMY_HEALTH_BAR_WIDTH; 
    }

    if (bar_y < 0) { 
        bar_y = 0; 
    }

    uint8_t filled_width = (enemy->health * GAME3_ENEMY_HEALTH_BAR_WIDTH) / enemy->max_health; 

    Game3_Render_Draw_Rect_Camera(bar_x, bar_y, GAME3_ENEMY_HEALTH_BAR_WIDTH, GAME3_ENEMY_HEALTH_BAR_HEIGHT, GAME3_ENEMY_HEALTH_BAR_BG_COLOUR, 1, camera);  

    if (filled_width > 0) { 
        Game3_Render_Draw_Rect_Camera(bar_x, bar_y, filled_width, GAME3_ENEMY_HEALTH_BAR_HEIGHT, GAME3_ENEMY_HEALTH_BAR_FILL_COLOUR, 1, camera);  
    }
}

void Game3_Render_Draw_ChargerEnemy(const Game3_ChargerEnemy *enemy, const Game3_Camera *camera) { 
    if (!Game3_ChargerEnemy_Is_Alive(enemy)) { 
        return; 
    }

    uint8_t colour = Game3_ChargerEnemy_Is_Hit_Flashing(enemy) ? 2 : 8; 

    Game3_Render_Draw_Rect_Camera(enemy->x, enemy->y, enemy->width, enemy->height, colour, 1, camera);  
    Game3_Render_Draw_ChargerEnemy_Health_Bar(enemy, camera); 
}