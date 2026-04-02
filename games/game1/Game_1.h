#ifndef GAME_1_H
#define GAME_1_H

#include <stdbool.h>
#include "game_api.h"
#include "Menu.h"

extern const GameApi game1_api;

bool Game1_ShouldExit(void);
MenuState Game1_Run(void);

#endif