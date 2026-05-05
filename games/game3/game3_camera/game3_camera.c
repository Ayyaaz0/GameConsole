#include "game3_camera.h"

// TUNING

#define GAME3_CAMERA_LOOKAHEAD_X    6
#define GAME3_CAMERA_LOOKAHEAD_Y    4
#define GAME3_CAMERA_SMOOTHING  4

// SMOOTHING

// Eases the camera one step toward the desired offset. Small gaps move 1 px,
// big gaps divide by SMOOTHING so the camera never snaps.
static int16_t Game3_Camera_Smooth_Value(int16_t current, int16_t desired) {
    int16_t difference = desired - current;

    if (difference == 0) {
        return current;
    }

    if (difference > 0 && difference < GAME3_CAMERA_SMOOTHING) {
        return current + 1;
    }

    if (difference < 0 && difference > -GAME3_CAMERA_SMOOTHING) {
        return current - 1;
    }

    return current + (difference / GAME3_CAMERA_SMOOTHING);
}

// CAMERA API

void Game3_Camera_Init(Game3_Camera *camera) { 
    camera->x = 0; 
    camera->y = 0; 
}

void Game3_Camera_Update(Game3_Camera *camera, const Game3_Player *player) { 
    int16_t desired_x = 0; 
    int16_t desired_y = 0; 

    if (player->vx > 0) { 
        desired_x = GAME3_CAMERA_LOOKAHEAD_X;
    } else if (player->vx < 0) { 
        desired_x = -GAME3_CAMERA_LOOKAHEAD_X;
    }

    if (player->vy > 0) {
        desired_y = GAME3_CAMERA_LOOKAHEAD_Y; 
    } else if (player->vy < 0) { 
        desired_y = -GAME3_CAMERA_LOOKAHEAD_Y;
    }

    camera->x = Game3_Camera_Smooth_Value(camera->x, desired_x);
    camera->y = Game3_Camera_Smooth_Value(camera->y, desired_y);
}