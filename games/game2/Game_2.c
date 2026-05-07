#include "Game_2.h"

#include "Buzzer.h"
#include "InputHandler.h"
#include "LCD.h"
#include "stm32l4xx_hal.h"

#include "boost/race_boost.h"
#include "camera/race_camera.h"
#include "car/race_car.h"
#include "collisions/race_collisions.h"
#include "config/race_config.h"
#include "input/race_input.h"
#include "render/race_render.h"
#include "state/race_state.h"
#include "track/race_track.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

extern ST7789V2_cfg_t cfg0;
extern Buzzer_cfg_t buzzer_cfg;

static RaceCar g_player_car;
static RaceTrack g_track;
static RaceCamera g_camera;
static RaceCollisionState g_collision;
static RaceState g_race_state;
static RaceBoostState g_boost;

static bool g_game2_should_exit = false;
static bool g_game2_sfx_active = false;
static bool g_finish_sfx_played = false;
static uint32_t g_game2_sfx_stop_ms = 0U;
static uint32_t g_crash_started_ms = 0U;
static uint32_t g_lap_started_ms = 0U;
static uint32_t g_best_lap_ms = 0U;

#define GAME2_FLASH_MESSAGE_MS 800U

static char g_flash_message[24] = {0};
static uint32_t g_flash_message_until_ms = 0U;
static float Game2_AbsFloat(float value) {
  return (value < 0.0f) ? -value : value;
}

static float Game2_GetCarImpactSpeed(const RaceCar *car) {
  float abs_vx = 0.0f;
  float abs_vy = 0.0f;

  if (car == NULL) {
    return 0.0f;
  }

  abs_vx = Game2_AbsFloat(car->vx);
  abs_vy = Game2_AbsFloat(car->vy);

  return (abs_vx > abs_vy) ? abs_vx : abs_vy;
}

static void Game2_StopSfx(void) {
  buzzer_off(&buzzer_cfg);
  g_game2_sfx_active = false;
  g_game2_sfx_stop_ms = 0U;
}

static void Game2_PlayTone(uint32_t frequency_hz, uint32_t duration_ms) {
  buzzer_tone(&buzzer_cfg, frequency_hz, 3U);
  g_game2_sfx_active = true;
  g_game2_sfx_stop_ms = HAL_GetTick() + duration_ms;
}

static void Game2_UpdateSfx(void) {
  if (g_game2_sfx_active == false) {
    return;
  }

  if (HAL_GetTick() >= g_game2_sfx_stop_ms) {
    Game2_StopSfx();
  }
}

static void Game2_ResetLapTiming(void) {
  g_lap_started_ms = HAL_GetTick();
  g_best_lap_ms = 0U;
}

static void Game2_RecordCompletedLap(void) {
  uint32_t now_ms = HAL_GetTick();
  uint32_t lap_ms = 0U;

  if (g_lap_started_ms == 0U) {
    g_lap_started_ms = now_ms;
    return;
  }

  lap_ms = now_ms - g_lap_started_ms;

  // Ignore impossibly short laps caused by accidental trigger overlap
  if (lap_ms < 1500U) {
    return;
  }

  if ((g_best_lap_ms == 0U) || (lap_ms < g_best_lap_ms)) {
    g_best_lap_ms = lap_ms;
  }

  g_lap_started_ms = now_ms;
}

static void Game2_EnterCrashState(void) {
  if (RaceState_IsCrashed(&g_race_state)) {
    return;
  }

  RaceState_SetCrashed(&g_race_state);

  // Freeze the car exactly where the heavy impact happened
  g_player_car.vx = 0.0f;
  g_player_car.vy = 0.0f;
  g_player_car.speed = 0.0f;

  g_crash_started_ms = HAL_GetTick();
  Game2_PlayTone(160U, 260U);
}

static bool Game2_CrashRestartAllowed(void) {
  return ((HAL_GetTick() - g_crash_started_ms) >= RACE_CRASH_RESTART_DELAY_MS);
}

