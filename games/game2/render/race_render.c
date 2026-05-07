#include "race_render.h"

#include "../boost/race_boost.h"
#include "../config/race_config.h"
#include "../track/race_track_layout.h"
#include "LCD.h"
#include "stm32l4xx_hal.h"


#include <stddef.h>
#include <stdio.h>

extern ST7789V2_cfg_t cfg0;

#define RACE_RENDER_BLACK 0U
#define RACE_RENDER_WHITE 1U
#define RACE_RENDER_RED 2U
#define RACE_RENDER_GREEN 3U
#define RACE_RENDER_BLUE 4U
#define RACE_RENDER_YELLOW 6U
#define RACE_RENDER_BROWN 12U
#define RACE_RENDER_GREY 13U

#define RACE_RENDER_TRANSPARENT 255U

#define RACE_RENDER_GANTRY_POST_PX 5
#define RACE_RENDER_GANTRY_BEAM_PX 8
#define RACE_RENDER_GANTRY_POST_GAP_PX 12
#define RACE_RENDER_CHECKER_TILE_PX 6U

#define RACE_RENDER_ROTATED_CHAR_W 7
#define RACE_RENDER_ROTATED_CHAR_H 5
#define RACE_RENDER_VERTICAL_TEXT_STEP_PX 6

#define RACE_PLAYER_SPRITE_ROWS 14U
#define RACE_PLAYER_SPRITE_COLS 10U

#define RACE_FLAG_SCALE 3U
#define RACE_FLAG_GRASS_GAP_PX 8
#define RACE_FLAG_SPACING_Y 28

// 10x14 top-down open-wheel race car sprite
// 255 = transparent
// 0   = black tyres / outline
// 1   = white body
// 2   = red rear lights
// 4   = blue cockpit/windows
// 6   = yellow headlights
// 13  = grey tyre highlight
static const uint8_t
    g_player_car_sprite[RACE_PLAYER_SPRITE_ROWS * RACE_PLAYER_SPRITE_COLS] = {
        255, 255, 255, 6,   1,   1,   6,   255, 255, 255, 255, 255, 1,   1,
        1,   1,   1,   1,   255, 255, 255, 0,   1,   4,   4,   4,   4,   1,
        0,   255, 0,   0,   1,   4,   4,   4,   4,   1,   0,   0,   0,   13,
        1,   1,   4,   4,   1,   1,   13,  0,   255, 0,   1,   1,   1,   1,
        1,   1,   0,   255, 255, 255, 1,   1,   0,   0,   1,   1,   255, 255,
        255, 255, 1,   1,   0,   0,   1,   1,   255, 255, 255, 0,   1,   1,
        1,   1,   1,   1,   0,   255, 0,   13,  1,   1,   1,   1,   1,   1,
        13,  0,   0,   0,   1,   1,   4,   4,   1,   1,   0,   0,   255, 0,
        1,   4,   4,   4,   4,   1,   0,   255, 255, 255, 1,   1,   2,   2,
        1,   1,   255, 255, 255, 255, 255, 2,   2,   2,   2,   255, 255, 255,
};

typedef enum {
  RACE_RENDER_DIRECTION_NORTH = 0,
  RACE_RENDER_DIRECTION_EAST,
  RACE_RENDER_DIRECTION_SOUTH,
  RACE_RENDER_DIRECTION_WEST
} RaceRenderDirection;

static RaceRenderDirection g_last_player_direction =
    RACE_RENDER_DIRECTION_NORTH;

// Draws a filled rectangle but clips it so LCD_Draw_Rect never receives
// negative or off-screen coordinates
static void RaceRender_DrawRectClipped(const RaceTrack *track, int16_t x,
                                       int16_t y, int16_t width, int16_t height,
                                       uint8_t colour) {
  int16_t screen_width = 0;
  int16_t screen_height = 0;

  if ((track == NULL) || (width <= 0) || (height <= 0)) {
    return;
  }

  screen_width = (int16_t)track->screen_width;
  screen_height = (int16_t)track->screen_height;

  if ((x >= screen_width) || (y >= screen_height) || ((x + width) <= 0) ||
      ((y + height) <= 0)) {
    return;
  }

  if (x < 0) {
    width = (int16_t)(width + x);
    x = 0;
  }

  if (y < 0) {
    height = (int16_t)(height + y);
    y = 0;
  }

  if ((x + width) > screen_width) {
    width = (int16_t)(screen_width - x);
  }

  if ((y + height) > screen_height) {
    height = (int16_t)(screen_height - y);
  }

  if ((width <= 0) || (height <= 0)) {
    return;
  }

  LCD_Draw_Rect((uint16_t)x, (uint16_t)y, (uint16_t)width, (uint16_t)height,
                colour, 1U);
}
static void RaceRender_UpdateSkidMarks(const RaceCar *player_car,
                                       const RaceBoostState *boost_state,
                                       const RaceState *race_state);

static void RaceRender_DrawSkidMarks(const RaceTrack *track);

static void RaceRender_DrawSpriteClipped(const RaceTrack *track, int16_t x,
                                         int16_t y, uint16_t rows,
                                         uint16_t cols, const uint8_t *sprite,
                                         RaceRenderDirection direction) {
  uint16_t row = 0U;
  uint16_t col = 0U;

  if ((track == NULL) || (sprite == NULL)) {
    return;
  }

  for (row = 0U; row < rows; row++) {
    for (col = 0U; col < cols; col++) {
      uint8_t colour = sprite[(row * cols) + col];
      int16_t dst_x = 0;
      int16_t dst_y = 0;
      int16_t pixel_x = 0;
      int16_t pixel_y = 0;

      if (colour == RACE_RENDER_TRANSPARENT) {
        continue;
      }

      switch (direction) {
      case RACE_RENDER_DIRECTION_EAST:
        // Rotate north-facing sprite 90 degrees clockwise
        dst_x = (int16_t)((rows - 1U) - row);
        dst_y = (int16_t)col;
        break;

      case RACE_RENDER_DIRECTION_SOUTH:
        // Rotate north-facing sprite 180 degrees
        dst_x = (int16_t)((cols - 1U) - col);
        dst_y = (int16_t)((rows - 1U) - row);
        break;

      case RACE_RENDER_DIRECTION_WEST:
        // Rotate north-facing sprite 90 degrees counter-clockwise
        dst_x = (int16_t)row;
        dst_y = (int16_t)((cols - 1U) - col);
        break;

      case RACE_RENDER_DIRECTION_NORTH:
      default:
        dst_x = (int16_t)col;
        dst_y = (int16_t)row;
        break;
      }

      pixel_x = (int16_t)(x + dst_x);
      pixel_y = (int16_t)(y + dst_y);

      if ((pixel_x < 0) || (pixel_y < 0) ||
          (pixel_x >= (int16_t)track->screen_width) ||
          (pixel_y >= (int16_t)track->screen_height)) {
        continue;
      }

      LCD_Set_Pixel((uint16_t)pixel_x, (uint16_t)pixel_y, colour);
    }
  }
}

static uint16_t RaceRender_StringLength(const char *text);

static void RaceRender_DrawAnimatedFlags(const RaceTrack *track);
static bool RaceRender_IsFlagGrassTile(RaceTileType tile);
static bool RaceRender_IsFlagTrackEdgeTile(RaceTileType tile);
static bool RaceRender_TileTouchesTrackEdge(const RaceTrack *track, int tile_x,
                                            int tile_y);
static bool RaceRender_ShouldDrawFlagOnTile(int tile_x, int tile_y);
static void RaceRender_DrawFlagSpriteAtWorld(const RaceTrack *track,
                                             float world_x, float world_y,
                                             uint8_t frame);
static void RaceRender_DrawFlagSpriteAtWorld(const RaceTrack *track,
                                             float world_x, float world_y,
                                             uint8_t frame);

static void RaceRender_DrawRotatedChar(const RaceTrack *track, char c,
                                       int16_t x, int16_t y, uint8_t colour,
                                       bool clockwise) {
  uint8_t col = 0U;
  uint8_t row = 0U;

  if (track == NULL) {
    return;
  }

  if ((c < 32) || (c > 127)) {
    return;
  }

  for (col = 0U; col < 5U; col++) {
    uint8_t column_bits = font5x7_[(uint16_t)(c - 32) * 5U + col];

    for (row = 0U; row < 7U; row++) {
      int16_t pixel_x = 0;
      int16_t pixel_y = 0;

      if ((column_bits & (1U << row)) == 0U) {
        continue;
      }

      if (clockwise == true) {
        // Original (col,row) -> rotated 90 degrees clockwise
        pixel_x = (int16_t)(x + (6 - row));
        pixel_y = (int16_t)(y + col);
      } else {
        // Original (col,row) -> rotated 90 degrees counter-clockwise
        pixel_x = (int16_t)(x + row);
        pixel_y = (int16_t)(y + (4 - col));
      }

      if ((pixel_x < 0) || (pixel_y < 0) ||
          (pixel_x >= (int16_t)track->screen_width) ||
          (pixel_y >= (int16_t)track->screen_height)) {
        continue;
      }

      LCD_Set_Pixel((uint16_t)pixel_x, (uint16_t)pixel_y, colour);
    }
  }
}

static uint16_t RaceRender_StringLength(const char *text) {
  uint16_t length = 0U;

  if (text == NULL) {
    return 0U;
  }

  while (text[length] != '\0') {
    length++;

    // Safety limit so a broken string cannot run forever
    if (length >= 80U) {
      break;
    }
  }

  return length;
}

