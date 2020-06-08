#define MAX_DUNGEON_LEVEL 10
#define MAX_DUNGEON_SIZE 256
#define MAX_DUNGEON_ROOMS 512

typedef enum
{
    tile_none,
    
    tile_stone_wall_1,
    tile_stone_wall_2,
    tile_stone_wall_3,
    tile_stone_wall_4,
    tile_stone_wall_5,
    tile_stone_wall_6,
    
    tile_stone_wall_torch_1,
    tile_stone_wall_torch_2,
    tile_stone_wall_torch_3,
    tile_stone_wall_torch_4,
    tile_stone_wall_torch_5,
    
    tile_stone_wall_grate_1,
    tile_stone_wall_grate_2,
    
    tile_stone_wall_vines_1,
    tile_stone_wall_vines_2,
    tile_stone_wall_vines_3,
    tile_stone_wall_vines_4,
    tile_stone_wall_vines_5,
    
    tile_stone_wall_banner_1,
    tile_stone_wall_banner_2,
    tile_stone_wall_banner_3,
    
    tile_stone_floor_1,
    tile_stone_floor_2,
    tile_stone_floor_3,
    tile_stone_floor_4,
    
    tile_stone_floor_grate_1,
    tile_stone_floor_grate_2,
    tile_stone_floor_grate_3,
    tile_stone_floor_grate_4,
    tile_stone_floor_grate_5,
    tile_stone_floor_grate_6,
    
    tile_stone_door_closed,
    tile_stone_door_open,
    
    tile_stone_path_up,
    tile_stone_path_down,
    tile_escape,
    
    tile_red_blood_puddle_1,
    tile_red_blood_puddle_2,
    tile_red_blood_puddle_3,
    tile_red_blood_splatter_2,
    tile_red_blood_splatter_3,
    tile_red_blood_splatter_4,
    
    tile_green_blood_puddle_1,
    tile_green_blood_puddle_2,
    tile_green_blood_puddle_3,
    tile_green_blood_splatter_2,
    tile_green_blood_splatter_3,
    tile_green_blood_splatter_4,
    
    //
    
    tile_wall_start = tile_stone_wall_1,
    tile_wall_end = tile_stone_wall_banner_3,
    
    tile_floor_start = tile_stone_floor_1,
    tile_floor_end = tile_stone_floor_grate_6
} tile;

typedef enum
{
    room_type_none,
    
    room_type_rect,
    room_type_double_rect,
    room_type_automaton
} room_type;

typedef enum
{
    dungeon_type_none,
    
    dungeon_type_cavern
} dungeon_type;

typedef struct
{
    b32 is_seen;
    b32 has_been_seen;
    b32 is_occupied;
    
    tile remains;
    tile value;
} tile_t;

typedef struct
{
    u32 width;
    tile_t *array;
} tile_data_t;

typedef struct
{
    u32 count;
    v4u array[MAX_DUNGEON_ROOMS];
} room_data_t;

struct dungeon_t
{
    dungeon_type type;
    u32 level;
    u32 w, h;
    
    u32 pathfind_map[MAX_DUNGEON_SIZE * MAX_DUNGEON_SIZE];
    tile_data_t tiles;
    room_data_t rooms;
    
    b32 can_have_rect_rooms;
    u32 rect_min_size;
    u32 rect_max_size;
    
    b32 can_have_double_rect_rooms;
    u32 double_rect_min_size;
    u32 double_rect_max_size;
    
    b32 can_have_automaton_rooms;
    u32 automaton_min_size;
    u32 automaton_max_size;
};