// Simple unit tests for the pure helpers in game3_enemy.c.
// The whole file is wrapped in #ifdef GAME3_HOST_TESTS so the STM32 
// build compiles it to nothing, avoiding a duplicate main() found in other directories. 

#ifdef GAME3_HOST_TESTS

#include <stdint.h>
#include <stdio.h>

// PRODUCTION CONSTANTS 

#define GAME3_TILE_SIZE              8
#define GAME3_ROOM_WIDTH             30
#define GAME3_WORLD_WIDTH_PX         (GAME3_ROOM_WIDTH * GAME3_TILE_SIZE)
#define GAME3_KNOCKBACK_DX_MAGNITUDE 2

// HELPERS UNDER TEST (copies of game3_enemy.c)

static uint8_t Game3_AABB_Is_Touching(int16_t ax, int16_t ay, uint8_t aw, uint8_t ah, int16_t bx, int16_t by, uint8_t bw, uint8_t bh) {
    if ((ax + aw) <= bx) return 0;
    if (ax >= (bx + bw)) return 0;
    if ((ay + ah) <= by) return 0;
    if (ay >= (by + bh)) return 0;
    return 1;
}

static int16_t Game3_Box_Centre_X(int16_t x, uint8_t width) {
    return x + (width / 2);
}

static void Game3_Clamp_X_To_World(int16_t *x, uint8_t width) {
    if (*x < 0) *x = 0;
    if (*x > (GAME3_WORLD_WIDTH_PX - width)) *x = GAME3_WORLD_WIDTH_PX - width;
}

static uint8_t Game3_Hit_Cooldown_Active(uint32_t last_hit_ms, uint32_t now, uint32_t cooldown_ms) {
    if (last_hit_ms == 0) return 0;
    return (now - last_hit_ms) < cooldown_ms;
}

static int8_t Game3_Knockback_Dx_From_Facing(int16_t facing_dx) {
    if (facing_dx < 0) return -GAME3_KNOCKBACK_DX_MAGNITUDE;
    return GAME3_KNOCKBACK_DX_MAGNITUDE;
}

static void Game3_Apply_Knockback_Step(int16_t *x, int16_t dx, int16_t speed, uint8_t width) {
    *x += dx * speed;
    Game3_Clamp_X_To_World(x, width);
}

static void Game3_Approach_With_Deadzone(int16_t *value, int16_t target, int16_t speed, int16_t deadzone) {
    if (*value < target - deadzone) {
        *value += speed;
    } else if (*value > target + deadzone) {
        *value -= speed;
    }
}

// TESTS