static void RaceRender_DrawVerticalTextRotated(
    const RaceTrack *track, const char *text, int16_t x, int16_t y,
    int16_t height, uint8_t colour, bool bottom_to_top, bool clockwise) {
  uint16_t i = 0U;
  uint16_t length = 0U;
  int16_t text_span = 0;
  int16_t start_y = 0;

  if ((track == NULL) || (text == NULL) || (height <= 0)) {
    return;
  }

  length = RaceRender_StringLength(text);

  if (length == 0U) {
    return;
  }

  // Total vertical space used by the rotated text
  text_span =
      (int16_t)(RACE_RENDER_ROTATED_CHAR_H +
                ((int16_t)(length - 1U) * RACE_RENDER_VERTICAL_TEXT_STEP_PX));

  start_y = (int16_t)(y + ((height - text_span) / 2));

  for (i = 0U; i < length; i++) {
    int16_t char_y = 0;

    if (bottom_to_top == true) {
      char_y = (int16_t)(start_y + text_span - RACE_RENDER_ROTATED_CHAR_H -
                         ((int16_t)i * RACE_RENDER_VERTICAL_TEXT_STEP_PX));
    } else {
      char_y =
          (int16_t)(start_y + ((int16_t)i * RACE_RENDER_VERTICAL_TEXT_STEP_PX));
    }

    // Space = vertical gap only
    if (text[i] == ' ') {
      continue;
    }

    RaceRender_DrawRotatedChar(track, text[i], x, char_y, colour, clockwise);
  }
}

static uint8_t RaceRender_GetTileColour(RaceTileType tile) {
  switch (tile) {
  case RACE_TILE_GRASS:
  case RACE_TILE_GRASS_DARK:
  case RACE_TILE_TREE:
    return RACE_RENDER_GREEN;

  case RACE_TILE_ROAD:
  case RACE_TILE_ROAD_DARK:
  case RACE_TILE_START:
  case RACE_TILE_CHECKPOINT:
  case RACE_TILE_JOKER:
  case RACE_TILE_CURB_RED:
  case RACE_TILE_CURB_WHITE:
    return RACE_RENDER_GREY;

  case RACE_TILE_BUILDING:
    return RACE_RENDER_GREEN;

  case RACE_TILE_SAND:
  case RACE_TILE_TYRE:
  case RACE_TILE_BARRIER:
  default:
    return RACE_RENDER_GREEN;
  }
}

static bool RaceRender_IsRaceSurfaceTile(RaceTileType tile) {
  return ((tile == RACE_TILE_ROAD) || (tile == RACE_TILE_ROAD_DARK) ||
          (tile == RACE_TILE_START) || (tile == RACE_TILE_CHECKPOINT) ||
          (tile == RACE_TILE_JOKER));
}

static uint8_t RaceRender_GetKerbColour(int tile_x, int tile_y) {
  if (((tile_x + tile_y) & 1) == 0) {
    return RACE_RENDER_RED;
  }

  return RACE_RENDER_WHITE;
}

static void RaceRender_DrawRoadEdgeDetails(const RaceTrack *track, int tile_x,
                                           int tile_y, int16_t screen_x,
                                           int16_t screen_y) {
  RaceTileType current_tile = RACE_TILE_GRASS;
  RaceTileType north_tile = RACE_TILE_GRASS;
  RaceTileType south_tile = RACE_TILE_GRASS;
  RaceTileType west_tile = RACE_TILE_GRASS;
  RaceTileType east_tile = RACE_TILE_GRASS;

  uint8_t kerb_colour = RACE_RENDER_WHITE;

  if (track == NULL) {
    return;
  }

  current_tile = RaceTrack_GetTileAt(track, tile_x, tile_y);

  if (RaceRender_IsRaceSurfaceTile(current_tile) == false) {
    return;
  }

  north_tile = RaceTrack_GetTileAt(track, tile_x, tile_y - 1);
  south_tile = RaceTrack_GetTileAt(track, tile_x, tile_y + 1);
  west_tile = RaceTrack_GetTileAt(track, tile_x - 1, tile_y);
  east_tile = RaceTrack_GetTileAt(track, tile_x + 1, tile_y);

  kerb_colour = RaceRender_GetKerbColour(tile_x, tile_y);

  // North edge: 1px white line + 2px red/white kerb inside the road
  if (RaceRender_IsRaceSurfaceTile(north_tile) == false) {
    RaceRender_DrawRectClipped(track, screen_x, screen_y,
                               (int16_t)RACE_TILE_SIZE, 1, RACE_RENDER_WHITE);

    RaceRender_DrawRectClipped(track, screen_x, (int16_t)(screen_y + 1),
                               (int16_t)RACE_TILE_SIZE, 2, kerb_colour);
  }

  // South edge
  if (RaceRender_IsRaceSurfaceTile(south_tile) == false) {
    RaceRender_DrawRectClipped(track, screen_x,
                               (int16_t)(screen_y + RACE_TILE_SIZE - 1),
                               (int16_t)RACE_TILE_SIZE, 1, RACE_RENDER_WHITE);

    RaceRender_DrawRectClipped(track, screen_x,
                               (int16_t)(screen_y + RACE_TILE_SIZE - 3),
                               (int16_t)RACE_TILE_SIZE, 2, kerb_colour);
  }

  // West edge
  if (RaceRender_IsRaceSurfaceTile(west_tile) == false) {
    RaceRender_DrawRectClipped(track, screen_x, screen_y, 1,
                               (int16_t)RACE_TILE_SIZE, RACE_RENDER_WHITE);

    RaceRender_DrawRectClipped(track, (int16_t)(screen_x + 1), screen_y, 2,
                               (int16_t)RACE_TILE_SIZE, kerb_colour);
  }

  // East edge
  if (RaceRender_IsRaceSurfaceTile(east_tile) == false) {
    RaceRender_DrawRectClipped(track, (int16_t)(screen_x + RACE_TILE_SIZE - 1),
                               screen_y, 1, (int16_t)RACE_TILE_SIZE,
                               RACE_RENDER_WHITE);

    RaceRender_DrawRectClipped(track, (int16_t)(screen_x + RACE_TILE_SIZE - 3),
                               screen_y, 2, (int16_t)RACE_TILE_SIZE,
                               kerb_colour);
  }
}

static void RaceRender_DrawTrackEdgeOverlay(const RaceTrack *track) {
  int tile_left = 0;
  int tile_right = 0;
  int tile_top = 0;
  int tile_bottom = 0;
  int tile_x = 0;
  int tile_y = 0;

  if (track == NULL) {
    return;
  }

  tile_left = (int)(track->camera_x / (float)RACE_TILE_SIZE) - 1;
  tile_top = (int)(track->camera_y / (float)RACE_TILE_SIZE) - 1;

  tile_right = tile_left + ((int)track->screen_width / (int)RACE_TILE_SIZE) + 3;
  tile_bottom =
      tile_top + ((int)track->screen_height / (int)RACE_TILE_SIZE) + 3;

  if (tile_left < 0) {
    tile_left = 0;
  }

  if (tile_top < 0) {
    tile_top = 0;
  }

  if (tile_right >= (int)RACE_WORLD_TILES_X) {
    tile_right = (int)RACE_WORLD_TILES_X - 1;
  }

  if (tile_bottom >= (int)RACE_WORLD_TILES_Y) {
    tile_bottom = (int)RACE_WORLD_TILES_Y - 1;
  }

  for (tile_y = tile_top; tile_y <= tile_bottom; tile_y++) {
    for (tile_x = tile_left; tile_x <= tile_right; tile_x++) {
      int16_t screen_x = RaceTrack_WorldToScreenX(
          track, (float)(tile_x * (int)RACE_TILE_SIZE));
      int16_t screen_y = RaceTrack_WorldToScreenY(
          track, (float)(tile_y * (int)RACE_TILE_SIZE));

      RaceRender_DrawRoadEdgeDetails(track, tile_x, tile_y, screen_x, screen_y);
    }
  }
}

#define RACE_GRID_SLOT_W 12
#define RACE_GRID_SLOT_H 8
#define RACE_GRID_CLEARANCE_PX 10

static void RaceRender_DrawWorldGridSlotMark(const RaceTrack *track,
                                             int16_t world_x, int16_t world_y,
                                             int16_t width, int16_t height,
                                             uint8_t colour) {
  int16_t screen_x = 0;
  int16_t screen_y = 0;
  int16_t arm_len = 7;

  if (track == NULL) {
    return;
  }

  screen_x = RaceTrack_WorldToScreenX(track, (float)world_x);
  screen_y = RaceTrack_WorldToScreenY(track, (float)world_y);

  // Single-sided grid slot mark, shaped like "["
  RaceRender_DrawRectClipped(track, screen_x, screen_y, 1, height, colour);
  RaceRender_DrawRectClipped(track, screen_x, screen_y, arm_len, 1, colour);
  RaceRender_DrawRectClipped(track, screen_x, (int16_t)(screen_y + height - 1),
                             arm_len, 1, colour);
}

static bool RaceRender_IsGridSurfaceTile(RaceTileType tile) {
  return ((tile == RACE_TILE_ROAD) || (tile == RACE_TILE_ROAD_DARK) ||
          (tile == RACE_TILE_START) || (tile == RACE_TILE_CHECKPOINT) ||
          (tile == RACE_TILE_JOKER));
}

static bool RaceRender_IsWorldPointOnGridSurface(const RaceTrack *track,
                                                 int16_t world_x,
                                                 int16_t world_y) {
  int tile_x = 0;
  int tile_y = 0;
  RaceTileType tile = RACE_TILE_GRASS;

  if (track == NULL) {
    return false;
  }

  tile_x = world_x / (int16_t)RACE_TILE_SIZE;
  tile_y = world_y / (int16_t)RACE_TILE_SIZE;

  tile = RaceTrack_GetTileAt(track, tile_x, tile_y);

  return RaceRender_IsGridSurfaceTile(tile);
}

