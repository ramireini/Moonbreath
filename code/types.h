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
    MouseButton_Left,
    MouseButton_Middle,
    MouseButton_Right,
    MouseButton_Extended1,
    MouseButton_Extended2,
    MouseButton_ScrollUp,
    MouseButton_ScrollDown,
    
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
    KeyboardKey_A,
    KeyboardKey_B,
    KeyboardKey_C,
    KeyboardKey_D,
    KeyboardKey_E,
    KeyboardKey_F,
    KeyboardKey_G,
    KeyboardKey_H,
    KeyboardKey_I,
    KeyboardKey_J,
    KeyboardKey_K,
    KeyboardKey_L,
    KeyboardKey_M,
    KeyboardKey_N,
    KeyboardKey_O,
    KeyboardKey_P,
    KeyboardKey_Q,
    KeyboardKey_R,
    KeyboardKey_S,
    KeyboardKey_T,
    KeyboardKey_U,
    KeyboardKey_V,
    KeyboardKey_W,
    KeyboardKey_X,
    KeyboardKey_Y,
    KeyboardKey_Z,
    
    KeyboardKey_PageUp,
    KeyboardKey_PageDown,
    KeyboardKey_Home,
    KeyboardKey_End,
    
    KeyboardKey_Shift,
    KeyboardKey_Control,
    KeyboardKey_Alt,
    
    KeyboardKey_Escape,
    KeyboardKey_Enter,
    KeyboardKey_Space,
    KeyboardKey_Backspace,
    
    KeyboardKey_ArrowLeft,
    KeyboardKey_ArrowRight,
    
    KeyboardKey_Count
} KeyboardKey;

typedef struct
{
    b32 repeat;
    b32 ended_down;
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
        InputState mouse[MouseButton_Count];
        struct
        {
            InputState MouseButton_Left;
            InputState MouseButton_Middle;
            InputState MouseButton_Right;
            InputState MouseButton_Extended1;
            InputState MouseButton_Extended2;
            InputState MouseButton_ScrollUp;
            InputState MouseButton_ScrollDown;
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
        InputState keyboard[KeyboardKey_Count];
        struct
        {
            InputState KeyboardKey_A;
            InputState KeyboardKey_B;
            InputState KeyboardKey_C;
            InputState KeyboardKey_D;
            InputState KeyboardKey_E;
            InputState KeyboardKey_F;
            InputState KeyboardKey_G;
            InputState KeyboardKey_H;
            InputState KeyboardKey_I;
            InputState KeyboardKey_J;
            InputState KeyboardKey_K;
            InputState KeyboardKey_L;
            InputState KeyboardKey_M;
            InputState KeyboardKey_N;
            InputState KeyboardKey_O;
            InputState KeyboardKey_P;
            InputState KeyboardKey_Q;
            InputState KeyboardKey_R;
            InputState KeyboardKey_S;
            InputState KeyboardKey_T;
            InputState KeyboardKey_U;
            InputState KeyboardKey_V;
            InputState KeyboardKey_W;
            InputState KeyboardKey_X;
            InputState KeyboardKey_Y;
            InputState KeyboardKey_Z;
            
            InputState KeyboardKey_PageUp;
            InputState KeyboardKey_PageDown;
            InputState KeyboardKey_Home;
            InputState KeyboardKey_End;
            
            InputState KeyboardKey_Shift;
            InputState KeyboardKey_Control;
            InputState KeyboardKey_Alt;
            
            InputState KeyboardKey_Escape;
            InputState KeyboardKey_Enter;
            InputState KeyboardKey_Space;
            InputState KeyboardKey_Backspace;
            
            InputState KeyboardKey_ArrowLeft;
            InputState KeyboardKey_ArrowRight;
        };
    };
} Input;

#include "assets.h"
#include "ui.h"
#include "random.h"
#include "item.h"
#include "dungeon.h"
#include "entity.h"
#include "moonbreath.h"

#if MOONBREATH_SLOW
global b32 fkey_active[13];
#endif

#define set(flags, new_flags) (flags |= (new_flags))
#define unset(flags, new_flags) (flags &= ~(new_flags))
#define is_set(flags, new_flags) ((flags & (new_flags)) == (new_flags))