#define MAX_LOG_MESSAGE_COUNT 512

typedef struct
{
    b32 is_comparing;
    u32 x, y, w, h;
    u32 offset_to_actions;
} ItemWindow;

typedef struct
{
    Font *font;
    u32 font_newline;
    u32 window_offset;
    
    String128 log_messages[MAX_LOG_MESSAGE_COUNT];
    View short_log_view;
    
    b32 is_full_log_open;
    b32 is_full_log_view_at_end;
    v4u full_log_rect;
    View full_log_view;
    
    ItemWindow item_window;
    RenderQueue render_queue[MAX_TEXT_QUEUE_COUNT];
} UI;

internal u32 get_view_range(View view);
internal void set_scroll_view_at_start(View *view);
internal void update_scroll_view(MouseScrollMove scroll_move, View *view);
internal void log_add(UI *ui, char *text, ...);