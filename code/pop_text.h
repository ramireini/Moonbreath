typedef enum
{
    text_normal_attack,
    text_critical_attack,
    text_heal
} text_type;

typedef struct
{
    b32 active;
    text_type type;
    char str[8];
    
    v2u pos;
    v2s pos_offset;
    v2f change_in_pos;
    v2u direction;
    
    v4f color;
    f32 speed;
    
    u32 duration_time;
    u32 start_time;
} pop_text_t;