static void Game2_PositionPlayerAtStart(void) {
  RaceCar_Init(&g_player_car);

  g_player_car.x = RACE_PLAYER_START_X;
  g_player_car.y = RACE_PLAYER_START_Y;
  g_player_car.prev_x = g_player_car.x;
  g_player_car.prev_y = g_player_car.y;
  g_player_car.vx = 0.0f;
  g_player_car.vy = 0.0f;
  g_player_car.speed = 0.0f;
}

// Flash-message helper prototypes
static void Game2_ClearFlashMessage(void);
static void Game2_ShowFlashMessage(const char *message, uint32_t duration_ms);
static const char *Game2_GetFlashMessage(void);

static void Game2_ResetRace(void) {
  RaceTrack_Init(&g_track, RACE_SCREEN_WIDTH, RACE_SCREEN_HEIGHT);
  RaceTrack_StartTimer(&g_track, HAL_GetTick());

  Game2_PositionPlayerAtStart();

  RaceCamera_Init(&g_camera);
  RaceCamera_Reset(&g_camera, &g_player_car);
  RaceTrack_SetCamera(&g_track, g_camera.x, g_camera.y);

  RaceCollision_Reset(&g_collision);
  RaceState_Init(&g_race_state);

  g_flash_message[0] = '\0';
  g_flash_message_until_ms = 0U;
  g_crash_started_ms = 0U;
  g_finish_sfx_played = false;
  Game2_ClearFlashMessage();
  Game2_ResetLapTiming();
  Game2_StopSfx();
}

static void Game2_ClearFlashMessage(void) {
  g_flash_message[0] = '\0';
  g_flash_message_until_ms = 0U;
}

static void Game2_ShowFlashMessage(const char *message, uint32_t duration_ms) {
  if (message == NULL) {
    Game2_ClearFlashMessage();
    return;
  }

  snprintf(g_flash_message, sizeof(g_flash_message), "%s", message);
  g_flash_message_until_ms = HAL_GetTick() + duration_ms;
}

void game2_init(void) {
  g_game2_should_exit = false;
  Game2_ResetRace();

  LCD_Fill_Buffer(0U);
  LCD_Refresh(&cfg0);
  RaceBoost_Init(&g_boost);
}

static const char *Game2_GetFlashMessage(void) {
  if (g_flash_message[0] == '\0') {
    return "";
  }

  if (HAL_GetTick() >= g_flash_message_until_ms) {
    Game2_ClearFlashMessage();
    return "";
  }

  return g_flash_message;
}

