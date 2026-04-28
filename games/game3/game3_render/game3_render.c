#include "game3_render.h"

#include "LCD.h"
#include "game3_player.h"
#include "game3_world.h"
#include "game3_enemy.h"

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

void Game3_Render_Draw_World(void) { 
    for (uint16_t tile_y = 0; tile_y < GAME3_ROOM_HEIGHT; tile_y++) {
         for (uint16_t tile_x = 0; tile_x < GAME3_ROOM_WIDTH; tile_x++) { 
            uint8_t tile = Game3_World_Get_Tile(tile_x, tile_y); 

            if (tile == GAME3_TILE_EMPTY) { 
                continue; 
            }

            uint16_t screen_x = tile_x * GAME3_TILE_SIZE; 
            uint16_t screen_y = tile_y * GAME3_TILE_SIZE; 

            if (screen_x + GAME3_TILE_SIZE <= 0 || screen_x >= GAME3_SCREEN_WIDTH || screen_y + GAME3_TILE_SIZE <= 0 || 
                screen_y >= GAME3_SCREEN_HEIGHT) { 
                continue; 
            }

            LCD_Draw_Rect(screen_x, screen_y, GAME3_TILE_SIZE, GAME3_TILE_SIZE, 2, 1);
         }
    }
}

void Game3_Render_Draw_Player (const Game3_Player *player) { 
    uint8_t colour = Game3_Player_Is_Damage_Flashing(player) ? 2 : 1;
    LCD_Draw_Rect(player->x, player->y, player->width, player->height, colour, 1);
}

static void Game3_Render_Draw_Enemy_Health_Bar(const Game3_Enemy *enemy) { 
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

    LCD_Draw_Rect(bar_x, bar_y, GAME3_ENEMY_HEALTH_BAR_WIDTH, GAME3_ENEMY_HEALTH_BAR_HEIGHT, GAME3_ENEMY_HEALTH_BAR_BG_COLOUR, 1);

    if (filled_width > 0) { 
        LCD_Draw_Rect(bar_x, bar_y, filled_width, GAME3_ENEMY_HEALTH_BAR_HEIGHT, GAME3_ENEMY_HEALTH_BAR_FILL_COLOUR, 1);
    }
}

void Game3_Render_Draw_Enemy(const Game3_Enemy *enemy) { 
    if (!Game3_Enemy_Is_Alive(enemy)) { 
        return; 
    }

    uint8_t colour = Game3_Enemy_Is_Hit_Flashing(enemy) ? 2 : 5; 
    LCD_Draw_Rect(enemy->x, enemy->y, enemy->width, enemy->height, colour, 1);
    Game3_Render_Draw_Enemy_Health_Bar(enemy);
}

void Game3_Render_Draw_Player_Attack(const Game3_Player *player) { 
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

    if (attack_x > (GAME3_SCREEN_WIDTH - GAME3_ATTACK_SIZE)) { 
        attack_x = GAME3_SCREEN_WIDTH - GAME3_ATTACK_SIZE; 
    }

    LCD_Draw_Rect(attack_x, attack_y, GAME3_ATTACK_SIZE, GAME3_ATTACK_SIZE, GAME3_ATTACK_COLOUR, 1);
}

void Game3_Render_Draw_Projectile(const Game3_Projectile *projectile) { 
    if (!projectile->is_active) { 
        return; 
    }

    LCD_Draw_Rect(projectile->x, projectile->y, projectile->width, projectile->height, 14, 1);
}

void Game3_Render_Draw_Armour_Pack(int16_t x, int16_t y, uint8_t is_active) { 
    if (!is_active) { 
        return; 
    }

    LCD_Draw_Rect(x, y, GAME3_ARMOUR_PACK_SIZE, GAME3_ARMOUR_PACK_SIZE, GAME3_ARMOUR_PACK_SIZE, 1);
}

static void Game3_Render_Draw_ChargerEnemy_Health_Bar(const Game3_ChargerEnemy *enemy) { 
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

    LCD_Draw_Rect(bar_x, bar_y, GAME3_ENEMY_HEALTH_BAR_WIDTH, GAME3_ENEMY_HEALTH_BAR_HEIGHT, GAME3_ENEMY_HEALTH_BAR_BG_COLOUR, 1);

    if (filled_width > 0) { 
        LCD_Draw_Rect(bar_x, bar_y, filled_width, GAME3_ENEMY_HEALTH_BAR_HEIGHT, GAME3_ENEMY_HEALTH_BAR_FILL_COLOUR, 1);
    }
}

void Game3_Render_Draw_ChargerEnemy(const Game3_ChargerEnemy *enemy) { 
    if (!Game3_ChargerEnemy_Is_Alive(enemy)) { 
        return; 
    }

    uint8_t colour = Game3_ChargerEnemy_Is_Hit_Flashing(enemy) ? 2 : 8; 

    LCD_Draw_Rect(enemy->x, enemy->y, enemy->width, enemy->height, colour, 1);
    Game3_Render_Draw_ChargerEnemy_Health_Bar(enemy); 
}