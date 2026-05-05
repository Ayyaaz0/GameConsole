#include "game3_spawner.h"

#include <stdlib.h>

#include "game3_world.h"
#include "stm32l4xx_hal.h"

// TUNING

#define GAME3_SPAWNER_GRACE_PERIOD_MS           3000     // No spawns for the first few seconds
#define GAME3_SPAWNER_CONTINUOUS_INTERVAL_MS    2500     // Time between trickle spawns
#define GAME3_SPAWNER_MAX_TOTAL_ALIVE           8        // Hard cap on alive enemies

// Surge wave tuning
#define GAME3_SPAWNER_SURGE_INTERVAL_MIN_MS 30000
#define GAME3_SPAWNER_SURGE_INTERVAL_MAX_MS 45000
#define GAME3_SPAWNER_SURGE_BURST_COUNT 5
#define GAME3_SPAWNER_SURGE_SPACING_MS  350

// Points/Time for new enemy type to begin spawning
#define GAME3_SPAWNER_UNLOCK_CHARGER_MS 60000
#define GAME3_SPAWNER_UNLOCK_FLYING_MS  120000

// Ground spawn y for basic / charger enemies
#define GAME3_SPAWNER_GROUND_Y(height) \
    (((GAME3_ROOM_HEIGHT - 2) * GAME3_TILE_SIZE) - (height))

// Air spawn y for flying enemies.
#define GAME3_SPAWNER_FLYING_SPAWN_Y    (4 * GAME3_TILE_SIZE)

typedef enum {
    GAME3_SPAWNER_TYPE_BASIC = 0,
    GAME3_SPAWNER_TYPE_CHARGER,
    GAME3_SPAWNER_TYPE_FLYING
} Game3_Spawner_Type;

// HELPERS

// Random ms between MIN and MAX for the next surge
static uint32_t Game3_Spawner_Random_Surge_Interval(void) {
    uint32_t span = GAME3_SPAWNER_SURGE_INTERVAL_MAX_MS - GAME3_SPAWNER_SURGE_INTERVAL_MIN_MS;
    return GAME3_SPAWNER_SURGE_INTERVAL_MIN_MS + (uint32_t)(rand() % (int)(span + 1));
}

static uint8_t Game3_Spawner_Count_Alive(const Game3_Enemy *basics, const Game3_ChargerEnemy *chargers, const Game3_FlyingEnemy *flyings) {
    uint8_t alive = 0;

    for (uint8_t i = 0; i < GAME3_MAX_BASIC_ENEMIES; i++) {
        if (basics[i].is_alive) {
            alive++;
        }
    }

    for (uint8_t i = 0; i < GAME3_MAX_CHARGER_ENEMIES; i++) {
        if (chargers[i].is_alive) {
            alive++;
        }
    }

    for (uint8_t i = 0; i < GAME3_MAX_FLYING_ENEMIES; i++) {
        if (flyings[i].is_alive) {
            alive++;
        }
    }

    return alive;
}

// Returns the first free slot in each enemy pool, or -1 if all are alive
static int8_t Game3_Spawner_Find_Free_Basic(const Game3_Enemy *basics) {
    for (uint8_t i = 0; i < GAME3_MAX_BASIC_ENEMIES; i++) {
        if (!basics[i].is_alive) {
            return (int8_t)i;
        }
    }

    return -1;
}

static int8_t Game3_Spawner_Find_Free_Charger(const Game3_ChargerEnemy *chargers) {
    for (uint8_t i = 0; i < GAME3_MAX_CHARGER_ENEMIES; i++) {
        if (!chargers[i].is_alive) {
            return (int8_t)i;
        }
    }

    return -1;
}

static int8_t Game3_Spawner_Find_Free_Flying(const Game3_FlyingEnemy *flyings) {
    for (uint8_t i = 0; i < GAME3_MAX_FLYING_ENEMIES; i++) {
        if (!flyings[i].is_alive) {
            return (int8_t)i;
        }
    }

    return -1;
}

// Pick the world edge furthest from the player so enemies don't spawn on top of them
static int16_t Game3_Spawner_Pick_Edge_X(const Game3_Player *player, uint8_t enemy_width) {
    int16_t world_centre = GAME3_WORLD_WIDTH_PX / 2;
    int16_t player_centre = player->x + (player->width / 2);

    if (player_centre < world_centre) {
        return GAME3_WORLD_WIDTH_PX - enemy_width;
    }

    return 0;
}

// Difficulty ramp: chargers unlock after 60s, flyers after 120s
static uint8_t Game3_Spawner_Unlocked_Type_Count(const Game3_Spawner *spawner) {
    uint32_t elapsed = HAL_GetTick() - spawner->game_start_time_ms;

    if (elapsed >= GAME3_SPAWNER_UNLOCK_FLYING_MS) {
        return 3;
    }

    if (elapsed >= GAME3_SPAWNER_UNLOCK_CHARGER_MS) {
        return 2;
    }

    return 1;
}

