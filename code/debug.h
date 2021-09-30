#define MAX_DEBUG_TREE_COUNT 8

typedef enum
{
    DebugVarType_Newline,
    DebugVarType_Group,
    DebugVarType_Text,
    
    DebugVarType_S32,
    DebugVarType_U32,
    DebugVarType_B32,
    DebugVarType_F32,
    DebugVarType_V2U,
    DebugVarType_V4U,
    DebugVarType_String,
    DebugVarType_Enum,
    DebugVarType_Flag
} DebugVarType;

typedef enum
{
    DebugHotType_None,
    
    DebugHotType_Item,
    DebugHotType_Entity,
    DebugHotType_Variable
} DebugHotType;

typedef enum
{
    DebugEventType_None,
    
    DebugEventType_MoveTree,
    DebugEventType_DeleteTree
} DebugEventType;

typedef enum
{
    DebugVarActionType_None,
    
    DebugVarActionType_DeleteItem,
    DebugVarActionType_KillEntity,
    DebugVarActionType_DeleteEntity,
} DebugVarActionType;

typedef struct DebugVar DebugVar;
typedef struct
{
    b32 is_expanded;
    
    DebugVar *first_child;
    DebugVar *last_child;
} DebugVarGroup;

struct DebugVar
{
    String32 name;
    Color color;
    
    DebugVar *next;
    DebugVar *parent_group;
    
    // Giving add_debug_enum() a function will make it be used as a callback function.
    // The enum value will be turned into a string by using that callback.
    char *(*enum_to_string_callback)(u32);
    
    // The state of the flag in the union will be checked for from this flags pointer.
    u32 *flags;
    
    DebugVarType type;
    union
    {
        b32 *b32;
        s32 *s32;
        u32 *u32;
        f32 *f32;
        v2u *v2u;
        v4u *v4u;
        char *string;
        u32 flag;
        
        DebugVarGroup group;
    };
    
    DebugVarActionType action;
    union
    {
        Item *item;
        Entity *entity;
    };
    
    EntityStatus *status;
    };

typedef struct
{
    Color active;
    Color inactive;
} DebugColorPair;

typedef struct
{
    b32 is_set;
    b32 is_moving;
    
    v2u pos;
    u32 rect_size;
    
    v4u move_rect;
    v4u delete_rect;
     char *delete_string;
    
    DebugColorPair move_color;
    DebugColorPair delete_color;
    DebugColorPair group_text_color;
    
    DebugVar *root;
} DebugTree;

typedef struct
{
    v2u pos;
    
    DebugHotType type;
    DebugVar *var;
} DebugHot;

typedef struct
{
    DebugEventType type;
    
    v2u *pos;
     DebugTree *tree;
    } DebugEvent;

typedef struct
{
 b32 is_open;
 b32 update_state;
 
    Font *font;
    v2u text_offset;
    
 DebugHot hot;
    DebugEvent event;
    
    memory_size memory_size;
    MemoryArena memory_arena;
    
     DebugTree trees[MAX_DEBUG_TREE_COUNT];
} DebugState;

internal void end_debug_group(DebugTree *tree);
internal void reset_debug_event(DebugEvent *event);
internal void add_debug_status_effect(DebugState *debug, DebugTree *tree, EntityStatus *status);
internal DebugVar *start_debug_group(DebugState *debug, DebugTree *tree, char *name, b32 is_expanded);