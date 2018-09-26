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
#define COLOR_TEXT_WHITE 0xFFFFF0
#define COLOR_TEXT_GREEN 0x1CF002

// console colors
#define COLOR_ACTION 0xFFFFF0
#define COLOR_STATUS 0x0060C8
#define COLOR_SPECIAL 0xFDD500

SDL_Keycode current_key;
unsigned char map[MAP_SIZE * MAP_SIZE];
unsigned char fov_map[MAP_SIZE * MAP_SIZE];
SDL_Rect tiles[NUMBER_OF_TILES_ON_TILESHEET];
entity_t *entities[ENTITY_AMOUNT];
item_t items[ITEMS_AMOUNT];
item_info_t item_info[ITEM_INFO_AMOUNT];
item_info_t inventory[INVENTORY_AMOUNT];
console_message_t console_messages[CONSOLE_MESSAGE_AMOUNT];

void render_text(SDL_Renderer *renderer, TTF_Font *text_font, int text_x, int text_y, char *text, unsigned int text_hex_color);
void render_inventory(SDL_Renderer *renderer, SDL_Texture *player_inventory_tex, SDL_Texture *player_inventory_highlight_tex, SDL_Texture *player_inventory_item_tex, TTF_Font *inventory_font, TTF_Font *font_item, int *player_inventory_highlight_index, int *player_inventory_current_item_amount);
void render_items(SDL_Renderer *renderer, SDL_Texture *itemset_tex, SDL_Rect *camera);
void add_item_into_inventory(entity_t *player_entity);
void add_console_message(char *message, unsigned int message_color);
void render_console_messages(SDL_Renderer *renderer, TTF_Font *font_console);
void process_input(unsigned char *map, entity_t *player_entity, int *game_is_running, int *current_key, int *display_inventory, int *player_inventory_highlight_index, int *player_inventory_current_item_amount, int *update_logic);
void process_events(int *game_is_running, int *current_key);
//void update_lighting(unsigned char *map, unsigned char *fov_map, entity_t *player);
void render_player(SDL_Renderer *renderer, SDL_Texture *player_tileset_tex, SDL_Rect *camera, entity_t *player);
void update_camera(SDL_Rect *camera, entity_t *player);
void render_level(SDL_Renderer *renderer, SDL_Texture *tileset_tex, SDL_Texture *tilemap_tex, unsigned char *map, unsigned char *fov_map, SDL_Rect *camera);
double distance(double x1, double y1, double x2, double y2);
player_t* new_player();
SDL_Color hex_to_rgb_color(unsigned int hex_color);

void cleanup(SDL_Window *window, SDL_Renderer *renderer, SDL_Texture *tileset_tex, SDL_Texture *player_tileset_tex, SDL_Texture *tilemap_tex, SDL_Texture *itemset_tex, SDL_Texture *player_inventory_tex, SDL_Texture *player_inventory_highlight_tex, SDL_Texture *player_inventory_item_tex, player_t *player, TTF_Font *font_console, TTF_Font *font_inventory, TTF_Font *font_item);
int entity_move(unsigned char *map, entity_t *entity, int x, int y, int *game_is_running);
int initialize(SDL_Window **window, SDL_Renderer **renderer);
entity_t* new_entity(int health_points, int x, int y, int width, int height, int speed, int view_distance);
SDL_Texture* load_texture(SDL_Renderer *renderer, const char *string);

extern void generate_dungeon(unsigned char *map, int map_pitch, int map_width, int map_height, int room_count, entity_t* player);
extern void initialize_map(unsigned char *map, int map_pitch, int map_width, int map_height);
extern void initialize_and_place_rooms(unsigned char *map, int map_pitch, int map_width, int map_height, int room_count, room_t *rooms);
extern void connect_rooms(unsigned char *map, int map_pitch, int room_count, room_t *rooms);
extern void place_corridors(unsigned char *map, int map_pitch, room_t room_a, room_t room_b, int direction);
extern int is_room_valid(unsigned char *map, int map_pitch, room_t room);
extern int random_int(int from, int to);
extern cell_t random_cell_in_rect(room_t room);

extern void place_player_spawn(entity_t *player, unsigned char *map, int map_width, int room_count, room_t *rooms);
extern void place_monster_spawns();

#endif // GAME_H