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
#define color_dark_red    V4f(0.54f, 0.14f, 0.14f, 1.0f)
#define color_green       V4f(0.0f, 0.8f, 0.0f, 1.0f)
#define color_blue        V4f(0.0f, 0.0f, 0.8f, 1.0f)

#define color_yellow      V4f(1.0f, 1.0f, 0.05f, 1.0f)
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
    state_exit,
    state_main_menu,
    state_in_game
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

typedef struct
{
    SDL_Texture *tex;
    u32 w, h;
} texture_t;

typedef struct
{
    texture_t tilemap;
    texture_t tileset;
    texture_t item_tileset;
    texture_t wearable_item_tileset;
    texture_t sprite_sheet;
    
    SDL_Texture *ui;
    v4u health_bar_outside;
    v4u health_bar_inside;
    v4u log_window;
    v4u inventory_window;
    v4u inventory_selected_slot;
    v4u inventory_equipped_slot;
    v4u item_window;
} textures_t;

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
    
    key_move_up_left,
    key_move_up_right,
    key_move_down_left,
    key_move_down_right,
    
    key_inventory,
    key_pick_up,
    key_drop,
    key_equip,
    key_consume,
    key_move,
    key_ascend,
    key_descend,
    
#if MOONBREATH_SLOW
    key_debug_fov,
    key_debug_player_traversable_check,
    key_debug_has_been_up_check,
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
    v2u mouse_pos;
    input_state_t mouse[button_count];
    input_state_t keyboard[key_count];
} game_input_t;

typedef struct
{
    b32 initialized;
    game_state state;
    
    v2u window_size;
    SDL_Window *window;
    SDL_Renderer *renderer;
    
    v4s camera;
    u32 turn;
    f32 dt;
    
    u32 keybinds[key_count];
} game_t;

#include "pop_text.h"
#include "assets.h"
#include "fov.h"
#include "dungeon.h"
#include "item.h"
#include "player.h"
#include "monster.h"
#include "ui.h"

global game_t game;
global textures_t textures;
global font_t *fonts[font_total];
global item_t items[128];
global item_info_t item_info[128];
global inventory_t inventory;
global log_message_t log_messages[8];
global pop_text_t pop_texts[32];
global dungeon_t dungeon;
global player_t player;
global monster_t monsters[32];
global u32 monster_spawn_chance[monster_total][MAX_DUNGEON_LEVEL];

#if MOONBREATH_SLOW

// TODO(rami): Global debug values
global b32 debug_fov;
global b32 debug_player_traversable;
global b32 debug_has_been_up;

#endif