int main(void) {
    int passed = 0;
    int failed = 0;

    printf("Game 3 helper tests\n");
    printf("-------------------\n");

    // Test 1: AABB - two boxes that overlap should be touching
    if (Game3_AABB_Is_Touching(10, 10, 8, 8, 12, 12, 8, 8) == 1) {
        printf("PASS  AABB: overlapping boxes are touching\n");
        passed++;
    } else {
        printf("FAIL  AABB: overlapping boxes should be touching\n");
        failed++;
    }

    // Test 2: AABB - two boxes far apart should not be touching
    if (Game3_AABB_Is_Touching(0, 0, 8, 8, 100, 100, 8, 8) == 0) {
        printf("PASS  AABB: far apart boxes are not touching\n");
        passed++;
    } else {
        printf("FAIL  AABB: far apart boxes should not be touching\n");
        failed++;
    }

    // Test 3: AABB - boxes that share an edge should NOT count as touching
    if (Game3_AABB_Is_Touching(0, 0, 8, 8, 8, 0, 8, 8) == 0) {
        printf("PASS  AABB: edge-touching boxes are not touching\n");
        passed++;
    } else {
        printf("FAIL  AABB: edge-touching boxes should not be touching\n");
        failed++;
    }

    // Test 4: centre x of a box at x=10, width 8, should be 14
    if (Game3_Box_Centre_X(10, 8) == 14) {
        printf("PASS  centre x of (x=10, w=8) is 14\n");
        passed++;
    } else {
        printf("FAIL  centre x of (x=10, w=8) was wrong\n");
        failed++;
    }

    // Test 5: clamp - a negative x should be clamped to 0
    int16_t x = -5;
    Game3_Clamp_X_To_World(&x, 8);
    if (x == 0) {
        printf("PASS  clamp: negative x clamps to 0\n");
        passed++;
    } else {
        printf("FAIL  clamp: negative x did not clamp to 0\n");
        failed++;
    }

    // Test 6: clamp - an x past the right wall should be clamped to WIDTH - w
    x = 1000;
    Game3_Clamp_X_To_World(&x, 8);
    if (x == GAME3_WORLD_WIDTH_PX - 8) {
        printf("PASS  clamp: huge x clamps to right edge\n");
        passed++;
    } else {
        printf("FAIL  clamp: huge x did not clamp to right edge\n");
        failed++;
    }

    // Test 7: cooldown - if the enemy has never been hit, no cooldown is active
    if (Game3_Hit_Cooldown_Active(0, 5000, 250) == 0) {
        printf("PASS  cooldown: never-hit enemy has no active cooldown\n");
        passed++;
    } else {
        printf("FAIL  cooldown: never-hit enemy should have no active cooldown\n");
        failed++;
    }

    // Test 8: cooldown - inside the cooldown window the gate is active
    if (Game3_Hit_Cooldown_Active(1000, 1100, 250) == 1) {
        printf("PASS  cooldown: 100ms after hit (250ms cooldown) is active\n");
        passed++;
    } else {
        printf("FAIL  cooldown: 100ms after hit should be active\n");
        failed++;
    }

    // Test 9: cooldown - past the cooldown window the gate has expired
    if (Game3_Hit_Cooldown_Active(1000, 1300, 250) == 0) {
        printf("PASS  cooldown: 300ms after hit (250ms cooldown) has expired\n");
        passed++;
    } else {
        printf("FAIL  cooldown: 300ms after hit should have expired\n");
        failed++;
    }

    // Test 10: knockback direction - facing left pushes the enemy left
    if (Game3_Knockback_Dx_From_Facing(-1) == -2) {
        printf("PASS  knockback: facing left gives dx = -2\n");
        passed++;
    } else {
        printf("FAIL  knockback: facing left should give dx = -2\n");
        failed++;
    }

    // Test 11: knockback direction - facing right pushes the enemy right
    if (Game3_Knockback_Dx_From_Facing(1) == 2) {
        printf("PASS  knockback: facing right gives dx = +2\n");
        passed++;
    } else {
        printf("FAIL  knockback: facing right should give dx = +2\n");
        failed++;
    }

    // Test 12: knockback step - a normal step moves x by dx * speed
    x = 50;
    Game3_Apply_Knockback_Step(&x, 2, 3, 8);
    if (x == 56) {
        printf("PASS  knockback step: x=50 + (2 * 3) = 56\n");
        passed++;
    } else {
        printf("FAIL  knockback step: expected 56, got %d\n", x);
        failed++;
    }

    // Test 13: knockback step - a step that would go past the wall is clamped
    x = 0;
    Game3_Apply_Knockback_Step(&x, -2, 3, 8);
    if (x == 0) {
        printf("PASS  knockback step: stepping left from 0 clamps to 0\n");
        passed++;
    } else {
        printf("FAIL  knockback step: stepping left from 0 should clamp to 0\n");
        failed++;
    }

    // Test 14: deadzone - if the value is at the target, it does not move
    int16_t v = 50;
    Game3_Approach_With_Deadzone(&v, 50, 3, 2);
    if (v == 50) {
        printf("PASS  deadzone: value at target does not move\n");
        passed++;
    } else {
        printf("FAIL  deadzone: value at target moved when it should not have\n");
        failed++;
    }

    // Test 15: deadzone - if the value is past the deadzone, it steps toward target
    v = 47;
    Game3_Approach_With_Deadzone(&v, 50, 3, 2);
    if (v == 50) {
        printf("PASS  deadzone: value past deadzone steps by speed toward target\n");
        passed++;
    } else {
        printf("FAIL  deadzone: value past deadzone did not step correctly\n");
        failed++;
    }

    printf("-------------------\n");
    printf("%d passed, %d failed\n", passed, failed);
    return failed == 0 ? 0 : 1;
}

#endif
