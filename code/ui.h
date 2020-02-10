typedef struct
{
    char str[128];
    v4f color;
} console_text_t;

typedef struct
{
    b32 is_comparing_items;
    
    u32 x, y, w, h;
    
    v2u at;
    u32 offset_per_row;
    u32 offset_to_actions;
} item_window_t;

internal void add_console_text(char *str, v4f color, ...);