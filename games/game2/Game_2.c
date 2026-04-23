#include "Game_2.h"

#include "InputHandler.h"
#include "LCD.h"

#include "car/race_car.h"
#include "config/race_config.h"
#include "input/race_input.h"
#include "render/race_render.h"
#include "track/race_track.h"

#include <stdbool.h>

extern ST7789V2_cfg_t cfg0;

static RaceCar g_player_car;
static RaceTrack g_track;
static bool g_game2_should_exit = false;

// Put the player at a sensible starting spot on the track.
static void Game2_ResetPlayerToTrackStart(void) {
  RaceCar_Init(&g_player_car);

  g_player_car.x =
      (float)(RaceTrack_GetCenterX(&g_track) - (g_player_car.width / 2U));
  g_player_car.y = RACE_PLAYER_START_Y;
}

// Keep the player inside the road width.
static void Game2_ClampPlayerToTrack(void) {
  int16_t min_x = 0;
  int16_t max_x = 0;

  RaceTrack_GetDriveBounds(&g_track, &min_x, &max_x);

  RaceCar_ClampToHorizontalRange(&g_player_car, (float)min_x,
                                 (float)(max_x - (int16_t)g_player_car.width));

  RaceCar_ClampToScreen(&g_player_car, RACE_SCREEN_WIDTH, RACE_SCREEN_HEIGHT);
}

static void Game2_UpdateTrack(void) {
  // Move the visible road window forward in world space
  RaceTrack_Advance(&g_track, RACE_TRACK_SCROLL_STEP);
}

// Update the player from input.
static void Game2_UpdatePlayer(void) {
  RaceInput input = {0};

  RaceInput_Read(&input);
  RaceCar_Move(&g_player_car, input.move_x, input.move_y);
  Game2_ClampPlayerToTrack();
}

void game2_init(void) {
  g_game2_should_exit = false;

  RaceTrack_Init(&g_track, RACE_SCREEN_WIDTH, RACE_SCREEN_HEIGHT);
  Game2_ResetPlayerToTrackStart();

  LCD_Fill_Buffer(0);
  LCD_Refresh(&cfg0);
}

void game2_update(void) {
  // main.c already calls Input_Read() once per frame in the shared game runner.
  // So this function should just consume the latest input state.

  if (current_input.b1_pressed) {
    g_game2_should_exit = true;
    return;
  }

  Game2_UpdateTrack();
  Game2_UpdatePlayer();
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