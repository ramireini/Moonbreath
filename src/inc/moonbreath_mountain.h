#ifndef MOONBREATH_MOUNTAIN_H
#define MOONBREATH_MOUNTAIN_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <types.h>
#include <tiles.h>

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

#define LEVEL_SIZE LEVEL_WIDTH / TILE_SIZE
// #define LEVEL_SIZE 10

SDL_Keycode current_key;
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
void consume_item(player_t *player, int *inv_hl_index, int *inv_item_count);
void equip_or_unequip_item(int *inv_hl_index);
void render_inventory(SDL_Texture *inv_tex, SDL_Texture *inv_hl_tex, SDL_Texture *inv_item_tex, font_t *font_inv, font_t *font_item, int *inv_hl_index, int *inv_item_count);
void render_text(font_t *font_struct, int x, int y, char *str, unsigned int text_color);
font_t* create_font_atlas(TTF_Font *font); // returns a MALLOC'd pointer, remember to FREE!
void render_inventory(SDL_Texture *inv_tex, SDL_Texture *inv_hl_tex, SDL_Texture *inv_item_tex, font_t *font_inv, font_t *font_item, int *inv_hl_index, int *inv_item_count);
void render_items(SDL_Texture *item_tileset_tex, SDL_Rect *camera);
void drop_item(player_t *player, int *inv_hl_index, int *inv_item_count);
void add_inventory_item(player_t *player);
void add_console_msg(char *msg, int msg_color);
void render_interface(player_t *player, SDL_Texture *interface_console_tex, SDL_Texture *interface_stats_tex, font_t *font_struct);
void handle_input(char *map, player_t *player, int *game_is_running, int *current_key, int *display_inventory, int *inv_hl_index, int *inv_item_count);
int handle_events(int *current_key);
void render_player(SDL_Texture *player_tileset_tex, SDL_Texture *item_tileset_tex, SDL_Rect *camera, player_t *player);
void update_camera(SDL_Rect *camera, player_t *player);
void render_level(SDL_Texture *tileset_tex, SDL_Texture *tilemap_tex, char *map, char *fov, SDL_Rect *camera);
void entity_move(char *map, player_t *entity, int x, int y);
double distance(double x1, double y1, double x2, double y2);
SDL_Color hex_to_rgba_color(unsigned int hex_color);
SDL_Texture* load_texture(char *path);

/* -- EXTERN FUNCS -- */
extern void generate_level(char *level, int level_width, int level_height, int level_pitch, int room_count, player_t* player);

#endif // MOONBREATH_MOUNTAIN_H