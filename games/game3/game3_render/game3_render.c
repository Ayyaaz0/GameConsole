#include "game3_render.h"

#include "LCD.h"
#include "game3_player.h"
#include "game3_world.h"
#include "game3_enemy.h"

#define GAME3_SCREEN_WIDTH  240
#define GAME3_SCREEN_HEIGHT 240 

#define GAME3_ATTACK_SIZE   8
#define GAME3_ATTACK_COLOUR 6

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

void Game3_Render_Draw_Enemy(const Game3_Enemy *enemy) { 
    uint8_t colour = Game3_Enemy_Is_Hit_Flashing(enemy) ? 2 : 5; 
    LCD_Draw_Rect(enemy->x, enemy->y, enemy->width, enemy->height, colour, 1);
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