#ifndef GAME3_INPUT_H 
#define GAME3_INPUT_H

#include <stdint.h> 

typedef struct { 
    int16_t dx; 
    uint8_t jump_pressed; 
    uint8_t dash_pressed; 
    int16_t dash_dx; 
    uint8_t attack_pressed; 
    uint8_t ability_pressed; 
} Game3_Input; 

void Game3_Input_Read(Game3_Input *input);

#endif