static bool RaceRender_IsGridSlotFullyOnTrack(const RaceTrack *track,
                                              int16_t world_x, int16_t world_y,
                                              int16_t width, int16_t height) {
  int16_t left = 0;
  int16_t right = 0;
  int16_t top = 0;
  int16_t bottom = 0;
  int16_t check_x = 0;
  int16_t check_y = 0;

  if (track == NULL) {
    return false;
  }

  // Expanded invisible safety box around the slot
  // This stops the grid mark from touching curbs or grass edges
  left = world_x - RACE_GRID_CLEARANCE_PX;
  right = world_x + width + RACE_GRID_CLEARANCE_PX;
  top = world_y - RACE_GRID_CLEARANCE_PX;
  bottom = world_y + height + RACE_GRID_CLEARANCE_PX;

  for (check_y = top; check_y <= bottom; check_y += 4) {
    for (check_x = left; check_x <= right; check_x += 4) {
      if (RaceRender_IsWorldPointOnGridSurface(track, check_x, check_y) ==
          false) {
        return false;
      }
    }
  }

  return true;
}

static void RaceRender_DrawGridSlotIfOnTrack(const RaceTrack *track,
                                             int16_t world_x, int16_t world_y) {
  static const int16_t offsets[][2] = {
      {0, 0},   {6, -6},  {10, -8}, {12, -4},  {8, 0},   {4, -10},
      {-6, -6}, {0, -12}, {6, 6},   {12, -12}, {16, -8},
  };

  uint8_t i = 0U;

  if (track == NULL) {
    return;
  }

  for (i = 0U; i < (sizeof(offsets) / sizeof(offsets[0])); i++) {
    int16_t candidate_x = world_x + offsets[i][0];
    int16_t candidate_y = world_y + offsets[i][1];

    if (RaceRender_IsGridSlotFullyOnTrack(track, candidate_x, candidate_y,
                                          RACE_GRID_SLOT_W,
                                          RACE_GRID_SLOT_H) == true) {
      RaceRender_DrawWorldGridSlotMark(track, candidate_x, candidate_y,
                                       RACE_GRID_SLOT_W, RACE_GRID_SLOT_H,
                                       RACE_RENDER_WHITE);
      return;
    }
  }
}

static void RaceRender_DrawStartGridBoxes(const RaceTrack *track) {
  if (track == NULL) {
    return;
  }

  //  staggered grid slots: two columns and two rows
  // Each slot is only drawn if it has enough road clearance around it

  RaceRender_DrawGridSlotIfOnTrack(track, 336, 512);
  RaceRender_DrawGridSlotIfOnTrack(track, 366, 526);

  RaceRender_DrawGridSlotIfOnTrack(track, 336, 550);
  RaceRender_DrawGridSlotIfOnTrack(track, 366, 564);

  RaceRender_DrawGridSlotIfOnTrack(track, 336, 588);
  RaceRender_DrawGridSlotIfOnTrack(track, 366, 602);
}

static void RaceRender_DrawWorldTiles(const RaceTrack *track) {
  int tile_left = 0;
  int tile_right = 0;
  int tile_top = 0;
  int tile_bottom = 0;
  int tile_x = 0;
  int tile_y = 0;

  if (track == NULL) {
    return;
  }

  tile_left = (int)(track->camera_x / (float)RACE_TILE_SIZE) - 1;
  tile_top = (int)(track->camera_y / (float)RACE_TILE_SIZE) - 1;

  tile_right = tile_left + ((int)track->screen_width / (int)RACE_TILE_SIZE) + 3;
  tile_bottom =
      tile_top + ((int)track->screen_height / (int)RACE_TILE_SIZE) + 3;

  if (tile_left < 0) {
    tile_left = 0;
  }

  if (tile_top < 0) {
    tile_top = 0;
  }

  if (tile_right >= (int)RACE_WORLD_TILES_X) {
    tile_right = (int)RACE_WORLD_TILES_X - 1;
  }

  if (tile_bottom >= (int)RACE_WORLD_TILES_Y) {
    tile_bottom = (int)RACE_WORLD_TILES_Y - 1;
  }

  for (tile_y = tile_top; tile_y <= tile_bottom; tile_y++) {
    for (tile_x = tile_left; tile_x <= tile_right; tile_x++) {
      RaceTileType tile = RaceTrack_GetTileAt(track, tile_x, tile_y);
      uint8_t colour = RaceRender_GetTileColour(tile);

      int16_t world_x = (int16_t)(tile_x * (int)RACE_TILE_SIZE);
      int16_t world_y = (int16_t)(tile_y * (int)RACE_TILE_SIZE);
      int16_t screen_x = RaceTrack_WorldToScreenX(track, (float)world_x);
      int16_t screen_y = RaceTrack_WorldToScreenY(track, (float)world_y);

      RaceRender_DrawRectClipped(track, screen_x, screen_y,
                                 (int16_t)RACE_TILE_SIZE,
                                 (int16_t)RACE_TILE_SIZE, colour);
    }
  }
}

// Draws the visible start/finish checkered floor line
// The rectangle passed in is the invisible trigger zone
static void RaceRender_DrawCheckeredZone(const RaceTrack *track,
                                         RaceTriggerZone zone) {
  int16_t zone_x = 0;
  int16_t zone_y = 0;
  uint16_t local_x = 0U;
  uint16_t local_y = 0U;

  if (track == NULL) {
    return;
  }

  zone_x = RaceTrack_WorldToScreenX(track, (float)zone.x);
  zone_y = RaceTrack_WorldToScreenY(track, (float)zone.y);

  for (local_y = 0U; local_y < zone.height;
       local_y += RACE_RENDER_CHECKER_TILE_PX) {
    for (local_x = 0U; local_x < zone.width;
         local_x += RACE_RENDER_CHECKER_TILE_PX) {
      uint16_t tile_width = RACE_RENDER_CHECKER_TILE_PX;
      uint16_t tile_height = RACE_RENDER_CHECKER_TILE_PX;
      uint8_t colour = RACE_RENDER_WHITE;

      if ((local_x + tile_width) > zone.width) {
        tile_width = (uint16_t)(zone.width - local_x);
      }

      if ((local_y + tile_height) > zone.height) {
        tile_height = (uint16_t)(zone.height - local_y);
      }

      if ((((local_x / RACE_RENDER_CHECKER_TILE_PX) +
            (local_y / RACE_RENDER_CHECKER_TILE_PX)) &
           1U) != 0U) {
        colour = RACE_RENDER_BLACK;
      }

      RaceRender_DrawRectClipped(
          track, (int16_t)(zone_x + local_x), (int16_t)(zone_y + local_y),
          (int16_t)tile_width, (int16_t)tile_height, colour);
    }
  }
}

static bool RaceRender_IsGrassTile(RaceTileType tile) {
  return ((tile == RACE_TILE_GRASS) || (tile == RACE_TILE_GRASS_DARK));
}

static bool RaceRender_ScreenRectIsGrassOnly(const RaceTrack *track,
                                             int16_t screen_x, int16_t screen_y,
                                             int16_t width, int16_t height) {
  int16_t check_x = 0;
  int16_t check_y = 0;

  if ((track == NULL) || (width <= 0) || (height <= 0)) {
    return false;
  }

  // If any part is off-screen, do not draw it
  if ((screen_x < 0) || (screen_y < 0) ||
      ((screen_x + width) >= (int16_t)track->screen_width) ||
      ((screen_y + height) >= (int16_t)track->screen_height)) {
    return false;
  }

  // Check the whole tyre rectangle, not just the centre
  // This prevents tyres touching curbs/road edges
  for (check_y = screen_y; check_y < (screen_y + height); check_y += 2) {
    for (check_x = screen_x; check_x < (screen_x + width); check_x += 2) {
      float world_x = track->camera_x + (float)check_x;
      float world_y = track->camera_y + (float)check_y;
      RaceTileType tile = RaceTrack_GetTileAtWorld(track, world_x, world_y);

      if (RaceRender_IsGrassTile(tile) == false) {
        return false;
      }
    }
  }

  return true;
}

static void RaceRender_DrawTyreDotScreen(const RaceTrack *track,
                                         int16_t screen_x, int16_t screen_y) {
  if (track == NULL) {
    return;
  }

  // Tyres are scenery only. They must sit on grass, never road/curb/wood
  if (RaceRender_ScreenRectIsGrassOnly(track, screen_x, screen_y, 4, 4) ==
      false) {
    return;
  }

  // Small tyre: black outer rubber with grey centre
  RaceRender_DrawRectClipped(track, screen_x, screen_y, 4, 4,
                             RACE_RENDER_BLACK);

  RaceRender_DrawRectClipped(track, (int16_t)(screen_x + 1),
                             (int16_t)(screen_y + 1), 2, 2, RACE_RENDER_GREY);
}

static void RaceRender_DrawTyreColumnTwoThick(const RaceTrack *track,
                                              int16_t screen_x,
                                              int16_t screen_y, uint8_t count) {
  uint8_t i = 0U;

  if (track == NULL) {
    return;
  }

  for (i = 0U; i < count; i++) {
    int16_t y = (int16_t)(screen_y + (i * 5U));

    // Two tyres thick horizontally
    RaceRender_DrawTyreDotScreen(track, screen_x, y);
    RaceRender_DrawTyreDotScreen(track, (int16_t)(screen_x + 5), y);
  }
}

static void RaceRender_DrawTyreRowTwoThick(const RaceTrack *track,
                                           int16_t screen_x, int16_t screen_y,
                                           uint8_t count) {
  uint8_t i = 0U;

  if (track == NULL) {
    return;
  }

  for (i = 0U; i < count; i++) {
    int16_t x = (int16_t)(screen_x + (i * 5U));

    // Two tyres thick vertically
    RaceRender_DrawTyreDotScreen(track, x, screen_y);
    RaceRender_DrawTyreDotScreen(track, x, (int16_t)(screen_y + 5));
  }
}

