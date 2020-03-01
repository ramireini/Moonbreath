#define MAX_DUNGEON_LEVEL 10
#define MAX_DUNGEON_SIZE 128

typedef enum
{
    tile_stone_wall_1,
    tile_stone_wall_2,
    tile_stone_wall_3,
    tile_stone_wall_4,
    tile_stone_wall_5,
    tile_stone_wall_6,
    tile_stone_wall_7,
    tile_stone_wall_8,
    
    tile_stone_wall_torch_1,
    tile_stone_wall_torch_2,
    tile_stone_wall_torch_3,
    
    tile_stone_wall_grate_1,
    tile_stone_wall_grate_2,
    
    tile_stone_wall_vines_1,
    tile_stone_wall_vines_2,
    tile_stone_wall_vines_3,
    
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
    
    //
    
    tile_wall_start = tile_stone_wall_1,
    tile_wall_end = tile_stone_wall_banner_3,
    
    tile_floor_start = tile_stone_floor_1,
    tile_floor_end = tile_stone_floor_grate_6
} tile_type;

typedef enum
{
    room_none,
    room_rect,
    room_double_rect,
    room_automaton
} room_type;

typedef enum
{
    dungeon_cavern
} dungeon_type;

typedef struct
{
    dungeon_type type;
    u32 w, h;
    
    b32 can_have_rect_rooms;
    u32 rect_min_size;
    u32 rect_max_size;
    
    b32 can_have_double_rect_rooms;
    u32 double_rect_min_size;
    u32 double_rect_max_size;
    
    b32 can_have_automaton_rooms;
    u32 automaton_min_size;
    u32 automaton_max_size;
} dungeon_spec_t;

#if 0
// TODO(rami): Maybe have comments on the right side of values like Brevik.
dungeon_spec_t cavern_spec_small =
{
    dungeon_cavern,
    32, 32
};

dungeon_spec_t cavern_spec_medium =
{
    dungeon_cavern,
    48, 48,
    
    false,
    0, 0,
    
    false,
    0, 0,
    
    true,
    8, 16
};

dungeon_spec_t cavern_spec_large =
{
    dungeon_cavern,
    64, 64
};
#endif

typedef struct
{
    b32 occupied;
    u32 value;
} tile_t;

typedef struct
{
    tile_t *ptr;
    u32 width;
} automaton_t;

typedef struct
{
    dungeon_type type;
    u32 level;
    u32 w, h;
    
    fov_tile_t *fov_tiles;
    tile_t *tiles;
    u32 *pathfind_map;
    
    b32 can_have_rect_rooms;
    u32 rect_min_size;
    u32 rect_max_size;
    
    b32 can_have_double_rect_rooms;
    u32 double_rect_min_size;
    u32 double_rect_max_size;
    
    b32 can_have_automaton_rooms;
    u32 automaton_min_size;
    u32 automaton_max_size;
} dungeon_t;