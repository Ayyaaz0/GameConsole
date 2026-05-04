#include "race_render.h"

#include "../config/race_config.h"
#include "LCD.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

extern ST7789V2_cfg_t cfg0;

static uint8_t RaceRender_GetTileColour(RaceTileType tile) {
  switch (tile) {
  case RACE_TILE_GRASS:
    return RACE_COLOUR_GRASS;

  case RACE_TILE_ROAD:
    return RACE_COLOUR_ROAD;

  case RACE_TILE_ROAD_EDGE:
    return RACE_COLOUR_ROAD_DARK;

  case RACE_TILE_CURB_RED:
    return RACE_COLOUR_CURB_RED;

  case RACE_TILE_CURB_WHITE:
    return RACE_COLOUR_CURB_WHITE;

  case RACE_TILE_START:
    return RACE_COLOUR_ROAD;

  case RACE_TILE_CHECKPOINT:
    return RACE_COLOUR_ROAD;

  case RACE_TILE_JOKER:
    return RACE_COLOUR_ROAD;


  case RACE_TILE_TYRE:
    return RACE_COLOUR_TYRE;

  case RACE_TILE_BARRIER:
    return RACE_COLOUR_BARRIER;

  case RACE_TILE_BUILDING:
    return RACE_COLOUR_BROWN;

  case RACE_TILE_SAND:
  default:
    return RACE_COLOUR_SAND;
  }
}

static void RaceRender_DrawClippedRect(int16_t x, int16_t y,
                                       uint16_t width, uint16_t height,
                                       uint8_t colour) {
  int16_t x2 = 0;
  int16_t y2 = 0;

  if ((width == 0U) || (height == 0U)) {
    return;
  }

  x2 = (int16_t)(x + (int16_t)width);
  y2 = (int16_t)(y + (int16_t)height);

  if ((x2 <= 0) || (y2 <= 0) ||
      (x >= (int16_t)RACE_SCREEN_WIDTH) ||
      (y >= (int16_t)RACE_SCREEN_HEIGHT)) {
    return;
  }

  if (x < 0) {
    width = (uint16_t)((int16_t)width + x);
    x = 0;
  }

  if (y < 0) {
    height = (uint16_t)((int16_t)height + y);
    y = 0;
  }

  if ((x + (int16_t)width) > (int16_t)RACE_SCREEN_WIDTH) {
    width = (uint16_t)((int16_t)RACE_SCREEN_WIDTH - x);
  }

  if ((y + (int16_t)height) > (int16_t)RACE_SCREEN_HEIGHT) {
    height = (uint16_t)((int16_t)RACE_SCREEN_HEIGHT - y);
  }

  if ((width == 0U) || (height == 0U)) {
    return;
  }

  LCD_Draw_Rect((uint16_t)x, (uint16_t)y, width, height, colour, 1U);
}

static void RaceRender_DrawWorldRect(const RaceTrack *track,
                                     const RaceTriggerZone *zone,
                                     uint8_t colour) {
  int16_t screen_x = 0;
  int16_t screen_y = 0;

  if ((track == NULL) || (zone == NULL)) {
    return;
  }

  screen_x = RaceTrack_WorldToScreenX(track, (float)zone->x);
  screen_y = RaceTrack_WorldToScreenY(track, (float)zone->y);

  RaceRender_DrawClippedRect(screen_x, screen_y,
                             zone->width, zone->height, colour);
}

static void RaceRender_DrawWorldText(const RaceTrack *track,
                                     const char *text,
                                     uint16_t world_x,
                                     uint16_t world_y,
                                     uint8_t colour) {
  int16_t screen_x = 0;
  int16_t screen_y = 0;

  if ((track == NULL) || (text == NULL)) {
    return;
  }

  screen_x = RaceTrack_WorldToScreenX(track, (float)world_x);
  screen_y = RaceTrack_WorldToScreenY(track, (float)world_y);

  if ((screen_x < -80) || (screen_y < -16) ||
      (screen_x > (int16_t)RACE_SCREEN_WIDTH) ||
      (screen_y > (int16_t)RACE_SCREEN_HEIGHT)) {
    return;
  }

  if ((screen_x >= 0) && (screen_y >= 0)) {
    LCD_printString(text, (uint16_t)screen_x, (uint16_t)screen_y, colour, 1U);
  }
}

