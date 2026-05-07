#include "race_track.h"

#include <stddef.h>

// INVISIBLE GANTRY DETECTION ZONES
// These rectangles are invisible

// START/FINISH is vertical
// x/y is the top-left of the checkered crossing strip
// width is the thin crossing-line thickness
// height is the full road width
static const RaceTriggerZone g_start_zone = {312U, 486U, 18U, 84U};

static const RaceTriggerZone g_checkpoint_zones[RACE_CHECKPOINT_COUNT] = {
    // CP1 is horizontal across the left-side road
    {102U, 432U, 84U, 18U},

    // CP2 is vertical at the top-right road section
    {558U, 114U, 18U, 84U},

    // CP3 is horizontal across the right-side road
    {510U, 432U, 84U, 18U},
};

// JOKER IN is horizontal.
// Kept slightly wider than the joker road so the player cannot miss it
static const RaceTriggerZone g_joker_start_zone = {48U, 246U, 72U, 18U};

// No visible JOKER OUT gantry is drawn
// This invisible zone is kept for gameplay so the joker lap can still be marked
// as complete when the player rejoins near CP2
static const RaceTriggerZone g_joker_end_zone = {558U, 114U, 18U, 84U};

static void RaceTrack_Fill(RaceTrack *track, RaceTileType tile) {
  uint16_t y = 0U;
  uint16_t x = 0U;

  for (y = 0U; y < RACE_WORLD_TILES_Y; y++) {
    for (x = 0U; x < RACE_WORLD_TILES_X; x++) {
      track->tiles[y][x] = (uint8_t)tile;
    }
  }
}

static bool RaceTrack_TileInWorld(int tile_x, int tile_y) {
  return ((tile_x >= 0) && (tile_y >= 0) &&
          (tile_x < (int)RACE_WORLD_TILES_X) &&
          (tile_y < (int)RACE_WORLD_TILES_Y));
}

static void RaceTrack_SetTile(RaceTrack *track, int tile_x, int tile_y,
                              RaceTileType tile) {
  if ((track == NULL) || (RaceTrack_TileInWorld(tile_x, tile_y) == false)) {
    return;
  }

  track->tiles[tile_y][tile_x] = (uint8_t)tile;
}

static void RaceTrack_DrawTileRect(RaceTrack *track, int x, int y, int width,
                                   int height, RaceTileType tile) {
  int ix = 0;
  int iy = 0;

  for (iy = y; iy < (y + height); iy++) {
    for (ix = x; ix < (x + width); ix++) {
      RaceTrack_SetTile(track, ix, iy, tile);
    }
  }
}

static void RaceTrack_DrawDisc(RaceTrack *track, int center_x, int center_y,
                               int radius, RaceTileType tile) {
  int y = 0;
  int x = 0;

  for (y = center_y - radius; y <= center_y + radius; y++) {
    for (x = center_x - radius; x <= center_x + radius; x++) {
      int dx = x - center_x;
      int dy = y - center_y;

      if (((dx * dx) + (dy * dy)) <= (radius * radius)) {
        RaceTrack_SetTile(track, x, y, tile);
      }
    }
  }
}

static void RaceTrack_DrawLine(RaceTrack *track, int x0, int y0, int x1, int y1,
                               int radius, RaceTileType tile) {
  int dx = x1 - x0;
  int dy = y1 - y0;
  int steps = (dx < 0) ? -dx : dx;
  int i = 0;

  if (((dy < 0) ? -dy : dy) > steps) {
    steps = (dy < 0) ? -dy : dy;
  }

  if (steps == 0) {
    RaceTrack_DrawDisc(track, x0, y0, radius, tile);
    return;
  }

  for (i = 0; i <= steps; i++) {
    int x = x0 + ((dx * i) / steps);
    int y = y0 + ((dy * i) / steps);
    RaceTrack_DrawDisc(track, x, y, radius, tile);
  }
}

static void RaceTrack_AddRoadTexture(RaceTrack *track) { (void)track; }

