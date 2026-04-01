#ifndef GAME_API_H
#define GAME_API_H

typedef struct
{
    const char *name;
    void (*init)(void);
    void (*update)(void);
    void (*render)(void);
    void (*exit)(void);
} GameApi;

#endif
