#ifndef GAME_H
#define GAME_H

#include <types.h>

// larger
// #define WINDOW_WIDTH 1216
// #define WINDOW_HEIGHT 960

// medium
#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

// smaller
// #define WINDOW_WIDTH 768
// #define WINDOW_HEIGHT 640

#define CONSOLE_WIDTH WINDOW_WIDTH
#define CONSOLE_HEIGHT 160

extern game_state_t global_state;

int32 game_init();
void game_run();
void game_exit();

#endif // GAME_H