static Game3_Spawner_Type Game3_Spawner_Pick_Type(const Game3_Spawner *spawner) {
    uint8_t pool = Game3_Spawner_Unlocked_Type_Count(spawner);

    switch (rand() % pool) {
        case 1:
            return GAME3_SPAWNER_TYPE_CHARGER;
        case 2:
            return GAME3_SPAWNER_TYPE_FLYING;
        case 0:
        default:
            return GAME3_SPAWNER_TYPE_BASIC;
    }
}

// Spawn one enemy of the specified type into a free slot
static uint8_t Game3_Spawner_Spawn_One(Game3_Spawner_Type type, Game3_Enemy *basics, Game3_ChargerEnemy *chargers, Game3_FlyingEnemy *flyings, const Game3_Player *player) {
    if (type == GAME3_SPAWNER_TYPE_FLYING) {
        int8_t slot = Game3_Spawner_Find_Free_Flying(flyings);
        if (slot >= 0) {
            int16_t spawn_x = Game3_Spawner_Pick_Edge_X(player, 10 /* GAME3_FLYING_WIDTH */);
            Game3_FlyingEnemy_Spawn(&flyings[slot], spawn_x, GAME3_SPAWNER_FLYING_SPAWN_Y);
            return 1;
        }
    }

    if (type == GAME3_SPAWNER_TYPE_CHARGER) {
        int8_t slot = Game3_Spawner_Find_Free_Charger(chargers);
        if (slot >= 0) {
            int16_t spawn_x = Game3_Spawner_Pick_Edge_X(player, 10);
            Game3_ChargerEnemy_Spawn(&chargers[slot], spawn_x, GAME3_SPAWNER_GROUND_Y(10));
            return 1;
        }
    }

    int8_t slot = Game3_Spawner_Find_Free_Basic(basics);
    if (slot >= 0) {
        int16_t spawn_x = Game3_Spawner_Pick_Edge_X(player, 8);
        Game3_Enemy_Spawn(&basics[slot], spawn_x, GAME3_SPAWNER_GROUND_Y(8));
        return 1;
    }

    return 0;
}

// LIFECYCLE

void Game3_Spawner_Init(Game3_Spawner *spawner) {
    uint32_t now = HAL_GetTick();
    spawner->game_start_time_ms = now;
    spawner->next_continuous_spawn_ms = now + GAME3_SPAWNER_GRACE_PERIOD_MS;
    spawner->next_surge_time_ms = now + GAME3_SPAWNER_GRACE_PERIOD_MS + Game3_Spawner_Random_Surge_Interval();
    spawner->surge_remaining = 0;
    spawner->next_surge_spawn_ms = 0;
}

// PER FRAME

void Game3_Spawner_Update(Game3_Spawner *spawner, Game3_Enemy *basics, Game3_ChargerEnemy *chargers, Game3_FlyingEnemy *flyings, const Game3_Player *player, const Game3_Camera *camera) {
    (void)camera;       // currently unused, kept for future camera-aware spawning

    uint32_t now = HAL_GetTick();

    // Grace period at start: no spawns at all
    if (now < (spawner->game_start_time_ms + GAME3_SPAWNER_GRACE_PERIOD_MS)) {
        return;
    }

    uint8_t alive = Game3_Spawner_Count_Alive(basics, chargers, flyings);

    // If too many hostiles, don't spawn anymore enemies
    if (alive >= GAME3_SPAWNER_MAX_TOTAL_ALIVE) {
        spawner->next_continuous_spawn_ms = now + GAME3_SPAWNER_CONTINUOUS_INTERVAL_MS;
        return;
    }

    // Surge has begun
    if (spawner->surge_remaining > 0) {
        if (now >= spawner->next_surge_spawn_ms) {
            Game3_Spawner_Type type = Game3_Spawner_Pick_Type(spawner);
            if (Game3_Spawner_Spawn_One(type, basics, chargers, flyings, player)) {
                spawner->surge_remaining--;
            }
            spawner->next_surge_spawn_ms = now + GAME3_SPAWNER_SURGE_SPACING_MS;
        }

        spawner->next_continuous_spawn_ms = now + GAME3_SPAWNER_CONTINUOUS_INTERVAL_MS;

        if (spawner->surge_remaining == 0) {
            spawner->next_surge_time_ms = now + Game3_Spawner_Random_Surge_Interval();
        }
        return;
    }

    // Checks for surge
    if (now >= spawner->next_surge_time_ms) {
        spawner->surge_remaining = GAME3_SPAWNER_SURGE_BURST_COUNT;
        spawner->next_surge_spawn_ms = now;
        return;
    }

    if (now < spawner->next_continuous_spawn_ms) {
        return;
    }

    Game3_Spawner_Type type = Game3_Spawner_Pick_Type(spawner);
    Game3_Spawner_Spawn_One(type, basics, chargers, flyings, player);
    spawner->next_continuous_spawn_ms = now + GAME3_SPAWNER_CONTINUOUS_INTERVAL_MS;
}
