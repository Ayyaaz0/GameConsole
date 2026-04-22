#include "game3_player.h"
#include "game3_world.h"
#include "stm32l4xx_hal.h"
#include <stdint.h>

#define GAME3_PLAYER_MOVE_SPEED 2
#define GAME3_PLAYER_GRAVITY    1
#define GAME3_PLAYER_MAX_FALL_SPEED 6
#define GAME3_PLAYER_JUMP_VELOCITY  -8
#define GAME3_PLAYER_JUMP_COOLDOWN_MS   200

#define GAME3_PLAYER_DASH_SPEED 6
#define GAME3_PLAYER_DASH_DURATION_MS   120
#define GAME3_PLAYER_DASH_COOLDOWN_MS   400

static uint8_t Game3_Player_Is_Solid_At_Pixel(int16_t px, int16_t py) { 
    if (px < 0 || py < 0) { 
        return 1; 
    }   

    uint16_t tile_x = (uint16_t)px / GAME3_TILE_SIZE; 
    uint16_t tile_y = (uint16_t)py/ GAME3_TILE_SIZE; 

    return Game3_World_Is_Solid(tile_x, tile_y); 
}

static uint8_t Game3_Player_Is_On_Ground(const Game3_Player *player) { 
    int16_t foot_y = player->y + player->height; 
    int16_t left_x = player->x; 
    int16_t right_x = player->x + player->width - 1; 

    return Game3_Player_Is_Solid_At_Pixel(left_x, foot_y) || Game3_Player_Is_Solid_At_Pixel(right_x, foot_y);
}

void Game3_Player_Init(Game3_Player *player) { 
    player->width = 8; 
    player->height = 8; 

    player->vx = 0; 
    player->vy = 0; 

    player-> x = 4 * GAME3_TILE_SIZE;
    player->y = ((GAME3_ROOM_HEIGHT - 2) * GAME3_TILE_SIZE) - player->height; 

    player->is_grounded = 1; 
    player->last_jump_time_ms = 0; 

    player->is_dashing = 0; 
    player->dash_end_time_ms = 0; 
    player->last_dash_time_ms = 0; 
}

void Game3_Player_Update(Game3_Player *player, int16_t dx, uint8_t jump_pressed, uint8_t dash_pressed) { 
    uint32_t now = HAL_GetTick(); 

    player->is_grounded = Game3_Player_Is_On_Ground(player);

    if (dash_pressed && !player->is_dashing && (now - player->last_dash_time_ms >= GAME3_PLAYER_DASH_COOLDOWN_MS)) { 
        player->is_dashing = 1; 
        player->dash_end_time_ms = now + GAME3_PLAYER_DASH_DURATION_MS; 
        player->last_dash_time_ms = now; 
    }

    if (player->is_dashing) { 
        if (now >= player->dash_end_time_ms) { 
            player->is_dashing = 0;
        } 
    }

    if (player->is_dashing) { 
        player->vx = GAME3_PLAYER_DASH_SPEED;
    } else { 
        player->vx = dx * GAME3_PLAYER_MOVE_SPEED;
    }

    player->x += player->vx; 

    if (player->x < 0) { 
        player->x = 0; 
    }

    if (player->x > (GAME3_WORLD_HEIGHT_PX - player->width)) { 
        player->x = GAME3_WORLD_WIDTH_PX - player->width; 
    }

    if (jump_pressed && player->is_grounded && (now - player->last_jump_time_ms >= GAME3_PLAYER_JUMP_COOLDOWN_MS)) { 
        player->vy = GAME3_PLAYER_JUMP_VELOCITY; 
        player->is_grounded = 0; 
        player->last_jump_time_ms = now; 
    }

    if (!player->is_grounded) { 
        player->vy += GAME3_PLAYER_GRAVITY; 

        if (player->vy > GAME3_PLAYER_MAX_FALL_SPEED) { 
            player->vy = GAME3_PLAYER_MAX_FALL_SPEED;
        }
    }

    player->y += player->vy;

    if (player->vy >= 0) { 
        int16_t foot_y = player->y + player->height; 
        int16_t left_x = player->x; 
        int16_t right_x = player->x + player->width - 1; 

        if (Game3_Player_Is_Solid_At_Pixel(left_x, foot_y) || Game3_Player_Is_Solid_At_Pixel(right_x, foot_y)) { 
            uint16_t landing_tile_y = (uint16_t)foot_y / GAME3_TILE_SIZE; 
            player->y = ((int16_t)landing_tile_y * GAME3_TILE_SIZE) - player->height; 
            player->vy = 0; 
            player->is_grounded = 1; 
        } else { 
            player->is_grounded = 0; 
        }
    } else { 
        int16_t head_y = player->y; 
        int16_t left_x = player->x; 
        int16_t right_x = player->x + player->width - 1; 

        if (Game3_Player_Is_Solid_At_Pixel(left_x, head_y) || Game3_Player_Is_Solid_At_Pixel(right_x, head_y)) { 
            uint16_t ceiling_tile_y = (uint16_t)head_y / GAME3_TILE_SIZE; 
            player->y = ((int16_t)(ceiling_tile_y + 1) * GAME3_TILE_SIZE);
            player->vy = 0; 
        }

        player->is_grounded = 0; 
    }

    if (player->y > (GAME3_WORLD_HEIGHT_PX - player->height)) { 
        player->y = GAME3_WORLD_HEIGHT_PX - player->height; 
        player->vy = 0; 
        player->is_grounded = 1; 
    }
}
