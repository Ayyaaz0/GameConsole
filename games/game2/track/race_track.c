#include "race_track.h"

#include "../config/race_config.h"

#include <stddef.h>

static bool RaceTrack_RectContainsPoint(const RaceRect *rect, float x,
                                        float y) {
  if (rect == NULL) {
    return false;
  }

  return (
      (x >= (float)rect->x) && (x <= (float)(rect->x + (int16_t)rect->width)) &&
      (y >= (float)rect->y) && (y <= (float)(rect->y + (int16_t)rect->height)));
}

void RaceTrack_Init(RaceTrack *track, uint16_t screen_width,
                    uint16_t screen_height) {
  if (track == NULL) {
    return;
  }

  track->screen_width = screen_width;
  track->screen_height = screen_height;
  track->current_screen_index = RACE_TRACK_START_SCREEN_INDEX;
  track->active = true;
}

void RaceTrack_ResetToStart(RaceTrack *track) {
  if (track == NULL) {
    return;
  }

  track->current_screen_index = RACE_TRACK_START_SCREEN_INDEX;
  track->active = true;
}

void RaceTrack_GoToNextScreen(RaceTrack *track) {
  int screen_count = 0;

  if ((track == NULL) || (track->active == false)) {
    return;
  }

  screen_count = RaceTrackLayout_GetScreenCount();

  if (screen_count <= 0) {
    return;
  }

  track->current_screen_index++;

  if (track->current_screen_index >= screen_count) {
    track->current_screen_index = 0;
  }
}

const RaceTrackScreen *RaceTrack_GetCurrentScreen(const RaceTrack *track) {
  if (track == NULL) {
    return NULL;
  }

  return RaceTrackLayout_GetScreenByIndex(track->current_screen_index);
}

int RaceTrack_GetCurrentScreenIndex(const RaceTrack *track) {
  if (track == NULL) {
    return 0;
  }

  return track->current_screen_index;
}

RaceTrackEdge RaceTrack_GetCurrentExitEdge(const RaceTrack *track) {
  const RaceTrackScreen *screen = RaceTrack_GetCurrentScreen(track);

  if (screen == NULL) {
    return RACE_EDGE_TOP;
  }

  return screen->exit_edge;
}

float RaceTrack_GetSpawnX(const RaceTrack *track) {
  const RaceTrackScreen *screen = RaceTrack_GetCurrentScreen(track);

  if (screen == NULL) {
    return RACE_PLAYER_START_X;
  }

  return screen->spawn_x;
}

float RaceTrack_GetSpawnY(const RaceTrack *track) {
  const RaceTrackScreen *screen = RaceTrack_GetCurrentScreen(track);

  if (screen == NULL) {
    return RACE_PLAYER_START_Y;
  }

  return screen->spawn_y;
}

float RaceTrack_GetSpawnHeadingDeg(const RaceTrack *track) {
  const RaceTrackScreen *screen = RaceTrack_GetCurrentScreen(track);

  if (screen == NULL) {
    return 0.0f;
  }

  return screen->spawn_heading_deg;
}

RaceSector RaceTrack_GetCurrentSector(const RaceTrack *track) {
  const RaceTrackScreen *screen = RaceTrack_GetCurrentScreen(track);

  if (screen == NULL) {
    return RACE_SECTOR_1;
  }

  return screen->sector;
}

const char *RaceTrack_GetCurrentCornerName(const RaceTrack *track) {
  const RaceTrackScreen *screen = RaceTrack_GetCurrentScreen(track);

  if (screen == NULL) {
    return "UNKNOWN";
  }

  return screen->corner_name;
}

bool RaceTrack_CurrentScreenHasStartFinish(const RaceTrack *track) {
  const RaceTrackScreen *screen = RaceTrack_GetCurrentScreen(track);

  if (screen == NULL) {
    return false;
  }

  return screen->has_start_finish;
}

bool RaceTrack_PointIsOnRoad(const RaceTrack *track, float x, float y) {
  const RaceTrackScreen *screen = RaceTrack_GetCurrentScreen(track);
  uint8_t i = 0U;

  if (screen == NULL) {
    return false;
  }

  for (i = 0U; i < screen->road_rect_count; i++) {
    if (RaceTrack_RectContainsPoint(&screen->road_rects[i], x, y)) {
      return true;
    }
  }

  return false;
}

bool RaceTrack_CarCentreIsOnRoad(const RaceTrack *track, float car_x,
                                 float car_y, uint16_t car_width,
                                 uint16_t car_height) {
  float centre_x = car_x + ((float)car_width * 0.5f);
  float centre_y = car_y + ((float)car_height * 0.5f);

  return RaceTrack_PointIsOnRoad(track, centre_x, centre_y);
}

