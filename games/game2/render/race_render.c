#include "race_render.h"
#include "LCD.h"

extern ST7789V2_cfg_t cfg0;

void RaceRender_DrawFrame(const RaceCar *player_car) {
  if (player_car == NULL) {
    return;
  }

  LCD_Fill_Buffer(0);

  LCD_printString("Game 2 - Player Test", 5, 5, 1, 1);

  LCD_printString("[]", (uint16_t)player_car->x, (uint16_t)player_car->y, 1, 2);

  LCD_Refresh(&cfg0);
}