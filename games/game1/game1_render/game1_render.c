#include "game1_render.h"

#include "LCD.h"
#include "game1_world/game1_world.h"

#define GAME1_SCREEN_WIDTH 240
#define GAME1_SCREEN_HEIGHT 240

void Game1_Render_DrawWorld(const Game1_Camera *camera) {
  for (uint16_t tile_y = 0; tile_y < GAME1_ROOM_HEIGHT; tile_y++) {
    for (uint16_t tile_x = 0; tile_x < GAME1_ROOM_WIDTH; tile_x++) {
      if (!Game1_World_IsSolid(tile_x, tile_y)) {
        continue;
      }

      int16_t screen_x = (tile_x * GAME1_TILE_SIZE) - camera->x;
      int16_t screen_y = (tile_y * GAME1_TILE_SIZE) - camera->y;

      if (screen_x + GAME1_TILE_SIZE <= 0 || screen_x >= GAME1_SCREEN_WIDTH ||
          screen_y + GAME1_TILE_SIZE <= 0 || screen_y >= GAME1_SCREEN_HEIGHT) {
        continue;
      }

      LCD_Draw_Rect(screen_x, screen_y, GAME1_TILE_SIZE, GAME1_TILE_SIZE, 2, 1);
    }
  }
}

void Game1_Render_DrawPlayer(const Game1_Player *player, const Game1_Camera *camera) {
  int16_t screen_x = player->x - camera->x;
  int16_t screen_y = player->y - camera->y;

  LCD_Draw_Rect(screen_x, screen_y, player->width, player->height, 3, 1);
}