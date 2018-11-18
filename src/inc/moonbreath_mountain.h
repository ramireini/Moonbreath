#ifndef MOONBREATH_MOUNTAIN_H
#define MOONBREATH_MOUNTAIN_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <types.h>
#include <tiles.h>
#include <item.h>
#include <render.h>
#include <text_render.h>
#include <util_math.h>
#include <level_generation.h>

// global flag
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

// console dimensions
#define CONSOLE_WIDTH 1024
#define CONSOLE_HEIGHT 160

// level dimensions
#define LEVEL_WIDTH 2048
#define LEVEL_HEIGHT 2048
#define LEVEL_SIZE LEVEL_WIDTH / TILE_SIZE

// text colors
#define TEXT_COLOR_WHITE 0xFFFFF0FF
#define TEXT_COLOR_GREEN 0x1CF002FF
#define TEXT_COLOR_ORANGE 0xA47B55FF
#define TEXT_COLOR_YELLOW 0xFDD500FF
#define TEXT_COLOR_BLUE 0x0060C8FF

#define GAME_ITEMS_COUNT 10
#define INVENTORY_COUNT 10
#define ITEM_INFO_COUNT 10
#define CONSOLE_MESSAGE_COUNT 12

SDL_Keycode key_pressed;
char level[LEVEL_SIZE * LEVEL_SIZE];
char fov[LEVEL_SIZE * LEVEL_SIZE];
item_t game_items[GAME_ITEMS_COUNT];
item_t inventory[INVENTORY_COUNT];
item_info_t game_items_info[ITEM_INFO_COUNT];
console_message_t console_messages[CONSOLE_MESSAGE_COUNT];

// game initializing functions 
int game_init(player_t *player, font_t **font_console, font_t **font_inv, font_t **font_item, SDL_Texture **tileset_tex, SDL_Texture **player_tileset_tex, SDL_Texture **item_tileset_tex, SDL_Texture **tilemap_tex, SDL_Texture **inv_tex, SDL_Texture **player_inv_hl_tex, SDL_Texture **inv_item_tex, SDL_Texture **interface_console_tex, SDL_Texture **interface_stats_tex);
void game_exit(SDL_Texture *tileset_tex, SDL_Texture *player_tileset_tex, SDL_Texture *tilemap_tex, SDL_Texture *item_tileset_tex, SDL_Texture *inv_tex, SDL_Texture *inv_hl_tex, SDL_Texture *inv_item_tex, player_t *player, font_t *font_console, font_t *font_inv, font_t *font_item, SDL_Texture *interface_console_tex, SDL_Texture *interface_stats_tex);

// everything else
//void update_lighting(char *map, char *fov_map, player_t *player);
void add_console_msg(char *msg, int msg_color, ...);
void handle_input(char *map, player_t *player, int *key_pressed);
void handle_events(int *key_pressed);
void update_camera(SDL_Rect *camera, player_t *player);
void entity_move(char *map, player_t *entity, int x, int y);
SDL_Color hex_to_rgba_color(int hex_color);

#endif // MOONBREATH_MOUNTAIN_H