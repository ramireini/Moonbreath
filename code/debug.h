typedef enum
{
    DebugVariableType_Newline,
    DebugVariableType_Group,
    DebugVariableType_Text,
    
    DebugVariableType_U32,
    DebugVariableType_B32,
    DebugVariableType_F32,
    DebugVariableType_V2U
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
    DebugVariableType type;
    char name[32];
    Color color;
    
    DebugVariable *next;
    DebugVariable *parent_group;
    
    union
    {
        b32 *b32;
        u32 *u32;
        f32 *f32;
        v2u *v2u;
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