#define MAX_SELECT_LETTER_COUNT MAX_INVENTORY_SLOT_COUNT
#define MAX_LOG_MESSAGE_COUNT 1024
#define MAX_MARK_SIZE 64

typedef enum
{
    LetterParentType_None,
    
    LetterParentType_Entity,
    LetterParentType_Item,
    LetterParentType_Trap
} LetterParentType;

typedef struct
{
    LetterParentType type;
    
    union
    {
        Entity *entity;
        Item *item;
        DungeonTrap *trap;
    };
    } LetterParent;

typedef struct
{
    LetterParentType parent_type;
    
    union
    {
    Item *item;
    Entity *entity;
        DungeonTrap *trap;
    };
    
    char c;
    } Letter;

typedef struct
{
    u32 count;
    u32 start;
    u32 end;
} View;

typedef struct
{
    b32 render_cursor;
    u32 cursor_render_start;
    u32 cursor_index;
    
    // If this value is less than 650 it means that the cursor gets to blink an extra time
    // before we start to force render it when we hold down a key. Because of that we keep
    // it at a value higher than 650.
    u32 cursor_blink_duration;
    
    View view;
    char array[MAX_MARK_SIZE];
} Mark;

typedef struct
{
    b32 is_set;
    char string[128];
    } LogMessage;

typedef struct
{
    Font *font;
    u32 font_newline;
    u32 window_offset;
    
     LogMessage log_messages[MAX_LOG_MESSAGE_COUNT];
    
    b32 full_log_open;
    b32 full_log_at_end;
    
    View full_log_view;
    View short_log_view;
    
    Mark mark;
    Letter letters[MAX_SELECT_LETTER_COUNT];
    
    v4u defer_rect;
    Defer defer[MAX_DEFER_COUNT];
    
    u32 window_scroll_start_y;
} UI;

internal void set_view_at_end(View *view);
internal void set_view_at_start(View *view);
internal void log_add(UI *ui, char *text, ...);
internal void update_view_scrolling(View *view, Input *input);
internal void reset_letters(Letter *letters);
internal b32 is_view_scrolling(View view, u32 count);
internal u32 get_view_range(View view);
internal LetterParent get_letter_parent(Letter *letters, char c);