#include "game3_enemy.h"
#include "game3_player.h"
#include "game3_world.h"
#include "stm32l4xx_hal.h"

#include <stdint.h>

// REGULAR ENEMY DEFINE

#define GAME3_ENEMY_KNOCKBACK_DURATION_MS 150
#define GAME3_ENEMY_HIT_FLASH_MS    200
#define GAME3_ENEMY_ATTACK_HIT_COOLDOWN_MS 250
#define GAME3_ATTACK_SIZE   8
#define GAME3_ENEMY_START_HEALTH    3

// CHARGER ENEMY DEFINE

#define GAME3_CHARGER_START_HEALTH  2
#define GAME3_CHARGER_IDLE_MS   900
#define GAME3_CHARGER_COOLDOWN_MS   700

#define GAME3_CHARGER_SPEED 7

#define GAME3_CHARGER_HIT_FLASH_MS  200
#define GAME3_CHARGER_ATTACK_COOLDOWN_MS    250

#define GAME3_CHARGER_KNOCKBACK_DURATION_MS 150
#define GAME3_CHARGER_KNOCKBACK_SPEED   3

// FLYING ENEMY DEFINE

#define GAME3_FLYING_START_HEALTH   2
#define GAME3_FLYING_WIDTH  10
#define GAME3_FLYING_HEIGHT 8

#define GAME3_FLYING_SPEED  1
#define GAME3_FLYING_SHOOT_INTERVAL_MS  1300

#define GAME3_FLYING_PROJECTILE_SIZE    4
#define GAME3_FLYING_PROJECTILE_SPEED_X   2
#define GAME3_FLYING_PROJECTILE_SPEED_Y   2

#define GAME3_FLYING_HIT_FLASH_MS   200
#define GAME3_FLYING_ATTACK_COOLDOWN    250

#define GAME3_FLYING_HOVER_ABOVE_PLAYER_PX  22
#define GAME3_FLYING_FOLLOW_DEADZONE_PX 2

#define GAME3_KNOCKBACK_DX_MAGNITUDE    2

// SHARED HELPERS

// Axis-Aligned Bounding Box overlap check (each box treated as a rectangle, returns 1 if they overlap)
static uint8_t Game3_AABB_Is_Touching(int16_t ax, int16_t ay, uint8_t aw, uint8_t ah, int16_t bx, int16_t by, uint8_t bw, uint8_t bh) {
    if ((ax + aw) <= bx) {
        return 0;
    }

    if (ax >= (bx + bw)) {
        return 0;
    }

    if ((ay + ah) <= by) {
        return 0;
    }

    if (ay >= (by + bh)) {
        return 0;
    }

    return 1;
}

static int16_t Game3_Player_Centre_X(const Game3_Player *player) {
    return player->x + (player->width / 2);
}

static int16_t Game3_Box_Centre_X(int16_t x, uint8_t width) {
    return x + (width / 2);
}

// Keeps an enemy's x within the world bounds
static void Game3_Clamp_X_To_World(int16_t *x, uint8_t width) {
    if (*x < 0) {
        *x = 0;
    }

    if (*x > (GAME3_WORLD_WIDTH_PX - width)) {
        *x = GAME3_WORLD_WIDTH_PX - width;
    }
}

// True if a hit cooldown is active. last_hit_ms == 0 means never been hit.
static uint8_t Game3_Hit_Cooldown_Active(uint32_t last_hit_ms, uint32_t now, uint32_t cooldown_ms) {
    if (last_hit_ms == 0) {
        return 0;
    }
    return (now - last_hit_ms) < cooldown_ms;
}

// Knockback pointing away from the direction of player face
static int8_t Game3_Knockback_Dx_From_Facing(int16_t facing_dx) {
    if (facing_dx < 0) {
        return -GAME3_KNOCKBACK_DX_MAGNITUDE;
    }
    return GAME3_KNOCKBACK_DX_MAGNITUDE;
}

// Knockback pushing self away from other (compares centre points)
static int8_t Game3_Knockback_Dx_From_Centres(int16_t self_centre_x, int16_t other_centre_x) {
    if (self_centre_x < other_centre_x) {
        return -GAME3_KNOCKBACK_DX_MAGNITUDE;
    }
    return GAME3_KNOCKBACK_DX_MAGNITUDE;
}

