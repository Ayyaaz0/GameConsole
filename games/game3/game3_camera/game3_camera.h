#ifndef GAME3_CAMERA_H
#define GAME3_CAMERA_H

#include <stdint.h> 
#include "game3_player.h"

typedef struct { 
    int16_t x; 
    int16_t y; 
} Game3_Camera; 

void Game3_Camera_Init(Game3_Camera *camera);
void Game3_Camera_Update(Game3_Camera *camera, const Game3_Player *player);

#endif 