static void RaceRender_DrawVisibleTiles(const RaceTrack *track) {
  int start_tile_x = 0;
  int start_tile_y = 0;
  int offset_x = 0;
  int offset_y = 0;

  int view_tiles_x = (int)RACE_VIEW_TILES_X + 2;
  int view_tiles_y = (int)RACE_VIEW_TILES_Y + 2;

  int y = 0;
  int x = 0;

  if (track == NULL) {
    return;
  }

  start_tile_x = (int)(track->camera_x / (float)RACE_TILE_SIZE);
  start_tile_y = (int)(track->camera_y / (float)RACE_TILE_SIZE);

  offset_x = -((int)track->camera_x % (int)RACE_TILE_SIZE);
  offset_y = -((int)track->camera_y % (int)RACE_TILE_SIZE);

  for (y = 0; y < view_tiles_y; y++) {
    for (x = 0; x < view_tiles_x; x++) {
      int world_tile_x = start_tile_x + x;
      int world_tile_y = start_tile_y + y;

      int16_t screen_x =
          (int16_t)(offset_x + (x * (int)RACE_TILE_SIZE));
      int16_t screen_y =
          (int16_t)(offset_y + (y * (int)RACE_TILE_SIZE));

      RaceTileType tile =
          RaceTrack_GetTileAt(track, world_tile_x, world_tile_y);

      RaceRender_DrawClippedRect(screen_x, screen_y,
                                 RACE_TILE_SIZE, RACE_TILE_SIZE,
                                 RaceRender_GetTileColour(tile));
    }
  }
}

static void RaceRender_DrawGate(const RaceTrack *track,
                                const RaceTriggerZone *zone, const char *label,
                                uint8_t banner_colour, uint8_t text_colour) {
  int16_t screen_x = 0;
  int16_t screen_y = 0;
  int16_t post_y = 0;
  uint16_t post_height = 52U;

  (void)banner_colour;

  if ((track == NULL) || (zone == NULL) || (label == NULL)) {
    return;
  }

  screen_x = RaceTrack_WorldToScreenX(track, (float)zone->x);
  screen_y = RaceTrack_WorldToScreenY(track, (float)zone->y);
  post_y = (int16_t)(screen_y - 15);

  RaceRender_DrawClippedRect((int16_t)(screen_x - 10), post_y, 7U, post_height,
                             RACE_COLOUR_BROWN);
  RaceRender_DrawClippedRect((int16_t)(screen_x + (int16_t)zone->width + 3),
                             post_y, 7U, post_height, RACE_COLOUR_BROWN);

  RaceRender_DrawClippedRect((int16_t)(screen_x - 4), (int16_t)(screen_y - 13),
                             (uint16_t)(zone->width + 8U), 17U,
                             RACE_COLOUR_BLACK);
  RaceRender_DrawClippedRect((int16_t)(screen_x - 1), (int16_t)(screen_y - 10),
                             (uint16_t)(zone->width + 2U), 11U,
                             RACE_COLOUR_BROWN);

  RaceRender_DrawWorldText(track, label, (uint16_t)(zone->x + 14U),
                           (uint16_t)(zone->y - 9U), text_colour);
}

static void RaceRender_DrawStartFinishOverlay(const RaceTrack *track) {
  RaceTriggerZone zone = RaceTrack_GetStartFinishZone();
  uint16_t tile_x = 0U;
  uint16_t tile_y = 0U;
  uint16_t tile_size = 6U;
  int16_t screen_x = 0;
  int16_t screen_y = 0;

  if (track == NULL) {
    return;
  }

  screen_x = RaceTrack_WorldToScreenX(track, (float)zone.x);
  screen_y = RaceTrack_WorldToScreenY(track, (float)zone.y);

  for (tile_y = 0U; tile_y < zone.height; tile_y += tile_size) {
    for (tile_x = 0U; tile_x < zone.width; tile_x += tile_size) {
      uint8_t colour = RACE_COLOUR_WHITE;

      if ((((tile_x / tile_size) + (tile_y / tile_size)) % 2U) != 0U) {
        colour = RACE_COLOUR_BLACK;
      }

      RaceRender_DrawClippedRect((int16_t)(screen_x + (int16_t)tile_x),
                                 (int16_t)(screen_y + (int16_t)tile_y),
                                 tile_size, tile_size, colour);
    }
  }

  RaceRender_DrawGate(track, &zone, "START FIN", RACE_COLOUR_BROWN,
                      RACE_COLOUR_WHITE);
}

