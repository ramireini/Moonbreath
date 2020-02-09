typedef struct
{
    char str[256];
    v4f color;
} console_text_t;

internal void add_console_text(char *str, v4f color, ...);

// TODO(rami): TESTING, Revisit!
typedef struct
{
    u32 x, y, w, h;
    
    // TODO(rami): Would be a lot clearer if pos was something else
    // like "stat_pos" or "at".
    
    // TODO(rami): We probably don't even really need this,
    // could just use the x, y.
    v2u pos;
    
    // TODO(rami): Offset names could be better?
    u32 small_offset;
    u32 actions_offset;
} item_window_t;