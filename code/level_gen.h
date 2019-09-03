#ifndef LEVEL_GEN_H
#define LEVEL_GEN_H

#define MAX_DUNGEON_LEVEL 2

#define MAX_LEVEL_WIDTH 64
#define MAX_LEVEL_HEIGHT 64

#define MIN_ROOM_SIZE 4
#define MAX_ROOM_SIZE 12

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
    room_automata
} room_type;

typedef struct
{
    b32 valid;
    u32 x, y;
    u32 w, h;
} room_t;

typedef struct
{
    b32 occupied;
    tile_type tile;
} tile_t;

typedef struct
{
    tile_t *ptr;
    u32 width;
} automata_t;

typedef struct
{
    u32 current_level;
    u32 w, h;
    fov_tile_t fov_tiles[64][64];
    tile_t tiles[64][64];
} level_t;

#endif // LEVEL_GEN_H