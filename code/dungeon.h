#define MAX_DUNGEON_WIDTH 64
#define MAX_DUNGEON_HEIGHT 64
#define MAX_DUNGEON_LEVEL 10

#define MAX_ROOM_COUNT 64

// TODO(rami):
// Thinking about dungeons, I would like generate_dungeon to work with any dungeon size,
// and there would have to be a minimum size which you can't go under.

// The set_details function would need to select how many certain details it would
// want to place based on the size of the dungeon it's generating.

// This way everything is based on the dungeon you're trying to generate
// instead of predetermined values.

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
    room_rectangle,
    room_double_rectangle,
    room_automaton
} room_type;

typedef enum
{
    rectangle_min_size = 4,
    rectangle_max_size = 8,
    
    double_rectangle_min_size = 4,
    double_rectangle_max_size = 6,
    
    automaton_min_size = 8,
    automaton_max_size = 16
} room_size;

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
    u32 level;
    u32 w, h;
    fov_tile_t fov_tiles[MAX_DUNGEON_HEIGHT][MAX_DUNGEON_WIDTH];
    tile_t tiles[MAX_DUNGEON_HEIGHT][MAX_DUNGEON_WIDTH];
} dungeon_t;