#include "Game_2.h"

#include "InputHandler.h"
#include "LCD.h"

#include "car/race_car.h"
#include "collisions/race_collisions.h"
#include "config/race_config.h"
#include "input/race_input.h"
#include "render/race_render.h"
#include "state/race_state.h"
#include "track/race_track.h"

#include <stdbool.h>

extern ST7789V2_cfg_t cfg0;

static RaceCar g_player_car;
static RaceTrack g_track;
static RaceCollisionState g_collision;
static RaceState g_race_state;
static bool g_game2_should_exit = false;

static void Game2_PositionCarAtCurrentTileSpawn(void) {
  g_player_car.x = RaceTrack_GetSpawnX(&g_track);
  g_player_car.y = RaceTrack_GetSpawnY(&g_track);
  g_player_car.prev_x = g_player_car.x;
  g_player_car.prev_y = g_player_car.y;
  g_player_car.heading_deg = RaceTrack_GetSpawnHeadingDeg(&g_track);
  g_player_car.prev_heading_deg = g_player_car.heading_deg;
  g_player_car.speed = 0.0f;
}

static void Game2_ResetRace(void) {
  RaceTrack_ResetToStart(&g_track);
  RaceCar_Init(&g_player_car);
  Game2_PositionCarAtCurrentTileSpawn();

  RaceCollision_Reset(&g_collision);
  RaceState_Init(&g_race_state);
}

static bool Game2_CarReachedCorrectExit(void) {
  RaceTrackEdge exit_edge = RaceTrack_GetCurrentExitEdge(&g_track);

  switch (exit_edge) {
  case RACE_EDGE_TOP:
    return g_player_car.y <= 24.0f;

  case RACE_EDGE_RIGHT:
    return (g_player_car.x + (float)g_player_car.width) >=
           ((float)RACE_SCREEN_WIDTH - 2.0f);

  case RACE_EDGE_BOTTOM:
    return (g_player_car.y + (float)g_player_car.height) >=
           ((float)RACE_SCREEN_HEIGHT - 2.0f);

  case RACE_EDGE_LEFT:
    return g_player_car.x <= 2.0f;

  default:
    return false;
  }
}

static void Game2_MoveToNextTile(void) {
  RaceTrack_GoToNextScreen(&g_track);
  Game2_PositionCarAtCurrentTileSpawn();
}

static void Game2_CheckCrashFromCollision(float impact_speed) {
  if ((g_collision.hit_any_edge == true) &&
      (impact_speed >= RACE_CRASH_SPEED_THRESHOLD)) {
    RaceState_SetCrashed(&g_race_state);

    g_player_car.speed = 0.0f;
  }
}

void game2_init(void) {
  g_game2_should_exit = false;

  RaceTrack_Init(&g_track, RACE_SCREEN_WIDTH, RACE_SCREEN_HEIGHT);
  RaceCar_Init(&g_player_car);
  Game2_PositionCarAtCurrentTileSpawn();

  RaceCollision_Reset(&g_collision);
  RaceState_Init(&g_race_state);

  LCD_Fill_Buffer(0);
  LCD_Refresh(&cfg0);
}

void game2_update(void) {
  RaceInput input = {0};
  float impact_speed = 0.0f;

  if (current_input.b1_pressed) {
    g_game2_should_exit = true;
    return;
  }

  if (RaceState_IsCrashed(&g_race_state)) {
    RaceInput_Read(&input);

    if (input.brake > 0.0f) {
      Game2_ResetRace();
    }

    return;
  }

  RaceInput_Read(&input);
  RaceCar_UpdatePhysics(&g_player_car, &input);

  impact_speed = g_player_car.speed;

  if (impact_speed < 0.0f) {
    impact_speed = -impact_speed;
  }

  if (Game2_CarReachedCorrectExit()) {
    Game2_MoveToNextTile();
    return;
  }

  RaceCollision_HandleRoadEdges(&g_collision, &g_player_car, &g_track);
  Game2_CheckCrashFromCollision(impact_speed);
}

void game2_render(void) { RaceRender_DrawFrame(&g_track, &g_player_car); }

void game2_shutdown(void) {
  LCD_Fill_Buffer(0);
  LCD_Refresh(&cfg0);
}

bool Game2_ShouldExit(void) { return g_game2_should_exit; }

const GameApi game2_api = {
    .name = "Game 2",
    .init = game2_init,
    .update = game2_update,
    .render = game2_render,
    .shutdown = game2_shutdown,
    .should_exit = Game2_ShouldExit,
};