// Step x by one knockback frame and clamp back into the world
static void Game3_Apply_Knockback_Step(int16_t *x, int16_t dx, int16_t speed, uint8_t width) {
    *x += dx * speed;
    Game3_Clamp_X_To_World(x, width);
}

// REGULAR ENEMY

uint8_t Game3_Enemy_Is_Touching_Player(const Game3_Enemy *enemy, const Game3_Player *player) {
    if (!enemy->is_alive) {
        return 0;
    }

    return Game3_AABB_Is_Touching(enemy->x, enemy->y, enemy->width, enemy->height, player->x, player->y, player->width, player->height);
}

void Game3_Enemy_Spawn(Game3_Enemy *enemy, int16_t x, int16_t y) {
    enemy->width = 8;
    enemy->height = 8;
    enemy->move_speed = 1;
    enemy->knockback_speed = 3;
    enemy->knockback_dx = 0;

    enemy->x = x;
    enemy->y = y;

    enemy->is_in_knockback = 0;
    enemy->knockback_end_time_ms = 0;

    enemy->hit_flash_end_time_ms = 0;
    enemy->last_attack_hit_time_ms = 0;

    enemy->max_health = GAME3_ENEMY_START_HEALTH;
    enemy->health = enemy->max_health;
    enemy->is_alive = 1;
}

void Game3_Enemy_Init(Game3_Enemy *enemy) {
    int16_t default_x = 20 * GAME3_TILE_SIZE;
    int16_t default_y = ((GAME3_ROOM_HEIGHT - 2) * GAME3_TILE_SIZE) - 8;
    Game3_Enemy_Spawn(enemy, default_x, default_y);
}

void Game3_Enemy_Update(Game3_Enemy *enemy, const Game3_Player *player) {

    if (!enemy->is_alive) {
        return;
    }

    uint32_t now = HAL_GetTick();

    if (enemy->is_in_knockback) {
        if (now >= enemy->knockback_end_time_ms) {
            enemy->is_in_knockback = 0;
        } else {
            Game3_Apply_Knockback_Step(&enemy->x, enemy->knockback_dx, enemy->knockback_speed, enemy->width);
            return;
        }
    }

    if (Game3_Enemy_Is_Touching_Player(enemy, player)) {
        enemy->is_in_knockback = 1;
        enemy->knockback_end_time_ms = now + GAME3_ENEMY_KNOCKBACK_DURATION_MS;

        enemy->knockback_dx = Game3_Knockback_Dx_From_Centres(Game3_Box_Centre_X(enemy->x, enemy->width), Game3_Player_Centre_X(player));

        Game3_Apply_Knockback_Step(&enemy->x, enemy->knockback_dx, enemy->knockback_speed, enemy->width);
        return;
    }

    int16_t enemy_centre_x = Game3_Box_Centre_X(enemy->x, enemy->width);
    int16_t player_centre_x = Game3_Player_Centre_X(player);

    if (enemy_centre_x < player_centre_x) {
        enemy->x += enemy->move_speed;
    } else if (enemy_centre_x > player_centre_x) {
        enemy->x -= enemy->move_speed;
    }

    Game3_Clamp_X_To_World(&enemy->x, enemy->width);
}

uint8_t Game3_Enemy_Is_Touching_Player_Attack(const Game3_Enemy *enemy, const Game3_Player *player) {

    if (!enemy->is_alive) {
        return 0;
    }

    if (!Game3_Player_Is_Attacking(player)) {
        return 0;
    }

    int16_t attack_x;
    int16_t attack_y;
    Game3_Player_Get_Attack_Hitbox(player, &attack_x, &attack_y);

    return Game3_AABB_Is_Touching(enemy->x, enemy->y, enemy->width, enemy->height, attack_x, attack_y, GAME3_ATTACK_SIZE, GAME3_ATTACK_SIZE);
}

uint8_t Game3_Enemy_Start_Attack_Knockback(Game3_Enemy *enemy, const Game3_Player *player) {
    uint32_t now = HAL_GetTick();

    if (!enemy->is_alive) {
        return 0;
    }

    if (Game3_Hit_Cooldown_Active(enemy->last_attack_hit_time_ms, now, GAME3_ENEMY_ATTACK_HIT_COOLDOWN_MS)) {
        return 0;
    }

    enemy->last_attack_hit_time_ms = now;

    Game3_Enemy_Take_Damage(enemy, 1);

    enemy->hit_flash_end_time_ms = now + GAME3_ENEMY_HIT_FLASH_MS;

    if (!enemy->is_alive) {
        return 1;
    }

    enemy->is_in_knockback = 1;
    enemy->knockback_end_time_ms = now + GAME3_ENEMY_KNOCKBACK_DURATION_MS;

    enemy->knockback_dx = Game3_Knockback_Dx_From_Facing(player->facing_dx);

    Game3_Apply_Knockback_Step(&enemy->x, enemy->knockback_dx, enemy->knockback_speed, enemy->width);

    return 1;
}

