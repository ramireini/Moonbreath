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

// window dimensions, standard
#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

// small
// #define WINDOW_WIDTH 768
// #define WINDOW_HEIGHT 640

// big
// #define WINDOW_WIDTH 1216
// #define WINDOW_HEIGHT 960

#define CONSOLE_WIDTH 1024
#define CONSOLE_HEIGHT 160

#define GAME_ITEMS_COUNT 10
#define INVENTORY_COUNT 10
#define ITEM_INFO_COUNT 10
#define CONSOLE_MESSAGE_COUNT 9

SDL_Keycode key_pressed;

// make these not global?
item_t game_items[GAME_ITEMS_COUNT];
item_t inventory[INVENTORY_COUNT];
item_info_t game_items_info[ITEM_INFO_COUNT];
console_message_t console_messages[CONSOLE_MESSAGE_COUNT];

int game_init(font_t **font_one, font_t **font_two, player_t *player, SDL_Texture **tileset_tex, SDL_Texture **player_tileset_tex, SDL_Texture **item_tileset_tex, SDL_Texture **tilemap_tex, SDL_Texture **inv_tex, SDL_Texture **player_inv_hl_tex, SDL_Texture **inv_item_tex, SDL_Texture **interface_console_tex, SDL_Texture **interface_stats_tex);

// void game_run(); // maybe use this after all the array stuff is figured out, otherwise you'd have to pass all of this ^ to this function.
void game_exit(font_t *font_one, font_t *font_two, char *level, char *fov, SDL_Texture *tileset_tex, SDL_Texture *player_tileset_tex, SDL_Texture *tilemap_tex, SDL_Texture *item_tileset_tex, SDL_Texture *inv_tex, SDL_Texture *inv_hl_tex, SDL_Texture *inv_item_tex, player_t *player, SDL_Texture *interface_console_tex, SDL_Texture *interface_stats_tex);

#endif // GAME_H