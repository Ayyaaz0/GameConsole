#include "Game_3.h"

#include "InputHandler.h"
#include "LCD.h"
#include "game3_enemy/game3_enemy.h"
#include "game3_input.h"
#include "game3_enemy.h"
#include "game3_player.h"
#include "game3_render.h"
#include "game3_ui/game3_ui.h"
#include "game3_world.h"
#include "game3_ui.h"
#include "stm32l4xx_hal.h"

#include <stdbool.h>
#include <stdio.h>

extern ST7789V2_cfg_t cfg0;

static bool game3_shutdown_requested = false;
static Game3_Player player; 
static Game3_Enemy enemy; 
static Game3_Hud hud; 

#define GAME3_ABILITY_GAIN_ON_HIT 1
#define GAME3_MAX_ABILITY 4

static uint32_t Game3_Get_Current_Score(void) { 
  uint32_t elapsed_seconds = (HAL_GetTick() - hud.start_time_ms) / 1000; 
  return elapsed_seconds * 10; 
}

static void game3_init(void) {
  game3_shutdown_requested = false;
  
  Game3_World_Init();
  Game3_Player_Init(&player);
  Game3_Enemy_Init(&enemy);

  hud.max_health = 3; 
  hud.health = 3; 
  hud.max_armour = 3; 
  hud.armour = 3; 
  hud.start_time_ms = HAL_GetTick(); 
  hud.is_game_over = 0; 
  hud.final_score = 0; 
  hud.ability = 0; 
  hud.max_ability = GAME3_MAX_ABILITY; 

  LCD_Fill_Buffer(0);
  LCD_Refresh(&cfg0);
}

static void game3_update(void) {
  if (hud.is_game_over) { 
    return; 
  }

  Game3_Input input = {0}; 

  Game3_Input_Read(&input);

  Game3_Player_Update(&player, input.dx, input.jump_pressed, input.dash_pressed, input.dash_dx, input.attack_pressed);

  if (Game3_Enemy_Is_Touching_Player_Attack(&enemy, &player)) { 
    if (Game3_Enemy_Start_Attack_Knockback(&enemy, &player)) { 
      if (hud.ability < hud.max_ability) { 
        hud.ability += GAME3_ABILITY_GAIN_ON_HIT;

        if (hud.ability > hud.max_ability) { 
          hud.ability = hud.max_ability; 
        }
      }
    }
  } 

  if (Game3_Enemy_Is_Touching_Player(&enemy, &player)) { 
    Game3_Player_Take_Damage(&player, 1);
  }

  Game3_Enemy_Update(&enemy, &player);

  if (Game3_Enemy_Is_Touching_Player(&enemy, &player)) { 
    Game3_Player_Take_Damage(&player, 1);
  }


  hud.health = player.health; 
  hud.max_health = player.max_health; 

  hud.armour = player.armour; 
  hud.max_armour = player.max_armour; 

  if (player.health == 0) { 
    hud.is_game_over = 1; 
    hud.final_score = Game3_Get_Current_Score();
  }
}

static void game3_render(void) {
  LCD_Fill_Buffer(0);

  if (hud.is_game_over) { 
    Game3_UI_Draw_Game_Over(&hud);
    LCD_Refresh(&cfg0);
    return; 
  }

  Game3_Render_Draw_World();
  Game3_Render_Draw_Player(&player);
  Game3_Render_Draw_Player_Attack(&player);
  Game3_Render_Draw_Enemy(&enemy);
  Game3_UI_Draw(&hud);

  LCD_Refresh(&cfg0);
}

static void game3_shutdown(void) { 
  LCD_Fill_Buffer(0);
  LCD_Refresh(&cfg0);
 }

bool Game3_ShouldExit(void) { return game3_shutdown_requested; }

const GameApi game3_api = {
    .name = "Game 3",
    .init = game3_init,
    .update = game3_update,
    .render = game3_render,
    .shutdown = game3_shutdown,
    .should_exit = Game3_ShouldExit,
};