static void RaceRender_DrawCheckpointOverlay(const RaceTrack *track,
                                             uint8_t checkpoint_number) {
  RaceTriggerZone zone = RaceTrack_GetCheckpointZone(checkpoint_number);
  const char *label = "CHECKPOINT";

  if ((track == NULL) || (zone.width == 0U) || (zone.height == 0U)) {
    return;
  }

  if (checkpoint_number == 1U) {
    label = "CHECKPOINT 1";
  } else if (checkpoint_number == 2U) {
    label = "CHECKPOINT 2";
  } else {
    label = "CHECKPOINT 3";
  }

  RaceRender_DrawGate(track, &zone, label, RACE_COLOUR_BROWN,
                      RACE_COLOUR_WHITE);
}

static void RaceRender_DrawJokerOverlay(const RaceTrack *track) {
  RaceTriggerZone start_zone = RaceTrack_GetJokerStartZone();
  RaceTriggerZone end_zone = RaceTrack_GetJokerEndZone();

  if (track == NULL) {
    return;
  }

  RaceRender_DrawGate(track, &start_zone, "JOKER START", RACE_COLOUR_BROWN,
                      RACE_COLOUR_WHITE);
  RaceRender_DrawGate(track, &end_zone, "JOKER END", RACE_COLOUR_BROWN,
                      RACE_COLOUR_WHITE);
}

static void RaceRender_DrawTrackMarkings(const RaceTrack *track) {
  RaceRender_DrawStartFinishOverlay(track);

  RaceRender_DrawCheckpointOverlay(track, 1U);
  RaceRender_DrawCheckpointOverlay(track, 2U);
  RaceRender_DrawCheckpointOverlay(track, 3U);

  RaceRender_DrawJokerOverlay(track);
}

static void RaceRender_FormatTime(char *buffer, size_t buffer_size,
                                  uint32_t elapsed_ms) {
  uint32_t total_seconds = elapsed_ms / 1000U;
  uint32_t minutes = total_seconds / 60U;
  uint32_t seconds = total_seconds % 60U;
  uint32_t centiseconds = (elapsed_ms % 1000U) / 10U;

  snprintf(buffer, buffer_size, "%02lu:%02lu.%02lu",
           (unsigned long)minutes,
           (unsigned long)seconds,
           (unsigned long)centiseconds);
}

static void RaceRender_DrawHud(const RaceTrack *track) {
  char text[32];
  uint8_t next_cp = 0U;

  if (track == NULL) {
    return;
  }

  LCD_Draw_Rect(0U, 0U, RACE_SCREEN_WIDTH, RACE_HUD_HEIGHT,
                RACE_COLOUR_HUD_BG, 1U);

  snprintf(text, sizeof(text), "L%u/%u",
           RaceTrack_GetCurrentLap(track),
           RaceTrack_GetTotalLaps(track));

  LCD_printString(text, 3U, 3U, RACE_COLOUR_HUD_TEXT, 1U);

  RaceRender_FormatTime(text, sizeof(text), RaceTrack_GetElapsedMs(track));
  LCD_printString(text, 48U, 3U, RACE_COLOUR_HUD_TEXT, 1U);

  next_cp = RaceTrack_GetNextCheckpoint(track);

  if (next_cp == 0U) {
    LCD_printString("CP:FIN", 125U, 3U, RACE_COLOUR_HUD_TEXT, 1U);
  } else {
    snprintf(text, sizeof(text), "CP:%u/%u",
             next_cp, RaceTrack_GetCheckpointCount(track));

    LCD_printString(text, 125U, 3U, RACE_COLOUR_HUD_TEXT, 1U);
  }

  if (RaceTrack_IsJokerLapDone(track)) {
    LCD_printString("J:Y", 185U, 3U, RACE_COLOUR_HUD_TEXT, 1U);
  } else {
    LCD_printString("J:N", 185U, 3U, RACE_COLOUR_RED, 1U);
  }

  LCD_printString(RaceTrack_GetCurrentCornerName(track),
                  3U, 14U, RACE_COLOUR_HUD_TEXT, 1U);

  if (RaceTrack_IsFinished(track)) {
    LCD_printString("FINISHED", 168U, 14U, RACE_COLOUR_START, 1U);
  }
}

