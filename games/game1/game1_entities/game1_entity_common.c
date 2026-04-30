#include "game1_entity_common.h"

#include "LCD.h"

void Game1_Entity_DrawSprite(int16_t screen_x, int16_t screen_y, const Game1_TileSprite *sprite) {
  if (sprite == 0 || sprite->pixels == 0) {
    return;
  }

  LCD_Draw_Sprite(screen_x, screen_y, sprite->height, sprite->width, sprite->pixels);
}