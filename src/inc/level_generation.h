#ifndef LEVEL_GENERATION_H
#define LEVEL_GENERATION_H

#define LEVEL_WIDTH 2048
#define LEVEL_HEIGHT 2048
#define TILE_SIZE 32
#define LEVEL_SIZE LEVEL_WIDTH / TILE_SIZE

#include <game.h>
#include <util_io.h>

// NOTE(Rami): redo the level gen, first generate corridors, then place rooms, then place doors
// currently we generate rooms, place corridors and then place doors which creates really bad
// door placement

// NOTE(Rami): Remember to add level tiles
// to the functions that use them.
typedef enum
{
  TILE_WALL_STONE = 0,
  TILE_FLOOR_GRASS,
  TILE_FLOOR_STONE,
  TILE_DOOR_CLOSED,
  TILE_DOOR_OPEN,
  TILE_PATH_UP,
  TILE_PATH_DOWN
} level_tiles_e;

typedef struct
{
  int32 x;
  int32 y;
  int32 w;
  int32 h;
} room_t;

void place_spawns(char *level_level, int32 level_pitch, int32 room_count, room_t *rooms);
void generate_level(char *level, int32 level_width, int32 level_height, int32 level_pitch, int32 room_count);
void init_level(char *level, int32 level_width, int32 level_height, int32 level_pitch);
void init_and_place_rooms(char *level, int32 level_width, int32 level_height, int32 level_pitch, int32 room_count, room_t *rooms);
void connect_rooms(char *level, int32 level_pitch, int32 room_count, room_t *rooms);
int32 is_room_valid(char *level, int32 level_pitch, room_t room);
void place_tile(char *level, int32 level_pitch, int32 x, int32 y, int32 tile);
pos_t rand_pos_in_rect(room_t room);

#endif // LEVEL_GENERATION_H