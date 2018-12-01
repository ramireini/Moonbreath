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

#define TEXTURE_COUNT 10
#define FONT_COUNT 10
#define ITEMS_COUNT 10
#define INVENTORY_COUNT 10
#define ITEMS_INFO_COUNT 10
#define MESSAGE_COUNT 9

SDL_Keycode key_pressed;

// these will be arrays that hold all our the games fonts and textures,
// and because this is global for now at least, we won't have to pass anything
// since all function can access it at will
// 
// we will also have enum lists that tell which element is which font/texture
// and the game init code will set these into the elements of the arrays
SDL_Texture *textures[TEXTURE_COUNT];
font_t *fonts[FONT_COUNT];

// make some of these not global?
SDL_Keycode key_pressed;
item_t items[ITEMS_COUNT];
item_info_t items_info[ITEMS_INFO_COUNT];
item_t inventory[INVENTORY_COUNT];
console_message_t messages[MESSAGE_COUNT];

int game_init(font_t **font_one, font_t **font_two, player_t *player, SDL_Texture **tileset_tex, SDL_Texture **player_tileset_tex, SDL_Texture **item_tileset_tex, SDL_Texture **tilemap_tex, SDL_Texture **inv_tex, SDL_Texture **player_inv_hl_tex, SDL_Texture **inv_item_tex, SDL_Texture **interface_console_tex, SDL_Texture **interface_stats_tex);

// void game_run(); // maybe use this after all the array stuff is figured out, otherwise you'd have to pass all of this ^ to this function.
void game_exit(font_t *font_one, font_t *font_two, char *level, char *fov, SDL_Texture *tileset_tex, SDL_Texture *player_tileset_tex, SDL_Texture *tilemap_tex, SDL_Texture *item_tileset_tex, SDL_Texture *inv_tex, SDL_Texture *inv_hl_tex, SDL_Texture *inv_item_tex, player_t *player, SDL_Texture *interface_console_tex, SDL_Texture *interface_stats_tex);

#endif // GAME_H