void Game3_Enemy_Take_Damage(Game3_Enemy *enemy, uint8_t amount) {
    if (!enemy->is_alive) {
        return;
    }

    if (amount >= enemy->health) {
        enemy->health = 0;
        enemy->is_alive = 0;

        enemy->is_in_knockback = 0;
        enemy->knockback_dx = 0;
        enemy->hit_flash_end_time_ms = 0;
        return;
    }

    enemy->health -= amount;
}

uint8_t Game3_Enemy_Is_Alive(const Game3_Enemy *enemy) {
    return enemy->is_alive;
}

uint8_t Game3_Enemy_Is_Hit_Flashing(const Game3_Enemy *enemy) {
    return HAL_GetTick() < enemy->hit_flash_end_time_ms;
}

// CHARGER ENEMY

void Game3_ChargerEnemy_Spawn(Game3_ChargerEnemy *enemy, int16_t x, int16_t y) {
    enemy->width = 10;
    enemy->height = 10;

    enemy->x = x;
    enemy->y = y;

    enemy->charge_dx = 1;
    enemy->charge_target_x = enemy->x;

    enemy->knockback_dx = 0;
    enemy->knockback_speed = GAME3_CHARGER_KNOCKBACK_SPEED;
    enemy->is_in_knockback = 0;
    enemy->knockback_end_time_ms = 0;

    enemy->max_health = GAME3_CHARGER_START_HEALTH;
    enemy->health = enemy->max_health;
    enemy->is_alive = 1;

    enemy->state = GAME3_CHARGER_STATE_IDLE;
    enemy->state_end_time_ms = HAL_GetTick() + GAME3_CHARGER_IDLE_MS;

    enemy->hit_flash_end_time_ms = 0;
    enemy->last_attack_hit_time_ms = 0;
}

void Game3_ChargerEnemy_Init(Game3_ChargerEnemy *enemy) {
    int16_t default_x = 10 * GAME3_TILE_SIZE;
    int16_t default_y = ((GAME3_ROOM_HEIGHT - 2) * GAME3_TILE_SIZE) - 10;
    Game3_ChargerEnemy_Spawn(enemy, default_x, default_y);
}

void Game3_ChargerEnemy_Update(Game3_ChargerEnemy *enemy, const Game3_Player *player) {
    if (!enemy->is_alive) {
        return;
    }

    uint32_t now = HAL_GetTick();

    if (enemy->is_in_knockback) {
        if (now >= enemy->knockback_end_time_ms) {
            enemy->is_in_knockback = 0;
            enemy->knockback_dx = 0;
        } else {
            Game3_Apply_Knockback_Step(&enemy->x, enemy->knockback_dx, enemy->knockback_speed, enemy->width);
            return;
        }
    }

    if (enemy->state == GAME3_CHARGER_STATE_IDLE) {
        if (now >= enemy->state_end_time_ms) {
            int16_t enemy_centre_x = Game3_Box_Centre_X(enemy->x, enemy->width);
            int16_t player_centre_x = Game3_Player_Centre_X(player);

            enemy->charge_target_x = player_centre_x;

            if (enemy_centre_x < player_centre_x) {
                enemy->charge_dx = 1;
            } else {
                enemy->charge_dx = -1;
            }

            enemy->state = GAME3_CHARGER_STATE_CHARGING;
        }

        return;
    }

    if (enemy->state == GAME3_CHARGER_STATE_CHARGING) {
        enemy->x += enemy->charge_dx * GAME3_CHARGER_SPEED;
        Game3_Clamp_X_To_World(&enemy->x, enemy->width);

        int16_t enemy_centre_x = Game3_Box_Centre_X(enemy->x, enemy->width);

        uint8_t reached_target = 0;

        if (enemy->charge_dx > 0 && enemy_centre_x >= enemy->charge_target_x) {
            reached_target = 1;
        } else if (enemy->charge_dx < 0 && enemy_centre_x <= enemy->charge_target_x) {
            reached_target = 1;
        }

        if (reached_target || Game3_ChargerEnemy_Is_Touching_Player(enemy, player)) {
            enemy->state = GAME3_CHARGER_STATE_COOLDOWN;
            enemy->state_end_time_ms = now + GAME3_CHARGER_COOLDOWN_MS;
        }

        return;
    }

    if (enemy->state == GAME3_CHARGER_STATE_COOLDOWN) {
        if (now >= enemy->state_end_time_ms) {
            enemy->state = GAME3_CHARGER_STATE_IDLE;
            enemy->state_end_time_ms = now + GAME3_CHARGER_IDLE_MS;
        }

        return;
    }
}

