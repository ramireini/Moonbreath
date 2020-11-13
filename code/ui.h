#define MAX_LOG_MESSAGE_COUNT 128

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
    b32 is_full_log_open;
    View full_log;
    View short_log;
    
    ItemWindow item_window;
    RenderQueue render_queue[MAX_TEXT_QUEUE_COUNT];
} UI;

internal void log_add(UI *ui, char *text, ...);