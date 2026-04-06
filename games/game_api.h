#ifndef GAME_API_H
#define GAME_API_H

#include <stdbool.h>

typedef struct
{
    const char *name;
    void (*init)(void);
    void (*update)(void);
    void (*render)(void);
    void (*shutdown)(void);
    bool (*should_exit)(void);
} GameApi;

#endif