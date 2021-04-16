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
    u32 entry_count;
    v2u pos;
    View view;
} DeferWindow;

typedef struct
{
    b32 render_cursor;
    u32 cursor_render_start;
    u32 cursor_index;
    
    // If cursor_blink_duration is less than 650 when we hold down a key, the cursor will
    // blink one extra time which looks bad.
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
    
    b32 full_log_open;
    b32 full_log_at_end;
    View full_log_view;
    View short_log_view;
     LogMessage log_messages[MAX_LOG_MESSAGE_COUNT];
    
    Letter select_letters[MAX_SELECT_LETTER_COUNT];
    
    v4u defer_rect;
    Defer defer[MAX_DEFER_COUNT];
    
    u32 window_entry_size;
    u32 window_scroll_start_y;
} UI;

internal void clear_letter(Letter *letters, char search_c);
internal void reset_letters(Letter *letters);
internal void set_view_at_end(View *view);
internal void set_view_at_start(View *view);
internal void log_add(UI *ui, char *text, ...);
internal void update_view_scrolling(View *view, Input *input);
internal char set_letter(Letter *letters, Letter *letter, void *parent, LetterParentType parent_type);
internal char set_next_letter(Letter *letters, void *parent, LetterParentType parent_type);
internal b32 is_view_scrolling(View view, u32 count);
internal b32 is_entry_in_view(View view, u32 entry);
internal u32 get_view_range(View view);
internal Letter *get_letter(Letter *letters, char search_c);
internal String8 get_item_letter_string(Item *item);