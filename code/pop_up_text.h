#ifndef POP_UP_TEXT_H
#define POP_UP_TEXT_H

#define POP_UP_TEXT_COUNT 16

typedef enum
{
    text_normal_attack,
    text_critical_attack
} text_type;

typedef struct
{
    b32 active;
    char str[16];
    v2i pos;
    v2i offset;
    r32 change;
    v4i color;
    text_type type;
    r32 speed;
    u32 duration_time;
    u32 start_time;
} pop_up_text_t;

#endif // POP_UP_TEXT_H