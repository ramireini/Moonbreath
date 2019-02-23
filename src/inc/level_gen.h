#ifndef LEVEL_GEN_H
#define LEVEL_GEN_H

#define LEVEL_WIDTH_IN_PIXELS 2048
#define LEVEL_HEIGHT_IN_PIXELS 2048
#define TILE_SIZE 32
#define LEVEL_WIDTH_IN_TILES LEVEL_WIDTH_IN_PIXELS / TILE_SIZE
#define LEVEL_HEIGHT_IN_TILES LEVEL_HEIGHT_IN_PIXELS / TILE_SIZE

#define ROOM_COUNT 30

#define START_ALIVE_CHANCE 55
#define SMOOTHING_ITERATIONS 5
#define DEATH_LIMIT 3
#define BIRTH_LIMIT 3

#define ALIVE tile_floor_stone
#define DEAD tile_none

// NOTE(Rami): Remember to add level tiles
// to the functions that use them.
typedef enum
{
  tile_none = 0,
  tile_wall_stone,
  tile_floor_grass,
  tile_floor_stone,
  tile_door_closed,
  tile_door_open,
  tile_path_up,
  tile_path_down
} tile_e;

typedef enum
{
  type_rectangle = 0,
  type_overlaid_rectangle,
  type_cellular_automata,
} type_e;

typedef struct
{
  u8 room_buffer[LEVEL_WIDTH_IN_TILES * LEVEL_HEIGHT_IN_TILES];
  u8 temp_buffer[LEVEL_WIDTH_IN_TILES * LEVEL_HEIGHT_IN_TILES];
} level_gen_buffers_t;

typedef struct
{
  i32 x;
  i32 y;
} pos_t;

typedef struct
{
  i32 w;
  i32 h;
} dimensions_t;

typedef struct
{
  i32 x;
  i32 y;
  i32 w;
  i32 h;
} room_t;

extern u8 level[LEVEL_WIDTH_IN_TILES * LEVEL_HEIGHT_IN_TILES];

i32 count_alive_neighbours(level_gen_buffers_t *buffers, pos_t p);
i32 copy_src_to_dst(u8 *src, u8 *dst, SDL_Rect src_r, pos_t dst_c);
i32 set_rect_to_dst(u8 *dst, SDL_Rect r, i32 tile);
i32 is_rect_in_dst_unused(u8 *dst, SDL_Rect r);
i32 clear_dst(u8 *dst);
i32 search_for_door_position(pos_t c, pos_t *door);
void add_walls_to_rect_in_dst(u8 *dst, SDL_Rect r);
i32 can_room_be_placed(level_gen_buffers_t *buffers, SDL_Rect r);
void smoothing(level_gen_buffers_t *buffers, dimensions_t r);
i32 generate_room(level_gen_buffers_t *buffers, SDL_Rect r);
void generate_level();

#endif // LEVEL_GEN_H