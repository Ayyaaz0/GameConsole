#include "game3_render.h"

#include "LCD.h"
#include "game3_player.h"
#include "game3_world.h"

#define GAME3_SCREEN_WIDTH  240
#define GAME3_SCREEN_HEIGHT 240 

void Game3_Render_Draw_World(void) { 
    LCD_printString("GAME 3", 10, 10, 1, 2); 
    LCD_printString("Static Arena...", 10, 30, 1, 1); 

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
    LCD_Draw_Rect(player->x, player->y, player->width, player->height, 1, 1);
}

