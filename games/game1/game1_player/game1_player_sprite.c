#include "game1_player_sprite.h"

#include "game1_entity_common.h"
#include "game1_tiles.h"

#include <stdint.h>

#define HERO_ANIM_SPEED 8

#define HERO_IDLE_0_GID 30717
#define HERO_IDLE_1_GID 30788
#define HERO_MOVE_0_GID 30717
#define HERO_MOVE_1_GID 30718
#define HERO_JUMP_0_GID 30717
#define HERO_JUMP_1_GID 30731
#define HERO_DEATH_0_GID 30717
#define HERO_DEATH_1_GID 30801
#define HERO_DEATH_2_GID 30815

static uint32_t animation_counter = 0;
static uint8_t facing_left = 0;

static const uint16_t idle_frames[] = {
    HERO_IDLE_0_GID,
    HERO_IDLE_1_GID,
};

static const uint16_t move_frames[] = {
    HERO_MOVE_0_GID,
    HERO_MOVE_1_GID,
};

static const uint16_t jump_frames[] = {
    HERO_JUMP_0_GID,
    HERO_JUMP_1_GID,
};

static const uint16_t death_frames[] = {
    HERO_DEATH_0_GID,
    HERO_DEATH_1_GID,
    HERO_DEATH_2_GID,
};

void Game1_PlayerSprite_Init(void) {
  animation_counter = 0;
  facing_left = 0;
}

static uint16_t select_frame(const uint16_t *frames, uint8_t frame_count) {
  uint8_t index = (animation_counter / HERO_ANIM_SPEED) % frame_count;
  return frames[index];
}

void Game1_PlayerSprite_Render(const Game1_Player *player,
                               const Game1_Camera *camera) {
  uint16_t gid;

  if (player->vx < 0) {
    facing_left = 1;
  } else if (player->vx > 0) {
    facing_left = 0;
  }

  if (!player->alive) {
    gid = select_frame(death_frames, 3);
  } else if (!player->grounded) {
    gid = select_frame(jump_frames, 2);
  } else if (player->vx != 0) {
    gid = select_frame(move_frames, 2);
  } else {
    gid = select_frame(idle_frames, 2);
  }

  const Game1_TileSprite *sprite = Game1_Tiles_Find(gid);

  Game1_Entity_DrawSprite_Flipped(player->x - camera->x, player->y - camera->y,
                                  sprite, facing_left);

  animation_counter++;
}