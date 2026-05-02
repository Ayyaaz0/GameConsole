#ifndef GAME1_COIN_H
#define GAME1_COIN_H

#include "game1_camera.h"
#include "game1_player.h"
#include "room0_entities.h"

#include <stdint.h>

#define GAME1_MAX_COINS 32

void Game1_Coin_Reset(void);
void Game1_Coin_Load(const Game1_Entity *entity);
void Game1_Coin_UpdateAll(Game1_Player *player);
void Game1_Coin_RenderAll(const Game1_Camera *camera);
void Game1_Coin_ResetScore(void);
uint16_t Game1_Coin_GetScore(void);

#endif