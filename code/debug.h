#define MAX_DEBUG_TREE_COUNT 64

typedef enum
{
    DebugVariableType_Newline,
    DebugVariableType_Group,
    DebugVariableType_Text,
    
    DebugVariableType_S32,
    DebugVariableType_U32,
    DebugVariableType_B32,
    DebugVariableType_F32,
    DebugVariableType_V2U,
    DebugVariableType_V4U,
    DebugVariableType_String,
    DebugVariableType_Enum,
        DebugVariableType_Flag
} DebugVariableType;

typedef enum
{
    DebugHotType_None,
    
    DebugHotType_Item,
    DebugHotType_Entity,
    DebugHotType_Variable
} DebugHotType;

typedef enum
{
    DebugInteractionType_None,
    
    DebugInteractionType_Move,
        DebugInteractionType_Delete
} DebugInteractionType;

typedef struct DebugVariable DebugVariable;
typedef struct
{
    b32 is_expanded;
    
    DebugVariable *first_child;
    DebugVariable *last_child;
} DebugVariableGroup;

struct DebugVariable
{
    char name[32];
    Color color;
    
    DebugVariable *next;
    DebugVariable *parent_group;
    
    // Giving add_debug_enum() a function will make it be used as a callback function.
    // The enum value will be turned into a string by using that callback.
    char *(*enum_to_string_callback)(u32);
    
    // The state of the flag in the union will be checked for from this flags pointer.
    u32 *flags;
    
    DebugVariableType type;
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
        
        DebugVariableGroup group;
    };
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
    
    DebugColorPair move_color;
    DebugColorPair delete_color;
    DebugColorPair group_text_color;
    Color text_color_active;
    
    DebugVariable *root;
} DebugTree;

typedef struct
{
    v2u pos;
    
    DebugHotType type;
    union
    {
        DebugVariable *var;
    };
} DebugHot;

typedef struct
{
    DebugInteractionType type;
    
    v2u *pos;
     DebugTree *tree;
    } DebugInteraction;

typedef struct
{
    b32 is_shown;
    
    Font *font;
    v2u text_offset;
    
    memory_size memory_size;
    MemoryArena memory_arena;
    
     DebugTree trees[MAX_DEBUG_TREE_COUNT];
    
    DebugInteraction hot_interaction;
    DebugHot hot;
    } DebugState;