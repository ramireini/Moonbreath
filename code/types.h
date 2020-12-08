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
    GameMode_Quit,
    GameMode_MainMenu,
    GameMode_Playing,
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

typedef struct
{
    u32 entry_count;
    u32 start;
    u32 end;
} View;

typedef struct
{
    b32 ended_down;
    b32 has_been_up;
} InputState;

typedef enum
{
    MouseButton_Left,
    MouseButton_Middle,
    MouseButton_Right,
    MouseButton_Extended1,
    MouseButton_Extended2,
    
    MouseButton_Count
} MouseButton;

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
    AlphabetKey_A,
    AlphabetKey_B,
    AlphabetKey_C,
    AlphabetKey_D,
    AlphabetKey_E,
    AlphabetKey_F,
    AlphabetKey_G,
    AlphabetKey_H,
    AlphabetKey_I,
    AlphabetKey_J,
    AlphabetKey_K,
    AlphabetKey_L,
    AlphabetKey_M,
    AlphabetKey_N,
    AlphabetKey_O,
    AlphabetKey_P,
    AlphabetKey_Q,
    AlphabetKey_R,
    AlphabetKey_S,
    AlphabetKey_T,
    AlphabetKey_U,
    AlphabetKey_V,
    AlphabetKey_W,
    AlphabetKey_X,
    AlphabetKey_Y,
    AlphabetKey_Z,
    
    AlphabetKey_Count
} AlphabetKey;

typedef enum
{
    MouseScroll_None,
    
    MouseScroll_Up,
    MouseScroll_Down
} MouseScroll;

typedef enum
{
    ModifierKey_None,
    
    ModifierKey_Shift,
    ModifierKey_Control,
    ModifierKey_Alt
} ModifierKey;

typedef enum
{
    PageMoveKey_None,
    
    PageMoveKey_PageUp,
    PageMoveKey_PageDown
} PageMoveKey;

typedef struct
{
    f32 frame_dt;
    v2u mouse_pos;
    v2u mouse_tile_pos;
    
    // TODO(rami): Actually use page_move_key
    MouseScroll mouse_scroll;
    PageMoveKey page_move_key;
    b32 is_shift_down;
    b32 is_control_down;
    b32 is_alt_down;
    
    InputState fkeys[13];
    
    union
    {
        InputState mouse[MouseButton_Count];
        struct
        {
            InputState Button_Left;
            InputState Button_Middle;
            InputState Button_Right;
            InputState Button_Extended1;
            InputState Button_Extended2;
        };
    };
    
    InputState alphabet_keys[AlphabetKey_Count];
    
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
} Input;

#if MOONBREATH_SLOW
global b32 fkey_active[13];
#endif

#include "random.h"
#include "item.h"
#include "dungeon.h"
#include "assets.h"
#include "ui.h"
#include "entity.h"
#include "moonbreath.h"

internal void render_item_window(Game *game,
                                 UI *ui,
                                 Inventory *inventory,
                                 Item *item,
                                 Assets *assets);