static void RaceRender_DrawVerticalPostProtection(const RaceTrack *track,
                                                  int16_t post_x,
                                                  int16_t post_y,
                                                  int16_t post_width,
                                                  int16_t post_height) {
  int16_t left_guard_x = 0;
  int16_t right_guard_x = 0;
  int16_t side_guard_y = 0;
  int16_t bottom_guard_x = 0;
  int16_t bottom_guard_y = 0;
  uint8_t side_count = 0U;
  uint8_t bottom_count = 0U;

  if (track == NULL) {
    return;
  }

  left_guard_x = (int16_t)(post_x - 10);
  right_guard_x = (int16_t)(post_x + post_width + 1);

  // Start lower down the post, away from the wooden banner connection
  side_guard_y = (int16_t)(post_y + 8);

  bottom_guard_x = (int16_t)(post_x - 5);
  bottom_guard_y = (int16_t)(post_y + post_height + 1);

  side_count = (uint8_t)((post_height - 8) / 5);
  if (side_count < 1U) {
    side_count = 1U;
  }

  bottom_count = (uint8_t)((post_width + 10) / 5);
  if (bottom_count < 2U) {
    bottom_count = 2U;
  }

  // Left side protection, two tyres thick
  RaceRender_DrawTyreColumnTwoThick(track, left_guard_x, side_guard_y,
                                    side_count);

  // Right side protection, two tyres thick
  RaceRender_DrawTyreColumnTwoThick(track, right_guard_x, side_guard_y,
                                    side_count);

  // Bottom protection, two tyres thick
  RaceRender_DrawTyreRowTwoThick(track, bottom_guard_x, bottom_guard_y,
                                 bottom_count);
}

static void RaceRender_DrawHorizontalPostProtection(
    const RaceTrack *track, int16_t post_x, int16_t post_y, int16_t post_width,
    int16_t post_height, bool protect_above) {
  int16_t left_guard_x = 0;
  int16_t right_guard_x = 0;
  int16_t outer_guard_y = 0;

  if (track == NULL) {
    return;
  }

  left_guard_x = (int16_t)(post_x - 10);
  right_guard_x = (int16_t)(post_x + post_width + 1);

  if (protect_above == true) {
    outer_guard_y = (int16_t)(post_y - 10);
  } else {
    outer_guard_y = (int16_t)(post_y + post_height + 1);
  }

  // Left end, two tyres thick
  RaceRender_DrawTyreRowTwoThick(track, left_guard_x, outer_guard_y, 2U);

  // Right end, two tyres thick
  RaceRender_DrawTyreRowTwoThick(track, right_guard_x, outer_guard_y, 2U);
}

// Horizontal gantry: CP1, CP3, JOKER IN
// Posts are placed outside the trigger rectangle, so they sit in the grass and
// do not touch the track
static void RaceRender_DrawHorizontalGantry(const RaceTrack *track,
                                            RaceTriggerZone zone,
                                            const char *label) {
  int16_t x = 0;
  int16_t y = 0;
  int16_t beam_y = 0;
  int16_t beam_x = 0;
  int16_t beam_width = 0;
  int16_t left_post_x = 0;
  int16_t right_post_x = 0;
  int16_t text_x = 0;
  int16_t text_y = 0;
  int16_t text_width = 0;
  int16_t post_top_y = 0;
  int16_t post_height = 0;

  if ((track == NULL) || (label == NULL)) {
    return;
  }

  x = RaceTrack_WorldToScreenX(track, (float)zone.x);
  y = RaceTrack_WorldToScreenY(track, (float)zone.y);

  beam_y = (int16_t)(y + ((int16_t)zone.height / 2) -
                     (RACE_RENDER_GANTRY_BEAM_PX / 2));

  left_post_x = (int16_t)(x - RACE_RENDER_GANTRY_POST_GAP_PX -
                          RACE_RENDER_GANTRY_POST_PX);
  right_post_x = (int16_t)(x + zone.width + RACE_RENDER_GANTRY_POST_GAP_PX);

  // Beam now stretches all the way from left post to right post
  // so it doesn't look like floating wood
  beam_x = left_post_x;
  beam_width = (int16_t)(zone.width + (2 * RACE_RENDER_GANTRY_POST_GAP_PX) +
                         (2 * RACE_RENDER_GANTRY_POST_PX));

  post_top_y = (int16_t)(beam_y - 5);
  post_height = (int16_t)(RACE_RENDER_GANTRY_BEAM_PX + 10);

  // Main wooden beam connected to the posts
  RaceRender_DrawRectClipped(track, beam_x, beam_y, beam_width,
                             RACE_RENDER_GANTRY_BEAM_PX, RACE_RENDER_BROWN);

  // Left post
  RaceRender_DrawRectClipped(track, left_post_x, post_top_y,
                             RACE_RENDER_GANTRY_POST_PX, post_height,
                             RACE_RENDER_BROWN);

  // Right post
  RaceRender_DrawRectClipped(track, right_post_x, post_top_y,
                             RACE_RENDER_GANTRY_POST_PX, post_height,
                             RACE_RENDER_BROWN);

  // Tyre protection around each gantry post
  RaceRender_DrawVerticalPostProtection(
      track, left_post_x, post_top_y, RACE_RENDER_GANTRY_POST_PX, post_height);

  RaceRender_DrawVerticalPostProtection(
      track, right_post_x, post_top_y, RACE_RENDER_GANTRY_POST_PX, post_height);

  // White sign text with black shadow
  text_width = (int16_t)(RaceRender_StringLength(label) * 6U);
  text_x = (int16_t)(beam_x + (beam_width / 2) - (text_width / 2));
  text_y = (int16_t)(beam_y + 1);

  // Main wooden beam connected to the posts
  RaceRender_DrawRectClipped(track, beam_x, beam_y, beam_width,
                             RACE_RENDER_GANTRY_BEAM_PX, RACE_RENDER_BROWN);

  LCD_printString(label, (uint16_t)(text_x + 1), (uint16_t)(text_y + 1),
                  RACE_RENDER_BLACK, 1U);
  LCD_printString(label, (uint16_t)text_x, (uint16_t)text_y, RACE_RENDER_WHITE,
                  1U);
}

static void RaceRender_DrawVerticalGantry(const RaceTrack *track,
                                          RaceTriggerZone zone,
                                          const char *label,
                                          bool text_bottom_to_top,
                                          bool rotate_clockwise) {
  int16_t x = 0;
  int16_t y = 0;
  int16_t beam_x = 0;
  int16_t beam_y = 0;
  int16_t beam_height = 0;
  int16_t top_post_y = 0;
  int16_t bottom_post_y = 0;
  int16_t text_x = 0;

  if ((track == NULL) || (label == NULL)) {
    return;
  }

  x = RaceTrack_WorldToScreenX(track, (float)zone.x);
  y = RaceTrack_WorldToScreenY(track, (float)zone.y);

  beam_x = (int16_t)(x + ((int16_t)zone.width / 2) -
                     (RACE_RENDER_GANTRY_BEAM_PX / 2));

  top_post_y = (int16_t)(y - RACE_RENDER_GANTRY_POST_GAP_PX -
                         RACE_RENDER_GANTRY_POST_PX);
  bottom_post_y = (int16_t)(y + zone.height + RACE_RENDER_GANTRY_POST_GAP_PX);

  // Connected wooden beam from top post to bottom post
  beam_y = top_post_y;
  beam_height = (int16_t)(zone.height + (2 * RACE_RENDER_GANTRY_POST_GAP_PX) +
                          (2 * RACE_RENDER_GANTRY_POST_PX));

  RaceRender_DrawRectClipped(track, beam_x, beam_y, RACE_RENDER_GANTRY_BEAM_PX,
                             beam_height, RACE_RENDER_BROWN);

  // Top post
  RaceRender_DrawRectClipped(track, (int16_t)(beam_x - 5), top_post_y,
                             (int16_t)(RACE_RENDER_GANTRY_BEAM_PX + 10),
                             RACE_RENDER_GANTRY_POST_PX, RACE_RENDER_BROWN);

  // Bottom post
  RaceRender_DrawRectClipped(track, (int16_t)(beam_x - 5), bottom_post_y,
                             (int16_t)(RACE_RENDER_GANTRY_BEAM_PX + 10),
                             RACE_RENDER_GANTRY_POST_PX, RACE_RENDER_BROWN);

  // Tyre protection for the top horizontal post
  // Tyres go ABOVE it, away from the wooden banner
  RaceRender_DrawHorizontalPostProtection(
      track, (int16_t)(beam_x - 5), top_post_y,
      (int16_t)(RACE_RENDER_GANTRY_BEAM_PX + 10), RACE_RENDER_GANTRY_POST_PX,
      true);

  // Tyre protection for the bottom horizontal post
  // Tyres go BELOW it, away from the wooden banner
  RaceRender_DrawHorizontalPostProtection(
      track, (int16_t)(beam_x - 5), bottom_post_y,
      (int16_t)(RACE_RENDER_GANTRY_BEAM_PX + 10), RACE_RENDER_GANTRY_POST_PX,
      false);

  // Centre the rotated text on the wooden beam
  text_x =
      (int16_t)(beam_x +
                ((RACE_RENDER_GANTRY_BEAM_PX - RACE_RENDER_ROTATED_CHAR_W) /
                 2));

  // Black shadow
  RaceRender_DrawVerticalTextRotated(
      track, label, (int16_t)(text_x + 1), (int16_t)(beam_y + 1), beam_height,
      RACE_RENDER_BLACK, text_bottom_to_top, rotate_clockwise);

  // White text
  RaceRender_DrawVerticalTextRotated(track, label, text_x, beam_y, beam_height,
                                     RACE_RENDER_WHITE, text_bottom_to_top,
                                     rotate_clockwise);
}

