typedef struct
{
    b32 is_seen;
    b32 has_been_seen;
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

internal void set_visible_status(dungeon_t *dungeon, v2u pos, b32 value);