#ifndef DUNGEON_GEN_H
#define DUNGEON_GEN_H

#define MAX_DUNGEON_WIDTH 64
#define MAX_DUNGEON_HEIGHT 64

#define MAX_DUNGEON_LEVEL 2

typedef enum
{
    tile_stone_wall_one,
    tile_stone_wall_two,
    tile_stone_wall_three,
    tile_stone_wall_four,
    tile_stone_wall_grate_one,
    tile_stone_wall_grate_two,
    tile_stone_wall_grate_three,
    tile_stone_wall_grate_four,
    tile_stone_wall_grate_five,
    tile_stone_wall_grate_six,
    tile_stone_wall_grate_seven,
    tile_stone_wall_grate_eight,
    tile_stone_wall_grate_nine,
    tile_stone_wall_grate_ten,
    tile_stone_wall_grate_eleven,
    tile_stone_wall_grate_twelve,
    tile_stone_wall_unlit_torch_one,
    tile_stone_wall_unlit_torch_two,
    tile_stone_wall_unlit_torch_three,
    tile_stone_wall_unlit_torch_four,
    
    tile_stone_floor_one,
    tile_stone_floor_two,
    tile_stone_floor_three,
    tile_stone_floor_four,
    tile_stone_floor_five,
    tile_grass_floor_one,
    tile_grass_floor_two,
    tile_grass_floor_three,
    tile_grass_floor_four,
    
    tile_stone_door_closed,
    tile_stone_door_open,
    
    tile_stone_path_up,
    tile_stone_path_down
} tile_type;

typedef enum
{
    room_rectangle,
    room_double_rectangle,
    room_automaton
} room_type;

typedef enum
{
    rectangle_min_size = 4,
    rectangle_max_size = 10,
    
    double_rectangle_min_size = 3,
    double_rectangle_max_size = 6,
    
    automaton_min_size = 8,
    automaton_max_size = 12,
} room_size;

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