static void RaceTrack_AddScenery(RaceTrack *track) {

  RaceTrack_DrawDisc(track, 13, 31, 2, RACE_TILE_TREE);
  RaceTrack_DrawDisc(track, 19, 34, 2, RACE_TILE_TREE);
  RaceTrack_DrawDisc(track, 101, 74, 2, RACE_TILE_TREE);
  RaceTrack_DrawDisc(track, 108, 80, 2, RACE_TILE_TREE);
  RaceTrack_DrawDisc(track, 58, 52, 2, RACE_TILE_TREE);
  RaceTrack_DrawDisc(track, 61, 57, 2, RACE_TILE_TREE);
  RaceTrack_DrawDisc(track, 44, 8, 2, RACE_TILE_TREE);
  RaceTrack_DrawDisc(track, 49, 10, 2, RACE_TILE_TREE);
}

static void RaceTrack_AddPaintedMarkers(RaceTrack *track) {
  if (track == NULL) {
    return;
  }

  // Start finish strip only
  RaceTrack_DrawTileRect(track, 52, 85, 3, 21, RACE_TILE_START);
}

static void RaceTrack_BuildMap(RaceTrack *track) {
  if (track == NULL) {
    return;
  }

  // Green circuit base
  RaceTrack_Fill(track, RACE_TILE_GRASS);
  RaceTrack_AddScenery(track);

  // Main route
  RaceTrack_DrawLine(track, 60, 88, 36, 88, 7, RACE_TILE_ROAD);
  RaceTrack_DrawLine(track, 36, 88, 24, 74, 7, RACE_TILE_ROAD);
  RaceTrack_DrawLine(track, 24, 74, 24, 52, 7, RACE_TILE_ROAD);
  RaceTrack_DrawLine(track, 24, 52, 34, 40, 7, RACE_TILE_ROAD);
  RaceTrack_DrawLine(track, 34, 40, 58, 34, 7, RACE_TILE_ROAD);
  RaceTrack_DrawLine(track, 58, 34, 88, 26, 7, RACE_TILE_ROAD);
  RaceTrack_DrawLine(track, 88, 26, 104, 28, 7, RACE_TILE_ROAD);
  RaceTrack_DrawLine(track, 104, 28, 96, 48, 7, RACE_TILE_ROAD);
  RaceTrack_DrawLine(track, 96, 48, 92, 64, 7, RACE_TILE_ROAD);
  RaceTrack_DrawLine(track, 92, 64, 92, 82, 7, RACE_TILE_ROAD);
  RaceTrack_DrawLine(track, 92, 82, 76, 88, 7, RACE_TILE_ROAD);
  RaceTrack_DrawLine(track, 76, 88, 60, 88, 7, RACE_TILE_ROAD);

  // Joker route
  RaceTrack_DrawLine(track, 24, 52, 14, 46, 5, RACE_TILE_ROAD);
  RaceTrack_DrawLine(track, 14, 46, 12, 30, 5, RACE_TILE_ROAD);
  RaceTrack_DrawLine(track, 12, 30, 26, 20, 5, RACE_TILE_ROAD);
  RaceTrack_DrawLine(track, 26, 20, 50, 16, 5, RACE_TILE_ROAD);
  RaceTrack_DrawLine(track, 50, 16, 76, 18, 5, RACE_TILE_ROAD);
  RaceTrack_DrawLine(track, 76, 18, 88, 26, 5, RACE_TILE_ROAD);

  RaceTrack_AddRoadTexture(track);
  RaceTrack_AddPaintedMarkers(track);
}

static bool RaceTrack_PointInZone(float x, float y,
                                  const RaceTriggerZone *zone) {
  float left = 0.0f;
  float right = 0.0f;
  float top = 0.0f;
  float bottom = 0.0f;

  if (zone == NULL) {
    return false;
  }

  left = (float)zone->x;
  right = (float)(zone->x + zone->width);
  top = (float)zone->y;
  bottom = (float)(zone->y + zone->height);

  return ((x >= left) && (x < right) && (y >= top) && (y < bottom));
}

