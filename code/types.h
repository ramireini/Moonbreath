// TODO(rami): Adjust array and #define sizes!!!

#define DEBUG 1

#if DEBUG
#define assert(expression, message) \
{ \
    if(!(expression)) \
    { \
        fprintf(stderr, ("Assertion failed in %s, %u: %s\n"), __FILE__, __LINE__, message); \
        *(int *)0 = 0; \
    } \
}
#else
#define assert(expression, message)
#endif

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

#define color_white V4u(255, 255, 240, 255)
#define color_red V4u(200, 0, 0, 255)
#define color_blue V4u(0, 128, 255, 255)
#define color_green V4u(0, 179, 0, 255)
#define color_yellow V4u(204, 204, 0, 255)
#define color_orange V4u(255, 165, 0, 255)
#define color_brown V4u(140, 70, 20, 255)
#define color_light_brown V4u(205, 133, 63, 255)
#define color_grey V4u(128, 128, 128, 255)
#define color_black V4u(0, 0, 0, 255)

#define INVENTORY_WIDTH 8
#define INVENTORY_HEIGHT 4
#define INVENTORY_SLOT_COUNT INVENTORY_WIDTH * INVENTORY_HEIGHT

#define SPRITE_ANIMATION_OFFSET 5

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
} direction;

typedef enum
{
    state_dead,
    state_idle
} entity_state;

typedef struct
{
    v2u current_frame;
    
    v2u idle_start_frame;
    u32 idle_frame_count;
    u32 idle_frame_duration;
    u32 idle_frame_last_changed;
    
    v2u dead_start_frame;
    u32 dead_frame_count;
    u32 dead_frame_duration;
    u32 dead_frame_last_changed;
} sprite_t;

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
    b32 is_open;
    
    item_t slots[INVENTORY_SLOT_COUNT];
    v2u current_slot;
    u32 item_count;
    
    b32 item_is_being_moved;
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
    v4s camera;
    u32 turn;
    b32 turn_changed;
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