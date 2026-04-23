#include "game3_input.h"

#include "Joystick.h"
#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_tim.h"
#include <stdint.h>

extern Joystick_cfg_t joystick_cfg;
extern Joystick_t joystick_data;

#define GAME3_DASH_TAP_WINDOW_MS 500

void Game3_Input_Read(Game3_Input *input) { 
    static uint8_t last_jump_held = 0; 

    static uint8_t last_right_held = 0;
    static uint8_t last_left_held = 0; 
    
    static uint32_t last_right_tap_time_ms = 0; 
    static uint32_t last_left_tap_time_ms = 0; 

    uint8_t jump_held = 0; 
    uint8_t right_held = 0;
    uint8_t left_held = 0; 

    uint8_t right_pressed_this_frame = 0; 
    uint8_t left_pressed_this_frame = 0;

    uint32_t now = HAL_GetTick();

    input->dx = 0; 
    input->jump_pressed = 0; 
    input->dash_pressed = 0; 
    input->dash_dx = 0; 

    Joystick_Read(&joystick_cfg, &joystick_data);

    switch (joystick_data.direction) { 
        case W: 
        case NW: 
        case SW: 
            input->dx = -1; 
            left_held = 1; 
            break; 

        case E: 
        case NE: 
        case SE: 
            input->dx = 1; 
            right_held = 1; 
            break; 

        default: 
            input->dx = 0;
            break; 
    }

    if (joystick_data.direction == N || joystick_data.direction == NE || joystick_data.direction == NW) { 
        jump_held = 1; 
    }

    input->jump_pressed = (jump_held && !last_jump_held) ? 1 : 0; 
    right_pressed_this_frame = (right_held && !last_right_held) ? 1 : 0; 
    left_pressed_this_frame = (left_held && !last_left_held) ? 1 : 0; 

    if (right_pressed_this_frame) { 
        if ((now - last_right_tap_time_ms) <= GAME3_DASH_TAP_WINDOW_MS) { 
            input->dash_pressed = 1; 
            input->dash_dx = 1; 
            last_right_tap_time_ms = 0; 
        } else { 
            last_right_tap_time_ms = now; 
        }
    }

    if (left_pressed_this_frame) { 
        if ((now - last_left_tap_time_ms) <= GAME3_DASH_TAP_WINDOW_MS) { 
            input->dash_pressed = 1; 
            input->dash_dx = -1; 
            last_left_tap_time_ms = 0; 
        } else { 
            last_left_tap_time_ms = now; 
        }
    }

    last_jump_held = jump_held; 
    last_right_held = right_held; 
    last_left_held = left_held; 
}
