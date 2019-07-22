#ifndef LEVEL_GEN_H
#define LEVEL_GEN_H

#define LEVEL_PIXEL_WIDTH 2048
#define LEVEL_PIXEL_HEIGHT 2048

// NOTE(rami): Is TILE_SIZE needed?
#define TILE_SIZE 32
#define LEVEL_TILE_WIDTH LEVEL_PIXEL_WIDTH / TILE_SIZE
#define LEVEL_TILE_HEIGHT LEVEL_PIXEL_HEIGHT / TILE_SIZE

#define START_ALIVE_CHANCE 55
#define SMOOTHING_ITERATIONS 5
#define DEATH_LIMIT 2
#define BIRTH_LIMIT 3

#define ALIVE tile_floor_stone
#define DEAD tile_none

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
    lighting_t lighting[LEVEL_TILE_WIDTH * LEVEL_TILE_HEIGHT];
    u32 tiles[LEVEL_TILE_WIDTH * LEVEL_TILE_HEIGHT];
    v4i rooms[ROOM_COUNT];
} level_t;

typedef struct
{
    u32 buff_one[LEVEL_TILE_WIDTH * LEVEL_TILE_HEIGHT];
    u32 buff_two[LEVEL_TILE_WIDTH * LEVEL_TILE_HEIGHT];
} level_gen_buffers_t;

typedef struct
{
    b32 success;
    v4i room;
} room_data_t;

#endif // LEVEL_GEN_H