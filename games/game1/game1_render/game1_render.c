#include "game1_render.h"
#include "game1_world.h"
#include "LCD.h"
#include "stm32l476xx.h"

extern ST7789V2_cfg_t cfg0;

void Game1_Render_DrawWorld(void) {
  for (uint16_t y = 0; y < GAME1_MAP_HEIGHT; y++) {
    for (uint16_t x = 0; x < GAME1_MAP_WIDTH; x++) {

      if (Game1_World_IsSolid(x, y)) {

        LCD_Draw_Rect(x * GAME1_TILE_SIZE, y * GAME1_TILE_SIZE, GAME1_TILE_SIZE,
                      GAME1_TILE_SIZE,
                      2, // colour
                      1  // filled
        );
      }
    }
  }
}

void Game1_Render_DrawPlayer(const Game1_Player *player) {
  LCD_Draw_Rect(player->x, player->y, player->width, player->height, 3, 1);
}