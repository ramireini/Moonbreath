internal void
set_tile_is_seen(Tiles tiles, v2u pos, b32 value)
{
    tiles.array[(pos.y * tiles.width) + pos.x].is_seen = value;
}

internal void
set_tile_has_been_seen(Tiles tiles, v2u pos, b32 value)
{
    tiles.array[(pos.y * tiles.width) + pos.x].has_been_seen = value;
}

internal b32
is_tile_seen(Tiles tiles, v2u pos)
{
    b32 result = (tiles.array[(pos.y * tiles.width) + pos.x].is_seen);
    return(result);
}

internal b32
has_tile_been_seen(Tiles tiles, v2u pos)
{
    b32 result = (tiles.array[(pos.y * tiles.width) + pos.x].has_been_seen);
    return(result);
}

internal void
set_tile_is_seen_and_has_been_seen(Tiles tiles, v2u pos, b32 value)
{
    set_tile_is_seen(tiles, pos, value);
    set_tile_has_been_seen(tiles, pos, value);
}

internal void
cast_light(Dungeon *dungeon,
           v2u start,
           u32 fov_range,
           u32 row,
           f32 start_slope,
           f32 end_slope,
           v2u multiplier_x,
           v2u multiplier_y)
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
                
                // left_slope and right_slope store the
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
                
                // Get the offset for the current position in the
                // current sector by using the multipliers.
                v2s offset =
                {
                    (dx * multiplier_x.x) + (dy * multiplier_x.y),
                    (dx * multiplier_y.x) + (dy * multiplier_y.y)
                };
                
                v2u current = {start.x + offset.x, start.y + offset.y};
                if(is_inside_rect(make_v4u(0, 0, dungeon->width, dungeon->height), current))
                {
                    set_tile_is_seen_and_has_been_seen(dungeon->tiles, current, true);
                    
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
                        
                        // This position is blocking so start a child scan.
                        cast_light(dungeon,
                                   start,
                                   fov_range,
                                   y + 1,
                                   start_slope,
                                   left_slope,
                                   multiplier_x,
                                   multiplier_y);
                    }
                }
            }
            
            // Scan the next row unless if the last
            // position of the current row is blocking.
            if(is_current_blocked)
            {
                break;
            }
        }
    }
}

internal void
update_fov(Dungeon *dungeon, Entity *player)
{
#if MOONBREATH_SLOW
    if(fkey_active[1])
    {
        for(u32 y = 0; y < dungeon->height; ++y)
        {
            for(u32 x = 0; x < dungeon->width; ++x)
            {
                set_tile_is_seen(dungeon->tiles, make_v2u(x, y), true);
            }
        }
    }
    
    else
#endif
    
    {
        // Reset visibility.
        for(u32 y = 0; y < dungeon->height; ++y)
        {
            for(u32 x = 0; x < dungeon->width; ++x)
            {
                set_tile_is_seen(dungeon->tiles, make_v2u(x, y), false);
            }
        }
        
        // Player is visible by default.
        set_tile_is_seen_and_has_been_seen(dungeon->tiles, player->pos, true);
        
        // For transforming positions into other sectors.
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
            
            cast_light(dungeon,
                       player->pos,
                       player->fov,
                       1,
                       1.0f,
                       0.0f,
                       multiplier_x,
                       multiplier_y);
        }
    }
}