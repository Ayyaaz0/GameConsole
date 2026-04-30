#ifndef ROOM0_ENTITIES_H
#define ROOM0_ENTITIES_H

#include <stdint.h>

#define ROOM0_ENTITY_COUNT 7

typedef enum {
  ENTITY_UNKNOWN,
  ENTITY_SPAWN,
  ENTITY_KEY,
  ENTITY_DOOR,
  ENTITY_COIN
} Game1_EntityType;

typedef struct {
  Game1_EntityType type;
  int16_t x;
  int16_t y;
  uint8_t w;
  uint8_t h;
  uint8_t key_id;
  uint8_t locked;
  uint16_t sprite_gid;
  uint16_t closed_gid;
  uint16_t opening_gid;
  uint16_t open_gid;
  uint8_t value;
} Game1_Entity;

extern const Game1_Entity room0_entities[];
extern const uint16_t room0_entity_count;

#endif