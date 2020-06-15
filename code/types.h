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
#define internal_global static
#define persisting_local static

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

typedef enum
{
    Button_Left,
    Button_Middle,
    Button_Right,
    Button_X1,
    Button_X2,
    
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
    
    Key_Inventory,
    Key_EquipOrConsumeItem,
    Key_PickupOrDropItem,
    Key_IdentifyOrEnchantItem,
    Key_MoveItem,
    Key_AscendOrDescend,
    Key_Wait,
    Key_Yes,
    Key_No,
    
    // TODO(rami): I would like to have a rest button, we'd rest until
    // HP or Mana is full. Before that we need to add automatic generation of
    // HP and Mana.
    
#if MOONBREATH_SLOW
    Key_ToggleFov,
    Key_ToggleTraversable,
    Key_ToggleHasBeenUp,
    Key_ToggleIdentify,
#endif
    
    Key_Count
} Key;

typedef struct
{
    char str[128];
} String128;

typedef struct
{
    u32 size;
    char *contents;
} File;

typedef struct
{
    b32 success;
    u32 value;
} u32_bool;

typedef struct
{
    b32 success;
    v2u pos;
} v2u_bool;

typedef struct
{
    b32 success;
    v4u rect;
} v4u_bool;

typedef struct
{
    b32 is_down;
    b32 has_been_up;
} InputState;

typedef struct
{
    f32 dt;
    v2u mouse_pos;
    
    union
    {
        InputState mouse[Button_Count];
        struct
        {
            InputState Button_Left;
            InputState Button_Middle;
            InputState Button_Right;
            InputState Button_X1;
            InputState Button_X2;
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
            
            InputState Key_Inventory;
            InputState Key_EquipOrConsumeItem;
            InputState Key_PickupOrDropItem;
            InputState Key_IdentifyOrEnchantItem;
            InputState Key_MoveItem;
            InputState Key_AscendOrDescend;
            InputState Key_Wait;
            InputState Key_Yes;
            InputState Key_No;
            
#if MOONBREATH_SLOW
            InputState Key_ToggleFov;
            InputState Key_ToggleTraversable;
            InputState Key_ToggleHasBeenUp;
            InputState Key_ToggleIdentify;
#endif
        };
    };
} GameInput;

#include "random.h"
#include "moonbreath.h"
#include "dungeon.h"
#include "assets.h"
#include "entity.h"
#include "item.h"
#include "ui.h"

#if MOONBREATH_SLOW
// Global debug values.
internal_global b32 debug_fov;
internal_global b32 debug_traversable;
internal_global b32 debug_has_been_up;
#endif