static void Game2_UpdateRunningRace(void) {
  RaceInput input = {0};
  float impact_speed = 0.0f;

  uint8_t old_checkpoint = 0U;
  uint8_t new_checkpoint = 0U;
  uint8_t old_lap = 0U;
  uint8_t new_lap = 0U;

  bool old_joker_active = false;
  bool new_joker_active = false;
  bool old_joker_done = false;
  bool new_joker_done = false;
  bool old_finished = false;
  bool new_finished = false;

  char message[24];

  RaceInput_Read(&input);

  RaceCar_UpdatePhysics(&g_player_car, &input);
  RaceBoost_Update(&g_boost, &g_player_car, &input, &g_track, HAL_GetTick());
  RaceCar_ClampToWorld(&g_player_car);

  // Capture speed before collision response slows the car down
  impact_speed = Game2_GetCarImpactSpeed(&g_player_car);

  RaceCollision_HandleRoadEdges(&g_collision, &g_player_car, &g_track);

  if ((g_collision.hit_any_edge == true) &&
      (impact_speed >= RACE_CRASH_SPEED_THRESHOLD)) {
    Game2_EnterCrashState();
    RaceCamera_Update(&g_camera, &g_player_car, &g_track);
    return;
  }

  old_checkpoint = RaceTrack_GetNextCheckpoint(&g_track);
  old_lap = RaceTrack_GetCurrentLap(&g_track);
  old_joker_active = RaceTrack_IsJokerLapActive(&g_track);
  old_joker_done = RaceTrack_IsJokerLapDone(&g_track);
  old_finished = RaceTrack_IsFinished(&g_track);

  RaceTrack_UpdateProgress(&g_track, g_player_car.x, g_player_car.y,
                           g_player_car.width, g_player_car.height);

  new_checkpoint = RaceTrack_GetNextCheckpoint(&g_track);
  new_lap = RaceTrack_GetCurrentLap(&g_track);
  new_joker_active = RaceTrack_IsJokerLapActive(&g_track);
  new_joker_done = RaceTrack_IsJokerLapDone(&g_track);
  new_finished = RaceTrack_IsFinished(&g_track);

  if ((new_finished == true) && (old_finished == false)) {
    Game2_RecordCompletedLap();
    Game2_ShowFlashMessage("FINISH", GAME2_FLASH_MESSAGE_MS);

    if (g_finish_sfx_played == false) {
      g_finish_sfx_played = true;
      Game2_PlayTone(900U, 220U);
    }

  } else if ((old_joker_done == false) && (new_joker_done == true)) {
    Game2_ShowFlashMessage("JOKER DONE", GAME2_FLASH_MESSAGE_MS);
    Game2_PlayTone(760U, 120U);

  } else if ((old_joker_active == false) && (new_joker_active == true)) {
    Game2_ShowFlashMessage("JOKER IN", GAME2_FLASH_MESSAGE_MS);
    Game2_PlayTone(620U, 120U);

  } else if (new_lap > old_lap) {
    Game2_RecordCompletedLap();

    snprintf(message, sizeof(message), "LAP %u/%u", new_lap,
             RaceTrack_GetTotalLaps(&g_track));

    Game2_ShowFlashMessage(message, GAME2_FLASH_MESSAGE_MS);
    Game2_PlayTone(820U, 120U);

  } else if (new_checkpoint != old_checkpoint) {
    if ((old_checkpoint >= 1U) && (old_checkpoint <= RACE_CHECKPOINT_COUNT)) {
      snprintf(message, sizeof(message), "CP%u PASS", old_checkpoint);

      Game2_ShowFlashMessage(message, GAME2_FLASH_MESSAGE_MS);
      Game2_PlayTone(700U, 100U);
    }
  }

  RaceTrack_UpdateTimer(&g_track, HAL_GetTick());
  RaceCamera_Update(&g_camera, &g_player_car, &g_track);
}

static void Game2_UpdateCrashedRace(void) {
  RaceInput input = {0};

  RaceInput_Read(&input);

  // Keep camera locked to the crashed car so the overlay appears over the scene
  RaceCamera_Update(&g_camera, &g_player_car, &g_track);

  if ((input.brake > 0.0f) && Game2_CrashRestartAllowed()) {
    Game2_ResetRace();
  }
}

static void Game2_UpdateFinishedRace(void) {
  RaceInput input = {0};

  RaceInput_Read(&input);

  g_player_car.vx *= 0.90f;
  g_player_car.vy *= 0.90f;
  g_player_car.x += g_player_car.vx;
  g_player_car.y += g_player_car.vy;

  RaceCar_ClampToWorld(&g_player_car);

  if (input.brake > 0.0f) {
    Game2_ResetRace();
  }

  RaceCamera_Update(&g_camera, &g_player_car, &g_track);
}

void game2_update(void) {
  Game2_UpdateSfx();

  if (current_input.b1_pressed) {
    Game2_StopSfx();
    g_game2_should_exit = true;
    return;
  }

  if (RaceState_IsCrashed(&g_race_state)) {
    Game2_UpdateCrashedRace();
    return;
  }

  if (RaceTrack_IsFinished(&g_track)) {
    Game2_UpdateFinishedRace();
    return;
  }

  Game2_UpdateRunningRace();
}

void game2_render(void) {
  RaceRender_DrawFrame(&g_track, &g_player_car, &g_camera, &g_race_state,
                       Game2_GetFlashMessage(), g_best_lap_ms, &g_boost);
}

void game2_shutdown(void) {
  Game2_StopSfx();
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