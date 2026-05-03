#include "game1_render.h"
#include "LCD.h"
#include "game1_player_sprite.h"
#include "game1_tiles.h"
#include "game1_world/game1_world.h"

#define GAME1_SCREEN_WIDTH 240
#define GAME1_SCREEN_HEIGHT 240
#define GAME1_ANIMATION_SPEED_FRAMES 2

static uint32_t animation_frame_counter = 0;

static void Game1_Render_DrawTile(int16_t screen_x, int16_t screen_y, const Game1_TileSprite *sprite) {
  if (sprite == 0 || sprite->pixels == 0) {
    return;
  }

  LCD_Draw_Sprite(screen_x, screen_y, sprite->height, sprite->width, sprite->pixels);
}

void Game1_Render_DrawWorld(const Game1_Camera *camera) {
  uint32_t animation_frame = animation_frame_counter / GAME1_ANIMATION_SPEED_FRAMES;

  /*
   * Determine which horizontal tiles are visible on screen.
   * Instead of rendering the whole room, we only render tiles
   * that fall within the camera viewport.
   */
  uint16_t first_tile_x = camera->x / GAME1_TILE_SIZE;
  uint16_t last_tile_x = first_tile_x + (GAME1_SCREEN_WIDTH / GAME1_TILE_SIZE) + 1;

  uint16_t room_width = Game1_World_GetCurrentRoomWidthTiles();

  if (last_tile_x > room_width) {
    last_tile_x = room_width;
  }

  for (uint16_t tile_y = 0; tile_y < GAME1_ROOM_HEIGHT; tile_y++) {
    for (uint16_t tile_x = first_tile_x; tile_x < last_tile_x; tile_x++) {
      uint16_t tile = Game1_World_GetVisualTile(tile_x, tile_y);

      if (tile == 0) {
        continue;
      }

      int16_t screen_x = (tile_x * GAME1_TILE_SIZE) - camera->x;
      int16_t screen_y = (tile_y * GAME1_TILE_SIZE) - camera->y;

      uint16_t draw_tile =
          Game1_Tiles_ResolveAnimation(tile, animation_frame);

      const Game1_TileSprite *sprite = Game1_Tiles_Find(draw_tile);

      if (sprite != 0) {
        Game1_Render_DrawTile(screen_x, screen_y, sprite);
      }
    }
  }

  animation_frame_counter++;
}

void Game1_Render_DrawPlayer(const Game1_Player *player, const Game1_Camera *camera) {
  Game1_PlayerSprite_Render(player, camera);
}