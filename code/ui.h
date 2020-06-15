#define MAX_LOG_ENTRIES 8

typedef struct
{
    b32 is_comparing_items;
    
    v2u at;
    u32 x, y, w, h;
    u32 next_line_advance;
    u32 widnow_actions_advance;
} ItemWindow;

internal void log_text(String128 *log, char *text, ...);
