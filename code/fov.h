typedef struct
{
    b32 is_seen;
    b32 has_been_seen;
} fov_tile_t;

internal void set_visible_status(dungeon_t *dungeon, v2u pos, b32 value);