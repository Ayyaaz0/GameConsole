#include "Game_2.h"

#include "InputHandler.h"
#include "LCD.h"
#include "stm32l4xx_hal.h"

#include "camera/race_camera.h"
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
static RaceCamera g_camera;
static RaceCollisionState g_collision;
static RaceState g_race_state;

static bool g_game2_should_exit = false;

static void Game2_PositionPlayerAtStart(void) {
  RaceCar_Init(&g_player_car);

  g_player_car.x = RACE_PLAYER_START_X;
  g_player_car.y = RACE_PLAYER_START_Y;

  g_player_car.prev_x = g_player_car.x;
  g_player_car.prev_y = g_player_car.y;

  g_player_car.vx = 0.0f;
  g_player_car.vy = 0.0f;
  g_player_car.speed = 0.0f;

  g_player_car.heading_deg = 0.0f;
  g_player_car.prev_heading_deg = 0.0f;
}

static void Game2_ResetRace(void) {
  RaceTrack_Init(&g_track, RACE_SCREEN_WIDTH, RACE_SCREEN_HEIGHT);
  RaceTrack_StartTimer(&g_track, HAL_GetTick());

  Game2_PositionPlayerAtStart();

  RaceCamera_Init(&g_camera);
  RaceCamera_Reset(&g_camera, &g_player_car);

  RaceTrack_SetCamera(&g_track, g_camera.x, g_camera.y);

  RaceCollision_Reset(&g_collision);
  RaceState_Init(&g_race_state);
}

void game2_init(void) {
  g_game2_should_exit = false;

  Game2_ResetRace();

  LCD_Fill_Buffer(0U);
  LCD_Refresh(&cfg0);
}

static void Game2_UpdateRunningRace(void) {
  RaceInput input = {0};

  RaceInput_Read(&input);

  RaceCar_UpdatePhysics(&g_player_car, &input);

  RaceCar_ClampToWorld(&g_player_car, RACE_WORLD_WIDTH_PX,
                       RACE_WORLD_HEIGHT_PX);

  RaceCollision_HandleRoadEdges(&g_collision, &g_player_car, &g_track);

  RaceTrack_UpdateProgress(&g_track, g_player_car.x, g_player_car.y,
                           g_player_car.width, g_player_car.height);

  RaceTrack_UpdateTimer(&g_track, HAL_GetTick());

  RaceCamera_Update(&g_camera, &g_player_car, &g_track);
}

static void Game2_UpdateFinishedRace(void) {
  RaceInput input = {0};

  RaceInput_Read(&input);

  g_player_car.vx *= 0.90f;
  g_player_car.vy *= 0.90f;

  g_player_car.x += g_player_car.vx;
  g_player_car.y += g_player_car.vy;

  RaceCar_ClampToWorld(&g_player_car, RACE_WORLD_WIDTH_PX,
                       RACE_WORLD_HEIGHT_PX);

  if (input.brake > 0.0f) {
    Game2_ResetRace();
  }

  RaceCamera_Update(&g_camera, &g_player_car, &g_track);
}

void game2_update(void) {
  if (current_input.b1_pressed) {
    g_game2_should_exit = true;
    return;
  }

  if (RaceState_IsCrashed(&g_race_state)) {
    Game2_ResetRace();
    return;
  }

  if (RaceTrack_IsFinished(&g_track)) {
    Game2_UpdateFinishedRace();
    return;
  }

  Game2_UpdateRunningRace();
}

void game2_render(void) {
  RaceRender_DrawFrame(&g_track, &g_player_car, &g_camera);
}

void game2_shutdown(void) {
  LCD_Fill_Buffer(0U);
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