internal u32
get_pathfind_value(Pathfind *pathfind, v2u pos)
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
next_pathfind_pos(Pathfind *pathfind, Tiles tiles, v2u origin_pos, v2u target_pos)
{
    v2u result = origin_pos;
    u32 closest_distance = get_pathfind_value(pathfind, origin_pos);
    
    for(Direction direction = Direction_Up; direction <= Direction_DownRight; ++direction)
    {
        v2u direction_pos = get_direction_pos(origin_pos, direction);
        u32 distance = get_pathfind_value(pathfind, direction_pos);
        
        if(distance < closest_distance)
        {
            if(is_tile_traversable_and_not_occupied(tiles, direction_pos) ||
               is_tile_id(tiles, direction_pos, TileID_StoneDoorClosed) ||
               equal_v2u(direction_pos, target_pos))
            {
                closest_distance = distance;
                result = direction_pos;
            }
        }
    }
    
    return(result);
}

internal void
update_pathfind_map(Dungeon *dungeon, Pathfind *pathfind, v2u pos)
{
    if(is_tile_traversable(dungeon->tiles, pos))
    {
        // Initialize to a high value.
        for(u32 y = 0; y < dungeon->height; ++y)
        {
            for(u32 x = 0; x < dungeon->width; ++x)
            {
                set_pathfind_value(pathfind, make_v2u(x, y), U32_MAX);
            }
        }
        
        // This is the lowest number, the goal.
        set_pathfind_value(pathfind, pos, 0);
        
        for(;;)
        {
            next_iteration:
            
            for(u32 y = 0; y < dungeon->height; ++y)
            {
                for(u32 x = 0; x < dungeon->width; ++x)
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
                            u32 closest_distance = get_pathfind_value(pathfind, current);
                            
                            for(Direction direction = Direction_Up; direction <= Direction_DownRight; ++direction)
                            {
                                v2u direction_pos = get_direction_pos(current, direction);
                                
                                u32 pos_distance = get_pathfind_value(pathfind, direction_pos);
                                if(pos_distance < closest_distance)
                                {
                                    closest_distance = pos_distance;
                                    set_pathfind_value(pathfind, current, closest_distance + 1);
                                }
                            }
                        }
                    }
                }
            }
            
#if 0
            printf("\n\nPathfind Map\n");
            for(u32 y = 0; y < dungeon->height; ++y)
            {
                for(u32 x = 0; x < dungeon->width; ++x)
                {
                    v2u current = {x, y};
                    u32 value = get_pathfind_value(pathfind, current);
                    if(value != U32_MAX)
                    {
                        printf("%u ", value);
                    }
                }
                
                printf("\n");
            }
#endif
            
            for(u32 y = 0; y < dungeon->height; ++y)
            {
                for(u32 x = 0; x < dungeon->width; ++x)
                {
                    v2u current = {x, y};
                    if(is_tile_traversable(dungeon->tiles, current) &&
                       get_pathfind_value(pathfind, current) == U32_MAX)
                    {
                        goto next_iteration;
                    }
                }
            }
            
            return;
        }
    }
}