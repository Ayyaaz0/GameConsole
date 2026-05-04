#include "race_track.h"

#include <stddef.h>

static const RaceTriggerZone g_start_zone = {40U, 344U, 128U, 22U};

static const RaceTriggerZone g_checkpoint_zones[RACE_CHECKPOINT_COUNT] = {
    {40U, 136U, 128U, 22U},
    {584U, 184U, 128U, 22U},
    {416U, 336U, 128U, 22U},
};

static const RaceTriggerZone g_joker_start_zone = {152U, 80U, 120U, 22U};
static const RaceTriggerZone g_joker_end_zone = {600U, 136U, 112U, 22U};
static const RaceTriggerZone g_joker_zone = {144U, 8U, 568U, 176U};

// VISUAL TRACK MAP BUILDING

// Track idea:
// - Main lap uses a large flowing outer loop.
// - Joker lap branches off through an inner slower route.
// - Road is wide enough for smooth arcade movement.
// - Decoration tiles sit outside the driveable road.

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

// Draw a filled circular brush in tile coordinates.
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

// Draw a thick road line between two tile points.
static void RaceTrack_DrawRoadLine(RaceTrack *track, int x0, int y0, int x1,
                                   int y1, int radius, RaceTileType tile) {
  int dx = x1 - x0;
  int dy = y1 - y0;
  int steps = 0;
  int i = 0;

  steps = (dx < 0) ? -dx : dx;

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

// Draw a rectangle in tile coordinates.
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

// Add red/white curbs wherever road touches sand/grass.
static bool RaceTrack_TileIsTrackSurface(uint8_t tile) {
  return ((tile == (uint8_t)RACE_TILE_ROAD) ||
          (tile == (uint8_t)RACE_TILE_JOKER));
}

static bool RaceTrack_TileNeedsCurb(uint8_t tile) {
  return ((tile == (uint8_t)RACE_TILE_SAND) ||
          (tile == (uint8_t)RACE_TILE_GRASS) ||
          (tile == (uint8_t)RACE_TILE_TYRE) ||
          (tile == (uint8_t)RACE_TILE_BARRIER));
}

static void RaceTrack_AddCurbs(RaceTrack *track) {
  uint16_t y = 0U;
  uint16_t x = 0U;

  if (track == NULL) {
    return;
  }

  for (y = 1U; y < (RACE_WORLD_TILES_Y - 1U); y++) {
    for (x = 1U; x < (RACE_WORLD_TILES_X - 1U); x++) {
      if (RaceTrack_TileIsTrackSurface(track->tiles[y][x])) {
        bool near_edge = false;

        if (RaceTrack_TileNeedsCurb(track->tiles[y - 1U][x]) ||
            RaceTrack_TileNeedsCurb(track->tiles[y + 1U][x]) ||
            RaceTrack_TileNeedsCurb(track->tiles[y][x - 1U]) ||
            RaceTrack_TileNeedsCurb(track->tiles[y][x + 1U])) {
          near_edge = true;
        }

        if (near_edge == true) {
          if (((x + y) % 2U) == 0U) {
            track->tiles[y][x] = (uint8_t)RACE_TILE_CURB_RED;
          } else {
            track->tiles[y][x] = (uint8_t)RACE_TILE_CURB_WHITE;
          }
        }
      }
    }
  }
}

static void RaceTrack_AddRunOffSand(RaceTrack *track) {
  if (track == NULL) {
    return;
  }

  RaceTrack_DrawDisc(track, 16, 17, 5, RACE_TILE_SAND);
  RaceTrack_DrawDisc(track, 79, 24, 6, RACE_TILE_SAND);
  RaceTrack_DrawDisc(track, 59, 45, 5, RACE_TILE_SAND);
  RaceTrack_DrawDisc(track, 27, 48, 4, RACE_TILE_SAND);
  RaceTrack_DrawDisc(track, 37, 31, 4, RACE_TILE_SAND);
  RaceTrack_DrawDisc(track, 70, 34, 4, RACE_TILE_SAND);
}

static void RaceTrack_AddScenery(RaceTrack *track) {
  if (track == NULL) {
    return;
  }

  RaceTrack_DrawTileRect(track, 3, 4, 8, 4, RACE_TILE_BUILDING);
  RaceTrack_DrawTileRect(track, 5, 53, 12, 4, RACE_TILE_BUILDING);
  RaceTrack_DrawTileRect(track, 76, 6, 8, 3, RACE_TILE_BUILDING);

  RaceTrack_DrawTileRect(track, 17, 18, 1, 5, RACE_TILE_TYRE);
  RaceTrack_DrawTileRect(track, 76, 24, 6, 1, RACE_TILE_TYRE);
  RaceTrack_DrawTileRect(track, 56, 45, 1, 6, RACE_TILE_TYRE);
  RaceTrack_DrawTileRect(track, 28, 46, 6, 1, RACE_TILE_TYRE);
  RaceTrack_DrawTileRect(track, 36, 31, 1, 5, RACE_TILE_TYRE);
  RaceTrack_DrawTileRect(track, 69, 33, 5, 1, RACE_TILE_TYRE);

  RaceTrack_DrawTileRect(track, 42, 13, 5, 1, RACE_TILE_BARRIER);
  RaceTrack_DrawTileRect(track, 45, 34, 8, 1, RACE_TILE_BARRIER);
  RaceTrack_DrawTileRect(track, 70, 30, 6, 1, RACE_TILE_BARRIER);
}

static void RaceTrack_BuildMap(RaceTrack *track) {
  if (track == NULL) {
    return;
  }

  RaceTrack_Fill(track, RACE_TILE_GRASS);
  RaceTrack_AddRunOffSand(track);

  RaceTrack_DrawRoadLine(track, 12, 50, 12, 15, 5, RACE_TILE_ROAD);
  RaceTrack_DrawRoadLine(track, 12, 15, 23, 8, 5, RACE_TILE_ROAD);
  RaceTrack_DrawRoadLine(track, 23, 8, 45, 7, 5, RACE_TILE_ROAD);
  RaceTrack_DrawRoadLine(track, 45, 7, 62, 10, 5, RACE_TILE_ROAD);
  RaceTrack_DrawRoadLine(track, 62, 10, 78, 17, 5, RACE_TILE_ROAD);
  RaceTrack_DrawRoadLine(track, 78, 17, 81, 25, 5, RACE_TILE_ROAD);
  RaceTrack_DrawRoadLine(track, 81, 25, 73, 31, 5, RACE_TILE_ROAD);
  RaceTrack_DrawRoadLine(track, 73, 31, 60, 30, 5, RACE_TILE_ROAD);
  RaceTrack_DrawRoadLine(track, 60, 30, 54, 35, 5, RACE_TILE_ROAD);
  RaceTrack_DrawRoadLine(track, 54, 35, 61, 43, 5, RACE_TILE_ROAD);
  RaceTrack_DrawRoadLine(track, 61, 43, 58, 52, 5, RACE_TILE_ROAD);
  RaceTrack_DrawRoadLine(track, 58, 52, 43, 54, 5, RACE_TILE_ROAD);
  RaceTrack_DrawRoadLine(track, 43, 54, 28, 51, 5, RACE_TILE_ROAD);
  RaceTrack_DrawRoadLine(track, 28, 51, 18, 45, 5, RACE_TILE_ROAD);
  RaceTrack_DrawRoadLine(track, 18, 45, 12, 50, 5, RACE_TILE_ROAD);

  RaceTrack_DrawRoadLine(track, 24, 13, 24, 5, 4, RACE_TILE_JOKER);
  RaceTrack_DrawRoadLine(track, 24, 5, 42, 3, 4, RACE_TILE_JOKER);
  RaceTrack_DrawRoadLine(track, 42, 3, 64, 4, 4, RACE_TILE_JOKER);
  RaceTrack_DrawRoadLine(track, 64, 4, 82, 8, 4, RACE_TILE_JOKER);
  RaceTrack_DrawRoadLine(track, 82, 8, 86, 16, 4, RACE_TILE_JOKER);
  RaceTrack_DrawRoadLine(track, 86, 16, 84, 23, 4, RACE_TILE_JOKER);
  RaceTrack_DrawRoadLine(track, 84, 23, 78, 25, 4, RACE_TILE_JOKER);

  RaceTrack_AddCurbs(track);
  RaceTrack_AddScenery(track);

  RaceTrack_DrawTileRect(track, 6, 43, 14, 3, RACE_TILE_START);
  RaceTrack_DrawTileRect(track, 6, 17, 14, 3, RACE_TILE_CHECKPOINT);
  RaceTrack_DrawTileRect(track, 73, 23, 14, 3, RACE_TILE_CHECKPOINT);
  RaceTrack_DrawTileRect(track, 54, 45, 14, 3, RACE_TILE_CHECKPOINT);

  RaceTrack_DrawTileRect(track, 19, 10, 15, 3, RACE_TILE_JOKER);
  RaceTrack_DrawTileRect(track, 75, 17, 14, 3, RACE_TILE_JOKER);

}
static bool RaceTrack_PointInZone(float x, float y,
                                  const RaceTriggerZone *zone) {
  if (zone == NULL) {
    return false;
  }

  return ((x >= (float)zone->x) && (x <= (float)(zone->x + zone->width)) &&
          (y >= (float)zone->y) && (y <= (float)(zone->y + zone->height)));
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

  track->joker_lap_done = false;
  track->joker_lap_required = true;
  track->joker_lap_active = false;

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
    return RACE_TILE_SAND;
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

  return hits >= 2U;
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

  if ((world_x - track->camera_x) >= 0.0f) {
    return (int16_t)((world_x - track->camera_x) + 0.5f);
  }

  return (int16_t)((world_x - track->camera_x) - 0.5f);
}

int16_t RaceTrack_WorldToScreenY(const RaceTrack *track, float world_y) {
  if (track == NULL) {
    return (int16_t)world_y;
  }

  if ((world_y - track->camera_y) >= 0.0f) {
    return (int16_t)((world_y - track->camera_y) + 0.5f);
  }

  return (int16_t)((world_y - track->camera_y) - 0.5f);
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
  float center_x = 0.0f;
  float center_y = 0.0f;

  if ((track == NULL) || (track->race_finished == true)) {
    return;
  }

  center_x = car_x + ((float)car_width * 0.5f);
  center_y = car_y + ((float)car_height * 0.5f);

  if ((track->joker_lap_done == false) && (track->joker_lap_active == false) &&
      (track->next_checkpoint == 2U) &&
      RaceTrack_PointInZone(center_x, center_y, &g_joker_start_zone)) {
    track->joker_lap_active = true;
  }

  if (track->joker_lap_active == true) {
    if (RaceTrack_PointInZone(center_x, center_y, &g_joker_end_zone)) {
      track->joker_lap_done = true;
      track->joker_lap_active = false;
    } else {
      return;
    }
  }

  if ((track->next_checkpoint >= 1U) &&
      (track->next_checkpoint <= RACE_CHECKPOINT_COUNT)) {
    uint8_t checkpoint_index = (uint8_t)(track->next_checkpoint - 1U);

    if (RaceTrack_PointInZone(center_x, center_y,
                              &g_checkpoint_zones[checkpoint_index])) {
      track->next_checkpoint++;

      if (track->next_checkpoint > RACE_CHECKPOINT_COUNT) {
        track->next_checkpoint = 0U;
      }
    }

    return;
  }

  if (RaceTrack_PointInZone(center_x, center_y, &g_start_zone)) {
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
  if (track == NULL) {
    return 1U;
  }

  return track->current_lap;
}

uint8_t RaceTrack_GetTotalLaps(const RaceTrack *track) {
  if (track == NULL) {
    return RACE_TOTAL_LAPS;
  }

  return track->total_laps;
}

uint8_t RaceTrack_GetNextCheckpoint(const RaceTrack *track) {
  if (track == NULL) {
    return 1U;
  }

  return track->next_checkpoint;
}

uint8_t RaceTrack_GetCheckpointCount(const RaceTrack *track) {
  if (track == NULL) {
    return RACE_CHECKPOINT_COUNT;
  }

  return track->checkpoint_count;
}

uint32_t RaceTrack_GetElapsedMs(const RaceTrack *track) {
  if (track == NULL) {
    return 0U;
  }

  return track->elapsed_ms;
}

bool RaceTrack_IsFinished(const RaceTrack *track) {
  if (track == NULL) {
    return false;
  }

  return track->race_finished;
}

bool RaceTrack_IsJokerLapDone(const RaceTrack *track) {
  if (track == NULL) {
    return false;
  }

  return track->joker_lap_done;
}

bool RaceTrack_IsJokerLapRequired(const RaceTrack *track) {
  if (track == NULL) {
    return true;
  }

  return track->joker_lap_required;
}

RaceSector RaceTrack_GetCurrentSector(const RaceTrack *track) {
  uint8_t cp = 1U;

  if (track == NULL) {
    return RACE_SECTOR_1;
  }

  cp = track->next_checkpoint;

  if (cp <= 1U) {
    return RACE_SECTOR_1;
  }

  if (cp == 2U) {
    return RACE_SECTOR_2;
  }

  return RACE_SECTOR_3;
}

const char *RaceTrack_GetCurrentCornerName(const RaceTrack *track) {
  if (track == NULL) {
    return "UNKNOWN";
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

RaceTriggerZone RaceTrack_GetJokerZone(void) { return g_joker_zone; }

RaceTriggerZone RaceTrack_GetJokerStartZone(void) {
  return g_joker_start_zone;
}

RaceTriggerZone RaceTrack_GetJokerEndZone(void) {
  return g_joker_end_zone;
}

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