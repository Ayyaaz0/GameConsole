#include "Game_2.h"

#include "InputHandler.h"
#include "LCD.h"

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

static float g_track_scroll_remainder = 0.0f;

// Put the player at a sensible starting spot on the track.
static void Game2_ResetPlayerToTrackStart(void) {
  RaceCar_Init(&g_player_car);

  g_player_car.x =
      (float)(RaceTrack_GetCenterX(&g_track) - (g_player_car.width / 2U));
  g_player_car.y = RACE_PLAYER_START_Y;
}

static void Game2_UpdateTrackFromCarSpeed(void) {
  int32_t scroll_amount = 0;

  // Keep fractional speed so slow speeds still eventually scroll the road
  g_track_scroll_remainder +=
      (g_player_car.speed > 0.0f ? g_player_car.speed : 0.0f);

  scroll_amount = (int32_t)g_track_scroll_remainder;

  if (scroll_amount != 0) {
    RaceTrack_Advance(&g_track, scroll_amount);
    g_track_scroll_remainder -= (float)scroll_amount;
  }
}
// Update the player from input.
static void Game2_UpdatePlayer(void) {
  RaceInput input = {0};

  RaceInput_Read(&input);
  RaceCar_UpdatePhysics(&g_player_car, &input);
}

void game2_init(void) {
  g_game2_should_exit = false;

  RaceTrack_Init(&g_track, RACE_SCREEN_WIDTH, RACE_SCREEN_HEIGHT);
  Game2_ResetPlayerToTrackStart();
  RaceCamera_Init(&g_camera);
  g_track_scroll_remainder = 0.0f;

  RaceCollision_Reset(&g_collision);
  RaceState_Init(&g_race_state);

  LCD_Fill_Buffer(0);
  LCD_Refresh(&cfg0);
}

static void Game2_ResetRace(void)
{
    RaceTrack_Init(&g_track, RACE_SCREEN_WIDTH, RACE_SCREEN_HEIGHT);
    Game2_ResetPlayerToTrackStart();
    RaceState_Init(&g_race_state);
    g_track_scroll_remainder = 0.0f;
}

static void Game2_CheckCrashFromCollision(float impact_speed)
{
    if ((g_collision.hit_any_edge == true) &&
        (impact_speed >= RACE_CRASH_SPEED_THRESHOLD))
    {
        RaceState_SetCrashed(&g_race_state);

        g_player_car.speed = 0.0f;
        g_player_car.heading_deg = 0.0f;
    }
}

void game2_update(void) {
  if (current_input.b1_pressed) {
    g_game2_should_exit = true;
    return;
  }

  if (RaceState_IsCrashed(&g_race_state)) {
    // Restart: push joystick down to restart.
    RaceInput restart_input = {0};
    RaceInput_Read(&restart_input);

    if (restart_input.brake > 0.0f) {
      Game2_ResetRace();
    }

    return;
  }

  float impact_speed = 0.0f;

  Game2_UpdatePlayer();

  impact_speed = g_player_car.speed;
  if (impact_speed < 0.0f)
  {
     impact_speed = -impact_speed;
  }

  RaceCollision_HandleRoadEdges(&g_collision, &g_player_car, &g_track);
  Game2_CheckCrashFromCollision(impact_speed);
  Game2_UpdateTrackFromCarSpeed();
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