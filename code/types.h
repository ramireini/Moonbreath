#if MOONBREATH_SLOW
#define assert(expression) \
{ \
if(!(expression)) \
{ \
fprintf(stderr, ("Assertion in %s, %u\n"), __FILE__, __LINE__); \
*(u32 *)0 = 0; \
} \
}
#else
#define assert(expression)
#endif

#define invalid_default_case default: {assert(0);} break;

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

typedef u32 b32;
enum {false, true};

// NOTE(rami): RGB colors, with text color codes on the right.
#define color_black V4u(0, 0, 0, 255)             // ##0
#define color_white V4u(238, 238, 236, 255)       // ##1

#define color_light_gray V4u(186, 189, 182, 255)  // ##2
#define color_dark_gray V4u(85, 87, 83, 255)      // ##3

#define color_light_red V4u(240, 15, 15, 255)     // ##4
#define color_dark_red V4u(164, 0, 0, 255)        // ##5

#define color_light_green V4u(80, 248, 80, 255)   // ##6
#define color_dark_green V4u(78, 154, 6, 255)     // ##7

#define color_light_blue V4u(114, 159, 207, 255)  // ##8
#define color_dark_blue V4u(0, 0, 0, 255) // TODO(Rami): // ##9

#define color_light_brown V4u(0, 0, 0, 255) // TODO(Rami): // ##A
#define color_dark_brown V4u(128, 79, 1, 255) // ##B

#define color_cyan V4u(6, 152, 154, 255)          // ##C
#define color_yellow V4u(252, 233, 79, 255)       // ##D
#define color_purple V4u(89, 51, 99, 255)         // ##E
#define color_orange V4u(0, 0, 0, 255) // TODO(Rami): // ##F

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
    game_state_exit,
    game_state_main_menu,
    game_state_in_game
} game_state;

typedef enum
{
    dir_none,
    
    dir_up,
    dir_down,
    dir_left,
    dir_right,
    
    dir_up_left,
    dir_up_right,
    dir_down_left,
    dir_down_right,
    
    dir_count
} direction;

typedef enum
{
    button_left,
    button_middle,
    button_right,
    button_x1,
    button_x2,
    
    button_count
} mouse_button;

typedef struct
{
    char str[128];
} string_t;

typedef struct
{
    u32 size;
    char *contents;
} file_t;

typedef struct
{
    b32 success;
    u32 value;
} u32_bool_t;

typedef struct
{
    b32 success;
    v2u pos;
} v2u_bool_t;

typedef struct
{
    b32 success;
    v4u rect;
} v4u_bool_t;

typedef enum
{
    key_move_up,
    key_move_down,
    key_move_left,
    key_move_right,
    
    key_move_up_left,
    key_move_up_right,
    key_move_down_left,
    key_move_down_right,
    
    key_inventory,
    key_pick_up_item,
    key_drop_item,
    key_equip_item,
    key_consume_item,
    key_move_item,
    
    key_ascend,
    key_descend,
    key_wait,
    
    // TODO(rami): I would like to have a rest button, we'd rest until
    // HP or Mana is full. Before that we need to add automatic generation of
    // HP and Mana.
    
#if MOONBREATH_SLOW
    key_toggle_fov,
    key_toggle_traversable_check,
    key_toggle_has_been_up_check,
#endif
    
    key_count
} keyboard_key;

typedef struct
{
    b32 is_down;
    b32 has_been_up;
} input_state_t;

typedef struct
{
    f32 dt;
    v2u mouse_pos;
    input_state_t mouse[button_count];
    input_state_t keyboard[key_count];
} game_input_t;

typedef struct dungeon_t dungeon_t;

#include "random.c"
#include "fov.h"
#include "ui.h"
#include "dungeon.h"
#include "assets.h"

typedef struct
{
    b32 is_initialized;
    
    game_state state;
    random_state_t random;
    v4s camera;
    f32 time;
    
    v2u window_size;
    SDL_Window *window;
    SDL_Renderer *renderer;
    
    u32 keybinds[key_count];
} game_state_t;

#include "entity.h"
#include "item.h"

internal v4u tile_rect(v2u tile);
internal v4u game_dest(game_state_t *game, v2u pos);
internal b32 is_input_valid(input_state_t *state);

#if MOONBREATH_SLOW
// NOTE(rami): Global debug values.
global b32 debug_fov;
global b32 debug_traversable;
global b32 debug_has_been_up;
#endif