uint8_t Game3_ChargerEnemy_Is_Touching_Player(const Game3_ChargerEnemy *enemy, const Game3_Player *player) {
    if (!enemy->is_alive) {
        return 0;
    }

    return Game3_AABB_Is_Touching(enemy->x, enemy->y, enemy->width, enemy->height, player->x, player->y, player->width, player->height);
}

uint8_t Game3_ChargerEnemy_Is_Touching_Player_Attack(const Game3_ChargerEnemy *enemy, const Game3_Player *player) {
    if (!enemy->is_alive) {
        return 0;
    }

    if (!Game3_Player_Is_Attacking(player)) {
        return 0;
    }

    int16_t attack_x;
    int16_t attack_y;
    Game3_Player_Get_Attack_Hitbox(player, &attack_x, &attack_y);

    return Game3_AABB_Is_Touching(enemy->x, enemy->y, enemy->width, enemy->height, attack_x, attack_y, GAME3_ATTACK_SIZE, GAME3_ATTACK_SIZE);
}

uint8_t Game3_ChargerEnemy_Start_Attack_Hit(Game3_ChargerEnemy *enemy, const Game3_Player *player) {
    uint32_t now = HAL_GetTick();

    if (!enemy->is_alive) {
        return 0;
    }

    if (Game3_Hit_Cooldown_Active(enemy->last_attack_hit_time_ms, now, GAME3_CHARGER_ATTACK_COOLDOWN_MS)) {
        return 0;
    }

    enemy->last_attack_hit_time_ms = now;

    enemy->is_in_knockback = 1;
    enemy->knockback_end_time_ms = now + GAME3_CHARGER_KNOCKBACK_DURATION_MS;

    enemy->knockback_dx = Game3_Knockback_Dx_From_Centres(Game3_Box_Centre_X(enemy->x, enemy->width), Game3_Player_Centre_X(player));

    enemy->state = GAME3_CHARGER_STATE_COOLDOWN;
    enemy->state_end_time_ms = now + GAME3_CHARGER_COOLDOWN_MS;

    Game3_Apply_Knockback_Step(&enemy->x, enemy->knockback_dx, enemy->knockback_speed, enemy->width);

    return 1;
}

uint8_t Game3_ChargerEnemy_Start_Player_Attack_Knockback(Game3_ChargerEnemy *enemy, const Game3_Player *player) {
    uint32_t now = HAL_GetTick();

    if (!enemy->is_alive) {
        return 0;
    }

    if (Game3_Hit_Cooldown_Active(enemy->last_attack_hit_time_ms, now, GAME3_CHARGER_ATTACK_COOLDOWN_MS)) {
        return 0;
    }

    enemy->last_attack_hit_time_ms = now;

    Game3_ChargerEnemy_Take_Damage(enemy, 1);

    if (!enemy->is_alive) {
        return 1;
    }

    enemy->hit_flash_end_time_ms = now + GAME3_CHARGER_HIT_FLASH_MS;

    enemy->is_in_knockback = 1;
    enemy->knockback_end_time_ms = now + GAME3_CHARGER_KNOCKBACK_DURATION_MS;

    enemy->knockback_dx = Game3_Knockback_Dx_From_Facing(player->facing_dx);

    enemy->state = GAME3_CHARGER_STATE_COOLDOWN;
    enemy->state_end_time_ms = now + GAME3_CHARGER_COOLDOWN_MS;

    Game3_Apply_Knockback_Step(&enemy->x, enemy->knockback_dx, enemy->knockback_speed, enemy->width);

    return 1;
}


void Game3_ChargerEnemy_Take_Damage(Game3_ChargerEnemy *enemy, uint8_t amount) {
    if (!enemy->is_alive) {
        return;
    }

    if (amount >= enemy->health) {
        enemy->health = 0;
        enemy->is_alive = 0;
        return;
    }

    enemy->health -= amount;
}

