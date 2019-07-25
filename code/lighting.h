#ifndef LIGHTING_H
#define LIGHTING_H

enum
{
    lighting_min = 0,
    lighting_max = 255,
    lighting_seen = 32
};

typedef struct
{
    b32 seen;
    u32 value;
} light_block_t;

#endif // LIGHTING_H