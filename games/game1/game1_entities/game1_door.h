#ifndef GAME1_DOOR_H
#define GAME1_DOOR_H

#include "game1_camera.h"
#include "game1_player.h"
#include "room0_entities.h"

#include <stdint.h>

#define GAME1_MAX_DOORS 4

void Game1_Door_Reset(void);
void Game1_Door_Load(const Game1_Entity *entity);
void Game1_Door_UpdateAll(Game1_Player *player, uint8_t interact_pressed);
void Game1_Door_RenderAll(const Game1_Camera *camera);

#endif