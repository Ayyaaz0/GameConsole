#include "game1_entities.h"

#include "LCD.h"
#include "room0_entities.h"

void Game1_Entities_Init(void) {
  // For now, generated room0_entities[] is already const data.
}

void Game1_Entities_Update(Game1_Player *player, uint8_t interact_pressed) {
  (void)player;
  (void)interact_pressed;
}

void Game1_Entities_Render(const Game1_Camera *camera) {
  for (uint16_t i = 0; i < room0_entity_count; i++) {
    const Game1_Entity *entity = &room0_entities[i];

    int16_t screen_x = entity->x - camera->x;
    int16_t screen_y = entity->y - camera->y;

    uint8_t colour = 1;

    if (entity->type == ENTITY_DOOR) {
      colour = 2;
    } else if (entity->type == ENTITY_KEY) {
      colour = 6;
    } else if (entity->type == ENTITY_SPAWN) {
      colour = 3;
    }

    LCD_Draw_Rect(screen_x, screen_y, entity->w, entity->h, colour, 1);
  }
}