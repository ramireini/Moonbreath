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
#define CONSOLE_MESSAGE_AMOUNT 12

entity_t *entities[ENTITY_AMOUNT];

console_message_t console_messages[CONSOLE_MESSAGE_AMOUNT];

void add_console_message(char *message, unsigned int message_color);
void render_console_messages(SDL_Renderer *renderer);
void update_game(unsigned char *map, entity_t *player_entity, int *game_is_running, int *current_key);
void process_events(int *game_is_running, int *current_key);
void update_lighting(unsigned char *map, unsigned char *fov_map, entity_t *player);
void render_player(SDL_Renderer *renderer, SDL_Texture *player_tileset_tex, SDL_Rect *camera, entity_t *player);
void update_camera(SDL_Rect *camera, entity_t *player);
void render_background_texture(SDL_Renderer *renderer, SDL_Texture *tileset_tex, SDL_Texture *tilemap_tex, unsigned char *map, unsigned char *fov_map, SDL_Rect *camera);
double distance(double x1, double y1, double x2, double y2);
player_t* player_new(int class);

void cleanup(SDL_Window *window, SDL_Renderer *renderer, SDL_Texture *tileset_tex, SDL_Texture *player_tileset_tex, SDL_Texture *tilemap_tex, player_t *player);
int entity_move(unsigned char *map, entity_t *entity, int x, int y, int *game_is_running);
int initialize(SDL_Window **window, SDL_Renderer **renderer);
entity_t* entity_new(int x, int y, int x_dir, int width, int height, int speed, int view_distance);
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