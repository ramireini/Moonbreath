internal u32
get_pathfind_value(PathfindMap *pathfind_map, v2u pos)
{
    u32 result = pathfind_map->array[(pos.y * pathfind_map->width) + pos.x];
    return(result);
}

internal void
set_pathfind_value(PathfindMap *pathfind_map, v2u pos, u32 value)
{
    pathfind_map->array[(pos.y * pathfind_map->width) + pos.x] = value;
}

internal v2u
get_pathfind_pos(PathfindMap *pathfind_map, Tiles tiles, v2u origin_pos, v2u target_pos)
{
    v2u result = origin_pos;
    u32 closest_distance = get_pathfind_value(pathfind_map, origin_pos);
    
    //printf("\ndistance from origin to target: %u\n", closest_distance);
    
    for(Direction direction = Direction_Up; direction <= Direction_DownRight; ++direction)
    {
        v2u direction_pos = get_direction_pos(origin_pos, direction);
        u32 distance = get_pathfind_value(pathfind_map, direction_pos);
        
        if(distance < closest_distance)
        {
            if(is_tile_traversable_and_not_occupied(tiles, direction_pos) ||
               is_tile_id(tiles, direction_pos, TileID_StoneDoorClosed) ||
                   is_v2u_equal(direction_pos, target_pos))
            {
                closest_distance = distance;
                result = direction_pos;
            }
        }
    }
    
    assert(!is_v2u_equal(origin_pos, result));
    return(result);
}

#if MOONBREATH_SLOW
internal void
print_pathfind_map(Dungeon *dungeon, PathfindMap *pathfind_map)
{
    printf("\n\nPathfind Map\n");
    for(u32 y = 0; y < dungeon->height; ++y)
    {
        for(u32 x = 0; x < dungeon->width; ++x)
        {
            v2u pos = {x, y};
            u32 value = get_pathfind_value(pathfind_map, pos);
            if(value != U32_MAX)
            {
                printf("%u ", value);
            }
        }
        
        printf("\n");
    }
}
#endif

internal void
update_pathfind_map(Dungeon *dungeon, PathfindMap *pathfind_map, v2u start_pos)
{
    if(dungeon->ready_for_pathfinding &&
       is_tile_traversable(dungeon->tiles, start_pos))
    {
        // Initialize to a high value.
        for(u32 y = 0; y < dungeon->height; ++y)
        {
            for(u32 x = 0; x < dungeon->width; ++x)
            {
                set_pathfind_value(pathfind_map, make_v2u(x, y), U32_MAX);
            }
        }
        
        // This is the lowest number, the goal.
        set_pathfind_value(pathfind_map, start_pos, 0);
        
        for(;;)
        {
            next_iteration:
            
            for(u32 y = 0; y < dungeon->height; ++y)
            {
                for(u32 x = 0; x < dungeon->width; ++x)
                {
                    v2u pos = {x, y};
                    
                    // We need to be able to go through closed doors
                    // with this so we don't infinite loop. If we were to have
                    // different doors in the future, we would need something like
                    // a is_door() function to be used here instead.
                    if(is_tile_traversable(dungeon->tiles, pos) ||
                           is_tile_id(dungeon->tiles, pos, TileID_StoneDoorClosed))
                    {
                        if(is_inside_dungeon(dungeon, pos))
                        {
                            u32 closest_distance = get_pathfind_value(pathfind_map, pos);
                            
                            for(Direction direction = Direction_Up; direction <= Direction_DownRight; ++direction)
                            {
                                v2u direction_pos = get_direction_pos(pos, direction);
                                
                                u32 pos_distance = get_pathfind_value(pathfind_map, direction_pos);
                                if(pos_distance < closest_distance)
                                {
                                    closest_distance = pos_distance;
                                    set_pathfind_value(pathfind_map, pos, closest_distance + 1);
                                }
                            }
                        }
                    }
                }
            }
            
#if 0
            print_pathfind_map(dungeon, pathfind_map);
#endif
            
            // Keep iterating if there are still traversable tiles
            // that haven't had their pathfind value set.
            for(u32 y = 0; y < dungeon->height; ++y)
            {
                for(u32 x = 0; x < dungeon->width; ++x)
                {
                    v2u current = {x, y};
                    if(is_tile_traversable(dungeon->tiles, current) &&
                           get_pathfind_value(pathfind_map, current) == U32_MAX)
                    {
                        goto next_iteration;
                    }
                }
            }
            
            return;
        }
    }
}

internal void
init_pathfind_map(Dungeon *dungeon, PathfindMap *pathfind_map, v2u pos)
{
    pathfind_map->width = dungeon->width;
    update_pathfind_map(dungeon, pathfind_map, pos);
}