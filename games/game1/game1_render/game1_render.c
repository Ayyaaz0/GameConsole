#include "game1_render.h"
#include "LCD.h"
#include "game1_world/game1_world.h"
#include "room0_tiles.h"

#define GAME1_SCREEN_WIDTH 240
#define GAME1_SCREEN_HEIGHT 240
#define GAME1_ANIMATION_SPEED_FRAMES 2

static uint32_t animation_frame_counter = 0;

static void Game1_Render_DrawTile(int16_t screen_x, int16_t screen_y, const Game1_TileSprite *sprite) {
  if (sprite == 0 || sprite->pixels == 0) {
    return;
  }

  for (uint8_t y = 0; y < sprite->height; y++) {
    for (uint8_t x = 0; x < sprite->width; x++) {
      uint8_t colour = sprite->pixels[y * sprite->width + x];

      if (colour == 255) {
        continue;
      }

      LCD_Draw_Rect(screen_x + x, screen_y + y, 1, 1, colour, 1);
    }
  }
}

void Game1_Render_DrawWorld(const Game1_Camera *camera) {
  uint32_t animation_frame =
      animation_frame_counter / GAME1_ANIMATION_SPEED_FRAMES;

  for (uint16_t tile_y = 0; tile_y < GAME1_ROOM_HEIGHT; tile_y++) {
    for (uint16_t tile_x = 0; tile_x < GAME1_ROOM_WIDTH; tile_x++) {
      uint16_t tile = Game1_World_GetVisualTile(tile_x, tile_y);

      int16_t screen_x = (tile_x * GAME1_TILE_SIZE) - camera->x;
      int16_t screen_y = (tile_y * GAME1_TILE_SIZE) - camera->y;

      if (screen_x + GAME1_TILE_SIZE <= 0 || screen_x >= GAME1_SCREEN_WIDTH ||
          screen_y + GAME1_TILE_SIZE <= 0 || screen_y >= GAME1_SCREEN_HEIGHT) {
        continue;
      }

      if (tile == 0) {
        continue;
      }

      uint16_t draw_tile = Game1_Tiles_ResolveAnimation(tile, animation_frame);

      const Game1_TileSprite *sprite = Game1_Tiles_Find(draw_tile);

      if (sprite != 0) {
        Game1_Render_DrawTile(screen_x, screen_y, sprite);
      }
    }
  }

  animation_frame_counter++;
}

void Game1_Render_DrawPlayer(const Game1_Player *player, const Game1_Camera *camera) {
  int16_t screen_x = player->x - camera->x;
  int16_t screen_y = player->y - camera->y;

  LCD_Draw_Rect(screen_x, screen_y, player->width, player->height, 3, 1);
}