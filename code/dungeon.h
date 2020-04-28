#define MAX_DUNGEON_LEVEL 10
#define MAX_DUNGEON_SIZE 256
#define MAX_DUNGEON_ROOMS 512

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
    tile_stone_wall_torch_4,
    tile_stone_wall_torch_5,
    
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
    
    tile_blood_puddle_1,
    tile_blood_puddle_2,
    tile_blood_puddle_3,
    tile_blood_splatter_1,
    tile_blood_splatter_2,
    tile_blood_splatter_3,
    tile_blood_splatter_4,
    
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

// TODO(Rami): Do we need this?
#if 0
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
#endif

typedef struct
{
    b32 is_occupied;
    
    u32 blood_tile;
    u32 tile;
} tile_t;

typedef struct
{
    u32 count;
    v4u array[MAX_DUNGEON_ROOMS];
} rooms_t;

typedef struct
{
    u32 width;
    tile_t *array;
} tile_data_t;

struct dungeon_t
{
    dungeon_type type;
    u32 level;
    
    u32 width;
    u32 height;
    
    u32 pathfind_map[MAX_DUNGEON_SIZE * MAX_DUNGEON_SIZE];
    fov_tile_t fov_tiles[MAX_DUNGEON_SIZE * MAX_DUNGEON_SIZE];
    tile_t tiles[MAX_DUNGEON_SIZE * MAX_DUNGEON_SIZE];
    
    // TODO(Rami): Not a huge fan of this being of rooms_t type.
    // We could have it be a v4u since we'd know what array elements aren't
    // initialized from the 0 in every member.
    rooms_t rooms;
    
    b32 can_have_rect_rooms;
    b32 can_have_double_rect_rooms;
    b32 can_have_automaton_rooms;
    
    u32 rect_min_size;
    u32 rect_max_size;
    
    u32 double_rect_min_size;
    u32 double_rect_max_size;
    
    u32 automaton_min_size;
    u32 automaton_max_size;
};