#include "game3_ui.h"

#include "LCD.h"
#include "stm32l4xx_hal.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define GAME3_UI_HEART_START_X      14
#define GAME3_UI_HEART_Y            14
#define GAME3_UI_HEART_RADIUS       5
#define GAME3_UI_HEART_SPACING      16

#define GAME3_UI_ARMOUR_START_X     14
#define GAME3_UI_ARMOUR_Y           30
#define GAME3_UI_ARMOUR_RADIUS      3
#define GAME3_UI_ARMOUR_SPACING     12

#define GAME3_UI_TIME_Y             10
#define GAME3_UI_SCREEN_WIDTH       240

#define GAME3_UI_ABILITY_X  185
#define GAME3_UI_ABILITY_Y  10
#define GAME3_UI_ABILITY_WIDTH  45
#define GAME3_UI_ABILITY_HEIGHT 8
#define GAME3_UI_ABILITY_FILL_WIDTH 30

#define GAME3_UI_ABILITY_BG_COLOUR  13
#define GAME3_UI_ABILITY_FILL_COLOUR    14
#define GAME3_UI_ABILITY_BORDER_COLOUR  1

static uint16_t Game3_UI_Get_Text_Width(const char *text, uint8_t font_size) { 
    return (uint16_t)(strlen(text) * 6 * font_size); 
}

static uint16_t Game3_UI_Get_Centred_X(const char *text, uint8_t font_size) { 
    uint16_t width = Game3_UI_Get_Text_Width(text, font_size); 

    if (width >= GAME3_UI_SCREEN_WIDTH) { 
        return 0; 
    }

    return (GAME3_UI_SCREEN_WIDTH - width) / 2; 
}

static void Game3_UI_Draw_Health(const Game3_Hud *hud) { 
    for (uint8_t i = 0; i < hud->max_health; i++) { 
        uint16_t x = GAME3_UI_HEART_START_X + (i * GAME3_UI_HEART_SPACING); 

        if (i < hud->health) { 
            LCD_Draw_Circle(x, GAME3_UI_HEART_Y, GAME3_UI_HEART_RADIUS, 2, 1); 
        } else { 
            LCD_Draw_Circle(x, GAME3_UI_HEART_Y, GAME3_UI_HEART_RADIUS, 13, 0); 
        }
    }
}

static void Game3_UI_Draw_Armour(const Game3_Hud *hud) { 
    for (uint8_t i = 0; i < hud->max_armour; i++) { 
        uint16_t x = GAME3_UI_ARMOUR_START_X + (i * GAME3_UI_ARMOUR_SPACING); 

        if (i < hud->armour) { 
            LCD_Draw_Circle(x, GAME3_UI_ARMOUR_Y, GAME3_UI_ARMOUR_RADIUS, 4, 1); 
        } 
    }
}

static void Game3_UI_Draw_Time_And_Score(const Game3_Hud *hud) { 
    char buffer[32]; 
    uint32_t elapsed_seconds = (HAL_GetTick() - hud->start_time_ms) / 1000;
    uint32_t score = elapsed_seconds * 10; 

    snprintf(buffer, sizeof(buffer), "Score: %lu", (unsigned long)score);
    
    LCD_printString(buffer, Game3_UI_Get_Centred_X(buffer, 2), GAME3_UI_TIME_Y, 1, 2);
}

void Game3_UI_Draw_Game_Over(const Game3_Hud *hud) { 
    char score_text[32]; 

    snprintf(score_text, sizeof(score_text), "Score: %lu", (unsigned long)hud->final_score);

    LCD_printString("Game Over", Game3_UI_Get_Centred_X("Game Over", 3), 90, 2, 3);
    LCD_printString(score_text, Game3_UI_Get_Centred_X(score_text, 2), 125, 1, 2);
}

static void Game3_UI_Draw_Ability_Meter(const Game3_Hud *hud) { 
    uint8_t ability_fill_width = 0; 

    if (hud->max_ability > 0) { 
        ability_fill_width = (hud->ability * GAME3_UI_ABILITY_WIDTH) / hud->max_ability; 
    }

    LCD_printString("Ability Meter:", GAME3_UI_ABILITY_X, GAME3_UI_ABILITY_Y, 1, 1);

    LCD_Draw_Rect(GAME3_UI_ABILITY_X, GAME3_UI_ABILITY_Y + 10, GAME3_UI_ABILITY_WIDTH, GAME3_UI_ABILITY_HEIGHT, GAME3_UI_ABILITY_BG_COLOUR, 1);

    LCD_Draw_Rect(GAME3_UI_ABILITY_X, GAME3_UI_ABILITY_Y + 10, ability_fill_width, GAME3_UI_ABILITY_HEIGHT, GAME3_UI_ABILITY_FILL_COLOUR, 1);

    LCD_Draw_Rect(GAME3_UI_ABILITY_X, GAME3_UI_ABILITY_Y + 10, GAME3_UI_ABILITY_WIDTH, GAME3_UI_ABILITY_HEIGHT, GAME3_UI_ABILITY_BORDER_COLOUR, 0);
}

void Game3_UI_Draw(const Game3_Hud *hud) { 
    Game3_UI_Draw_Health(hud);
    Game3_UI_Draw_Armour(hud);
    Game3_UI_Draw_Time_And_Score(hud);
    Game3_UI_Draw_Ability_Meter(hud);
}

