#include "race_render.h"
#include "LCD.h"

extern ST7789V2_cfg_t cfg0;

// Internal helper prototypes
static void RaceRender_DrawHud(void);
static void RaceTrackRenderer_DrawEdges(const RaceTrack *track);
static void RaceTrackRenderer_DrawCentreLine(const RaceTrack *track);
static void RaceTrackRenderer_DrawStartFinish(const RaceTrack *track);
static void RaceRender_DrawPlayer(const RaceCar *player_car);

// Draw simple HUD text for this stage
static void RaceRender_DrawHud(void) {
  LCD_printString("Game 2 - Track", 5, 5, 1, 1);
  LCD_printString("B1 Exit", 180, 5, 1, 1);
}

// Draw the road edges down the whole visible screen
static void RaceTrackRenderer_DrawEdges(const RaceTrack *track) {
  uint16_t y = 0U;
  int16_t left_x = 0;
  int16_t right_x = 0;

  if (track == NULL) {
    return;
  }

  left_x = RaceTrack_GetLeftEdgeX(track);
  right_x = RaceTrack_GetRightEdgeX(track);

  for (y = 20U; y < track->screen_height; y += 12U) {
    LCD_printString("|", (uint16_t)left_x, y, 1, 1);
    LCD_printString("|", (uint16_t)right_x, y, 1, 1);
  }
}

// Draw centre lane markers using world-space track data
static void RaceTrackRenderer_DrawCentreLine(const RaceTrack *track) {
  int32_t world_top = 0;
  int32_t world_bottom = 0;
  int32_t world_y = 0;
  int32_t period = 0;
  int16_t center_x = 0;

  if (track == NULL) {
    return;
  }

  world_top = RaceTrack_GetWorldTopY(track);
  world_bottom = RaceTrack_GetWorldBottomY(track);
  center_x = RaceTrack_GetCenterX(track);

  period = (int32_t)track->lane_dash_length + (int32_t)track->lane_dash_gap;
  if (period <= 0) {
    return;
  }

  // Start from the nearest visible dash anchor in world space
  world_y = world_top - (world_top % period);

  for (; world_y <= world_bottom; world_y += period) {
    int16_t screen_y = RaceTrack_WorldToScreenY(track, world_y);

    if ((screen_y >= 20) && (screen_y < (int16_t)track->screen_height)) {
      LCD_printString(":", (uint16_t)center_x, (uint16_t)screen_y, 1, 1);
    }
  }
}

// Draw a static start/finish banner if it is visible
static void RaceTrackRenderer_DrawStartFinish(const RaceTrack *track) {
  int16_t left_x = 0;
  int16_t right_x = 0;
  int16_t center_x = 0;
  int16_t banner_y = 24;
  int16_t x = 0;
  int tile_index = 0;

  if (track == NULL) {
    return;
  }

  // Use the track width so the banner lines up with the road,
  // but keep the banner's Y position fixed on screen.
  left_x = RaceTrack_GetLeftEdgeX(track);
  right_x = RaceTrack_GetRightEdgeX(track);
  center_x = RaceTrack_GetCenterX(track);

  // Two-line title so it fits better on the LCD
  LCD_printString("START", (uint16_t)(center_x - 12), 6, 1, 1);
  LCD_printString("FINISH", (uint16_t)(center_x - 14), 14, 1, 1);

  // Top checkered row
  tile_index = 0;
  for (x = (int16_t)(left_x + 2); x < (int16_t)(right_x - 6); x += 8) {
    if ((tile_index % 2) == 0) {
      LCD_printString("##", (uint16_t)x, (uint16_t)banner_y, 1, 1);
    } else {
      LCD_printString("==", (uint16_t)x, (uint16_t)banner_y, 1, 1);
    }

    tile_index++;
  }

  // Bottom checkered row
  tile_index = 0;
  for (x = (int16_t)(left_x + 2); x < (int16_t)(right_x - 6); x += 8) {
    if ((tile_index % 2) == 0) {
      LCD_printString("==", (uint16_t)x, (uint16_t)(banner_y + 6), 1, 1);
    } else {
      LCD_printString("##", (uint16_t)x, (uint16_t)(banner_y + 6), 1, 1);
    }

    tile_index++;
  }

  // Side posts
  LCD_printString("|", (uint16_t)(left_x + 1), (uint16_t)(banner_y - 6), 1, 1);
  LCD_printString("|", (uint16_t)(right_x - 2), (uint16_t)(banner_y - 6), 1, 1);
}

// Draw the player placeholder
static void RaceRender_DrawPlayer(const RaceCar *player_car) {
  if (player_car == NULL) {
    return;
  }

  LCD_printString("[]", (uint16_t)player_car->x, (uint16_t)player_car->y, 1, 2);
}

// Draw the full frame
void RaceRender_DrawFrame(const RaceTrack *track, const RaceCar *player_car) {
  if ((track == NULL) || (player_car == NULL)) {
    return;
  }

  LCD_Fill_Buffer(0);

  RaceRender_DrawHud();
  RaceTrackRenderer_DrawEdges(track);
  RaceTrackRenderer_DrawCentreLine(track);
  RaceTrackRenderer_DrawStartFinish(track);
  RaceRender_DrawPlayer(player_car);

  LCD_Refresh(&cfg0);
}