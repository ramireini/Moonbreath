#define FOV_SECTOR_COUNT 8

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
           v2u start_pos,
           u32 range,
           u32 start_row,
           v2f slope,
           v4u multiplier)
{
        if(slope.start <= slope.end)
    {
        f32 next_slope_start = slope.start;
        
        for(u32 row = start_row; row <= range; ++row)
        {
            b32 current_pos_blocked = false;
            
            for(s32 x = -row; x <= 0; ++x)
            {
                s32 y = -row;
                
                // Offsets for current_pos based on row x, row y and multipliers
                v2s offset =
                {
                    (x * multiplier.x_x) + (y * multiplier.x_y),
                    (x * multiplier.y_x) + (y * multiplier.y_y)
                };
                
                v4u dungeon_rect = make_v4u(0, 0, dungeon->width, dungeon->height);
                    v2u current_pos = {start_pos.x + offset.x, start_pos.y + offset.y};
                
                if(is_inside_rect(dungeon_rect, current_pos))
                {
                    v2f pos_slope =
                    {
                        (x - 0.5f) / (y + 0.5f),
                        (x + 0.5f) / (y - 0.5f)
                    };
                    
                    set_tile_is_seen_and_has_been_seen(dungeon->tiles, current_pos, true);
                    
                    if(current_pos_blocked)
                    {
                        if(is_tile_traversable(dungeon->tiles, current_pos))
                        {
                            current_pos_blocked = false;
                            slope.start = next_slope_start;
                        }
                        else
                        {
                            next_slope_start = pos_slope.end;
                            continue;
                        }
                    }
                    else if(!is_tile_traversable(dungeon->tiles, current_pos))
                    {
                        current_pos_blocked = true;
                        next_slope_start = pos_slope.end;
                        
                        // Pos is blocking so start a child scan
                        cast_light(dungeon,
                                       start_pos,
                                       range,
                                       row + 1,
                                       slope,
                                   multiplier);
                    }
                }
            }
            
            // Don't scan the next row if the current pos is blocked
            if(current_pos_blocked)
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
        
        return;
    }
    
#endif
        // Reset visibility
        for(u32 y = 0; y < dungeon->height; ++y)
        {
            for(u32 x = 0; x < dungeon->width; ++x)
            {
                set_tile_is_seen(dungeon->tiles, make_v2u(x, y), false);
            }
        }
        
        // Player is visible by default
        set_tile_is_seen_and_has_been_seen(dungeon->tiles, player->pos, true);
    
    s32 multipliers[FOV_SECTOR_COUNT][4] = 
    {
        // Each sector has 4 multipliers, 2 for X and Y.
        // Format: x_x, x_y, y_x, y_y
        
        -1, 0, 0, 1,
        0, -1, 1, 0,
        0, -1, -1, 0,
        -1, 0, 0, -1,
        1, 0, 0, -1,
        0, 1, -1, 0,
        0, 1, 1, 0,
        1, 0, 0, 1,
        };
    
    for(u32 sector = 0; sector < FOV_SECTOR_COUNT; ++sector)
    {
        v2f slope = {0.0f, 1.0f};
        
        v4u multiplier =
        {
            multipliers[sector][0],
            multipliers[sector][1],
            multipliers[sector][2],
            multipliers[sector][3]
        };
        
            cast_light(dungeon,
                       player->pos,
                       player->fov,
                       1,
                       slope,
                       multiplier);
        }
    }