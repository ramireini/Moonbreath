#ifndef POP_UP_TEXT_H
#define POP_UP_TEXT_H

#define POP_UP_TEXT_COUNT 4

typedef enum
{
  type_solid,
  type_fading
} text_type;

typedef struct
{
  b32 active;
  char str[16];
  r32 x, y;
  i32 x_offset, y_offset;
  r32 change;
  iv4 color;
  text_type type;
  r32 speed;
  u32 duration_time;
  u32 start_time;
} pop_up_text_t;

#endif // POP_UP_TEXT_H