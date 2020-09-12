internal u32
pathfind_value(Pathfind *pathfind, v2u pos)
{
    u32 result = pathfind->array[(pos.y * pathfind->width) + pos.x];
    return(result);
}

internal void
set_pathfind_value(Pathfind *pathfind, v2u pos, u32 value)
{
    pathfind->array[(pos.y * pathfind->width) + pos.x] = value;
}

internal v2u
next_pathfind_pos(Pathfind *pathfind, DungeonTiles tiles, v2u player_pos, v2u enemy_pos)
{
    v2u result = {0};
    u32 closest_distance = pathfind_value(pathfind, enemy_pos);
    
    for(Direction direction = Direction_Up; direction <= Direction_DownRight; ++direction)
    {
        v2u direction_pos = get_direction_pos(enemy_pos, direction);
        
        u32 pos_distance = pathfind_value(pathfind, direction_pos);
        if(pos_distance < closest_distance)
        {
            if(!is_tile_occupied(tiles, direction_pos) ||
               equal_v2u(direction_pos, player_pos))
            {
                closest_distance = pos_distance;
                result = direction_pos;
            }
        }
    }
    
    return(result);
}

internal void
update_pathfind_map(Dungeon *dungeon, v2u player_pos)
{
    if(is_tile_traversable(dungeon->tiles, player_pos))
    {
        // Initialize to a high value.
        for(u32 y = 0; y < dungeon->h; ++y)
        {
            for(u32 x = 0; x < dungeon->w; ++x)
            {
                set_pathfind_value(&dungeon->pathfind, make_v2u(x, y), U32_MAX);
            }
        }
        
        // This is the lowest number, the goal.
        set_pathfind_value(&dungeon->pathfind, player_pos, 0);
        
        for(;;)
        {
            next_iteration:
            
            for(u32 y = 0; y < dungeon->h; ++y)
            {
                for(u32 x = 0; x < dungeon->w; ++x)
                {
                    v2u current = {x, y};
                    
                    // We need to be able to go through closed doors
                    // with this so we don't infinite loop. If we were to have
                    // different doors in the future, we would need something like
                    // a is_door() function to be used here instead.
                    if(is_tile_traversable(dungeon->tiles, current) ||
                       is_tile_id(dungeon->tiles, current, TileID_StoneDoorClosed))
                    {
                        if(is_inside_dungeon(dungeon, current))
                        {
                            u32 closest_distance = pathfind_value(&dungeon->pathfind, current);
                            
                            for(Direction direction = Direction_Up; direction <= Direction_DownRight; ++direction)
                            {
                                v2u direction_pos = get_direction_pos(current, direction);
                                
                                u32 pos_distance = pathfind_value(&dungeon->pathfind, direction_pos);
                                if(pos_distance < closest_distance)
                                {
                                    closest_distance = pos_distance;
                                    set_pathfind_value(&dungeon->pathfind, current, closest_distance + 1);
                                }
                            }
                        }
                    }
                }
            }
            
#if 0
            printf("\n\nPathfind Map\n");
            for(u32 y = 0; y < dungeon->h; ++y)
            {
                for(u32 x = 0; x < dungeon->w; ++x)
                {
                    v2u current = {x, y};
                    u32 value = pathfind_value(&dungeon->pathfind, current);
                    if(value != U32_MAX)
                    {
                        printf("%u ", value);
                    }
                }
                
                printf("\n");
            }
#endif
            
            for(u32 y = 0; y < dungeon->h; ++y)
            {
                for(u32 x = 0; x < dungeon->w; ++x)
                {
                    v2u current = {x, y};
                    if(is_tile_traversable(dungeon->tiles, current) &&
                       pathfind_value(&dungeon->pathfind, current) == U32_MAX)
                    {
                        goto next_iteration;
                    }
                }
            }
            
            return;
        }
    }
}