uint8_t Game3_ChargerEnemy_Is_Alive(const Game3_ChargerEnemy *enemy) {
    return enemy->is_alive;
}

uint8_t Game3_ChargerEnemy_Is_Hit_Flashing(const Game3_ChargerEnemy *enemy) {
    return HAL_GetTick() < enemy->hit_flash_end_time_ms;
}

// FLYING ENEMY

void Game3_FlyingEnemy_Clear_Projectiles(Game3_FlyingEnemy *enemy) {
    for (uint8_t i = 0; i < 2; i++) {
        enemy->projectiles[i].is_active = 0;
        enemy->projectiles[i].x = 0;
        enemy->projectiles[i].y = 0;
        enemy->projectiles[i].vx = 0;
        enemy->projectiles[i].vy = 0;
        enemy->projectiles[i].width = GAME3_FLYING_PROJECTILE_SIZE;
        enemy->projectiles[i].height = GAME3_FLYING_PROJECTILE_SIZE;
    }
}

void Game3_FlyingEnemy_Spawn(Game3_FlyingEnemy *enemy, int16_t x, int16_t y) {
    enemy->width = GAME3_FLYING_WIDTH;
    enemy->height = GAME3_FLYING_HEIGHT;

    enemy->x = x;
    enemy->y = y;

    enemy->move_dx = 1;
    enemy->move_speed = GAME3_FLYING_SPEED;

    enemy->max_health = GAME3_FLYING_START_HEALTH;
    enemy->health = enemy->max_health;
    enemy->is_alive = 1;

    enemy->next_shot_time_ms = HAL_GetTick() + GAME3_FLYING_SHOOT_INTERVAL_MS;
    enemy->hit_flash_end_time_ms = 0;
    enemy->last_attack_hit_time_ms = 0;

    Game3_FlyingEnemy_Clear_Projectiles(enemy);
}

void Game3_FlyingEnemy_Init(Game3_FlyingEnemy *enemy) {
    Game3_FlyingEnemy_Spawn(enemy, 15 * GAME3_TILE_SIZE, 6 * GAME3_TILE_SIZE);
}

static void Game3_FlyingEnemy_Fire_Projectiles(Game3_FlyingEnemy *enemy) {
    int16_t start_x = enemy->x + (enemy->width / 2) - (GAME3_FLYING_PROJECTILE_SIZE / 2);
    int16_t start_y = enemy->y + enemy->height;

    enemy->projectiles[0].x = start_x;
    enemy->projectiles[0].y = start_y;
    enemy->projectiles[0].vx = -GAME3_FLYING_PROJECTILE_SPEED_X;
    enemy->projectiles[0].vy = GAME3_FLYING_PROJECTILE_SPEED_Y;
    enemy->projectiles[0].width = GAME3_FLYING_PROJECTILE_SIZE;
    enemy->projectiles[0].height = GAME3_FLYING_PROJECTILE_SIZE;
    enemy->projectiles[0].is_active = 1;

    enemy->projectiles[1].x = start_x;
    enemy->projectiles[1].y = start_y;
    enemy->projectiles[1].vx = GAME3_FLYING_PROJECTILE_SPEED_X;
    enemy->projectiles[1].vy = GAME3_FLYING_PROJECTILE_SPEED_Y;
    enemy->projectiles[1].width = GAME3_FLYING_PROJECTILE_SIZE;
    enemy->projectiles[1].height = GAME3_FLYING_PROJECTILE_SIZE;
    enemy->projectiles[1].is_active = 1;
}

static void Game3_FlyingEnemy_Update_Projectile(Game3_FlyingEnemy *enemy) {
    for (uint8_t i = 0; i < 2; i++) {
        Game3_FlyingProjectile *projectile = &enemy->projectiles[i];

        if (!projectile->is_active) {
            continue;
        }

        projectile->x += projectile->vx;
        projectile->y += projectile->vy;

        if (projectile->x < 0 || projectile->x > GAME3_WORLD_WIDTH_PX || projectile->y < 0 || projectile->y > GAME3_WORLD_HEIGHT_PX) {
            projectile->is_active = 0;
        }
    }
}