static bool RaceTrack_CenterInZone(float car_x, float car_y, uint16_t car_width,
                                   uint16_t car_height,
                                   const RaceTriggerZone *zone) {
  float center_x = car_x + ((float)car_width * 0.5f);
  float center_y = car_y + ((float)car_height * 0.5f);

  return RaceTrack_PointInZone(center_x, center_y, zone);
}

void RaceTrack_Init(RaceTrack *track, uint16_t screen_width,
                    uint16_t screen_height) {
  if (track == NULL) {
    return;
  }

  track->screen_width = screen_width;
  track->screen_height = screen_height;
  track->camera_x = 0.0f;
  track->camera_y = 0.0f;

  track->current_lap = 1U;
  track->total_laps = RACE_TOTAL_LAPS;
  track->next_checkpoint = 1U;
  track->checkpoint_count = RACE_CHECKPOINT_COUNT;

  track->joker_lap_active = false;
  track->joker_lap_done = false;
  track->joker_lap_required = true;

  track->race_start_ms = 0U;
  track->elapsed_ms = 0U;
  track->race_finished = false;
  track->active = true;

  RaceTrack_BuildMap(track);
}

void RaceTrack_ResetToStart(RaceTrack *track) {
  if (track == NULL) {
    return;
  }

  RaceTrack_Init(track, track->screen_width, track->screen_height);
}

RaceTileType RaceTrack_GetTileAt(const RaceTrack *track, int tile_x,
                                 int tile_y) {
  if ((track == NULL) || (RaceTrack_TileInWorld(tile_x, tile_y) == false)) {
    return RACE_TILE_GRASS;
  }

  return (RaceTileType)track->tiles[tile_y][tile_x];
}

RaceTileType RaceTrack_GetTileAtWorld(const RaceTrack *track, float world_x,
                                      float world_y) {
  int tile_x = (int)(world_x / (float)RACE_TILE_SIZE);
  int tile_y = (int)(world_y / (float)RACE_TILE_SIZE);

  return RaceTrack_GetTileAt(track, tile_x, tile_y);
}

bool RaceTrack_PointIsDriveable(const RaceTrack *track, float world_x,
                                float world_y) {
  RaceTileType tile = RACE_TILE_GRASS;

  if ((world_x < 0.0f) || (world_y < 0.0f) ||
      (world_x >= (float)RACE_WORLD_WIDTH_PX) ||
      (world_y >= (float)RACE_WORLD_HEIGHT_PX)) {
    return false;
  }

  tile = RaceTrack_GetTileAtWorld(track, world_x, world_y);
  return RaceTrackLayout_IsDriveableTile(tile);
}

bool RaceTrack_CarIsDriveable(const RaceTrack *track, float car_x, float car_y,
                              uint16_t car_width, uint16_t car_height) {
  float left = car_x + 2.0f;
  float right = car_x + (float)car_width - 2.0f;
  float top = car_y + 2.0f;
  float bottom = car_y + (float)car_height - 2.0f;
  float center_x = car_x + ((float)car_width * 0.5f);
  float center_y = car_y + ((float)car_height * 0.5f);
  uint8_t hits = 0U;

  if (RaceTrack_PointIsDriveable(track, center_x, center_y)) {
    hits++;
  }

  if (RaceTrack_PointIsDriveable(track, left, top)) {
    hits++;
  }

  if (RaceTrack_PointIsDriveable(track, right, top)) {
    hits++;
  }

  if (RaceTrack_PointIsDriveable(track, left, bottom)) {
    hits++;
  }

  if (RaceTrack_PointIsDriveable(track, right, bottom)) {
    hits++;
  }

  return hits >= 3U;
}

void RaceTrack_SetCamera(RaceTrack *track, float camera_x, float camera_y) {
  if (track == NULL) {
    return;
  }

  track->camera_x = camera_x;
  track->camera_y = camera_y;
}

int16_t RaceTrack_WorldToScreenX(const RaceTrack *track, float world_x) {
  if (track == NULL) {
    return (int16_t)world_x;
  }

  return (int16_t)((world_x - track->camera_x) + 0.5f);
}