const RaceRect *RaceTrack_GetRoadRects(const RaceTrack *track,
                                       uint8_t *rect_count) {
  const RaceTrackScreen *screen = RaceTrack_GetCurrentScreen(track);

  if (rect_count != NULL) {
    *rect_count = 0U;
  }

  if (screen == NULL) {
    return NULL;
  }

  if (rect_count != NULL) {
    *rect_count = screen->road_rect_count;
  }

  return screen->road_rects;
}

RaceRect RaceTrack_GetCurbRect(const RaceTrack *track) {
  const RaceTrackScreen *screen = RaceTrack_GetCurrentScreen(track);
  RaceRect empty_rect = {0, 0, 0, 0};

  if (screen == NULL) {
    return empty_rect;
  }

  return screen->curb_rect;
}

RaceCurbPlacement RaceTrack_GetCurbPlacement(const RaceTrack *track) {
  const RaceTrackScreen *screen = RaceTrack_GetCurrentScreen(track);

  if (screen == NULL) {
    return RACE_CURB_NONE;
  }

  return screen->curb_side;
}

/* Compatibility helpers. These now return a sensible value from the first road
 * rectangle. */

int16_t RaceTrack_GetCenterX(const RaceTrack *track) {
  const RaceTrackScreen *screen = RaceTrack_GetCurrentScreen(track);

  if ((screen == NULL) || (screen->road_rect_count == 0U)) {
    return 120;
  }

  return (int16_t)(screen->road_rects[0].x +
                   (int16_t)(screen->road_rects[0].width / 2U));
}

int16_t RaceTrack_GetLeftEdgeX(const RaceTrack *track) {
  const RaceTrackScreen *screen = RaceTrack_GetCurrentScreen(track);

  if ((screen == NULL) || (screen->road_rect_count == 0U)) {
    return 80;
  }

  return screen->road_rects[0].x;
}

int16_t RaceTrack_GetRightEdgeX(const RaceTrack *track) {
  const RaceTrackScreen *screen = RaceTrack_GetCurrentScreen(track);

  if ((screen == NULL) || (screen->road_rect_count == 0U)) {
    return 160;
  }

  return (int16_t)(screen->road_rects[0].x +
                   (int16_t)screen->road_rects[0].width);
}

int16_t RaceTrack_GetCenterXAtScreenY(const RaceTrack *track,
                                      uint16_t screen_y) {
  (void)screen_y;
  return RaceTrack_GetCenterX(track);
}

uint16_t RaceTrack_GetWidthAtScreenY(const RaceTrack *track,
                                     uint16_t screen_y) {
  const RaceTrackScreen *screen = RaceTrack_GetCurrentScreen(track);
  (void)screen_y;

  if ((screen == NULL) || (screen->road_rect_count == 0U)) {
    return 80U;
  }

  return screen->road_rects[0].width;
}

int16_t RaceTrack_GetLeftEdgeXAtScreenY(const RaceTrack *track,
                                        uint16_t screen_y) {
  (void)screen_y;
  return RaceTrack_GetLeftEdgeX(track);
}

int16_t RaceTrack_GetRightEdgeXAtScreenY(const RaceTrack *track,
                                         uint16_t screen_y) {
  (void)screen_y;
  return RaceTrack_GetRightEdgeX(track);
}

void RaceTrack_GetDriveBounds(const RaceTrack *track, int16_t *min_x,
                              int16_t *max_x) {
  if ((min_x == NULL) || (max_x == NULL)) {
    return;
  }

  *min_x = RaceTrack_GetLeftEdgeX(track);
  *max_x = RaceTrack_GetRightEdgeX(track);
}

bool RaceTrack_HasLeftCurbAtScreenY(const RaceTrack *track, uint16_t screen_y) {
  const RaceTrackScreen *screen = RaceTrack_GetCurrentScreen(track);

  if (screen == NULL) {
    return false;
  }

  if ((screen_y < (uint16_t)screen->curb_rect.y) ||
      (screen_y > (uint16_t)(screen->curb_rect.y + screen->curb_rect.height))) {
    return false;
  }

  return ((screen->curb_side == RACE_CURB_LEFT) ||
          (screen->curb_side == RACE_CURB_BOTH));
}

bool RaceTrack_HasRightCurbAtScreenY(const RaceTrack *track,
                                     uint16_t screen_y) {
  const RaceTrackScreen *screen = RaceTrack_GetCurrentScreen(track);

  if (screen == NULL) {
    return false;
  }

  if ((screen_y < (uint16_t)screen->curb_rect.y) ||
      (screen_y > (uint16_t)(screen->curb_rect.y + screen->curb_rect.height))) {
    return false;
  }

  return ((screen->curb_side == RACE_CURB_RIGHT) ||
          (screen->curb_side == RACE_CURB_BOTH));
}

float RaceTrack_GetNextSpawnX(const RaceTrack *track) {
  return RaceTrack_GetSpawnX(track);
}