#include "Game_3.h"

#include "InputHandler.h"
#include "LCD.h"
#include "stm32l4xx_hal.h"

#include "game3_attacks/game3_attacks.h"
#include "game3_camera/game3_camera.h"
#include "game3_enemy/game3_enemy.h"
#include "game3_input/game3_input.h"
#include "game3_player/game3_player.h"
#include "game3_render/game3_render.h"
#include "game3_spawner/game3_spawner.h"
#include "game3_ui/game3_ui.h"
#include "game3_world/game3_world.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GAME3_ABILITY_GAIN_ON_HIT 1
#define GAME3_MAX_ABILITY 4
#define GAME3_SCORE_PER_SECOND  10
#define GAME3_ABILITY_COST  GAME3_MAX_ABILITY
#define GAME3_ARMOUR_DROP_CHANCE_PERCENT  30
#define GAME3_ARMOUR_PACK_SIZE  6
#define GAME3_CHARGER_ARMOUR_DROP_CHANCE_PERCENT  40
#define GAME3_PROJECTILE_DAMAGE 3
#define GAME3_CONTACT_DAMAGE  1

extern ST7789V2_cfg_t cfg0;

static bool                  game3_shutdown_requested = false;
static Game3_Player          player;
static Game3_Enemy           enemies[GAME3_MAX_BASIC_ENEMIES];
static Game3_ChargerEnemy    charger_enemies[GAME3_MAX_CHARGER_ENEMIES];
static Game3_FlyingEnemy     flying_enemies[GAME3_MAX_FLYING_ENEMIES];
static Game3_Projectile      projectile;
static Game3_Hud             hud;
static Game3_Camera          camera;
static Game3_Spawner         spawner;

static uint8_t armour_pack_active = 0;
static int16_t armour_pack_x = 0;
static int16_t armour_pack_y = 0;

// SMALL HELPERS 

static uint32_t Game3_Get_Current_Score(void) {
  uint32_t elapsed_seconds = (HAL_GetTick() - hud.start_time_ms) / 1000;
  return elapsed_seconds * GAME3_SCORE_PER_SECOND;
}

static void Game3_Hud_Gain_Ability(uint8_t amount) {
  if (hud.ability >= hud.max_ability) {
    return;
  }
  hud.ability += amount;
  if (hud.ability > hud.max_ability) {
    hud.ability = hud.max_ability;
  }
}

// Generic armour-pack drop 
static void Game3_Try_Drop_Armour_Pack_At(int16_t entity_x, int16_t entity_y, uint16_t entity_w, uint16_t entity_h, uint8_t drop_chance_percent) {
  if (armour_pack_active) {
    return;
  }
  if ((rand() % 100) >= drop_chance_percent) {
    return;
  }

  armour_pack_x = entity_x + (entity_w / 2) - (GAME3_ARMOUR_PACK_SIZE / 2);
  armour_pack_y = entity_y + (entity_h / 2) - GAME3_ARMOUR_PACK_SIZE;
  armour_pack_active = 1;
}

static uint8_t Game3_Player_Is_Touching_Armour_Pack(const Game3_Player *p) {
  if (!armour_pack_active) {
    return 0;
  }
  if ((p->x + p->width) <= armour_pack_x) return 0;
  if ( p->x >= (armour_pack_x + GAME3_ARMOUR_PACK_SIZE)) return 0;
  if ((p->y + p->height) <= armour_pack_y) return 0;
  if ( p->y >= (armour_pack_y + GAME3_ARMOUR_PACK_SIZE)) return 0;
  return 1;
}

// MOVING PLATFORM HANDLING

static void Game3_Apply_Moving_Platform_Drag(Game3_Player *p) {
  const Game3_MovingPlatform *plat = Game3_World_Get_Moving_Platform();
  p->x += plat->dx;

  if (p->x < 0) {
    p->x = 0;
  }
  if (p->x > (GAME3_WORLD_WIDTH_PX - p->width)) {
    p->x = GAME3_WORLD_WIDTH_PX - p->width;
  }
}

// PLAYER ATTACK PASSES

static void Game3_Process_Player_Attack_Basic(void) {
  for (uint8_t i = 0; i < GAME3_MAX_BASIC_ENEMIES; i++) {
    Game3_Enemy *e = &enemies[i];

    if (!Game3_Enemy_Is_Touching_Player_Attack(e, &player)) {
      continue;
    }

    uint8_t was_alive = Game3_Enemy_Is_Alive(e);

    if (Game3_Enemy_Start_Attack_Knockback(e, &player)) {
      Game3_Hud_Gain_Ability(GAME3_ABILITY_GAIN_ON_HIT);

      if (was_alive && !Game3_Enemy_Is_Alive(e)) {
        Game3_Try_Drop_Armour_Pack_At(e->x, e->y, e->width, e->height, GAME3_ARMOUR_DROP_CHANCE_PERCENT);
      }
    }
  }
}

