#ifndef LIGHTING_H
#define LIGHTING_H

enum
{
  lighting_min = 0,
  lighting_max = 255,
  lighting_seen = 16
};

typedef struct
{
  b32 seen;
  u32 value;
} lighting_t;

#endif // LIGHTING_H