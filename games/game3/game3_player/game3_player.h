#ifndef GAME3_PLAYER_H
#define GAME3_PLAYER_H


#include <stdint.h> 

typedef struct { 
    int16_t x; 
    int16_t y; 
    int16_t vx; 
    int16_t vy; 
    uint8_t width; 
    uint8_t height;

    uint8_t is_grounded; 
    uint32_t last_jump_time_ms; 

    uint8_t is_dashing; 
    uint32_t dash_end_time_ms; 
    uint32_t last_dash_time_ms;
    int16_t dash_dx;  
} Game3_Player; 

void Game3_Player_Init(Game3_Player *player);
void Game3_Player_Update(Game3_Player *player, int16_t dx, uint8_t jump_pressed, uint8_t dash_pressed, int16_t dash_dx);

#endif