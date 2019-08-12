#ifndef LEVEL_GEN_H
#define LEVEL_GEN_H

#define MAX_LEVEL_WIDTH 64
#define MAX_LEVEL_HEIGHT 64

#define START_ALIVE_CHANCE 55
#define SMOOTHING_ITERATIONS 5
#define DEATH_LIMIT 2
#define BIRTH_LIMIT 3
#define ALIVE tile_floor_stone
#define DEAD tile_none

#define MAX_DUNGEON_LEVEL 2
#define ROOM_COUNT 40

enum
{
    tile_none,
    tile_wall_stone,
    tile_floor_grass,
    tile_floor_stone,
    tile_door_closed,
    tile_door_open,
    tile_path_up,
    tile_path_down
};

enum
{
    type_horizontal,
    type_vertical
};

enum
{
    type_rectangle,
    type_corridor,
    type_cellular_automata
};

typedef struct
{
    u32 buff_one[MAX_LEVEL_WIDTH][MAX_LEVEL_HEIGHT];
    u32 buff_two[MAX_LEVEL_WIDTH][MAX_LEVEL_HEIGHT];
} level_gen_buffers_t;

typedef struct
{
    b32 valid;
    u32 x, y;
    u32 w, h;
} room_t;

typedef struct
{
    u32 current_level;
    u32 width;
    u32 height;
    fov_pos_t fov_tiles[MAX_LEVEL_WIDTH][MAX_LEVEL_HEIGHT];
    u32 tiles[MAX_LEVEL_WIDTH][MAX_LEVEL_HEIGHT];
    room_t rooms[ROOM_COUNT];
} level_t;

#endif // LEVEL_GEN_H