static void RaceRender_DrawGantries(const RaceTrack *track) {
  RaceTriggerZone start_finish_zone = RaceTrack_GetStartFinishZone();
  RaceTriggerZone cp1_zone = RaceTrack_GetCheckpointZone(1U);
  RaceTriggerZone cp2_zone = RaceTrack_GetCheckpointZone(2U);
  RaceTriggerZone cp3_zone = RaceTrack_GetCheckpointZone(3U);
  RaceTriggerZone joker_in_zone = RaceTrack_GetJokerStartZone();

  if (track == NULL) {
    return;
  }

  RaceRender_DrawCheckeredZone(track, start_finish_zone);

  RaceRender_DrawVerticalGantry(track, start_finish_zone, "START FINISH", true,
                                false);

  RaceRender_DrawHorizontalGantry(track, cp1_zone, "CP1");
  RaceRender_DrawHorizontalGantry(track, cp3_zone, "CP3");

  // CP2
  RaceRender_DrawVerticalGantry(track, cp2_zone, "CP2", false, true);

  RaceRender_DrawHorizontalGantry(track, joker_in_zone, "JOKER IN");
}

static float RaceRender_AbsFloat(float value) {
  if (value < 0.0f) {
    return -value;
  }

  return value;
}

static void RaceRender_DrawSpeedBar(const RaceTrack *track,
                                    const RaceCar *player_car,
                                    const RaceBoostState *boost_state) {
  float speed_abs = 0.0f;
  float speed_ratio = 0.0f;

  int16_t bar_x = 5;
  int16_t bar_y = 214;
  int16_t bar_width = 42;
  int16_t bar_height = 8;
  int16_t boosted_width = 0;
  int16_t fill_width = 0;

  uint8_t fill_colour = RACE_RENDER_BLUE;
  uint32_t now_ms = 0U;
  bool boost_active = false;

  if ((track == NULL) || (player_car == NULL) || (boost_state == NULL)) {
    return;
  }

  now_ms = HAL_GetTick();
  boost_active = RaceBoost_IsActive(boost_state, now_ms);

  speed_abs = player_car->speed;

  if (speed_abs < 0.0f) {
    speed_abs = -speed_abs;
  }

  speed_ratio = speed_abs / RACE_CAR_MAX_SPEED;

  if (speed_ratio > 1.0f) {
    speed_ratio = 1.0f;
  }

  if (boost_active == true) {
    // Show boost as 125% on the bar.
    speed_ratio = 1.25f;
    fill_colour = RACE_RENDER_RED;
  } else {
    fill_colour = RACE_RENDER_BLUE;
  }

  fill_width = (int16_t)(speed_ratio * (float)bar_width);
  boosted_width = (int16_t)((float)bar_width * 1.25f);

  LCD_printString("SPD", (uint16_t)bar_x, (uint16_t)(bar_y - 10),
                  RACE_RENDER_WHITE, 1);

  // Black backing behind the full possible boosted width
  RaceRender_DrawRectClipped(track, (int16_t)(bar_x - 1), (int16_t)(bar_y - 1),
                             (int16_t)(boosted_width + 2),
                             (int16_t)(bar_height + 2), RACE_RENDER_BLACK);

  // Normal 100% speed bar outline
  RaceRender_DrawRectClipped(track, bar_x, bar_y, bar_width, 1,
                             RACE_RENDER_WHITE);
  RaceRender_DrawRectClipped(track, bar_x, (int16_t)(bar_y + bar_height - 1),
                             bar_width, 1, RACE_RENDER_WHITE);
  RaceRender_DrawRectClipped(track, bar_x, bar_y, 1, bar_height,
                             RACE_RENDER_WHITE);
  RaceRender_DrawRectClipped(track, (int16_t)(bar_x + bar_width - 1), bar_y, 1,
                             bar_height, RACE_RENDER_WHITE);

  // Empty inside of the normal bar
  RaceRender_DrawRectClipped(track, (int16_t)(bar_x + 1), (int16_t)(bar_y + 1),
                             (int16_t)(bar_width - 2),
                             (int16_t)(bar_height - 2), RACE_RENDER_GREY);

  // Fill. During boost this is allowed to go beyond the white outline
  if (fill_width > 0) {
    RaceRender_DrawRectClipped(track, (int16_t)(bar_x + 1),
                               (int16_t)(bar_y + 1), fill_width,
                               (int16_t)(bar_height - 2), fill_colour);
  }

  // 125% boost end marker
  if (boost_active == true) {
    RaceRender_DrawRectClipped(track, (int16_t)(bar_x + boosted_width), bar_y,
                               1, bar_height, RACE_RENDER_RED);
  }
}

static void RaceRender_FormatLapTime(uint32_t lap_ms, char *buffer,
                                     uint16_t buffer_size) {
  uint32_t seconds = 0U;
  uint32_t hundredths = 0U;

  if ((buffer == NULL) || (buffer_size == 0U)) {
    return;
  }

  if (lap_ms == 0U) {
    snprintf(buffer, buffer_size, "--.--s");
    return;
  }

  seconds = lap_ms / 1000U;
  hundredths = (lap_ms % 1000U) / 10U;

  snprintf(buffer, buffer_size, "%lu.%02lus", (unsigned long)seconds,
           (unsigned long)hundredths);
}

static void RaceRender_FormatTotalTime(uint32_t time_ms, char *buffer,
                                       uint16_t buffer_size) {
  uint32_t total_seconds = 0U;
  uint32_t minutes = 0U;
  uint32_t seconds = 0U;
  uint32_t hundredths = 0U;

  if ((buffer == NULL) || (buffer_size == 0U)) {
    return;
  }

  total_seconds = time_ms / 1000U;
  minutes = total_seconds / 60U;
  seconds = total_seconds % 60U;
  hundredths = (time_ms % 1000U) / 10U;

  snprintf(buffer, buffer_size, "%lu:%02lu.%02lu", (unsigned long)minutes,
           (unsigned long)seconds, (unsigned long)hundredths);
}

static void RaceRender_DrawHud(const RaceTrack *track,
                               const RaceCar *player_car, uint32_t best_lap_ms,
                               const RaceBoostState *boost_state) {
  char line1[32];
  char line2[32];
  char best_lap_text[16];
  char best_line[24];

  uint8_t next_checkpoint = 0U;
  uint32_t elapsed_ms = 0U;
  uint32_t elapsed_seconds = 0U;
  uint32_t elapsed_hundredths = 0U;

  if (track == NULL) {
    return;
  }

  next_checkpoint = RaceTrack_GetNextCheckpoint(track);
  elapsed_ms = RaceTrack_GetElapsedMs(track);
  elapsed_seconds = elapsed_ms / 1000U;
  elapsed_hundredths = (elapsed_ms % 1000U) / 10U;

  if (RaceTrack_IsFinished(track)) {
    LCD_printString("FINISHED", 5, 4, RACE_RENDER_WHITE, 1);
    LCD_printString("DOWN RESET", 150, 4, RACE_RENDER_WHITE, 1);
    return;
  }

  if (next_checkpoint == 0U) {
    snprintf(line1, sizeof(line1), "LAP %u/%u TARGET SF",
             RaceTrack_GetCurrentLap(track), RaceTrack_GetTotalLaps(track));
  } else {
    snprintf(line1, sizeof(line1), "LAP %u/%u TARGET CP%u",
             RaceTrack_GetCurrentLap(track), RaceTrack_GetTotalLaps(track),
             next_checkpoint);
  }

  snprintf(line2, sizeof(line2), "TIME %lu.%02lus",
           (unsigned long)elapsed_seconds, (unsigned long)elapsed_hundredths);

  RaceRender_FormatLapTime(best_lap_ms, best_lap_text, sizeof(best_lap_text));
  snprintf(best_line, sizeof(best_line), "BEST %s", best_lap_text);

  LCD_printString(line1, 5, 4, RACE_RENDER_WHITE, 1);
  LCD_printString(line2, 5, 14, RACE_RENDER_WHITE, 1);
  LCD_printString(best_line, 5, 24, RACE_RENDER_WHITE, 1);
  LCD_printString("B1 EXIT", 185, 4, RACE_RENDER_WHITE, 1);

  RaceRender_DrawSpeedBar(track, player_car, boost_state);
}

static RaceRenderDirection
RaceRender_GetPlayerDirection(const RaceCar *player_car) {
  float abs_vx = 0.0f;
  float abs_vy = 0.0f;

  if (player_car == NULL) {
    return g_last_player_direction;
  }

  abs_vx = RaceRender_AbsFloat(player_car->vx);
  abs_vy = RaceRender_AbsFloat(player_car->vy);

  // Keep the previous facing direction when almost stopped
  // This prevents the car flickering between directions at low speed
  if ((abs_vx < 0.12f) && (abs_vy < 0.12f)) {
    return g_last_player_direction;
  }

  if (abs_vx > abs_vy) {
    if (player_car->vx > 0.0f) {
      g_last_player_direction = RACE_RENDER_DIRECTION_EAST;
    } else {
      g_last_player_direction = RACE_RENDER_DIRECTION_WEST;
    }
  } else {
    if (player_car->vy > 0.0f) {
      g_last_player_direction = RACE_RENDER_DIRECTION_SOUTH;
    } else {
      g_last_player_direction = RACE_RENDER_DIRECTION_NORTH;
    }
  }

  return g_last_player_direction;
}

static uint16_t RaceRender_GetPlayerSpriteWidth(RaceRenderDirection direction) {
  if ((direction == RACE_RENDER_DIRECTION_EAST) ||
      (direction == RACE_RENDER_DIRECTION_WEST)) {
    return RACE_PLAYER_SPRITE_ROWS;
  }

  return RACE_PLAYER_SPRITE_COLS;
}

static uint16_t
RaceRender_GetPlayerSpriteHeight(RaceRenderDirection direction) {
  if ((direction == RACE_RENDER_DIRECTION_EAST) ||
      (direction == RACE_RENDER_DIRECTION_WEST)) {
    return RACE_PLAYER_SPRITE_COLS;
  }

  return RACE_PLAYER_SPRITE_ROWS;
}

