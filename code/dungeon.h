#ifndef DUNGEON_GEN_H
#define DUNGEON_GEN_H

#define MAX_DUNGEON_WIDTH 64
#define MAX_DUNGEON_HEIGHT 64

#define MAX_DUNGEON_LEVEL 2

typedef enum
{
    tile_none,
    tile_wall_stone,
    tile_floor_grass,
    tile_floor_stone,
    tile_door_closed,
    tile_door_open,
    tile_path_up,
    tile_path_down
} tile_type;

typedef enum
{
    room_rectangle,
    room_double_rectangle,
    room_automaton,
    
    room_type_first = room_rectangle,
    room_type_last = room_automaton
} room_type;

typedef enum
{
    rectangle_min_size = 4,
    rectangle_max_size = 12,
    
    double_rectangle_min_size = 3,
    double_rectangle_max_size = 6,
    
    automaton_min_size = 8,
    automaton_max_size = 12,
} room_size;

typedef struct
{
    b32 valid;
    v4u room;
} room_result_t;

typedef struct
{
    b32 occupied;
    tile_type tile;
} tile_t;

typedef struct
{
    tile_t *ptr;
    u32 width;
} automaton_t;

typedef struct
{
    u32 level;
    u32 w, h;
    fov_tile_t fov_tiles[64][64];
    tile_t tiles[64][64];
} dungeon_t;

#endif // DUNGEON_GEN_H