static void Game3_Process_Player_Attack_Chargers(void) {
  for (uint8_t i = 0; i < GAME3_MAX_CHARGER_ENEMIES; i++) {
    Game3_ChargerEnemy *c = &charger_enemies[i];

    if (!Game3_ChargerEnemy_Is_Touching_Player_Attack(c, &player)) {
      continue;
    }

    uint8_t was_alive = Game3_ChargerEnemy_Is_Alive(c);

    if (Game3_ChargerEnemy_Start_Player_Attack_Knockback(c, &player)) {
      Game3_Hud_Gain_Ability(GAME3_ABILITY_GAIN_ON_HIT);

      if (was_alive && !Game3_ChargerEnemy_Is_Alive(c)) {
        Game3_Try_Drop_Armour_Pack_At(c->x, c->y, c->width, c->height, GAME3_CHARGER_ARMOUR_DROP_CHANCE_PERCENT);
      }
    }
  }
}

static void Game3_Process_Player_Attack_Flyers(void) {
  for (uint8_t i = 0; i < GAME3_MAX_FLYING_ENEMIES; i++) {
    Game3_FlyingEnemy *f = &flying_enemies[i];

    if (!Game3_FlyingEnemy_Is_Touching_Player_Attack(f, &player)) {
      continue;
    }

    if (Game3_FlyingEnemy_Start_Player_Attack(f, &player)) {
      Game3_Hud_Gain_Ability(GAME3_ABILITY_GAIN_ON_HIT);
    }
  }
}

// ENEMY -> PLAYER DAMAGE

static void Game3_Process_Basic_Enemy_Contact(void) {
  for (uint8_t i = 0; i < GAME3_MAX_BASIC_ENEMIES; i++) {
    if (Game3_Enemy_Is_Touching_Player(&enemies[i], &player)) {
      Game3_Player_Take_Damage(&player, GAME3_CONTACT_DAMAGE);
    }
  }
}

static void Game3_Process_Charger_Contact(void) {
  for (uint8_t i = 0; i < GAME3_MAX_CHARGER_ENEMIES; i++) {
    Game3_ChargerEnemy *c = &charger_enemies[i];
    if (Game3_ChargerEnemy_Is_Touching_Player(c, &player)) {
      if (Game3_ChargerEnemy_Start_Attack_Hit(c, &player)) {
        Game3_Player_Take_Damage(&player, GAME3_CONTACT_DAMAGE);
      }
    }
  }
}

static void Game3_Process_Flyer_Projectile_Hits(void) {
  for (uint8_t i = 0; i < GAME3_MAX_FLYING_ENEMIES; i++) {
    Game3_FlyingEnemy *f = &flying_enemies[i];
    if (Game3_FlyingEnemy_Projectile_Is_Touching_Player(f, &player)) {
      Game3_Player_Take_Damage(&player, GAME3_CONTACT_DAMAGE);
      Game3_FlyingEnemy_Clear_Projectiles(f);
    }
  }
}


static void Game3_Update_All_Enemies(void) {
  for (uint8_t i = 0; i < GAME3_MAX_BASIC_ENEMIES; i++) {
    Game3_Enemy_Update(&enemies[i], &player);
  }
  for (uint8_t i = 0; i < GAME3_MAX_CHARGER_ENEMIES; i++) {
    Game3_ChargerEnemy_Update(&charger_enemies[i], &player);
  }
  for (uint8_t i = 0; i < GAME3_MAX_FLYING_ENEMIES; i++) {
    Game3_FlyingEnemy_Update(&flying_enemies[i], &player);
  }
}

// PLAYER PROJECILE HITS ENEMY

static void Game3_Process_Projectile_Hits(void) {
  if (!projectile.is_active) {
    return;
  }

  for (uint8_t i = 0; i < GAME3_MAX_BASIC_ENEMIES; i++) {
    Game3_Enemy *e = &enemies[i];

    if (!Game3_Projectile_Is_Touching_Enemy(&projectile, e)) {
      continue;
    }

    uint8_t was_alive = Game3_Enemy_Is_Alive(e);

    Game3_Enemy_Take_Damage(e, GAME3_PROJECTILE_DAMAGE);
    projectile.is_active = 0;

    if (was_alive && !Game3_Enemy_Is_Alive(e)) {
      Game3_Try_Drop_Armour_Pack_At(e->x, e->y, e->width, e->height, GAME3_ARMOUR_DROP_CHANCE_PERCENT);
    }
    break;
  }
}

