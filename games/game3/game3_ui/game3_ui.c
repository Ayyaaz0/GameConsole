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

    snprintf(buffer, sizeof(buffer), "Time: %lu", (unsigned long)score);
    
    LCD_printString(buffer, Game3_UI_Get_Centred_X(buffer, 2), GAME3_UI_TIME_Y, 1, 2);
}

void Game3_UI_Draw(const Game3_Hud *hud) { 
    Game3_UI_Draw_Health(hud);
    Game3_UI_Draw_Armour(hud);
    Game3_UI_Draw_Time_And_Score(hud);
}

