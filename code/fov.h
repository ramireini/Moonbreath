typedef struct
{
    b32 is_seen;
    u32 value;
} fov_tile_t;

typedef struct
{
    f32 start;
    f32 end;
} shadow_t;

typedef struct
{
    u32 count;
    shadow_t array[8];
} shadows_t;

internal void set_seen(v2u pos, u32 value);