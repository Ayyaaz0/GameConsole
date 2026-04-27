#ifndef ROOM0_ENTITIES_H
#define ROOM0_ENTITIES_H

#include <stdint.h>

typedef enum {
  ENTITY_SPAWN,
  ENTITY_KEY,
  ENTITY_DOOR
} Game1_EntityType;

typedef struct {
  Game1_EntityType type;
  int16_t x;
  int16_t y;
  uint8_t w;
  uint8_t h;
  uint8_t key_id;
  uint8_t locked;
} Game1_Entity;

extern const Game1_Entity room0_entities[];
extern const uint16_t room0_entity_count;

#endif
