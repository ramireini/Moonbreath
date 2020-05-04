#define MAX_LOG_STRINGS 8

typedef struct
{
    b32 is_comparing_items;
    
    u32 x, y, w, h;
    
    v2u at;
    u32 offset_per_row;
    u32 offset_to_actions;
} item_window_t;

internal void add_log_string(string_t *log, char *string, ...);