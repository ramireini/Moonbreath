#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <types.h>
#include <update.h>
#include <render.h>
#include <text_render.h>
#include <item.h>
#include <level_generation.h>
#include <player.h>
#include <slime.h>

// large
// #define WINDOW_WIDTH 1216
// #define WINDOW_HEIGHT 960

// standard
#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

// small
// #define WINDOW_WIDTH 768
// #define WINDOW_HEIGHT 640

#define CONSOLE_WIDTH WINDOW_WIDTH
#define CONSOLE_HEIGHT 160

#define TEXTURE_COUNT 10
#define MESSAGE_COUNT 9

extern SDL_Window *window;
extern SDL_Renderer *renderer;

extern int game_is_running;
extern int turn_changed;
// NOTE(Rami): At some point we might need this https://codereview.stackexchange.com/questions/171641/sprite-animation-with-sdl2
extern SDL_Keycode key_pressed;
extern uint32 time_elapsed;
extern SDL_Rect camera;

extern SDL_Texture *textures[TEXTURE_COUNT];
extern console_message_t messages[MESSAGE_COUNT];

int32 game_init();
void game_run(char *level, char *fov);
void game_exit(char *level, char *fov);

#endif // GAME_H