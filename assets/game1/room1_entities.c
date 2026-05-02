#include "room1_entities.h"

const Game1_Entity room1_entities[] = {
  { ENTITY_DOOR, 460, 185, 16, 16, 0, 1, 0, 30065, 30066, 30067, 0 },
  { ENTITY_KEY, 152, 112, 8, 8, 0, 0, 30426, 0, 0, 0, 0 },
  { ENTITY_SPAWN, 4, 185, 8, 8, 0, 0, 0, 0, 0, 0, 0 },
  { ENTITY_COIN, 208, 192, 8, 8, 0, 0, 30429, 0, 0, 0, 1 },
  { ENTITY_COIN, 216, 192, 8, 8, 0, 0, 30429, 0, 0, 0, 1 },
  { ENTITY_COIN, 288, 192, 8, 8, 0, 0, 30429, 0, 0, 0, 1 },
  { ENTITY_COIN, 296, 192, 8, 8, 0, 0, 30429, 0, 0, 0, 1 },
  { ENTITY_COIN, 272, 144, 8, 8, 0, 0, 30429, 0, 0, 0, 1 },
};

const uint16_t room1_entity_count = 8;
