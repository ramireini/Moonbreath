#define MAX_OWNER_COUNT 52 // The amount of characters in the alphabet, lower and uppercase.
#define MAX_LOG_MESSAGE_COUNT 512
#define MAX_DEFER_COUNT MAX_LOG_MESSAGE_COUNT
#define MAX_MARK_SIZE 64

typedef enum
{
    UIFlag_FullLogOpen = (1 << 1),
    UIFlag_PlayerStatusOpen = (1 << 2),
} UIFlag;

typedef enum
{
    OwnerType_None,
    
    OwnerType_Item,
    OwnerType_Spell,
    OwnerType_Entity,
    OwnerType_Trap
    } OwnerType;

typedef enum
{
    DeferType_None,
    
    DeferType_Text,
    DeferType_Tile,
    DeferType_FillRect,
    DeferType_OutlineRect,
    DeferType_Separator,
} DeferType;

typedef struct
{
    DeferType type;
    String256 text;
    
    v2u pos;
    v4u tile_src;
    v4u rect;
    Color rect_color;
} Defer;

typedef struct
{
    char c;
    OwnerType type;
    union
    {
        Item *item;
        Spell *spell;
    Entity *entity;
        DungeonTrap *trap;
    };
} Owner;

typedef struct
{
    b32 can_extend;
    
    u32 move_count;
    Direction direction;
    f32 step_multiplier;
    } ViewMoveParams;

typedef struct
{
    
#if MOONBREATH_SLOW
    b32 print_info;
#endif
    
    b32 is_moving;
    b32 can_extend;
    Direction direction;
    
    s32 offset;
    f32 moving_offset;
    f32 moving_offset_target;
    
    f32 step;
    u32 size;
    u32 count;
    } ViewMove;

typedef struct
{
    u32 entry_size;
    
    u32 count;
    u32 start;
    u32 end;
    
    f32 shared_step_multiplier;
    ViewMove move;
    
    v4u clip_rect;
    v4s content_rect;
} View;

// TODO(rami): Config: Different item mark styles:
// {This is a mark!}
// [This is a mark!]
// This is a mark! (Gray color, like a code comment)
typedef struct
{
    b32 render_cursor;
    u32 render_cursor_start_ms;
    u32 cursor;
    
    // If cursor_blink_duration is less than 650 when we hold down a key, the cursor will
    // blink one extra time which looks bad.
    u32 cursor_blink_duration;
    
    View view;
    char array[MAX_MARK_SIZE];
} Mark;

typedef struct
{
    b32 is_set;
    String256 string;
    } LogMessage;

typedef struct
{
    v2u pos;
    View view;
} DeferWindow;

typedef struct
{
    u32 flags;
    Font *font;
    
    View short_log_view;
    DeferWindow full_log;
     LogMessage log_messages[MAX_LOG_MESSAGE_COUNT];
    
    b32 defer_window_has_custom_width;
    v2u defer_window_padding;
    v4u defer_window_rect;
    Defer defers[MAX_DEFER_COUNT];
    
    u32 window_offset;
    u32 window_scroll_start_y;
    
    Owner mouse_highlight;
    f32 default_view_step_multiplier;
    
    Owner temp_owners[MAX_OWNER_COUNT];
    } UI;

// TODO(rami): Should we render a line in the full log to indicate what actions have taken place in
// the players last action turn.

internal void update_view_move(ViewMove *move);
internal void ui_print_view(char *name, View view);
internal void render_scrollbar(Game *game, v4u rect, View *view, UI *ui);
internal void clear_owner_from_character(Owner *owners, char *clear_c);
internal void reset_all_owner_select_letters(Owner *owners);
internal void set_view_at_end(View *view);
internal void set_view_at_start(View *view);
internal void log_add(char *text, UI *ui, ...);
internal void start_view_scrolling(Input *input, View *view);
internal void set_view_and_move_at_start(View *view);
internal void pos_newline(v2u *pos, u32 font_size, u32 count);
internal char set_owner_src(Owner *owner, void *parent, OwnerType type);
internal char add_new_char_to_owners(Owner *owners, void *parent, OwnerType type);
internal u32 get_font_newline(u32 font_size);
internal u32 get_view_end(View view);
internal b32 is_view_scrollable(View view);
internal b32 is_view_scrollable_with_count(View view, u32 count);
internal b32 can_view_go_up(View view);
internal b32 can_view_go_down(View view);
internal u32 get_view_range(View view);
internal Owner *get_owner_from_letter(Owner *owners, char search_c);
internal String8 get_item_letter_string(Item *item);