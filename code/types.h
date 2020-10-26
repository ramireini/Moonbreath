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
    GameMode_Game,
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
    b32 ended_down;
    b32 has_been_up;
} InputState;

typedef enum
{
    Button_Left,
    Button_Middle,
    Button_Right,
    Button_Extended1,
    Button_Extended2,
    
    Button_Count
} Button;

typedef enum
{
    Key_Up,
    Key_Down,
    Key_Left,
    Key_Right,
    
    Key_UpLeft,
    Key_UpRight,
    Key_DownLeft,
    Key_DownRight,
    
    Key_InventoryOpen,
    Key_InventoryAction,
    Key_InventoryMove,
    
    Key_PickupDrop,
    Key_AscendDescend,
    Key_AutoExplore,
    Key_IteratePassages,
    Key_Examine,
    Key_Log,
    
    Key_Wait,
    Key_Yes,
    Key_No,
    
    Key_Count
} Key;

typedef struct
{
    f32 frame_dt;
    v2u mouse_pos;
    v2u mouse_tile_pos;
    
    b32 is_shift_down;
    b32 is_control_down;
    b32 is_alt_down;
    
    InputState fkeys[13];
    
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
        };
    };
    
    union
    {
        InputState keyboard[Key_Count];
        struct
        {
            InputState Key_Up;
            InputState Key_Down;
            InputState Key_Left;
            InputState Key_Right;
            
            InputState Key_UpLeft;
            InputState Key_UpRight;
            InputState Key_DownLeft;
            InputState Key_DownRight;
            
            InputState Key_InventoryOpen;
            InputState Key_InventoryAction;
            InputState Key_InventoryMove;
            
            InputState Key_PickupDrop;
            InputState Key_AscendDescend;
            InputState Key_AutoExplore;
            InputState Key_IteratePassages;
            InputState Key_Examine;
            InputState Key_Log;
            
            InputState Key_Wait;
            InputState Key_Yes;
            InputState Key_No;
        };
    };
} GameInput;

#if MOONBREATH_SLOW
global b32 fkey_active[13];
#endif

#include "random.h"
#include "moonbreath.h"
#include "item.h"
#include "dungeon.h"
#include "assets.h"
#include "ui.h"
#include "entity.h"