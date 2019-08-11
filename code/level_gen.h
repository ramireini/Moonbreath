#ifndef LEVEL_GEN_H
#define LEVEL_GEN_H

#define LEVEL_WIDTH_PIXELS 2048
#define LEVEL_HEIGHT_PIXELS 2048

// TODO(rami): Is TILE_SIZE needed?
#define TILE_SIZE 32
#define LEVEL_WIDTH LEVEL_WIDTH_PIXELS / TILE_SIZE
#define LEVEL_HEIGHT LEVEL_HEIGHT_PIXELS / TILE_SIZE

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
    u32 current_level;
    fov_pos_t fov_tiles[LEVEL_WIDTH][LEVEL_HEIGHT];
    u32 tiles[LEVEL_WIDTH][LEVEL_HEIGHT];
    v4u rooms[ROOM_COUNT];
} level_t;

typedef struct
{
    u32 buff_one[LEVEL_WIDTH][LEVEL_HEIGHT];
    u32 buff_two[LEVEL_WIDTH][LEVEL_HEIGHT];
} level_gen_buffers_t;

typedef struct
{
    b32 success;
    v4u room;
} room_data_t;

#endif // LEVEL_GEN_H