#include "game_registry.h"

/* These will be defined by each game source file */
extern const GameApi game1_api;
extern const GameApi game2_api;
extern const GameApi game3_api;

const GameApi* game_list[] = {
    &game1_api,
    &game2_api,
    &game3_api
};

const int game_count = 3;