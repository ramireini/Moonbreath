#define MAX_DUNGEON_LEVEL 10
#define MAX_DUNGEON_SIZE 128

typedef enum
{
    tile_stone_wall_one,
    tile_stone_wall_two,
    tile_stone_wall_three,
    tile_stone_wall_four,
    tile_stone_wall_five,
    tile_stone_wall_six,
    tile_stone_wall_seven,
    tile_stone_wall_eight,
    tile_stone_wall_nine,
    
    tile_stone_wall_torch,
    tile_stone_wall_grate,
    tile_stone_wall_small_grate,
    
    tile_stone_wall_vines_one,
    tile_stone_wall_vines_two,
    tile_stone_wall_vines_three,
    
    tile_stone_wall_banner_one,
    tile_stone_wall_banner_two,
    tile_stone_wall_banner_three,
    
    tile_stone_floor_one,
    tile_stone_floor_two,
    tile_stone_floor_three,
    tile_stone_floor_four,
    
    tile_stone_floor_grate_one,
    tile_stone_floor_grate_two,
    tile_stone_floor_grate_three,
    tile_stone_floor_grate_four,
    tile_stone_floor_grate_five,
    tile_stone_floor_grate_six,
    
    tile_stone_door_closed,
    tile_stone_door_open,
    
    tile_stone_path_up,
    tile_stone_path_down,
    
    //
    
    tile_wall_start = tile_stone_wall_one,
    tile_wall_end = tile_stone_wall_banner_three,
    
    tile_floor_start = tile_stone_floor_one,
    tile_floor_end = tile_stone_floor_grate_six
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
    dungeon_small,
    dungeon_medium,
    dungeon_large
} dungeon_size;

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
#endif

dungeon_spec_t cavern_spec_large =
{
    dungeon_cavern,
    64, 64
};

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