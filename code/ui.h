#define MAX_LOG_ENTRIES 8

typedef struct
{
    b32 is_comparing_items;
    
    v2u at;
    u32 x, y, w, h;
    u32 offset_per_row;
    u32 offset_to_actions;
} item_window_t;

internal void log_text(string_128_t *log, char *text, ...);
