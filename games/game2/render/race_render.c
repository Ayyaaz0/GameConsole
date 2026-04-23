#include "race_render.h"
#include "LCD.h"

extern ST7789V2_cfg_t cfg0;

// Draw simple HUD text for this stage.
static void RaceRender_DrawHud(void) {
  LCD_printString("Game 2 - Track", 5, 5, 1, 1);
  LCD_printString("B1 Exit", 180, 5, 1, 1);
}

// Draw the road edges using simple text markers.
// This is intentionally basic for now so the architecture is correct first.
static void RaceRender_DrawTrackEdges(const RaceTrack *track) {
  uint16_t y = 0U;
  int16_t left_x = 0;
  int16_t right_x = 0;

  if (track == NULL) {
    return;
  }

  left_x = RaceTrack_GetLeftEdgeX(track);
  right_x = RaceTrack_GetRightEdgeX(track);

  for (y = track->top_margin; y < track->bottom_margin; y += 12U) {
    LCD_printString("|", (uint16_t)left_x, y, 1, 1);
    LCD_printString("|", (uint16_t)right_x, y, 1, 1);
  }
}

// Draw moving centre lane markers.
static void RaceRender_DrawTrackCentreLine(const RaceTrack *track) {
  int16_t center_x = 0;
  int16_t y = 0;

  if (track == NULL) {
    return;
  }

  center_x = RaceTrack_GetCenterX(track);

  for (y = (int16_t)track->top_margin - (int16_t)track->lane_dash_offset;
       y < (int16_t)track->bottom_margin; y += (int16_t)track->lane_dash_gap) {
    if (y >= (int16_t)track->top_margin) {
      LCD_printString(":", (uint16_t)center_x, (uint16_t)y, 1, 1);
    }
  }
}

// Draw the player placeholder.
static void RaceRender_DrawPlayer(const RaceCar *player_car) {
  if (player_car == NULL) {
    return;
  }

  LCD_printString("[]", (uint16_t)player_car->x, (uint16_t)player_car->y, 1, 2);
}

void RaceRender_DrawFrame(const RaceTrack *track, const RaceCar *player_car) {
  if ((track == NULL) || (player_car == NULL)) {
    return;
  }

  LCD_Fill_Buffer(0);

  RaceRender_DrawHud();
  RaceRender_DrawTrackEdges(track);
  RaceRender_DrawTrackCentreLine(track);
  RaceRender_DrawPlayer(player_car);

  LCD_Refresh(&cfg0);
}