static void RaceRender_DrawPlayer(const RaceTrack *track,
                                  const RaceCar *player_car) {
  int16_t screen_x = 0;
  int16_t screen_y = 0;
  int16_t sprite_x = 0;
  int16_t sprite_y = 0;
  uint16_t sprite_width = 0U;
  uint16_t sprite_height = 0U;
  RaceRenderDirection direction = RACE_RENDER_DIRECTION_NORTH;

  if ((track == NULL) || (player_car == NULL)) {
    return;
  }

  screen_x = RaceTrack_WorldToScreenX(track, player_car->x);
  screen_y = RaceTrack_WorldToScreenY(track, player_car->y);

  direction = RaceRender_GetPlayerDirection(player_car);
  sprite_width = RaceRender_GetPlayerSpriteWidth(direction);
  sprite_height = RaceRender_GetPlayerSpriteHeight(direction);

  // Centre the visual sprite around the car hitbox
  // The hitbox stays unchanged, only the drawing rotates
  sprite_x =
      (int16_t)(screen_x +
                (((int16_t)player_car->width - (int16_t)sprite_width) / 2));

  sprite_y =
      (int16_t)(screen_y +
                (((int16_t)player_car->height - (int16_t)sprite_height) / 2));

  RaceRender_DrawSpriteClipped(track, sprite_x, sprite_y,
                               RACE_PLAYER_SPRITE_ROWS, RACE_PLAYER_SPRITE_COLS,
                               g_player_car_sprite, direction);
}

static void RaceRender_DrawFinishOverlay(const RaceTrack *track,
                                         uint32_t best_lap_ms) {
  char total_time_text[16];
  char best_lap_text[16];
  char total_line[28];
  char best_line[28];

  uint32_t elapsed_ms = 0U;

  if (track == NULL) {
    return;
  }

  elapsed_ms = RaceTrack_GetElapsedMs(track);

  RaceRender_FormatTotalTime(elapsed_ms, total_time_text,
                             sizeof(total_time_text));
  RaceRender_FormatLapTime(best_lap_ms, best_lap_text, sizeof(best_lap_text));

  snprintf(total_line, sizeof(total_line), "TOTAL %s", total_time_text);
  snprintf(best_line, sizeof(best_line), "BEST LAP %s", best_lap_text);

  // Main black panel
  RaceRender_DrawRectClipped(track, 24, 42, 192, 150, RACE_RENDER_BLACK);

  // White border around result panel
  RaceRender_DrawRectClipped(track, 24, 42, 192, 2, RACE_RENDER_WHITE);
  RaceRender_DrawRectClipped(track, 24, 190, 192, 2, RACE_RENDER_WHITE);
  RaceRender_DrawRectClipped(track, 24, 42, 2, 150, RACE_RENDER_WHITE);
  RaceRender_DrawRectClipped(track, 214, 42, 2, 150, RACE_RENDER_WHITE);

  // Title
  LCD_printString("RACE", 78, 56, RACE_RENDER_WHITE, 2U);
  LCD_printString("COMPLETE", 54, 78, RACE_RENDER_WHITE, 2U);

  // Results
  LCD_printString(total_line, 58, 110, RACE_RENDER_WHITE, 1U);
  LCD_printString(best_line, 52, 126, RACE_RENDER_WHITE, 1U);

  // Joker status
  if (RaceTrack_IsJokerLapRequired(track) == true) {
    if (RaceTrack_IsJokerLapDone(track) == true) {
      LCD_printString("JOKER DONE", 82, 142, RACE_RENDER_WHITE, 1U);
    } else {
      LCD_printString("JOKER MISSED", 78, 142, RACE_RENDER_WHITE, 1U);
    }
  }

  // Controls
  LCD_printString("DOWN TO RESET", 66, 162, RACE_RENDER_WHITE, 1U);
  LCD_printString("B1 EXIT", 92, 176, RACE_RENDER_WHITE, 1U);
}

static void RaceRender_DrawFlashMessage(const RaceTrack *track,
                                        const char *message) {
  uint16_t text_length = 0U;
  int16_t text_width = 0;
  int16_t box_width = 0;
  int16_t box_x = 0;
  int16_t text_x = 0;

  if ((track == NULL) || (message == NULL) || (message[0] == '\0')) {
    return;
  }

  text_length = RaceRender_StringLength(message);
  text_width = (int16_t)(text_length * 6U);

  box_width = (int16_t)(text_width + 20);

  if (box_width < 90) {
    box_width = 90;
  }

  if (box_width > 190) {
    box_width = 190;
  }

  box_x = (int16_t)((240 - box_width) / 2);
  text_x = (int16_t)(box_x + ((box_width - text_width) / 2));

  RaceRender_DrawRectClipped(track, box_x, 34, box_width, 26,
                             RACE_RENDER_BLACK);

  RaceRender_DrawRectClipped(track, box_x, 34, box_width, 2, RACE_RENDER_WHITE);
  RaceRender_DrawRectClipped(track, box_x, 58, box_width, 2, RACE_RENDER_WHITE);
  RaceRender_DrawRectClipped(track, box_x, 34, 2, 26, RACE_RENDER_WHITE);
  RaceRender_DrawRectClipped(track, (int16_t)(box_x + box_width - 2), 34, 2, 26,
                             RACE_RENDER_WHITE);

  LCD_printString(message, (uint16_t)text_x, 43, RACE_RENDER_WHITE, 1U);
}

static void RaceRender_DrawCrashOverlay(const RaceTrack *track) {
  if (track == NULL) {
    return;
  }

  // Black box clears the busy track behind the message
  RaceRender_DrawRectClipped(track, 42, 72, 156, 82, RACE_RENDER_BLACK);

  // White border around the crash message panel
  RaceRender_DrawRectClipped(track, 42, 72, 156, 2, RACE_RENDER_WHITE);
  RaceRender_DrawRectClipped(track, 42, 152, 156, 2, RACE_RENDER_WHITE);
  RaceRender_DrawRectClipped(track, 42, 72, 2, 82, RACE_RENDER_WHITE);
  RaceRender_DrawRectClipped(track, 196, 72, 2, 82, RACE_RENDER_WHITE);

  LCD_printString("CRASHED!", 66, 88, RACE_RENDER_WHITE, 2U);
  LCD_printString("DOWN TO RESET", 64, 118, RACE_RENDER_WHITE, 1U);
  LCD_printString("B1 EXIT", 92, 134, RACE_RENDER_WHITE, 1U);
}

static const uint8_t boost_pickup_sprite[10][10] = {
    {255, 255, 255, 0, 0, 0, 255, 255, 255, 255},
    {255, 255, 0, 1, 5, 5, 0, 255, 255, 255},
    {255, 0, 1, 5, 5, 0, 255, 255, 255, 255},
    {255, 0, 5, 5, 0, 255, 255, 255, 255, 255},
    {255, 255, 0, 5, 5, 0, 0, 255, 255, 255},
    {255, 255, 255, 0, 5, 5, 5, 0, 255, 255},
    {255, 255, 255, 0, 5, 5, 0, 255, 255, 255},
    {255, 255, 0, 5, 5, 0, 255, 255, 255, 255},
    {255, 0, 13, 0, 0, 255, 255, 255, 255, 255},
    {255, 255, 0, 255, 255, 255, 255, 255, 255, 255},
};

static void RaceRender_DrawBoostPickups(const RaceTrack *track,
                                        const RaceBoostState *boost_state) {
  const RaceBoostPickup *pickups = NULL;
  uint8_t i = 0U;

  if ((track == NULL) || (boost_state == NULL)) {
    return;
  }

  pickups = RaceBoost_GetPickups(boost_state);

  if (pickups == NULL) {
    return;
  }

  for (i = 0U; i < RACE_BOOST_PICKUP_COUNT; i++) {
    int16_t screen_x = 0;
    int16_t screen_y = 0;

    if (pickups[i].collected == true) {
      continue;
    }

    screen_x = (int16_t)(RaceTrack_WorldToScreenX(track, pickups[i].x) - 5);
    screen_y = (int16_t)(RaceTrack_WorldToScreenY(track, pickups[i].y) - 5);

    // Do not cast negative values to uint16_t until after this check.
    if ((screen_x < -10) || (screen_x > (int16_t)track->screen_width) ||
        (screen_y < -10) || (screen_y > (int16_t)track->screen_height)) {
      continue;
    }

    if ((screen_x < 0) || (screen_y < 0)) {
      continue;
    }

    LCD_Draw_Sprite((uint16_t)screen_x, (uint16_t)screen_y, 10, 10,
                    (const uint8_t *)boost_pickup_sprite);
  }
}

static void RaceRender_DrawBoostHud(const RaceBoostState *boost_state) {
  char boost_text[16];
  uint32_t now_ms = 0U;

  if (boost_state == NULL) {
    return;
  }

  now_ms = HAL_GetTick();

  snprintf(boost_text, sizeof(boost_text), "BOOST x%u",
           RaceBoost_GetCharges(boost_state));

  LCD_printString(boost_text, 184, 228, RACE_RENDER_WHITE, 1);

  if (RaceBoost_IsActive(boost_state, now_ms) == true) {
    LCD_printString("BOOST ON", 184, 216, RACE_RENDER_YELLOW, 1);
  } else if (RaceBoost_ShouldShowMessage(boost_state, now_ms) == true) {
    LCD_printString("BOOST +1", 184, 216, RACE_RENDER_YELLOW, 1);
  }
}

#define RACE_SKID_MARK_COUNT 24U
#define RACE_SKID_MARK_LIFE_MS 1400U
#define RACE_SKID_MARK_INTERVAL_MS 55U

typedef enum {
  RACE_SKID_DIR_NORTH = 0,
  RACE_SKID_DIR_EAST,
  RACE_SKID_DIR_SOUTH,
  RACE_SKID_DIR_WEST
} RaceSkidDirection;

typedef struct {
  float x;
  float y;
  RaceSkidDirection direction;
  uint32_t expires_at_ms;
  bool active;
} RaceSkidMark;

static RaceSkidMark g_skid_marks[RACE_SKID_MARK_COUNT];
static uint8_t g_next_skid_mark_index = 0U;
static uint32_t g_last_skid_mark_ms = 0U;

