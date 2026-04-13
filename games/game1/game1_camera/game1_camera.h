#ifndef GAME1_CAMERA_H
#define GAME1_CAMERA_H

#include <stdint.h>

typedef struct {
    int16_t x;
    int16_t y;
    uint16_t width;
    uint16_t height;
} Game1_Camera;

void Game1_Camera_Init(Game1_Camera *camera, uint16_t width, uint16_t height);
void Game1_Camera_Update(Game1_Camera *camera,
                         int16_t target_x,
                         int16_t target_y,
                         uint16_t world_width_px,
                         uint16_t world_height_px);

#endif