int16_t RaceTrack_WorldToScreenY(const RaceTrack *track, float world_y) {
  if (track == NULL) {
    return (int16_t)world_y;
  }

  return (int16_t)((world_y - track->camera_y) + 0.5f);
}

void RaceTrack_StartTimer(RaceTrack *track, uint32_t now_ms) {
  if (track == NULL) {
    return;
  }

  track->race_start_ms = now_ms;
  track->elapsed_ms = 0U;
}

void RaceTrack_UpdateTimer(RaceTrack *track, uint32_t now_ms) {
  if ((track == NULL) || (track->race_finished == true)) {
    return;
  }

  track->elapsed_ms = now_ms - track->race_start_ms;
}

void RaceTrack_UpdateProgress(RaceTrack *track, float car_x, float car_y,
                              uint16_t car_width, uint16_t car_height) {
  if ((track == NULL) || (track->race_finished == true)) {
    return;
  }

  // Joker only counts after CP1, before CP2 This prevents random joker triggers
  if ((track->next_checkpoint == 2U) && (track->joker_lap_done == false)) {
    if (RaceTrack_CenterInZone(car_x, car_y, car_width, car_height,
                               &g_joker_start_zone)) {
      track->joker_lap_active = true;
    }

    if ((track->joker_lap_active == true) &&
        RaceTrack_CenterInZone(car_x, car_y, car_width, car_height,
                               &g_joker_end_zone)) {
      track->joker_lap_active = false;
      track->joker_lap_done = true;
    }
  }

  if ((track->next_checkpoint >= 1U) &&
      (track->next_checkpoint <= RACE_CHECKPOINT_COUNT)) {
    uint8_t checkpoint_index = (uint8_t)(track->next_checkpoint - 1U);

    if (RaceTrack_CenterInZone(car_x, car_y, car_width, car_height,
                               &g_checkpoint_zones[checkpoint_index])) {
      track->next_checkpoint++;

      if (track->next_checkpoint > RACE_CHECKPOINT_COUNT) {
        track->next_checkpoint = 0U;
      }
    }

    return;
  }

  if (RaceTrack_CenterInZone(car_x, car_y, car_width, car_height,
                             &g_start_zone)) {
    if (track->current_lap >= track->total_laps) {
      if ((track->joker_lap_required == false) ||
          (track->joker_lap_done == true)) {
        track->race_finished = true;
      }

      return;
    }

    track->current_lap++;
    track->next_checkpoint = 1U;
  }
}

uint8_t RaceTrack_GetCurrentLap(const RaceTrack *track) {
  return (track == NULL) ? 1U : track->current_lap;
}

uint8_t RaceTrack_GetTotalLaps(const RaceTrack *track) {
  return (track == NULL) ? RACE_TOTAL_LAPS : track->total_laps;
}

uint8_t RaceTrack_GetNextCheckpoint(const RaceTrack *track) {
  return (track == NULL) ? 1U : track->next_checkpoint;
}

uint8_t RaceTrack_GetCheckpointCount(const RaceTrack *track) {
  return (track == NULL) ? RACE_CHECKPOINT_COUNT : track->checkpoint_count;
}

uint32_t RaceTrack_GetElapsedMs(const RaceTrack *track) {
  return (track == NULL) ? 0U : track->elapsed_ms;
}

bool RaceTrack_IsFinished(const RaceTrack *track) {
  return (track == NULL) ? false : track->race_finished;
}

bool RaceTrack_IsJokerLapActive(const RaceTrack *track) {
  return (track == NULL) ? false : track->joker_lap_active;
}

bool RaceTrack_IsJokerLapDone(const RaceTrack *track) {
  return (track == NULL) ? false : track->joker_lap_done;
}

bool RaceTrack_IsJokerLapRequired(const RaceTrack *track) {
  return (track == NULL) ? true : track->joker_lap_required;
}

