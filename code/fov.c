internal b32
is_pos_in_shadow(f32 pos_slope, shadow_data_t *data)
{
    b32 result = false;
    
    for(u32 i = 0; i < data->shadow_count; ++i)
    {
        shadow_t shadow = data->shadows[i];
        if(shadow.start <= pos_slope && shadow.end >= pos_slope)
        {
            result = true;
            break;
        }
    }
    
    return(result);
}

internal v2u
get_tile_pos_for_local_pos(u32 sector, v2u player, v2u pos)
{
    v2u result = {0};
    
    switch(sector)
    {
        case 0: result = V2u(player.x + pos.x, player.y - pos.y); break;
        case 1: result = V2u(player.x + pos.y, player.y - pos.x); break;
        case 2: result = V2u(player.x + pos.y, player.y + pos.x); break;
        case 3: result = V2u(player.x + pos.x, player.y + pos.y); break;
        case 4: result = V2u(player.x - pos.x, player.y + pos.y); break;
        case 5: result = V2u(player.x - pos.y, player.y + pos.x); break;
        case 6: result = V2u(player.x - pos.y, player.y - pos.x); break;
        case 7: result = V2u(player.x - pos.x, player.y - pos.y); break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal void
add_shadow(shadow_t shadow, shadow_data_t *data)
{
    data->shadows[data->shadow_count] = shadow;
    ++data->shadow_count;
}

internal void
set_as_visible(v2u pos)
{
    dungeon.fov_tiles[pos.y][pos.x].seen = true;
    dungeon.fov_tiles[pos.y][pos.x].value = 1;
}

internal void
update_fov()
{
#if 1
    for(u32 y = 0; y < dungeon.h; ++y)
    {
        for(u32 x = 0; x < dungeon.w; ++x)
        {
            dungeon.fov_tiles[y][x].value = 1;
        }
    }
    
    return;
#else
    for(u32 y = 0; y < dungeon.h; ++y)
    {
        for(u32 x = 0; x < dungeon.w; ++x)
        {
            dungeon.fov_tiles[y][x].value = 0;
        }
    }
#endif
    
    set_as_visible(player.pos);
    
    for(u32 sector = 0; sector < 8; ++sector)
    {
        b32 previous_blocking = false;
        shadow_data_t data = {0};
        f32 shadow_start = 0.0f;
        f32 shadow_end = 0.0f;
        
        v2u pos = {0};
        for(pos.y = 0; pos.y < player.fov; ++pos.y)
        {
            previous_blocking = false;
            
            for(pos.x = 0; pos.x <= pos.y; ++pos.x)
            {
                v2u tile_pos = get_tile_pos_for_local_pos(sector, player.pos, pos);
                if(is_inside_dungeon(tile_pos))
                {
                    f32 pos_slope = line_slope(0, 0, pos.x, pos.y);
                    if(!is_pos_in_shadow(pos_slope, &data))
                    {
                        set_as_visible(tile_pos);
                        
                        if(!is_traversable(tile_pos))
                        {
                            if(!previous_blocking)
                            {
                                shadow_start = line_slope(0, 0, pos.x, pos.y);
                                previous_blocking = true;
                            }
                        }
                        else
                        {
                            if(previous_blocking)
                            {
                                shadow_end = line_slope(0, 0, pos.x + 0.5f, pos.y);
                                shadow_t shadow = {shadow_start, shadow_end};
                                add_shadow(shadow, &data);
                            }
                        }
                    }
                }
            }
            
            if(previous_blocking)
            {
                shadow_end = line_slope(0, 0, pos.y + 0.5f, pos.y);
                shadow_t shadow = {shadow_start, shadow_end};
                add_shadow(shadow, &data);
            }
        }
    }
}

internal void
set_seen(v2u pos, u32 value)
{
    dungeon.fov_tiles[pos.y][pos.x].seen = value;
}

internal b32
is_seen(v2u pos)
{
    b32 result = (dungeon.fov_tiles[pos.y][pos.x].value);
    return(result);
}

internal b32
has_been_seen(v2u pos)
{
    b32 result = (dungeon.fov_tiles[pos.y][pos.x].seen);
    return(result);
}
