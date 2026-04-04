#ifndef GAME_3_H
#define GAME_3_H

#include <stdbool.h>

#include "Menu.h"
#include "game_api.h"

extern const GameApi game3_api;

bool Game3_ShouldExit(void);
MenuState Game3_Run(void);

#endif