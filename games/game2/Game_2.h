#ifndef GAME_2_H
#define GAME_2_H

#include "game_api.h"
#include <stdbool.h>

extern const GameApi game2_api;

void game2_init(void);
void game2_update(void);
void game2_render(void);
void game2_shutdown(void);
bool Game2_ShouldExit(void);

#endif