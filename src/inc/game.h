#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "tiles.h"
#include "structs.h"

#define MAP_SIZE LEVEL_WIDTH / TILE_SIZE

// window dimensions
#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

// console dimensions
#define CONSOLE_WIDTH 1024
#define CONSOLE_HEIGHT 160

// level dimensions
#define LEVEL_WIDTH 2048
#define LEVEL_HEIGHT 2048

#define ENTITY_AMOUNT 3

#define CONSOLE_MESSAGE_AMOUNT 10

#define ITEMS_AMOUNT 10

#define ITEM_INFO_AMOUNT 10

#define INVENTORY_AMOUNT 10

// text colors
#define COLOR_TEXT_WHITE 0xFFFFF0FF
#define COLOR_TEXT_GREEN 0x1CF002FF
#define COLOR_TEXT_DESCRIPTION 0xA47B55FF

// console colors
#define COLOR_ACTION 0xFFFFF0FF
#define COLOR_STATUS 0x0060C8FF
#define COLOR_SPECIAL 0xFDD500FF

SDL_Keycode current_key;
unsigned char map[MAP_SIZE * MAP_SIZE];
unsigned char fov_map[MAP_SIZE * MAP_SIZE];
SDL_Rect tiles[NUMBER_OF_TILES_ON_TILESHEET];
entity_t *entities[ENTITY_AMOUNT];
item_t items[ITEMS_AMOUNT];
item_info_t item_info[ITEM_INFO_AMOUNT];
item_info_t inventory[INVENTORY_AMOUNT];
console_message_t console_messages[CONSOLE_MESSAGE_AMOUNT];

void render_text(SDL_Renderer *renderer, font_t *font_struct, int x, int y, char *str, int wrap_width, unsigned int text_hex_color);

font_t* create_font_atlas(SDL_Renderer *renderer, TTF_Font *font);

void render_inventory(SDL_Renderer *renderer, SDL_Texture *player_inventory_tex, SDL_Texture *player_inventory_highlight_tex, SDL_Texture *player_inventory_item_tex, font_t *font_inventory, font_t *font_item, int *player_inventory_highlight_index, int *player_inventory_current_item_amount);

void render_items(SDL_Renderer *renderer, SDL_Texture *itemset_tex, SDL_Rect *camera);

void add_item_into_inventory(entity_t *player_entity);

void add_console_message(char *message, int message_color);

void render_console_messages(SDL_Renderer *renderer, font_t *font_struct);

void process_input(unsigned char *map, entity_t *player_entity, int *game_is_running, int *current_key, int *display_inventory, int *player_inventory_highlight_index, int *player_inventory_current_item_amount, int *update_logic);

void process_events(int *game_is_running, int *current_key);

//void update_lighting(unsigned char *map, unsigned char *fov_map, entity_t *player);

void render_player(SDL_Renderer *renderer, SDL_Texture *player_tileset_tex, SDL_Rect *camera, entity_t *player);

void update_camera(SDL_Rect *camera, entity_t *player);

void render_level(SDL_Renderer *renderer, SDL_Texture *tileset_tex, SDL_Texture *tilemap_tex, unsigned char *map, unsigned char *fov_map, SDL_Rect *camera);

void free_resources(SDL_Window *window, SDL_Renderer *renderer, SDL_Texture *tileset_tex, SDL_Texture *player_tileset_tex, SDL_Texture *tilemap_tex, SDL_Texture *itemset_tex, SDL_Texture *player_inventory_tex, SDL_Texture *player_inventory_highlight_tex, SDL_Texture *player_inventory_item_tex, player_t *player, font_t *font_console, font_t *font_inventory, font_t *font_item);

int entity_move(unsigned char *map, entity_t *entity, int x, int y, int *game_is_running);

int initialize(SDL_Window **window, SDL_Renderer **renderer);

double distance(double x1, double y1, double x2, double y2);

player_t* new_player();

entity_t* new_entity(int level, int money, int hp, int xp, int x, int y, int w, int h, int speed, int view_distance);

SDL_Color hex_to_rgba_color(unsigned int hex_color);

SDL_Texture* load_texture(SDL_Renderer *renderer, const char *string);

extern void generate_dungeon(unsigned char *map, int map_pitch, int map_width, int map_height, int room_count, entity_t* player);
extern void initialize_map(unsigned char *map, int map_pitch, int map_width, int map_height);
extern void initialize_and_place_rooms(unsigned char *map, int map_pitch, int map_width, int map_height, int room_count, room_t *rooms);
extern void connect_rooms(unsigned char *map, int map_pitch, int room_count, room_t *rooms);
extern void place_corridors(unsigned char *map, int map_pitch, room_t room_a, room_t room_b, int direction);
extern void place_player_spawn(entity_t *player, unsigned char *map, int map_width, int room_count, room_t *rooms);
extern int is_room_valid(unsigned char *map, int map_pitch, room_t room);
extern int random_int(int from, int to);
extern cell_t random_cell_in_rect(room_t room);

#endif // GAME_H