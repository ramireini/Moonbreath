internal b32
is_pos_in_shadow(f32 pos_slope, shadows_t *shadows)
{
    b32 result = false;
    
    for(u32 index = 0;
        index < shadows->count;
        ++index)
    {
        shadow_t shadow = shadows->array[index];
        if(shadow.start <= pos_slope && shadow.end >= pos_slope)
        {
            result = true;
            break;
        }
    }
    
    return(result);
}

internal v2u
get_tile_pos_from_local_pos(u32 sector, v2u player, v2u local_pos)
{
    v2u result = {0};
    
    switch(sector)
    {
        case 0: result = V2u(player.x + local_pos.x, player.y - local_pos.y); break;
        case 1: result = V2u(player.x + local_pos.y, player.y - local_pos.x); break;
        case 2: result = V2u(player.x + local_pos.y, player.y + local_pos.x); break;
        case 3: result = V2u(player.x + local_pos.x, player.y + local_pos.y); break;
        case 4: result = V2u(player.x - local_pos.x, player.y + local_pos.y); break;
        case 5: result = V2u(player.x - local_pos.y, player.y + local_pos.x); break;
        case 6: result = V2u(player.x - local_pos.y, player.y - local_pos.x); break;
        case 7: result = V2u(player.x - local_pos.x, player.y - local_pos.y); break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal void
add_shadow(shadows_t *shadows, shadow_t shadow)
{
    assert(shadows->count < array_count(shadows->array));
    shadows->array[shadows->count++] = shadow;
}

internal void
set_visible_status(dungeon_t *dungeon, v2u pos, b32 value)
{
    dungeon->fov_tiles[(pos.y * dungeon->width) + pos.x].is_seen = value;
    dungeon->fov_tiles[(pos.y * dungeon->width) + pos.x].has_been_seen = value;
}

internal b32
is_seen(dungeon_t *dungeon, v2u pos)
{
    b32 result = (dungeon->fov_tiles[(pos.y * dungeon->width) + pos.x].is_seen);
    return(result);
}

internal b32
has_been_seen(dungeon_t *dungeon, v2u pos)
{
    b32 result = (dungeon->fov_tiles[(pos.y * dungeon->width) + pos.x].has_been_seen);
    return(result);
}

internal void
update_fov(dungeon_t *dungeon, entity_t *player)
{
#if MOONBREATH_SLOW
    if(debug_fov)
    {
        for(u32 y = 0; y < dungeon->height; ++y)
        {
            for(u32 x = 0; x < dungeon->width; ++x)
            {
                dungeon->fov_tiles[(y * dungeon->width) + x].is_seen = true;
            }
        }
    }
    
    else
#endif
    
    {
        for(u32 y = 0; y < dungeon->height; ++y)
        {
            for(u32 x = 0; x < dungeon->width; ++x)
            {
                dungeon->fov_tiles[(y * dungeon->width) + x].is_seen = false;
            }
        }
        
        set_visible_status(dungeon, player->pos, true);
        
        for(u32 sector = 0; sector < 8; ++sector)
        {
            f32 shadow_start = 0.0f;
            f32 shadow_end = 0.0f;
            shadows_t shadows = {0};
            
            for(u32 y = 0; y < player->p.fov; ++y)
            {
                b32 is_previous_traversable = true;
                
                for(u32 x = 0; x <= y; ++x)
                {
                    v2u tile_pos = get_tile_pos_from_local_pos(sector, player->pos, V2u(x, y));
                    if(is_pos_in_dungeon(dungeon, tile_pos))
                    {
                        f32 pos_slope = slope(0, 0, (f32)x, (f32)y);
                        if(!is_pos_in_shadow(pos_slope, &shadows))
                        {
                            set_visible_status(dungeon, tile_pos, true);
                            
                            if(is_tile_traversable(dungeon->tiles, tile_pos))
                            {
                                if(!is_previous_traversable)
                                {
                                    shadow_end = slope(0.0f, 0.0f, (f32)x, (f32)y);
                                    shadow_t shadow = {shadow_start, shadow_end};
                                    add_shadow(&shadows, shadow);
                                }
                            }
                            else
                            {
                                if(is_previous_traversable)
                                {
                                    shadow_start = slope(0, 0, (f32)x, (f32)y);
                                    is_previous_traversable = false;
                                }
                            }
                        }
                    }
                }
                
                if(!is_previous_traversable)
                {
                    shadow_end = slope(0.0f, 0.0f, (f32)y + 0.5f, (f32)y);
                    shadow_t shadow = {shadow_start, shadow_end};
                    add_shadow(&shadows, shadow);
                }
            }
        }
    }
}