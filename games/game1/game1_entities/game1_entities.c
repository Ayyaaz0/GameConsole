#include "game1_entities.h"

#include "LCD.h"
#include "room0_entities.h"
#include <stdint.h>

//sprite spawn variables
static int16_t spawn_x = 0;
static int16_t spawn_y = 0;
static uint8_t has_spawn = 0;

void Game1_Entities_Init(void) {
  has_spawn = 0; 

  for (uint16_t i = 0; i < room0_entity_count; i++) {
    const Game1_Entity *entity = &room0_entities[i];

    if (entity->type == ENTITY_SPAWN) {
      spawn_x = entity->x;
      spawn_y = entity->y;
      has_spawn = 1;
      return; 
    }
  }
}

void Game1_Entities_SpawnPlayer(Game1_Player *player) {
  if (!has_spawn) { 
    return;
  }
  player->x = spawn_x;
  player->y = spawn_y;
}

void Game1_Entities_Update(Game1_Player *player, uint8_t interact_pressed) {
  (void)player;
  (void)interact_pressed;
}

void Game1_Entities_Render(const Game1_Camera *camera) {
  for (uint16_t i = 0; i < room0_entity_count; i++) {
    const Game1_Entity *entity = &room0_entities[i];

    if (entity->type == ENTITY_SPAWN) {
      continue;
    }

    int16_t screen_x = entity->x - camera->x;
    int16_t screen_y = entity->y - camera->y;

    uint8_t colour = 1;

    if (entity->type == ENTITY_DOOR) {
      colour = 2;
    } else if (entity->type == ENTITY_KEY) {
      colour = 6;
    }

    LCD_Draw_Rect(screen_x, screen_y, entity->w, entity->h, colour, 1);
  }
}