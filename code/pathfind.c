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

internal b32
pathfind_map_has_default_values(Dungeon *dungeon, PathfindMap *pathfind_map)
{
    for(u32 y = 0; y < dungeon->size.h; ++y)
    {
        for(u32 x = 0; x < dungeon->size.w; ++x)
        {
            v2u pos = {x, y};
            
            if(is_dungeon_pos_traversable(dungeon->tiles, pos) &&
               get_pathfind_value(pathfind_map, pos) == U32_MAX)
            {
                //printf("Pos with default value: %u, %u\n", pos.x, pos.y);
                
                return(true);
            }
        }
    }
    
    return(false);
    }

internal v2u
get_pathfind_pos(PathfindMap *pathfind_map, DungeonTiles tiles, v2u origin_pos, v2u target_pos)
{
    v2u result = origin_pos;
    u32 closest_distance = get_pathfind_value(pathfind_map, origin_pos);
    
#if 0
    printf("Origin distance to target: %u\n", closest_distance);
    #endif
    
    // Origin needs to have a positive tile distance to target.
    // Target needs to have a distance of zero because the pathfind map is created from it.
    assert(get_pathfind_value(pathfind_map, origin_pos));
    assert(!get_pathfind_value(pathfind_map, target_pos));
    
    for(Direction direction = Direction_Up; direction <= Direction_DownRight; ++direction)
    {
        v2u direction_pos = get_direction_pos(origin_pos, direction);
        u32 distance = get_pathfind_value(pathfind_map, direction_pos);
        
        #if 0
        printf("\nDirection: %s\n", get_direction_string(direction));
        printf("Distance: %u\n", distance);
        #endif
        
        if(distance < closest_distance)
        {
            if(is_dungeon_pos_traversable_and_not_occupied(tiles, direction_pos) ||
                   is_dungeon_pos_closed_door(tiles, direction_pos) ||
                   is_v2u_equal(direction_pos, target_pos))
            {
                result = direction_pos;
                closest_distance = distance;
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
    
    for(u32 y = 0; y < dungeon->size.h; ++y)
    {
        for(u32 x = 0; x < dungeon->size.w; ++x)
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
    //printf("start_pos: %u, %u\n", start_pos.x, start_pos.y);
    //printf("dungeon->ready_for_pathfinding: %u\n", dungeon->ready_for_pathfinding);
    
    if(dungeon->ready_for_pathfinding &&
           is_dungeon_pos_traversable_or_closed_door(dungeon->tiles, start_pos))
    {
        // Initialize to a high value
        for(u32 y = 0; y < dungeon->size.h; ++y)
        {
            for(u32 x = 0; x < dungeon->size.w; ++x)
            {
                set_pathfind_value(pathfind_map, make_v2u(x, y), U32_MAX);
            }
        }
        
        // This is the lowest number, the goal
        set_pathfind_value(pathfind_map, start_pos, 0);
        
        for(;;)
        {
            for(u32 y = 0; y < dungeon->size.h; ++y)
            {
                for(u32 x = 0; x < dungeon->size.w; ++x)
                {
                    v2u pos = {x, y};
                    
                    if(is_dungeon_pos_traversable_or_closed_door(dungeon->tiles, pos))
                    {
                        assert(is_pos_inside_dungeon(dungeon->size, pos));
                        u32 pos_dist = get_pathfind_value(pathfind_map, pos);
                            
                            for(Direction direction = Direction_Up; direction <= Direction_DownRight; ++direction)
                            {
                                v2u direction_pos = get_direction_pos(pos, direction);
                            u32 direction_dist = get_pathfind_value(pathfind_map, direction_pos);
                            
                            // For every dungeon position, we look at all the neighbouring positions
                            // to see if any of them has a smaller pathfinding value than the current position.
                            if(direction_dist < pos_dist)
                                {
                                pos_dist = direction_dist;
                                set_pathfind_value(pathfind_map, pos, pos_dist + 1);
                                }
                        }
                    }
                }
            }
            
            b32 has_default_values = pathfind_map_has_default_values(dungeon, pathfind_map);
            if(has_default_values)
            {
                continue;
            }
            
            break;
        }
        
#if 0
        print_pathfind_map(dungeon, pathfind_map);
#endif
        
    }
}

internal void
init_pathfind_map(Dungeon *dungeon, PathfindMap *pathfind_map, v2u pos)
{
    pathfind_map->width = dungeon->size.w;
    update_pathfind_map(dungeon, pathfind_map, pos);
}