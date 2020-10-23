#define MAX_LOG_MESSAGE_COUNT 128

typedef struct
{
    b32 is_comparing;
    
    v2u at;
    u32 x, y, w, h;
    u32 next_line_advance;
    u32 offset_to_actions;
} ItemWindow;

typedef struct
{
    u32 start_index;
    u32 message_count;
} LogView;

typedef struct
{
    String128 messages[MAX_LOG_MESSAGE_COUNT];
    
    b32 is_full_view_open;
    LogView full_view;
    LogView short_view;
} Log;

internal void log_add(Log *log, char *text, ...);