static float RaceRender_SkidAbsFloat(float value) {
  if (value < 0.0f) {
    return -value;
  }

  return value;
}

static bool RaceRender_ShouldCreateSkidMark(const RaceCar *player_car,
                                            const RaceBoostState *boost_state,
                                            const RaceState *race_state,
                                            uint32_t now_ms) {
  static bool previous_speed_valid = false;
  static float previous_speed_abs = 0.0f;

  float speed_abs = 0.0f;
  float speed_ratio = 0.0f;
  bool boost_active = false;
  bool accelerating = false;

  if (player_car == NULL) {
    return false;
  }

  if ((now_ms - g_last_skid_mark_ms) < RACE_SKID_MARK_INTERVAL_MS) {
    return false;
  }

  speed_abs = RaceRender_SkidAbsFloat(player_car->speed);
  speed_ratio = speed_abs / RACE_CAR_MAX_SPEED;

  if (speed_ratio > 1.0f) {
    speed_ratio = 1.0f;
  }

  if (boost_state != NULL) {
    boost_active = RaceBoost_IsActive(boost_state, now_ms);
  }

  if (previous_speed_valid == false) {
    previous_speed_abs = speed_abs;
    previous_speed_valid = true;
    return false;
  }

  accelerating = (speed_abs > (previous_speed_abs + 0.04f));
  previous_speed_abs = speed_abs;

  // Crash: leave marks immediately
  if ((race_state != NULL) && (RaceState_IsCrashed(race_state) == true)) {
    return true;
  }

  // Boost: always allow skid marks during boost
  if ((boost_active == true) && (speed_abs > 1.0f)) {
    return true;
  }

  // Launch wheelspin / low-speed acceleration:
  // only from 0% to 50% of normal max speed
  if ((accelerating == true) && (speed_ratio <= 0.50f) && (speed_abs > 0.35f)) {
    return true;
  }

  return false;
}

static RaceSkidDirection
RaceRender_GetSkidDirectionFromMovement(const RaceCar *player_car) {
  static bool previous_position_valid = false;
  static float previous_x = 0.0f;
  static float previous_y = 0.0f;
  static RaceSkidDirection last_direction = RACE_SKID_DIR_NORTH;

  float dx = 0.0f;
  float dy = 0.0f;
  float abs_dx = 0.0f;
  float abs_dy = 0.0f;

  if (player_car == NULL) {
    return last_direction;
  }

  if (previous_position_valid == false) {
    previous_x = player_car->x;
    previous_y = player_car->y;
    previous_position_valid = true;
    return last_direction;
  }

  dx = player_car->x - previous_x;
  dy = player_car->y - previous_y;

  previous_x = player_car->x;
  previous_y = player_car->y;

  abs_dx = RaceRender_SkidAbsFloat(dx);
  abs_dy = RaceRender_SkidAbsFloat(dy);

  // Keep previous direction if movement is too small
  if ((abs_dx < 0.15f) && (abs_dy < 0.15f)) {
    return last_direction;
  }

  if (abs_dx > abs_dy) {
    if (dx > 0.0f) {
      last_direction = RACE_SKID_DIR_EAST;
    } else {
      last_direction = RACE_SKID_DIR_WEST;
    }
  } else {
    if (dy > 0.0f) {
      last_direction = RACE_SKID_DIR_SOUTH;
    } else {
      last_direction = RACE_SKID_DIR_NORTH;
    }
  }

  return last_direction;
}

static void RaceRender_AddSingleSkidMark(float x, float y,
                                         RaceSkidDirection direction,
                                         uint32_t now_ms) {
  g_skid_marks[g_next_skid_mark_index].x = x;
  g_skid_marks[g_next_skid_mark_index].y = y;
  g_skid_marks[g_next_skid_mark_index].direction = direction;
  g_skid_marks[g_next_skid_mark_index].expires_at_ms =
      now_ms + RACE_SKID_MARK_LIFE_MS;
  g_skid_marks[g_next_skid_mark_index].active = true;

  g_next_skid_mark_index++;

  if (g_next_skid_mark_index >= RACE_SKID_MARK_COUNT) {
    g_next_skid_mark_index = 0U;
  }
}

static void RaceRender_AddPlayerSkidMarks(const RaceCar *player_car,
                                          uint32_t now_ms) {
  RaceSkidDirection direction = RACE_SKID_DIR_NORTH;

  float mark_1_x = 0.0f;
  float mark_1_y = 0.0f;
  float mark_2_x = 0.0f;
  float mark_2_y = 0.0f;

  if (player_car == NULL) {
    return;
  }

  direction = RaceRender_GetSkidDirectionFromMovement(player_car);

  switch (direction) {
  case RACE_SKID_DIR_NORTH:
    // Car travelling up. Rear tyres are at bottom of car
    mark_1_x = player_car->x + 2.0f;
    mark_2_x = player_car->x + (float)player_car->width - 4.0f;
    mark_1_y = player_car->y + (float)player_car->height - 2.0f;
    mark_2_y = mark_1_y;
    break;

  case RACE_SKID_DIR_SOUTH:
    // Car travelling down. Rear tyres are at top of car
    mark_1_x = player_car->x + 2.0f;
    mark_2_x = player_car->x + (float)player_car->width - 4.0f;
    mark_1_y = player_car->y + 1.0f;
    mark_2_y = mark_1_y;
    break;

  case RACE_SKID_DIR_EAST:
    // Car travelling right. Rear tyres are on left side of car
    mark_1_x = player_car->x + 1.0f;
    mark_2_x = mark_1_x;
    mark_1_y = player_car->y + 3.0f;
    mark_2_y = player_car->y + (float)player_car->height - 5.0f;
    break;

  case RACE_SKID_DIR_WEST:
  default:
    // Car travelling left. Rear tyres are on right side of car
    mark_1_x = player_car->x + (float)player_car->width - 2.0f;
    mark_2_x = mark_1_x;
    mark_1_y = player_car->y + 3.0f;
    mark_2_y = player_car->y + (float)player_car->height - 5.0f;
    break;
  }

  RaceRender_AddSingleSkidMark(mark_1_x, mark_1_y, direction, now_ms);
  RaceRender_AddSingleSkidMark(mark_2_x, mark_2_y, direction, now_ms);

  g_last_skid_mark_ms = now_ms;
}

static void RaceRender_UpdateSkidMarks(const RaceCar *player_car,
                                       const RaceBoostState *boost_state,
                                       const RaceState *race_state) {
  uint32_t now_ms = HAL_GetTick();

  if (RaceRender_ShouldCreateSkidMark(player_car, boost_state, race_state,
                                      now_ms) == true) {
    RaceRender_AddPlayerSkidMarks(player_car, now_ms);
  }
}

static void RaceRender_DrawSkidMarks(const RaceTrack *track) {
  uint8_t i = 0U;
  uint32_t now_ms = HAL_GetTick();

  if (track == NULL) {
    return;
  }

  for (i = 0U; i < RACE_SKID_MARK_COUNT; i++) {
    int16_t screen_x = 0;
    int16_t screen_y = 0;
    uint8_t colour = RACE_RENDER_BLACK;

    if (g_skid_marks[i].active == false) {
      continue;
    }

    if (now_ms >= g_skid_marks[i].expires_at_ms) {
      g_skid_marks[i].active = false;
      continue;
    }

    // Fade older marks from black to grey near the end of their lifetime
    if ((g_skid_marks[i].expires_at_ms - now_ms) < 450U) {
      colour = RACE_RENDER_GREY;
    }

    screen_x = RaceTrack_WorldToScreenX(track, g_skid_marks[i].x);
    screen_y = RaceTrack_WorldToScreenY(track, g_skid_marks[i].y);

    if ((screen_x < -4) || (screen_x > (int16_t)track->screen_width) ||
        (screen_y < -8) || (screen_y > (int16_t)track->screen_height)) {
      continue;
    }

    switch (g_skid_marks[i].direction) {
    case RACE_SKID_DIR_EAST:
    case RACE_SKID_DIR_WEST:
      // Horizontal mark for cars travelling east/west
      RaceRender_DrawRectClipped(track, screen_x, screen_y, 7, 2, colour);
      break;

    case RACE_SKID_DIR_NORTH:
    case RACE_SKID_DIR_SOUTH:
    default:
      // Vertical mark for cars travelling north/south
      RaceRender_DrawRectClipped(track, screen_x, screen_y, 2, 7, colour);
      break;
    }
  }
}

// Help overlay

#define RACE_HELP_OVERLAY_SHOW_MS 5200U

static uint32_t g_race_help_overlay_started_ms = 0U;

static void RaceRender_DrawControlHelpOverlay(const RaceTrack *track) {
  uint32_t now_ms = HAL_GetTick();

  if (track == NULL) {
    return;
  }

  // Start the help overlay timer the first time the renderer runs
  if (g_race_help_overlay_started_ms == 0U) {
    g_race_help_overlay_started_ms = now_ms;
  }

  // Only show the controls briefly at the start
  if ((now_ms - g_race_help_overlay_started_ms) > RACE_HELP_OVERLAY_SHOW_MS) {
    return;
  }

  // Black panel near the bottom of the screen
  RaceRender_DrawRectClipped(track, 5, 177, 230, 56, RACE_RENDER_BLACK);

  // White border
  RaceRender_DrawRectClipped(track, 5, 177, 230, 1, RACE_RENDER_WHITE);
  RaceRender_DrawRectClipped(track, 5, 232, 230, 1, RACE_RENDER_WHITE);
  RaceRender_DrawRectClipped(track, 5, 177, 1, 56, RACE_RENDER_WHITE);
  RaceRender_DrawRectClipped(track, 234, 177, 1, 56, RACE_RENDER_WHITE);

  LCD_printString("CONTROLS", 92, 183, RACE_RENDER_YELLOW, 1);
  LCD_printString("UP ACCEL   DOWN BRAKE/RESET", 13, 196, RACE_RENDER_WHITE, 1);
  LCD_printString("LEFT/RIGHT STEER", 13, 208, RACE_RENDER_WHITE, 1);
  LCD_printString("BTN2 BOOST   B1 EXIT", 13, 220, RACE_RENDER_WHITE, 1);
}

