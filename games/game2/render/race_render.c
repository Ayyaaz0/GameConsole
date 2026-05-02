#include "race_render.h"

#include "../config/race_config.h"
#include "LCD.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

extern ST7789V2_cfg_t cfg0;

static void RaceRender_DrawHud(const RaceTrack *track);
static void RaceRender_DrawRoad(const RaceTrack *track);
static void RaceRender_DrawCurbs(const RaceTrack *track);
static void RaceRender_DrawStartFinish(const RaceTrack *track);
static void RaceRender_DrawPlayer(const RaceCar *player_car);

static void RaceRender_DrawHud(const RaceTrack *track) {
  char sector_text[16];

  LCD_printString("Game 2", 5, 5, 1, 1);
  LCD_printString("B1 Exit", 185, 5, 1, 1);

  if (track != NULL) {
    sprintf(sector_text, "S%d", (int)RaceTrack_GetCurrentSector(track));
    LCD_printString(sector_text, 5, 18, 1, 1);
    LCD_printString(RaceTrack_GetCurrentCornerName(track), 30, 18, 1, 1);
  }
}

static void RaceRender_DrawRoadRect(const RaceRect *rect) {
  if ((rect == NULL) || (rect->width == 0U) || (rect->height == 0U)) {
    return;
  }

  LCD_Draw_Rect((uint16_t)rect->x, (uint16_t)rect->y, rect->width, rect->height,
                RACE_TRACK_ROAD_COLOUR, 1);

  LCD_Draw_Rect((uint16_t)rect->x, (uint16_t)rect->y, rect->width, rect->height,
                RACE_TRACK_WALL_COLOUR, 0);
}

static void RaceRender_DrawRoad(const RaceTrack *track) {
  const RaceRect *rects = NULL;
  uint8_t rect_count = 0U;
  uint8_t i = 0U;

  if (track == NULL) {
    return;
  }

  rects = RaceTrack_GetRoadRects(track, &rect_count);

  if (rects == NULL) {
    return;
  }

  for (i = 0U; i < rect_count; i++) {
    RaceRender_DrawRoadRect(&rects[i]);
  }
}

static void RaceRender_DrawCurbs(const RaceTrack *track) {
  RaceRect curb_rect;
  RaceCurbPlacement placement = RACE_CURB_NONE;
  uint16_t i = 0U;
  uint8_t colour = RACE_TRACK_CURB_RED;

  if (track == NULL) {
    return;
  }

  placement = RaceTrack_GetCurbPlacement(track);

  if (placement == RACE_CURB_NONE) {
    return;
  }

  curb_rect = RaceTrack_GetCurbRect(track);

  if ((curb_rect.width == 0U) || (curb_rect.height == 0U)) {
    return;
  }

  if (curb_rect.width >= curb_rect.height) {
    for (i = 0U; i < curb_rect.width; i += 8U) {
      colour =
          (((i / 8U) % 2U) == 0U) ? RACE_TRACK_CURB_RED : RACE_TRACK_CURB_WHITE;

      LCD_Draw_Rect((uint16_t)(curb_rect.x + (int16_t)i), (uint16_t)curb_rect.y,
                    8U, curb_rect.height, colour, 1);
    }
  } else {
    for (i = 0U; i < curb_rect.height; i += 8U) {
      colour =
          (((i / 8U) % 2U) == 0U) ? RACE_TRACK_CURB_RED : RACE_TRACK_CURB_WHITE;

      LCD_Draw_Rect((uint16_t)curb_rect.x, (uint16_t)(curb_rect.y + (int16_t)i),
                    curb_rect.width, 8U, colour, 1);
    }
  }
}

static void RaceRender_DrawStartFinish(const RaceTrack *track) {
  uint16_t x = 0U;
  uint8_t tile_index = 0U;
  uint8_t colour = RACE_TRACK_CURB_WHITE;

  if ((track == NULL) ||
      (RaceTrack_CurrentScreenHasStartFinish(track) == false)) {
    return;
  }

  LCD_printString("START", 82, 158, 1, 1);
  LCD_printString("FINISH", 122, 158, 1, 1);

  for (x = 82U; x < 158U; x += 8U) {
    colour = ((tile_index % 2U) == 0U) ? RACE_TRACK_CURB_WHITE
                                       : RACE_TRACK_ROAD_COLOUR;

    LCD_Draw_Rect(x, RACE_TRACK_START_FINISH_Y, 8U, 6U, colour, 1);

    colour = ((tile_index % 2U) == 0U) ? RACE_TRACK_ROAD_COLOUR
                                       : RACE_TRACK_CURB_WHITE;

    LCD_Draw_Rect(x, (uint16_t)(RACE_TRACK_START_FINISH_Y + 6U), 8U, 6U, colour,
                  1);

    tile_index++;
  }
}

static void RaceRender_DrawPlayer(const RaceCar *player_car) {
  uint16_t centre_x = 0U;
  uint16_t centre_y = 0U;

  if (player_car == NULL) {
    return;
  }

  LCD_Draw_Rect((uint16_t)player_car->x, (uint16_t)player_car->y,
                player_car->width, player_car->height, 3, 1);

  LCD_Draw_Rect((uint16_t)player_car->x, (uint16_t)player_car->y,
                player_car->width, player_car->height, 1, 0);

  centre_x = (uint16_t)(player_car->x + ((float)player_car->width * 0.5f));
  centre_y = (uint16_t)(player_car->y + ((float)player_car->height * 0.5f));

  if ((player_car->heading_deg < 45.0f) ||
      (player_car->heading_deg >= 315.0f)) {
    LCD_Draw_Line(centre_x, centre_y, centre_x, (uint16_t)(centre_y - 10U), 1);
  } else if (player_car->heading_deg < 135.0f) {
    LCD_Draw_Line(centre_x, centre_y, (uint16_t)(centre_x + 10U), centre_y, 1);
  } else if (player_car->heading_deg < 225.0f) {
    LCD_Draw_Line(centre_x, centre_y, centre_x, (uint16_t)(centre_y + 10U), 1);
  } else {
    LCD_Draw_Line(centre_x, centre_y, (uint16_t)(centre_x - 10U), centre_y, 1);
  }
}

void RaceRender_DrawFrame(const RaceTrack *track, const RaceCar *player_car) {
  if ((track == NULL) || (player_car == NULL)) {
    return;
  }

  LCD_Fill_Buffer(0);

  RaceRender_DrawRoad(track);
  RaceRender_DrawCurbs(track);
  RaceRender_DrawStartFinish(track);
  RaceRender_DrawPlayer(player_car);
  RaceRender_DrawHud(track);

  LCD_Refresh(&cfg0);
}