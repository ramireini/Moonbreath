#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <types.h>
#include <tiles.h>
#include <update.h>
#include <item.h>
#include <render.h>
#include <text_render.h>
#include <level_generation.h>

// global running flag
int game_is_running;

// global window
SDL_Window *window;

// global renderer
SDL_Renderer *renderer;

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

#define TEXTURE_COUNT 9
#define FONT_COUNT 2
#define ITEM_COUNT 10
#define INVENTORY_COUNT 10
#define ITEM_INFO_COUNT 10
#define MESSAGE_COUNT 9

SDL_Keycode key_pressed;

// make some of these not global?
font_t *fonts[FONT_COUNT];
SDL_Texture *textures[TEXTURE_COUNT];

SDL_Keycode key_pressed;
item_t items[ITEM_COUNT];
item_info_t items_info[ITEM_INFO_COUNT];
item_t inventory[INVENTORY_COUNT];
console_message_t messages[MESSAGE_COUNT];

int game_init();

// void game_run(); // maybe use this after all the array stuff is figured out, otherwise you'd have to pass all of this ^ to this function.
void game_exit(char *level, char *fov, player_t *player);

#endif // GAME_H