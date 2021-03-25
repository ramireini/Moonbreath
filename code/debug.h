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
    DebugContextType_None,
    
    DebugContextType_Default,
    DebugContextType_Active,
    
    DebugContextType_Count
} DebugContextType;

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
    
    // This is a callback function that allows enum values to be turned into strings.
    // The callback can be given to the add_debug_enum() function.
    char *(*enum_to_string_callback)(u32);
    
    // Flags will point to the flags where we want to check from and flag in the union
    // is the flag we want to check for.
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
    v2u pos;
    
    DebugHotType type;
    union
    {
        Item *item;
        Entity *entity;
        DebugVariable *var;
    };
} DebugHot;

typedef struct
{
    v2u pos;
    DebugVariable *root;
} DebugContext;

typedef struct
{
    Font *font;
    
    u32 y_offset;
    u32 x_offset;
    
    memory_size memory_size;
    MemoryArena memory_arena;
    
    DebugContext contexts[DebugContextType_Count];
    DebugHot hot;
    } DebugState;