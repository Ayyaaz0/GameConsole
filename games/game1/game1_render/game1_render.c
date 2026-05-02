#include "game1_render.h"
#include "LCD.h"
#include "game1_player_sprite.h"
#include "game1_tiles.h"
#include "game1_world/game1_world.h"

#define GAME1_SCREEN_WIDTH 240
#define GAME1_SCREEN_HEIGHT 240
#define GAME1_ANIMATION_SPEED_FRAMES 2

static uint32_t animation_frame_counter = 0;

static void Game1_Render_DrawTile(int16_t screen_x, int16_t screen_y,
                                  const Game1_TileSprite *sprite) {
  if (sprite == 0 || sprite->pixels == 0) {
    return;
  }

  LCD_Draw_Sprite(screen_x, screen_y, sprite->height, sprite->width,
                  sprite->pixels);
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

void Game1_Render_DrawPlayer(const Game1_Player *player,
                             const Game1_Camera *camera) {
  Game1_PlayerSprite_Render(player, camera);
}