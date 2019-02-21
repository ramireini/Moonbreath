#ifndef LEVEL_GENERATION_H
#define LEVEL_GENERATION_H

#define LEVEL_WIDTH_IN_PIXELS 2048
#define LEVEL_HEIGHT_IN_PIXELS 2048
#define TILE_SIZE 32
#define LEVEL_WIDTH_IN_TILES LEVEL_WIDTH_IN_PIXELS / TILE_SIZE
#define LEVEL_HEIGHT_IN_TILES LEVEL_HEIGHT_IN_PIXELS / TILE_SIZE

#define ROOM_COUNT 30

#define START_ALIVE_CHANCE 55
#define SMOOTHING_ITERATIONS 5
#define DEATH_LIMIT 3
#define BIRTH_LIMIT 3

#define ALIVE tile_floor_stone
#define DEAD tile_none

// NOTE(Rami): Remember to add level tiles
// to the functions that use them.
typedef enum
{
  tile_none = 0,
  tile_wall_stone,
  tile_floor_grass,
  tile_floor_stone,
  tile_door_closed,
  tile_door_open,
  tile_path_up,
  tile_path_down
} tile_e;

typedef enum
{
  type_rectangle = 0,
  type_overlaid_rectangle,
  type_cellular_automata,
} type_e;

typedef struct
{
  uint8 room_buffer[LEVEL_WIDTH_IN_TILES * LEVEL_HEIGHT_IN_TILES];
  uint8 temp_buffer[LEVEL_WIDTH_IN_TILES * LEVEL_HEIGHT_IN_TILES];
} level_gen_buffers_t;

typedef struct
{
  int32 x;
  int32 y;
} pos_t;

typedef struct
{
  int32 w;
  int32 h;
} dimensions_t;

typedef struct
{
  int32 x;
  int32 y;
  int32 w;
  int32 h;
} room_t;

extern uint8 level[LEVEL_WIDTH_IN_TILES * LEVEL_HEIGHT_IN_TILES];

#endif // LEVEL_GENERATION_H