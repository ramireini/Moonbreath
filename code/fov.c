#define FOV_SECTOR_COUNT 8

internal void
set_tile_is_seen(DungeonTiles tiles, v2u pos, b32 value)
{
    tiles.array[(pos.y * tiles.width) + pos.x].is_seen = value;
}

internal void
set_tile_has_been_seen(DungeonTiles tiles, v2u pos, b32 value)
{
    tiles.array[(pos.y * tiles.width) + pos.x].has_been_seen = value;
}

internal b32
is_tile_seen(DungeonTiles tiles, v2u pos)
{
    b32 result = (tiles.array[(pos.y * tiles.width) + pos.x].is_seen);
    return(result);
}

internal b32
has_tile_been_seen(DungeonTiles tiles, v2u pos)
{
    b32 result = (tiles.array[(pos.y * tiles.width) + pos.x].has_been_seen);
    return(result);
}

internal void
set_tile_is_seen_and_has_been_seen(DungeonTiles tiles, v2u pos, b32 value)
{
    set_tile_is_seen(tiles, pos, value);
    set_tile_has_been_seen(tiles, pos, value);
}

internal b32
is_tile_seen_or_has_been_seen(DungeonTiles tiles, v2u pos)
{
    b32 result = (is_tile_seen(tiles, pos) || has_tile_been_seen(tiles, pos));
    return(result);
}

internal void
cast_light(Dungeon *dungeon,
           v2u start_pos,
           u32 range,
           u32 start_row,
           v2f slope,
           v4u multiplier)
{
    if(slope.start >= slope.end)
    {
        f32 next_start_slope = slope.start;
        
        for(u32 row = start_row; row <= range; ++row)
        {
            b32 is_pos_blocked = false;
            
            for(s32 x = -row; x <= 0; ++x)
            {
                s32 y = -row;
                
                // Left and right extremities of the pos tile
                f32 half = 0.5f;
                v2f pos_slope =
                {
                    (x - half) / (y + half),
                    (x + half) / (y - half)
                };
                
                if(slope.start < pos_slope.right)
                {
                    continue;
                }
                else if(slope.end > pos_slope.left)
                {
                    break;
                }
                
                // Offsets for pos based on row x, row y and multipliers
                v2s offset =
                {
                    (x * multiplier.x_x) + (y * multiplier.x_y),
                    (x * multiplier.y_x) + (y * multiplier.y_y)
                };
                
                v2u pos = {start_pos.x + offset.x, start_pos.y + offset.y};
                
                if(is_pos_inside_rect(dungeon->rect, pos))
                {
                    set_tile_is_seen_and_has_been_seen(dungeon->tiles, pos, true);
                    
                    if(is_pos_blocked)
                    {
                        if(is_dungeon_pos_traversable(dungeon->tiles, pos))
                        {
                            is_pos_blocked = false;
                            slope.start = next_start_slope;
                        }
                        else
                        {
                            next_start_slope = pos_slope.right;
                        }
                    }
                    else if(!is_dungeon_pos_traversable(dungeon->tiles, pos))
                    {
                        is_pos_blocked = true;
                        next_start_slope = pos_slope.right;
                        
                        // This position is blocking so start a child scan
                        v2f new_slope = {slope.start, pos_slope.left};
                        cast_light(dungeon, start_pos, range, row + 1, new_slope, multiplier);
                    }
                }
            }
            
            // Don't scan the next row if the pos is blocked
            if(is_pos_blocked)
            {
                break;
            }
        }
    }
}

internal void
update_player_view(Entity *player, Dungeon *dungeon)
{
    assert(is_entity_valid(player));
    assert(dungeon);
    
#if MOONBREATH_SLOW
    if(debug_toggles[DebugToggleType_TraverseAll])
    {
        for(u32 y = 0; y < dungeon->size.h; ++y)
        {
            for(u32 x = 0; x < dungeon->size.w; ++x)
            {
                v2u pos = {x, y};
                set_tile_is_seen(dungeon->tiles, pos, true);
            }
        }
        
        return;
    }
#endif
    
    // Reset visibility
    for(u32 y = 0; y < dungeon->size.h; ++y)
    {
        for(u32 x = 0; x < dungeon->size.w; ++x)
        {
            v2u pos = {x, y};
            set_tile_is_seen(dungeon->tiles, pos, false);
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
        v2f slope = {1.0f, 0.0f};
        
        v4u multiplier =
        {
            multipliers[sector][0],
            multipliers[sector][1],
            multipliers[sector][2],
            multipliers[sector][3]
        };
        
        cast_light(dungeon, player->pos, player->stats.fov, 1, slope, multiplier);
    }
}