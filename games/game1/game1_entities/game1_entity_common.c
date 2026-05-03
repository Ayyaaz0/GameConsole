#include "game1_entity_common.h"

#include "LCD.h"

#define GAME1_SCREEN_WIDTH 240
#define GAME1_SCREEN_HEIGHT 240

void Game1_Entity_DrawSprite(int16_t screen_x, int16_t screen_y, const Game1_TileSprite *sprite) {
  if (sprite == 0 || sprite->pixels == 0) {
    return;
  }

  LCD_Draw_Sprite(screen_x, screen_y, sprite->height, sprite->width, sprite->pixels);
}

void Game1_Entity_DrawSprite_Flipped(int16_t screen_x, int16_t screen_y, const Game1_TileSprite *sprite, uint8_t flip_x) {
  if (sprite == 0 || sprite->pixels == 0) {
    return;
  }

  if (!flip_x) {
    Game1_Entity_DrawSprite(screen_x, screen_y, sprite);
    return;
  }

  for (uint8_t y = 0; y < sprite->height; y++) {
    for (uint8_t x = 0; x < sprite->width; x++) {
      uint8_t source_x = sprite->width - 1 - x;
      uint8_t colour = sprite->pixels[y * sprite->width + source_x];

      if (colour == 255) {
        continue;
      }

      LCD_Set_Pixel(screen_x + x, screen_y + y, colour);
    }
  }
}

uint8_t Game1_Entity_IsVisibleOnScreen(int16_t screen_x, int16_t screen_y, uint8_t w, uint8_t h) {
  return !(screen_x + w <= 0 || screen_x >= GAME1_SCREEN_WIDTH || screen_y + h <= 0 || screen_y >= GAME1_SCREEN_HEIGHT);
}