void Game3_FlyingEnemy_Update(Game3_FlyingEnemy *enemy, const Game3_Player *player) {
    if (!enemy->is_alive) {
        return;
    }

    uint32_t now = HAL_GetTick();

    int16_t player_centre_x = Game3_Player_Centre_X(player);

    int16_t target_x = player_centre_x - (enemy->width / 2);
    int16_t target_y = player->y - GAME3_FLYING_HOVER_ABOVE_PLAYER_PX - enemy->height;

    if (target_y < GAME3_TILE_SIZE) {
        target_y = GAME3_TILE_SIZE;
    }

    if (target_x < 0) {
        target_x = 0;
    }

    if (target_x > (GAME3_WORLD_WIDTH_PX - enemy->width)) {
        target_x = GAME3_WORLD_WIDTH_PX - enemy->width;
    }

    if (enemy->x < target_x - GAME3_FLYING_FOLLOW_DEADZONE_PX) {
        enemy->x += enemy->move_speed;
    } else if (enemy->x > target_x + GAME3_FLYING_FOLLOW_DEADZONE_PX) {
        enemy->x -= enemy->move_speed;
    }

    if (enemy->y < target_y - GAME3_FLYING_FOLLOW_DEADZONE_PX) {
        enemy->y += enemy->move_speed;
    } else if (enemy->y > target_y + GAME3_FLYING_FOLLOW_DEADZONE_PX) {
        enemy->y -= enemy->move_speed;
    }

    if (now >= enemy->next_shot_time_ms) {
         Game3_FlyingEnemy_Fire_Projectiles(enemy);
         enemy->next_shot_time_ms = now + GAME3_FLYING_SHOOT_INTERVAL_MS;
    }

    Game3_FlyingEnemy_Update_Projectile(enemy);
}

uint8_t Game3_FlyingEnemy_Is_Touching_Player_Attack(const Game3_FlyingEnemy *enemy, const Game3_Player *player) {
    if (!enemy->is_alive) {
        return 0;
    }

    if (!Game3_Player_Is_Attacking(player)) {
        return 0;
    }

    int16_t attack_x;
    int16_t attack_y;
    Game3_Player_Get_Attack_Hitbox(player, &attack_x, &attack_y);

    return Game3_AABB_Is_Touching(enemy->x, enemy->y, enemy->width, enemy->height, attack_x, attack_y, GAME3_ATTACK_SIZE, GAME3_ATTACK_SIZE);
}

uint8_t Game3_FlyingEnemy_Start_Player_Attack(Game3_FlyingEnemy *enemy, const Game3_Player *player) {
    (void)player;

    uint32_t now = HAL_GetTick();

    if (!enemy->is_alive) {
        return 0;
    }

    if (Game3_Hit_Cooldown_Active(enemy->last_attack_hit_time_ms, now, GAME3_FLYING_ATTACK_COOLDOWN)) {
        return 0;
    }

    enemy->last_attack_hit_time_ms = now;
    enemy->hit_flash_end_time_ms = now + GAME3_FLYING_HIT_FLASH_MS;

    Game3_FlyingEnemy_Take_Damage(enemy, 1);

    return 1;
}

void Game3_FlyingEnemy_Take_Damage(Game3_FlyingEnemy *enemy, uint8_t amount) {
    if (!enemy->is_alive) {
        return;
    }

    if (amount >= enemy->health) {
        enemy->health = 0;
        enemy->is_alive = 0;
        enemy->hit_flash_end_time_ms = 0;
        Game3_FlyingEnemy_Clear_Projectiles(enemy);
        return;
    }

    enemy->health -= amount;
}

uint8_t Game3_FlyingEnemy_Is_Alive(const Game3_FlyingEnemy *enemy) {
    return enemy->is_alive;
}

uint8_t Game3_FlyingEnemy_Is_Hit_Flashing(const Game3_FlyingEnemy *enemy) {
    return HAL_GetTick() < enemy->hit_flash_end_time_ms;
}

uint8_t Game3_FlyingEnemy_Projectile_Is_Touching_Player(const Game3_FlyingEnemy *enemy, const Game3_Player *player) {
    for (uint8_t i = 0; i < 2; i++) {
        const Game3_FlyingProjectile *projectile = &enemy->projectiles[i];

        if (!projectile->is_active) {
            continue;
        }

        if (Game3_AABB_Is_Touching(projectile->x, projectile->y, projectile->width, projectile->height, player->x, player->y, player->width, player->height)) {
            return 1;
        }
    }

    return 0;
}
