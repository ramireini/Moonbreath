#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <types.h>
#include <update.h>
#include <item.h>
#include <render.h>
#include <text_render.h>
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
#define FONT_COUNT 2
#define ITEM_COUNT 10
#define INVENTORY_COUNT 10
#define ITEM_INFO_COUNT 10
#define MESSAGE_COUNT 9

extern SDL_Window *window;
extern SDL_Renderer *renderer;

extern int game_is_running;
extern int turn_changed;
extern SDL_Keycode key_pressed;

extern font_t *fonts[FONT_COUNT];
extern SDL_Texture *textures[TEXTURE_COUNT];

extern item_t items[ITEM_COUNT];
extern item_info_t items_info[ITEM_INFO_COUNT];
extern item_t inventory[INVENTORY_COUNT];
extern console_message_t messages[MESSAGE_COUNT];

extern SDL_Rect camera;

int game_init();
void game_run(char *level, char *fov);
void game_exit(char *level, char *fov);

#endif // GAME_H