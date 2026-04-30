#include "game1_entity_common.h"

#include "LCD.h"

void Game1_Entity_DrawSprite(int16_t screen_x, int16_t screen_y, const Game1_TileSprite *sprite) {
  if (sprite == 0 || sprite->pixels == 0) {
    return;
  }

  for (uint8_t y = 0; y < sprite->height; y++) {
    for (uint8_t x = 0; x < sprite->width; x++) {
      uint8_t colour = sprite->pixels[y * sprite->width + x];

      if (colour == 0) {
        continue;
      }

      LCD_Draw_Rect(screen_x + x, screen_y + y, 1, 1, colour, 1);
    }
  }
}