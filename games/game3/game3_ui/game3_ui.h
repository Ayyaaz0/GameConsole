#ifndef GAME3_UI_H
#define GAME3_UI_H

#include <stdint.h> 

typedef struct { 
    uint8_t health; 
    uint8_t max_health; 

    uint8_t armour; 
    uint8_t max_armour; 

    uint32_t start_time_ms; 
    uint8_t is_game_over;
    uint32_t final_score; 
} Game3_Hud; 

void Game3_UI_Draw(const Game3_Hud *hud); 
void Game3_UI_Draw_Game_Over(const Game3_Hud *hud);

#endif 