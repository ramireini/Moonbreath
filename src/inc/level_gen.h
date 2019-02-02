#ifndef LEVEL_GENERATION_H
#define LEVEL_GENERATION_H

// NOTE(Rami): Delete
// https://gamedevelopment.tutsplus.com/tutorials/generate-random-cave-levels-using-cellular-automata--gamedev-9664
// https://www.rockpapershotgun.com/2015/07/28/how-do-roguelikes-generate-levels/

// NOTE(Rami): Go over this .h and the .c file and clean them up!

#define LEVEL_WIDTH_IN_PIXELS 2048
#define LEVEL_HEIGHT_IN_PIXELS 2048
#define TILE_SIZE 32
#define LEVEL_WIDTH_IN_TILES LEVEL_WIDTH_IN_PIXELS / TILE_SIZE
#define LEVEL_HEIGHT_IN_TILES LEVEL_HEIGHT_IN_PIXELS / TILE_SIZE

#include <game.h>
#include <util_io.h>

// NOTE(Rami): Remember to add level tiles
// to the functions that use them.
typedef enum
{
  TILE_NONE = 0,
  TILE_WALL_STONE,
  TILE_FLOOR_GRASS,
  TILE_FLOOR_STONE,
  TILE_DOOR_CLOSED,
  TILE_DOOR_OPEN,
  TILE_PATH_UP,
  TILE_PATH_DOWN
} level_tiles_e;

extern uint8 *level;

void generate_level();
void place_spawns(int32 level_pitch, int32 room_count, SDL_Rect *rooms);
void init_level(int32 level_width, int32 level_height, int32 level_pitch);
void init_and_place_rooms(int32 level_width, int32 level_height, int32 level_pitch, int32 room_count, SDL_Rect *rooms);
void connect_rooms(int32 level_pitch, int32 room_count, SDL_Rect *rooms);
void place_tile(int32 x, int32 y, int32 tile);
int32 is_room_valid(int32 level_pitch, SDL_Rect room);
pos_t rand_pos_in_rect(SDL_Rect room);

#endif // LEVEL_GENERATION_H