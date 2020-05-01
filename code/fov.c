internal b32
is_pos_in_shadow(f32 pos_slope, shadows_t *shadows)
{
    b32 result = false;
    
    for(u32 index = 0;
        index < shadows->count;
        ++index)
    {
        shadow_t shadow = shadows->array[index];
        if(pos_slope >= shadow.start &&
           pos_slope <= shadow.end)
        {
            result = true;
            break;
        }
    }
    
    return(result);
}

internal v2u
get_tile_pos_from_local_pos(u32 sector, v2u player, u32 local_x, u32 local_y)
{
    v2u result = {0};
    
    switch(sector)
    {
        case 0: result = V2u(player.x + local_x, player.y - local_y); break;
        case 1: result = V2u(player.x + local_y, player.y - local_x); break;
        case 2: result = V2u(player.x + local_y, player.y + local_x); break;
        case 3: result = V2u(player.x + local_x, player.y + local_y); break;
        case 4: result = V2u(player.x - local_x, player.y + local_y); break;
        case 5: result = V2u(player.x - local_y, player.y + local_x); break;
        case 6: result = V2u(player.x - local_y, player.y - local_x); break;
        case 7: result = V2u(player.x - local_x, player.y - local_y); break;
        
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
cast_light(dungeon_t *dungeon,
           v2u start,
           u32 fov_range, u32 row,
           f32 start_slope, f32 end_slope,
           v2u multiplier_x, v2u multiplier_y)
{
    if(start_slope >= end_slope)
    {
        f32 next_start_slope = start_slope;
        
        for(u32 y = row; y <= fov_range; ++y)
        {
            b32 is_current_blocked =  false;
            
            for(s32 dx = -y; dx <= 0; ++dx)
            {
                s32 dy = -y;
                
                // NOTE(Rami): left_slope and right_slope store the
                // extremities of the square.
                f32 left_slope = (dx - 0.5f) / (dy + 0.5f);
                f32 right_slope = (dx + 0.5f) / (dy - 0.5f);
                
                if(start_slope < right_slope)
                {
                    continue;
                }
                else if(end_slope > left_slope)
                {
                    break;
                }
                
                // NOTE(Rami): Get the offset for the current
                // sector by using the multipliers.
                v2s offset =
                {
                    (dx * multiplier_x.x) + (dy * multiplier_x.y),
                    (dx * multiplier_y.x) + (dy * multiplier_y.y)
                };
                
                if((offset.x < 0 && (u32)abs(offset.x) > start.x) ||
                   (offset.y < 0 && (u32)abs(offset.y) > start.y))
                {
                    // TODO(Rami): Do we actually ever hit this??
                    assert(false);
                    continue;
                }
                
                v2u current = {start.x + offset.x, start.y + offset.y};
                if(is_pos_in_dungeon(dungeon, current))
                {
#if 0
                    u32 radius2 = radius * radius;
                    if((u32)(dx * dx + dy * dy) < radius2)
                    {
                        set_visible_status(dungeon, current, true);
                    }
#endif
                    
                    set_visible_status(dungeon, current, true);
                    
                    if(is_current_blocked)
                    {
                        if(is_tile_traversable(dungeon->tiles, current))
                        {
                            is_current_blocked = false;
                            start_slope = next_start_slope;
                        }
                        else
                        {
                            next_start_slope = right_slope;
                            continue;
                        }
                    }
                    else if(!is_tile_traversable(dungeon->tiles, current))
                    {
                        is_current_blocked = true;
                        next_start_slope = right_slope;
                        
                        // NOTE(Rami): This position is blocking,
                        // start a child scan.
                        cast_light(dungeon, start, fov_range,
                                   y + 1, start_slope, left_slope,
                                   multiplier_x, multiplier_y);
                    }
                }
            }
            
            // NOTE(Rami): Scan the next row unless if the current
            // rows last position is blocking.
            if(is_current_blocked)
            {
                break;
            }
        }
    }
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
        // NOTE(Rami): Reset visibility.
        for(u32 y = 0; y < dungeon->height; ++y)
        {
            for(u32 x = 0; x < dungeon->width; ++x)
            {
                dungeon->fov_tiles[(y * dungeon->width) + x].is_seen = false;
            }
        }
        
        // NOTE(Rami): Player is visible by default.
        set_visible_status(dungeon, player->pos, true);
        
        // NOTE(Rami): For transforming positions into other octants.
        s32 multipliers[4][8] =
        {
            {1, 0, 0, -1, -1, 0, 0, 1},
            {0, 1, -1, 0, 0, -1, 1, 0},
            {0, 1, 1, 0, 0, -1, -1, 0},
            {1, 0, 0, 1, -1, 0, 0, -1}
        };
        
        for(u32 sector = 0; sector < 8; ++sector)
        {
            v2u multiplier_x = {multipliers[0][sector], multipliers[1][sector]};
            v2u multiplier_y = {multipliers[2][sector], multipliers[3][sector]};
            
            cast_light(dungeon, player->pos, player->p.fov,
                       1, 1.0f, 0.0f,
                       multiplier_x, multiplier_y);
        }
    }
}