static void RaceRender_DrawPlayerDamageBar(int16_t screen_x, int16_t screen_y,
                                           const RaceCar *player_car) {
  uint8_t damage = 0U;
  uint16_t health_width = 0U;

  if (player_car == NULL) {
    return;
  }

  damage = RaceCar_GetDamage(player_car);
  health_width =
      (uint16_t)((18U * (RACE_MAX_DAMAGE - damage)) / RACE_MAX_DAMAGE);

  RaceRender_DrawClippedRect((int16_t)(screen_x - 4), (int16_t)(screen_y - 8),
                             20U, 5U, RACE_COLOUR_BLACK);
  RaceRender_DrawClippedRect((int16_t)(screen_x - 3), (int16_t)(screen_y - 7),
                             18U, 3U, RACE_COLOUR_RED);

  if (health_width > 0U) {
    RaceRender_DrawClippedRect((int16_t)(screen_x - 3), (int16_t)(screen_y - 7),
                               health_width, 3U, RACE_COLOUR_GREEN);
  }
}

static void RaceRender_DrawPlayerDirectionLine(uint16_t center_x,
                                               uint16_t center_y,
                                               float heading_deg) {
  if ((heading_deg < 45.0f) || (heading_deg >= 315.0f)) {
    if (center_y >= 7U) {
      LCD_Draw_Line(center_x, center_y, center_x,
                    (uint16_t)(center_y - 7U), RACE_COLOUR_PLAYER_NOSE);
    }
  } else if (heading_deg < 135.0f) {
    LCD_Draw_Line(center_x, center_y,
                  (uint16_t)(center_x + 7U), center_y,
                  RACE_COLOUR_PLAYER_NOSE);
  } else if (heading_deg < 225.0f) {
    LCD_Draw_Line(center_x, center_y, center_x,
                  (uint16_t)(center_y + 7U), RACE_COLOUR_PLAYER_NOSE);
  } else {
    if (center_x >= 7U) {
      LCD_Draw_Line(center_x, center_y,
                    (uint16_t)(center_x - 7U), center_y,
                    RACE_COLOUR_PLAYER_NOSE);
    }
  }
}

static void RaceRender_DrawPlayer(const RaceTrack *track,
                                  const RaceCar *player_car) {
  int16_t screen_x = 0;
  int16_t screen_y = 0;

  uint16_t center_x = 0U;
  uint16_t center_y = 0U;

  if ((track == NULL) || (player_car == NULL)) {
    return;
  }

  screen_x = RaceTrack_WorldToScreenX(track, player_car->x);
  screen_y = RaceTrack_WorldToScreenY(track, player_car->y);

  RaceRender_DrawClippedRect(screen_x, screen_y,
                             player_car->width, player_car->height,
                             RACE_COLOUR_PLAYER);

  RaceRender_DrawPlayerDamageBar(screen_x, screen_y, player_car);

  if ((screen_x < 0) || (screen_y < 0) ||
      (screen_x >= (int16_t)RACE_SCREEN_WIDTH) ||
      (screen_y >= (int16_t)RACE_SCREEN_HEIGHT)) {
    return;
  }

  center_x = (uint16_t)(screen_x + (int16_t)(player_car->width / 2U));
  center_y = (uint16_t)(screen_y + (int16_t)(player_car->height / 2U));

  RaceRender_DrawPlayerDirectionLine(center_x, center_y,
                                     player_car->heading_deg);
}

void RaceRender_DrawFrame(const RaceTrack *track, const RaceCar *player_car,
                          const RaceCamera *camera) {
  (void)camera;

  if ((track == NULL) || (player_car == NULL)) {
    return;
  }

  LCD_Fill_Buffer(RACE_COLOUR_GRASS);

  RaceRender_DrawVisibleTiles(track);
  RaceRender_DrawTrackMarkings(track);
  RaceRender_DrawPlayer(track, player_car);
  RaceRender_DrawHud(track);

  LCD_Refresh(&cfg0);
}