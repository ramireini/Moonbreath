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
    
    DebugContextType_Vars,
    DebugContextType_Colors,
    DebugContextType_Hot,
    
    DebugContextType_Count
} DebugContextType;

typedef enum
{
    DebugInteractionType_None,
    
    DebugInteractionType_Move
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
    // The enum value will be turned into a string using the callback given.
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
    b32 is_moving;
    
    v2u pos;
    u32 move_rect_size;
    v4u move_rect;
    
    Color group_move_color_active;
    Color group_move_color_inactive;
    
    Color group_text_color_active;
    Color group_text_color_inactive;
    
    Color text_color_active;
    
    DebugVariable *root;
} DebugContext;

typedef struct
{
    v2u pos;
    
    DebugHotType type;
    union
    {
        void *generic;
        DebugVariable *var;
        DebugContext *context;
    };
} DebugHot;

typedef struct
{
    DebugInteractionType type;
    
    v2u *pos;
    DebugContext *context;
    } DebugInteraction;

typedef struct
{
    Font *font;
    v2u text_offset;
    
    memory_size memory_size;
    MemoryArena memory_arena;
    
    DebugContext contexts[DebugContextType_Count];
    DebugInteraction hot_interaction;
    DebugHot hot;
    } DebugState;