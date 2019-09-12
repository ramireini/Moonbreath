#ifndef POP_UP_TEXT_H
#define POP_UP_TEXT_H

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
    v2u offset;
    v4u color;
    text_type type;
    f32 change;
    f32 speed;
    u32 duration_time;
    u32 start_time;
} pop_text_t;

#endif // POP_UP_TEXT_H