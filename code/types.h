// TODO(rami): Adjust array and #define sizes!!!

#if MOONBREATH_SLOW
#define assert(expression, message) \
{ \
    if(!(expression)) \
    { \
        fprintf(stderr, ("Assertion failed in %s, %u: %s\n"), __FILE__, __LINE__, message); \
        *(u32 *)0 = 0; \
    } \
}
#else
#define assert(expression, message)
#endif

#define invalid_code_path assert(0, "InvalidCodePath");
#define invalid_default_case default: {invalid_code_path;} break;

#define internal static
#define global static
#define local_persist static

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

typedef s32 b32;
enum {false, true};

#define color_black       V4f(0.0f, 0.0f, 0.0f, 1.0f)
#define color_gray        V4f(0.67f, 0.67f, 0.67f, 1.0f)
#define color_white       V4f(0.97f, 0.97f, 1.0f, 1.0f)

#define color_red         V4f(0.8f, 0.0f, 0.0f, 1.0f)
#define color_dark_red   V4f(0.54f, 0.14f, 0.14f, 1.0f)
#define color_green       V4f(0.0f, 0.8f, 0.0f, 1.0f)
#define color_blue        V4f(0.0f, 0.0f, 0.8f, 1.0f)

#define color_yellow      V4f(0.8f, 0.8f, 0.0f, 1.0f)
#define color_orange      V4f(1.0f, 0.55f, 0.0f, 1.0f)
#define color_brown       V4f(0.55f, 0.27f, 0.07f, 1.0f)
#define color_light_brown V4f(0.68f, 0.38f, 0.22f, 1.0f)

typedef union
{
    struct
    {
        s32 x, y;
    };
    
    struct
    {
        s32 w, h;
    };
} v2s;

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
        f32 w, h;
    };
} v2f;

typedef union
{
    struct
    {
        s32 r, g, b, a;
    };
    
    struct
    {
        s32 x, y, w, h;
    };
} v4s;

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
        f32 r, g, b, a;
    };
} v4f;

typedef enum
{
    state_quit,
    state_running
} game_state;

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
    
    direction_count
} direction;

typedef struct
{
    SDL_Texture *tex;
    u32 w, h;
} texture_t;

typedef struct
{
    u32 size;
    char *contents;
} file_t;

typedef struct
{
    b32 success;
    u32 value;
} u32_t;

typedef struct
{
    b32 success;
    v2u pos;
} v2u_t;

typedef struct
{
    b32 success;
    v4u rect;
} v4u_t;

typedef enum
{
    button_left,
    button_middle,
    button_right,
    button_x1,
    button_x2,
    
    button_count
} mouse_button;

typedef enum
{
    key_move_up,
    key_move_down,
    key_move_left,
    key_move_right,
    
    key_count
} keyboard_key;

typedef struct
{
    b32 is_down;
    b32 was_up;
} input_state_t;

typedef struct
{
    u32 mouse_x, mouse_y;
    input_state_t mouse[button_count];
    input_state_t keyboard[key_count];
} game_input_t;

#include "pop_text.h"
#include "assets.h"
#include "fov.h"
#include "dungeon.h"
#include "item.h"
#include "player.h"
#include "monster.h"
#include "ui.h"

typedef struct
{
    game_state state;
    SDL_Window *window;
    v2u window_size;
    v2u console_size;
    SDL_Renderer *renderer;
    v4s camera;
    u32 turn;
    b32 turn_has_changed;
    f32 dt;
} game_t;

global game_t game;
global texture_t textures[tex_total];
global font_t *fonts[font_total];
global item_t items[32];
global item_info_t item_info[32];
global inventory_t inventory;
global console_text_t console_texts[8];
global pop_text_t pop_texts[32];
global dungeon_t dungeon;
global player_t player;
global monster_t monsters[8];
global u32 monster_spawn_chance[monster_total][MAX_DUNGEON_LEVEL];

// NOTE(rami): Toggles
b32 toggle_fov;