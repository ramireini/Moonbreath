#ifndef TYPES_H
#define TYPES_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <math.h>
#include <time.h>
#include <stdint.h>

// TODO(rami): Adjust array and #define sizes!!!

#define assert(expression, message) \
{ \
    if(!(expression)) \
    { \
        fprintf(stderr, ("Assertion failed in %s, %u: %s\n"), __FILE__, __LINE__, message); \
        *(int *)0 = 0; \
    } \
}

#define internal static
#define global static
#define local_persist static

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

typedef u32 b32;
enum {false, true};

#define color_white V4u(255, 255, 240, 255)
#define color_red V4u(200, 0, 0, 255)
#define color_blue V4u(0, 128, 255, 255)
#define color_green V4u(0, 179, 0, 255)
#define color_yellow V4u(207, 175, 0, 255)
#define color_orange V4u(255, 165, 0, 255)
#define color_brown V4u(140, 70, 20, 255)
#define color_grey V4u(128, 128, 128, 255)
#define color_black V4u(0, 0, 0, 255)

typedef enum
{
    state_quit,
    state_running
} game_state;

typedef union
{
    struct
    {
        i32 x, y;
    };
    struct
    {
        i32 w, h;
    };
} v2i;

typedef union
{
    struct
    {
        u32 x, y;
    };
    struct
    {
        u32 w, h;
    };
} v2u;

typedef union
{
    struct
    {
        f32 x, y;
    };
    struct
    {
        u32 w, h;
    };
} v2f;

typedef union
{
    struct
    {
        u32 r, g, b, a;
    };
    struct
    {
        u32 x, y, w, h;
    };
} v4u;

typedef union
{
    struct
    {
        i32 r, g, b, a;
    };
    struct
    {
        i32 x, y, w, h;
    };
} v4i;

typedef struct
{
    u32 size;
    char *contents;
} file_t;

typedef enum
{
    up,
    down,
    left,
    right,
    
    top_left,
    top_right,
    bottom_left,
    bottom_right,
    
    cardinal_first = up,
    cardinal_last = top_left,
    cardinal_and_diagonal_last = bottom_right
} direction;

typedef struct
{
    v2u start_frame;
    v2u current_frame;
    u32 frame_count;
    u32 frame_duration;
    u32 frame_last_changed;
} sprite_t;

typedef struct
{
    b32 success;
    u32 value;
} return_data_t;

#include "pop_up_text.h"
#include "assets.h"
#include "fov.h"
#include "level_gen.h"
#include "item.h"
#include "player.h"
#include "monster.h"
#include "ui.h"

typedef struct
{
    item_t slots[INVENTORY_WIDTH * INVENTORY_HEIGHT];
    b32 open;
    v2u pos;
    u32 item_count;
    
    b32 item_is_moving;
    i32 moved_item_src_index;
    i32 moved_item_dest_index;
} inventory_t;

typedef struct
{
    game_state state;
    SDL_Window *window;
    v2u window_size;
    v2u console_size;
    SDL_Renderer *renderer;
    v4i camera;
    u32 turn;
    b32 turn_changed;
    f32 dt;
    f32 perf_count_frequency;
} game_t;

global game_t game;

global SDL_Texture *textures[tex_total];
global font_t *fonts[font_total];

global item_t items[64];
global item_info_t item_info[64];
global inventory_t inventory;

global message_t console_messages[9];

// TODO(rami): Make sure you can't go over this array element count
global pop_up_text_t pop_up_texts[32];

global level_t level;

global player_t player;
global monster_t monsters[32];
global u32 monster_spawn_chance[monster_total][MAX_DUNGEON_LEVEL];

#endif // TYPES_H