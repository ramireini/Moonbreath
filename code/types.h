#ifndef TYPES_H
#define TYPES_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <math.h>
#include <time.h>
#include <stdint.h>

// NOTE(rami): Change the array element values to some minimum needed amount.
// The config code uses these array sizes as well so make sure to change those too

// NOTE(rami): Do we really need this..
#if MOONBREATH_DEBUG
#define debug(fmt, ...) \
{ \
    fprintf(stderr, ("%s, %u: "fmt""), __FILE__, __LINE__, ##__VA_ARGS__); \
}
#else
#define debug(fmt, ...)
#endif

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
#define color_red V4u(140, 38, 38, 255)
#define color_blue V4u(0, 128, 255, 255)
#define color_green V4u(0, 179, 0, 255)
#define color_yellow V4u(207, 175, 0, 255)
#define color_orange V4u(255, 165, 0, 255)
#define color_brown V4u(231, 165, 106, 255)
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

typedef enum
{
    dir_up,
    dir_down,
    dir_left,
    dir_right,
    dir_left_up,
    dir_right_up,
    dir_left_down,
    dir_right_down,
    
    dir_count
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
    item_t slot[INVENTORY_SLOT_COUNT];
    b32 open;
    v2u pos;
    u32 item_count;
    
    b32 item_is_moving;
    u32 moved_item_src_index;
    u32 moved_item_dest_index;
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
global SDL_Texture *texture[tex_total];
global font_t font[font_total];
global inventory_t inventory;
global item_t item[ITEM_COUNT];
global item_info_t item_info[ITEM_INFO_COUNT];
global message_t console_message[CONSOLE_MESSAGE_COUNT];
global pop_up_text_t pop_up_text[POP_UP_TEXT_COUNT];
global level_t level;

global player_t player;
global monster_t monster[MONSTER_COUNT];

#endif // TYPES_H