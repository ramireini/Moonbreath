#define MAX_LOG_MESSAGE_COUNT 128

typedef struct
{
    b32 is_comparing;
    u32 x, y, w, h;
    u32 offset_to_actions;
} ItemWindow;

typedef struct
{
    u32 start_index;
    u32 message_count;
} LogView;

typedef struct
{
    Font *font;
    u32 font_newline;
    
    String128 log_messages[MAX_LOG_MESSAGE_COUNT];
    b32 is_full_log_open;
    LogView full_log;
    LogView short_log;
    
    ItemWindow item_window;
} UI;

internal void log_add(UI *ui, char *text, ...);