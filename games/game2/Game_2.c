#include "Game_2.h"

#include "car/race_car.h"
#include "input/race_input.h"
#include "render/race_render.h"

#include "InputHandler.h"
#include <stdbool.h>

#define GAME2_SCREEN_WIDTH 240
#define GAME2_SCREEN_HEIGHT 240

static RaceCar g_player_car;
static bool g_game2_should_exit = false;

static void Game2_UpdatePlayer(void) {
  RaceInput input;

  RaceInput_Read(&input);
  RaceCar_Move(&g_player_car, input.move_x, input.move_y);
  RaceCar_ClampToScreen(&g_player_car, GAME2_SCREEN_WIDTH, GAME2_SCREEN_HEIGHT);
}

void game2_init(void) {
  g_game2_should_exit = false;
  RaceCar_Init(&g_player_car);
}

void game2_update(void) {

  Input_Read();

  if (current_input.b1_pressed) {
    g_game2_should_exit = true;
    return;
  }

  Game2_UpdatePlayer();
}

void game2_render(void) { RaceRender_DrawFrame(&g_player_car); }

void game2_shutdown(void) {}

bool Game2_ShouldExit(void) { return g_game2_should_exit; }

const GameApi game2_api = {
    .name = "Game 2",
    .init = game2_init,
    .update = game2_update,
    .render = game2_render,
    .shutdown = game2_shutdown,
    .should_exit = Game2_ShouldExit,
};