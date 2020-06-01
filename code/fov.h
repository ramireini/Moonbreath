typedef struct
{
    b32 is_seen;
    b32 has_been_seen;
} fov_tile_t;

internal void set_tile_is_seen_and_has_been_seen(dungeon_t *dungeon, v2u pos, b32 value);