// HUD INIT

static void Game3_Sync_Hud_From_Player(void) {
  hud.health = player.health;
  hud.max_health = player.max_health;
  hud.armour = player.armour;
  hud.max_armour = player.max_armour;

  if (player.health == 0) {
    hud.is_game_over = 1;
    hud.final_score = Game3_Get_Current_Score();
  }
}

static void game3_init(void) {
  game3_shutdown_requested = false;

  Game3_World_Init();
  Game3_Player_Init(&player);
  Game3_Camera_Init(&camera);
  Game3_Projectile_Init(&projectile);

  Game3_Spawner_Init(&spawner);

  armour_pack_active = 0;
  armour_pack_x = 0;
  armour_pack_y = 0;

  hud.max_health = 3;
  hud.health = 3;
  hud.max_armour = 3;
  hud.armour = 3;
  hud.start_time_ms = HAL_GetTick();
  hud.is_game_over = 0;
  hud.final_score = 0;
  hud.max_ability = GAME3_MAX_ABILITY;
  hud.ability = 0;

  LCD_Fill_Buffer(0);
  LCD_Refresh(&cfg0);
}

static void game3_update(void) {
  if (hud.is_game_over) {
    return;
  }

  /* --- Input + ability --- */
  Game3_Input input = {0};
  Game3_Input_Read(&input);

  if (input.ability_pressed && hud.ability >= GAME3_ABILITY_COST) {
    hud.ability = 0;
    Game3_Projectile_Fire(&projectile, &player);
  }

   // Capture whether the player is riding the moving platform BEFORE the
   // world renders, so we can lock player onto. 
  uint8_t was_riding = Game3_World_Box_Is_On_Moving_Platform(player.x, player.y, player.width, player.height);

  Game3_World_Update();

  if (was_riding) {
    Game3_Apply_Moving_Platform_Drag(&player);
  }

  Game3_Player_Update(&player, input.dx, input.jump_pressed, input.dash_pressed, input.dash_dx, input.attack_pressed, input.up_held);
  Game3_Camera_Update(&camera, &player);
  Game3_Spawner_Update(&spawner, enemies, charger_enemies, flying_enemies, &player, &camera);

  Game3_Process_Player_Attack_Basic();
  Game3_Process_Player_Attack_Chargers();
  Game3_Process_Player_Attack_Flyers();

  Game3_Process_Basic_Enemy_Contact();
  Game3_Process_Charger_Contact();
  Game3_Process_Flyer_Projectile_Hits();

  Game3_Update_All_Enemies();

  Game3_Process_Basic_Enemy_Contact();

  // ARMOUR PACK PICKUP
  if (Game3_Player_Is_Touching_Armour_Pack(&player)) {
    Game3_Player_Gain_Armour(&player, 1);
    armour_pack_active = 0;
  }

  Game3_Projectile_Update(&projectile);
  Game3_Process_Projectile_Hits();

  Game3_Sync_Hud_From_Player();
}

static void game3_render(void) {
  LCD_Fill_Buffer(0);

  if (hud.is_game_over) {
    Game3_UI_Draw_Game_Over(&hud);
    LCD_Refresh(&cfg0);
    return;
  }

  Game3_Render_Draw_World(&camera);
  Game3_Render_Draw_Armour_Pack(armour_pack_x, armour_pack_y, armour_pack_active, &camera);
  Game3_Render_Draw_Player(&player, &camera);
  Game3_Render_Draw_Player_Attack(&player, &camera);

  for (uint8_t i = 0; i < GAME3_MAX_BASIC_ENEMIES; i++) {
    Game3_Render_Draw_Enemy(&enemies[i], &camera);
  }
  for (uint8_t i = 0; i < GAME3_MAX_CHARGER_ENEMIES; i++) {
    Game3_Render_Draw_ChargerEnemy(&charger_enemies[i], &camera);
  }
  for (uint8_t i = 0; i < GAME3_MAX_FLYING_ENEMIES; i++) {
    Game3_Render_Draw_FlyingEnemy(&flying_enemies[i], &camera);
  }

  Game3_Render_Draw_Projectile(&projectile, &camera);
  Game3_UI_Draw(&hud);

  LCD_Refresh(&cfg0);
}

static void game3_shutdown(void) {
  LCD_Fill_Buffer(0);
  LCD_Refresh(&cfg0);
}

bool Game3_ShouldExit(void) { return game3_shutdown_requested; }

const GameApi game3_api = {
    .name        = "Game 3",
    .init        = game3_init,
    .update      = game3_update,
    .render      = game3_render,
    .shutdown    = game3_shutdown,
    .should_exit = Game3_ShouldExit,
};
