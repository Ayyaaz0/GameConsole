#include "game_registry.h"

#include "game1/Game_1.h"
#include "game2/Game_2.h"
#include "game3/Game_3.h"

const GameApi *game_list[] = {
    &game1_api,
    &game2_api,
    &game3_api,
};

const int game_count = 3;