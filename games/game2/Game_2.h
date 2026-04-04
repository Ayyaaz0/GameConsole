#ifndef GAME_2_H
#define GAME_2_H

#include <stdbool.h>
#include "game_api.h"
#include "Menu.h"

extern const GameApi game2_api;

bool Game2_ShouldExit(void);
MenuState Game2_Run(void);

#endif