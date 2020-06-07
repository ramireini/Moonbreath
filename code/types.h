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

#define MAX_U32 ((u32)-1)

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

typedef enum
{
    key_up,
    key_down,
    key_left,
    key_right,
    
    key_up_left,
    key_up_right,
    key_down_left,
    key_down_right,
    
    key_inventory,
    key_equip_or_consume_item,
    key_pick_up_or_drop_item,
    key_identify_or_enchant_item,
    key_move_item,
    key_ascend_or_descend,
    key_wait,
    key_yes,
    key_no,
    
    // TODO(rami): I would like to have a rest button, we'd rest until
    // HP or Mana is full. Before that we need to add automatic generation of
    // HP and Mana.
    
#if MOONBREATH_SLOW
    key_toggle_fov,
    key_toggle_traversable_check,
    key_toggle_has_been_up_check,
    key_toggle_identify,
#endif
    
    key_count
} keyboard_key;

typedef struct
{
    char str[128];
} string_128_t;

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

typedef struct
{
    b32 is_down;
    b32 has_been_up;
} input_state_t;

typedef struct
{
    f32 dt;
    v2u mouse_pos;
    
    union
    {
        input_state_t mouse[button_count];
        struct
        {
            input_state_t button_left;
            input_state_t button_middle;
            input_state_t button_right;
            input_state_t button_x1;
            input_state_t button_x2;
        };
    };
    
    union
    {
        input_state_t keyboard[key_count];
        struct
        {
            input_state_t key_up;
            input_state_t key_down;
            input_state_t key_left;
            input_state_t key_right;
            
            input_state_t key_up_left;
            input_state_t key_up_right;
            input_state_t key_down_left;
            input_state_t key_down_right;
            
            input_state_t key_inventory;
            input_state_t key_equip_or_consume_item;
            input_state_t key_pick_up_or_drop_item;
            input_state_t key_identify_item;
            input_state_t key_move_item;
            input_state_t key_ascend_or_descend;
            input_state_t key_wait;
            input_state_t key_yes;
            input_state_t key_no;
            
#if MOONBREATH_SLOW
            input_state_t key_toggle_fov;
            input_state_t key_toggle_traversable_check;
            input_state_t key_toggle_has_been_up_check;
            input_state_t key_toggle_identify;
#endif
        };
    };
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
// Global debug values.
global b32 debug_fov;
global b32 debug_traversable;
global b32 debug_has_been_up;
#endif