#include "game1_sfx.h"

#include "Buzzer.h"
#include "stm32l4xx_hal.h"

#include <stdint.h>

#define GAME1_SFX_VOLUME 1

extern Buzzer_cfg_t buzzer_cfg;

static uint32_t sfx_stop_tick = 0;
static uint8_t sfx_active = 0;

static void Game1_Sfx_PlayTone(uint16_t freq_hz, uint16_t duration_ms) {
  buzzer_tone(&buzzer_cfg, freq_hz, GAME1_SFX_VOLUME);

  sfx_stop_tick = HAL_GetTick() + duration_ms;
  sfx_active = 1;
}

void Game1_Sfx_PlayCoin(void) {
  Game1_Sfx_PlayTone(1200, 60);
}

void Game1_Sfx_PlayDeath(void) {
  Game1_Sfx_PlayTone(180, 180);
}

void Game1_Sfx_PlayKey(void) {
  Game1_Sfx_PlayTone(800, 80);
}

void Game1_Sfx_PlayDoorLocked(void) {
  Game1_Sfx_PlayTone(200, 120);
}

void Game1_Sfx_PlayDoorOpen(void) {
  Game1_Sfx_PlayTone(900, 100);
}

void Game1_Sfx_PlayDoorEnter(void) {
  Game1_Sfx_PlayTone(500, 80);
}

void Game1_Sfx_Update(void) {
  if (!sfx_active) {
    return;
  }

  if (HAL_GetTick() >= sfx_stop_tick) {
    buzzer_off(&buzzer_cfg);
    sfx_active = 0;
  }
}

void Game1_Sfx_Stop(void) {
  buzzer_off(&buzzer_cfg);
  sfx_active = 0;
}