// Animated grass-side curb flags
// This system scans the visible tile map and places flags only on grass tiles
// that are directly beside road/curb tiles. The flags use world coordinates,
// then convert to screen coordinates, so they stay fixed to the circuit

#define RACE_FLAG_SPRITE_W 6U
#define RACE_FLAG_SPRITE_H 8U

#define RACE_FLAG_SCALE 3U
#define RACE_FLAG_DENSITY_HASH 4

// 255 = transparent
// 13 = grey pole, 2 = red, 1 = white, 4 = blue
static const uint8_t
    race_flag_sprite_frame_0[RACE_FLAG_SPRITE_H][RACE_FLAG_SPRITE_W] = {
        {13, 255, 255, 255, 255, 255}, {13, 2, 2, 1, 255, 255},
        {13, 2, 1, 4, 255, 255},       {13, 1, 4, 4, 255, 255},
        {13, 255, 255, 255, 255, 255}, {13, 255, 255, 255, 255, 255},
        {13, 255, 255, 255, 255, 255}, {13, 255, 255, 255, 255, 255},
};

static const uint8_t
    race_flag_sprite_frame_1[RACE_FLAG_SPRITE_H][RACE_FLAG_SPRITE_W] = {
        {13, 255, 255, 255, 255, 255}, {13, 255, 2, 2, 1, 255},
        {13, 255, 2, 1, 4, 255},       {13, 255, 1, 4, 4, 255},
        {13, 255, 255, 255, 255, 255}, {13, 255, 255, 255, 255, 255},
        {13, 255, 255, 255, 255, 255}, {13, 255, 255, 255, 255, 255},
};

static bool RaceRender_IsFlagGrassTile(RaceTileType tile) {
  return ((tile == RACE_TILE_GRASS) || (tile == RACE_TILE_GRASS_DARK));
}

static bool RaceRender_IsFlagTrackEdgeTile(RaceTileType tile) {
  return ((tile == RACE_TILE_ROAD) || (tile == RACE_TILE_ROAD_DARK) ||
          (tile == RACE_TILE_START) || (tile == RACE_TILE_CHECKPOINT) ||
          (tile == RACE_TILE_JOKER) || (tile == RACE_TILE_CURB_RED) ||
          (tile == RACE_TILE_CURB_WHITE));
}

static bool RaceRender_TileTouchesTrackEdge(const RaceTrack *track, int tile_x,
                                            int tile_y) {
  RaceTileType north_tile = RACE_TILE_GRASS;
  RaceTileType south_tile = RACE_TILE_GRASS;
  RaceTileType west_tile = RACE_TILE_GRASS;
  RaceTileType east_tile = RACE_TILE_GRASS;

  if (track == NULL) {
    return false;
  }

  north_tile = RaceTrack_GetTileAt(track, tile_x, tile_y - 1);
  south_tile = RaceTrack_GetTileAt(track, tile_x, tile_y + 1);
  west_tile = RaceTrack_GetTileAt(track, tile_x - 1, tile_y);
  east_tile = RaceTrack_GetTileAt(track, tile_x + 1, tile_y);

  return ((RaceRender_IsFlagTrackEdgeTile(north_tile) == true) ||
          (RaceRender_IsFlagTrackEdgeTile(south_tile) == true) ||
          (RaceRender_IsFlagTrackEdgeTile(west_tile) == true) ||
          (RaceRender_IsFlagTrackEdgeTile(east_tile) == true));
}

static bool RaceRender_ShouldDrawFlagOnTile(int tile_x, int tile_y) {
  // Deterministic hash: flags stay fixed in world-space and do not flicker
  // Lower RACE_FLAG_DENSITY_HASH gives more flags
  return ((((tile_x * 17) + (tile_y * 11)) % RACE_FLAG_DENSITY_HASH) == 0);
}

static void RaceRender_DrawFlagSpriteAtWorld(const RaceTrack *track,
                                             float world_x, float world_y,
                                             uint8_t frame) {
  int16_t screen_x = 0;
  int16_t screen_y = 0;

  const int16_t flag_draw_w = (int16_t)(RACE_FLAG_SPRITE_W * RACE_FLAG_SCALE);
  const int16_t flag_draw_h = (int16_t)(RACE_FLAG_SPRITE_H * RACE_FLAG_SCALE);

  const uint8_t *flag_sprite = NULL;

  if (track == NULL) {
    return;
  }

  screen_x = RaceTrack_WorldToScreenX(track, world_x);
  screen_y = RaceTrack_WorldToScreenY(track, world_y);

  // Centre the larger flag over the chosen grass tile
  screen_x = (int16_t)(screen_x - (flag_draw_w / 2));
  screen_y = (int16_t)(screen_y - (flag_draw_h / 2));

  if ((screen_x < 0) ||
      (screen_x > (int16_t)(RACE_SCREEN_WIDTH - flag_draw_w))) {
    return;
  }

  if ((screen_y < 0) ||
      (screen_y > (int16_t)(RACE_SCREEN_HEIGHT - flag_draw_h))) {
    return;
  }

  if (frame == 0U) {
    flag_sprite = &race_flag_sprite_frame_0[0][0];
  } else {
    flag_sprite = &race_flag_sprite_frame_1[0][0];
  }

  // Use the scaled sprite function
  LCD_Draw_Sprite_Scaled((uint16_t)screen_x, (uint16_t)screen_y,
                         RACE_FLAG_SPRITE_H, RACE_FLAG_SPRITE_W, flag_sprite,
                         RACE_FLAG_SCALE);
}

static void RaceRender_DrawAnimatedFlags(const RaceTrack *track) {
  int tile_left = 0;
  int tile_right = 0;
  int tile_top = 0;
  int tile_bottom = 0;
  int tile_x = 0;
  int tile_y = 0;
  uint8_t frame = 0U;

  if (track == NULL) {
    return;
  }

  frame = (uint8_t)((HAL_GetTick() / 300U) % 2U);

  // Scan visible tiles only
  tile_left = (int)(track->camera_x / (float)RACE_TILE_SIZE) - 2;
  tile_top = (int)(track->camera_y / (float)RACE_TILE_SIZE) - 2;

  tile_right = tile_left + ((int)track->screen_width / (int)RACE_TILE_SIZE) + 5;
  tile_bottom =
      tile_top + ((int)track->screen_height / (int)RACE_TILE_SIZE) + 5;

  if (tile_left < 0) {
    tile_left = 0;
  }

  if (tile_top < 0) {
    tile_top = 0;
  }

  if (tile_right >= (int)RACE_WORLD_TILES_X) {
    tile_right = (int)RACE_WORLD_TILES_X - 1;
  }

  if (tile_bottom >= (int)RACE_WORLD_TILES_Y) {
    tile_bottom = (int)RACE_WORLD_TILES_Y - 1;
  }

  for (tile_y = tile_top; tile_y <= tile_bottom; tile_y++) {
    for (tile_x = tile_left; tile_x <= tile_right; tile_x++) {
      RaceTileType tile = RACE_TILE_GRASS;
      float world_x = 0.0f;
      float world_y = 0.0f;

      tile = RaceTrack_GetTileAt(track, tile_x, tile_y);

      // Flags must be drawn on grass only
      if (RaceRender_IsFlagGrassTile(tile) == false) {
        continue;
      }

      // The grass tile must be directly beside road/curb
      if (RaceRender_TileTouchesTrackEdge(track, tile_x, tile_y) == false) {
        continue;
      }

      // Control how many flags are drawn
      if (RaceRender_ShouldDrawFlagOnTile(tile_x, tile_y) == false) {
        continue;
      }

      world_x = (float)(tile_x * (int)RACE_TILE_SIZE) +
                ((float)RACE_TILE_SIZE * 0.5f);
      world_y = (float)(tile_y * (int)RACE_TILE_SIZE) +
                ((float)RACE_TILE_SIZE * 0.5f);

      RaceRender_DrawFlagSpriteAtWorld(track, world_x, world_y, frame);
    }
  }
}

void RaceRender_DrawFrame(const RaceTrack *track, const RaceCar *player_car,
                          const RaceCamera *camera, const RaceState *race_state,
                          const char *flash_message, uint32_t best_lap_ms,
                          const RaceBoostState *boost_state) {
  (void)camera;

  if ((track == NULL) || (player_car == NULL)) {
    return;
  }

  LCD_Fill_Buffer(RACE_RENDER_BLACK);

  RaceRender_DrawWorldTiles(track);
  RaceRender_DrawTrackEdgeOverlay(track);
  RaceRender_DrawAnimatedFlags(track);
  RaceRender_DrawStartGridBoxes(track);
  RaceRender_DrawGantries(track);

  RaceRender_UpdateSkidMarks(player_car, boost_state, race_state);
  RaceRender_DrawSkidMarks(track);

  RaceRender_DrawBoostPickups(track, boost_state);

  RaceRender_DrawPlayer(track, player_car);
  RaceRender_DrawHud(track, player_car, best_lap_ms, boost_state);
  RaceRender_DrawBoostHud(boost_state);

  if ((RaceState_IsCrashed(race_state) == false) &&
      (RaceTrack_IsFinished(track) == false)) {
    RaceRender_DrawFlashMessage(track, flash_message);
  }

  if ((flash_message != NULL) && (flash_message[0] != '\0')) {
    RaceRender_DrawFlashMessage(track, flash_message);
  }

  if (RaceState_IsCrashed(race_state)) {
    RaceRender_DrawCrashOverlay(track);
  } else if (RaceTrack_IsFinished(track)) {
    RaceRender_DrawFinishOverlay(track, best_lap_ms);
  }
  RaceRender_DrawControlHelpOverlay(track);

  LCD_Refresh(&cfg0);
}