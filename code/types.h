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

#define kilobytes(value) ((value) * 1024)
#define megabytes(value) (kilobytes(value) * 1024)
#define gigabytes(value) (megabytes(value) * 1024)

#define internal static
#define global static
#define persist static

#define U32_MAX UINT32_MAX

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef size_t memory_size;

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
    
    struct
    {
        s32 min, max;
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
    
    struct
    {
        u32 min, max;
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
    
    struct
    {
        f32 start, end;
    };
    
    struct
    {
        f32 left, right;
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
        u32 x, y, w, h;
    };
    
    struct
    {
        u32 r, g, b, a;
    };
    
    struct
    {
        u32 x_x, x_y, y_x, y_y;
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
    GameMode_Quit,
    GameMode_MainMenu,
    GameMode_Playing
} GameMode;

typedef enum
{
    Direction_None,
    
    Direction_Up,
    Direction_Down,
    Direction_Left,
    Direction_Right,
    
    Direction_UpLeft,
    Direction_UpRight,
    Direction_DownLeft,
    Direction_DownRight,
    
    Direction_Count
} Direction;

typedef struct
{
    char str[128];
} String128;

typedef enum
{
    Button_Left,
    Button_Middle,
    Button_Right,
    Button_Extended1,
    Button_Extended2,
    Button_ScrollUp,
    Button_ScrollDown,
    
    Button_Count
} Button;

typedef enum
{
    GameKey_Up,
    GameKey_Down,
    GameKey_Left,
    GameKey_Right,
    
    GameKey_UpLeft,
    GameKey_UpRight,
    GameKey_DownLeft,
    GameKey_DownRight,
    
    GameKey_OpenInventory,
    GameKey_Pickup,
    GameKey_AscendDescend,
    GameKey_AutoExplore,
    GameKey_IteratePassages,
    GameKey_Examine,
    GameKey_Log,
    
    GameKey_Back,
    GameKey_Wait,
    GameKey_Yes,
    GameKey_No,
    
    GameKey_Count
} GameKey;

typedef enum
{
    Key_None,
    
    // Printable characters
    Key_A,
    Key_B,
    Key_C,
    Key_D,
    Key_E,
    Key_F,
    Key_G,
    Key_H,
    Key_I,
    Key_J,
    Key_K,
    Key_L,
    Key_M,
    Key_N,
    Key_O,
    Key_P,
    Key_Q,
    Key_R,
    Key_S,
    Key_T,
    Key_U,
    Key_V,
    Key_W,
    Key_X,
    Key_Y,
    Key_Z,
    
    Key_0,
    Key_1,
    Key_2,
    Key_3,
    Key_4,
    Key_5,
    Key_6,
    Key_7,
    Key_8,
    Key_9,
    
    Key_Space,
    Key_Plus,
    Key_Minus,
    Key_Comma,
    Key_Period,
    Key_LessThan,
    Key_GreaterThan,
    
    // Non-printable characters
    Key_Shift,
    Key_Control,
    Key_Alt,
    
    Key_PageUp,
    Key_PageDown,
    
    Key_Del,
    Key_Home,
    Key_End,
    Key_Enter,
    Key_Escape,
    Key_Backspace,
    
    Key_ArrowLeft,
    Key_ArrowRight,
    
    Key_Count
} Key;

typedef struct
{
    b32 repeat;
    b32 is_down;
    b32 has_been_up;
} InputState;

typedef struct
{
    f32 frame_dt;
    InputState fkeys[13];
    
    v2u mouse_pos;
    v2u mouse_tile_pos;
    union
    {
        InputState mouse[Button_Count];
        struct
        {
            InputState Button_Left;
            InputState Button_Middle;
            InputState Button_Right;
            InputState Button_Extended1;
            InputState Button_Extended2;
            InputState Button_ScrollUp;
            InputState Button_ScrollDown;
        };
    };
    
    union
    {
        InputState game_keys[GameKey_Count];
        struct
        {
            InputState GameKey_Up;
            InputState GameKey_Down;
            InputState GameKey_Left;
            InputState GameKey_Right;
            
            InputState GameKey_UpLeft;
            InputState GameKey_UpRight;
            InputState GameKey_DownLeft;
            InputState GameKey_DownRight;
            
            InputState GameKey_OpenInventory;
            InputState GameKey_Pickup;
            InputState GameKey_AscendDescend;
            InputState GameKey_AutoExplore;
            InputState GameKey_IteratePassages;
            InputState GameKey_Examine;
            InputState GameKey_Log;
            
            InputState GameKey_Back;
            InputState GameKey_Wait;
            InputState GameKey_Yes;
            InputState GameKey_No;
        };
    };
    
    union
    {
        InputState keyboard[Key_Count];
        struct
        {
            InputState Key_None;
            
            InputState Key_A;
            InputState Key_B;
            InputState Key_C;
            InputState Key_D;
            InputState Key_E;
            InputState Key_F;
            InputState Key_G;
            InputState Key_H;
            InputState Key_I;
            InputState Key_J;
            InputState Key_K;
            InputState Key_L;
            InputState Key_M;
            InputState Key_N;
            InputState Key_O;
            InputState Key_P;
            InputState Key_Q;
            InputState Key_R;
            InputState Key_S;
            InputState Key_T;
            InputState Key_U;
            InputState Key_V;
            InputState Key_W;
            InputState Key_X;
            InputState Key_Y;
            InputState Key_Z;
            
            InputState Key_0;
            InputState Key_1;
            InputState Key_2;
            InputState Key_3;
            InputState Key_4;
            InputState Key_5;
            InputState Key_6;
            InputState Key_7;
            InputState Key_8;
            InputState Key_9;
            
            InputState Key_Space;
            InputState Key_Plus;
            InputState Key_Minus;
            InputState Key_Comma;
            InputState Key_Period;
            InputState Key_LessThan;
            InputState Key_GreaterThan;
            
            InputState Key_Shift;
            InputState Key_Control;
            InputState Key_Alt;
            
            InputState Key_PageUp;
            InputState Key_PageDown;
            
            InputState Key_Del;
            InputState Key_Home;
            InputState Key_End;
            InputState Key_Enter;
            InputState Key_Escape;
            InputState Key_Backspace;
            
            InputState Key_ArrowLeft;
            InputState Key_ArrowRight;
        };
    };
} Input;

#include "assets.h"
#include "ui.h"
#include "random.c"
#include "item.h"
#include "dungeon.h"
#include "entity.h"
#include "moonbreath.h"

internal void log_add_okay(UI *ui);
internal void log_add_item_cursed_unequip(UI *ui, Item *item);
internal void log_add_item_action_text(UI *ui, Item *item, ItemActionType action);
internal void add_player_starting_item(Game *game, Entity *player, ItemState *items, Inventory *inventory, UI *ui, ItemID item_id, u32 x, u32 y);
internal void set_render_color(Game *game, Color color);
internal u32 tile_div(u32 value);
internal u32 tile_mul(u32 value);
internal u32 potion_chance_index(ItemID id);
internal u32 scroll_chance_index(ItemID id);
internal u32 get_pos_item_count(ItemState *items, v2u pos);
internal b32 is_item_equipment(ItemType type);
internal ItemID random_leather_armor(Random *random);
internal ItemID random_steel_armor(Random *random);
internal ItemID random_weapon(Random *random);
internal ItemID random_potion(Random *random);
internal ItemID random_scroll(Random *random);
internal Texture load_texture(Game *game, char *path, v4u *color_key);
internal char *get_direction_string(Direction direction);

#if MOONBREATH_SLOW
global b32 fkey_active[13];
#endif

#define set(flags, new_flags) (flags |= (new_flags))
#define unset(flags, new_flags) (flags &= ~(new_flags))
#define is_set(flags, new_flags) ((flags & (new_flags)) == (new_flags))