RaceSector RaceTrack_GetCurrentSector(const RaceTrack *track) {
  if (track == NULL) {
    return RACE_SECTOR_1;
  }

  if (track->next_checkpoint <= 1U) {
    return RACE_SECTOR_1;
  }

  if (track->next_checkpoint == 2U) {
    return RACE_SECTOR_2;
  }

  return RACE_SECTOR_3;
}

const char *RaceTrack_GetCurrentCornerName(const RaceTrack *track) {
  if (track == NULL) {
    return "UNKNOWN";
  }

  if ((track->next_checkpoint == 2U) && (track->joker_lap_done == false)) {
    return "JOKER OPTIONAL";
  }

  if ((track->current_lap >= track->total_laps) &&
      (track->next_checkpoint == 0U) && (track->joker_lap_required == true) &&
      (track->joker_lap_done == false)) {
    return "JOKER REQUIRED";
  }

  switch (track->next_checkpoint) {
  case 1U:
    return "TARGET CP1";

  case 2U:
    return "TARGET CP2";

  case 3U:
    return "TARGET CP3";

  default:
    return "RETURN START";
  }
}

RaceTriggerZone RaceTrack_GetStartFinishZone(void) { return g_start_zone; }

RaceTriggerZone RaceTrack_GetCheckpointZone(uint8_t checkpoint_number) {
  RaceTriggerZone empty_zone = {0U, 0U, 0U, 0U};

  if ((checkpoint_number == 0U) ||
      (checkpoint_number > RACE_CHECKPOINT_COUNT)) {
    return empty_zone;
  }

  return g_checkpoint_zones[checkpoint_number - 1U];
}

RaceTriggerZone RaceTrack_GetJokerStartZone(void) { return g_joker_start_zone; }

RaceTriggerZone RaceTrack_GetJokerEndZone(void) { return g_joker_end_zone; }

void RaceTrack_GoToNextScreen(RaceTrack *track) { (void)track; }

const void *RaceTrack_GetCurrentScreen(const RaceTrack *track) {
  (void)track;
  return NULL;
}

int RaceTrack_GetCurrentScreenIndex(const RaceTrack *track) {
  (void)track;
  return 0;
}

int16_t RaceTrack_GetCenterX(const RaceTrack *track) {
  (void)track;
  return 120;
}

int16_t RaceTrack_GetLeftEdgeX(const RaceTrack *track) {
  (void)track;
  return 40;
}

int16_t RaceTrack_GetRightEdgeX(const RaceTrack *track) {
  (void)track;
  return 200;
}

int16_t RaceTrack_GetCenterXAtScreenY(const RaceTrack *track,
                                      uint16_t screen_y) {
  (void)track;
  (void)screen_y;
  return 120;
}

uint16_t RaceTrack_GetWidthAtScreenY(const RaceTrack *track,
                                     uint16_t screen_y) {
  (void)track;
  (void)screen_y;
  return 160U;
}

int16_t RaceTrack_GetLeftEdgeXAtScreenY(const RaceTrack *track,
                                        uint16_t screen_y) {
  (void)track;
  (void)screen_y;
  return 40;
}

int16_t RaceTrack_GetRightEdgeXAtScreenY(const RaceTrack *track,
                                         uint16_t screen_y) {
  (void)track;
  (void)screen_y;
  return 200;
}

void RaceTrack_GetDriveBounds(const RaceTrack *track, int16_t *min_x,
                              int16_t *max_x) {
  (void)track;

  if (min_x != NULL) {
    *min_x = 0;
  }

  if (max_x != NULL) {
    *max_x = (int16_t)RACE_WORLD_WIDTH_PX;
  }
}

bool RaceTrack_HasLeftCurbAtScreenY(const RaceTrack *track, uint16_t screen_y) {
  (void)track;
  (void)screen_y;
  return false;
}

bool RaceTrack_HasRightCurbAtScreenY(const RaceTrack *track,
                                     uint16_t screen_y) {
  (void)track;
  (void)screen_y;
  return false;
}

float RaceTrack_GetNextSpawnX(const RaceTrack *track) {
  (void)track;
  return RACE_PLAYER_START_X;
}