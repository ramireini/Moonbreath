typedef enum
{
    text_normal_attack,
    text_critical_attack,
    text_heal
} text_type;

typedef struct
{
    b32 active;
    char str[8];
    v2u pos;
    v2u pos_offset;
    u32 direction;
    v4f color;
    text_type type;
    f32 change;
    f32 speed;
    u32 duration_time;
    u32 start_time;
} pop_text_t;