typedef struct
{
    b32 seen;
    u32 value;
} fov_tile_t;

typedef struct
{
    f32 start;
    f32 end;
} shadow_t;

typedef struct
{
    // NOTE(rami): The amount of elements this should have depends on
    // the field of view of the player.
    // With a fov of 4 the maximum amount of shadows for a single span
    // should be 3, and so if the fov changes we might have to adjust the
    // array element count.
    shadow_t shadows[8];
    u32 shadow_count;
} shadow_data_t;

internal void set_seen(v2u pos, u32 value);