#ifndef GAME3_SPAWNER_H
#define GAME3_SPAWNER_H

#include <stdint.h>

#include "game3_camera.h"
#include "game3_enemy.h"
#include "game3_player.h"

typedef struct {
    uint32_t game_start_time_ms;
    uint32_t next_continuous_spawn_ms;

    uint32_t next_surge_time_ms;
    uint8_t surge_remaining;
    uint32_t next_surge_spawn_ms;
} Game3_Spawner;

void Game3_Spawner_Init(Game3_Spawner *spawner);

void Game3_Spawner_Update(Game3_Spawner *spawner, Game3_Enemy *basics, Game3_ChargerEnemy *chargers, Game3_FlyingEnemy *flyings, const Game3_Player *player, const Game3_Camera *camera);

#endif
