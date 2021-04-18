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
get_pathfind_pos(PathfindMap *pathfind_map, DungeonTiles tiles, v2u origin_pos, v2u target_pos)
{
    v2u result = origin_pos;
    u32 closest_dist = get_pathfind_value(pathfind_map, origin_pos);
    
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
        u32 direction_dist = get_pathfind_value(pathfind_map, direction_pos);
        
        #if 0
        printf("\nDirection: %s\n", get_direction_string(direction));
        printf("Distance: %u\n", distance);
        #endif
        
        if(direction_dist < closest_dist)
        {
            if(is_dungeon_pos_traversable_and_unoccupied(tiles, direction_pos) ||
                   is_dungeon_pos_closed_door(tiles, direction_pos) ||
                   is_v2u_equal(direction_pos, target_pos))
            {
                result = direction_pos;
                closest_dist = direction_dist;
            }
        }
    }
    
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

internal b32
is_pos_pathfind(Dungeon *dungeon, v2u pos)
{
    b32 result = (is_dungeon_pos_traversable_or_closed_door(dungeon->tiles, pos) &&
                      !is_dungeon_pos_trap(&dungeon->traps, pos));
    
    return(result);
}

internal b32
pathfind_map_has_default_values(Dungeon *dungeon, PathfindMap *pathfind_map)
{
    for(u32 y = 0; y < dungeon->size.h; ++y)
    {
        for(u32 x = 0; x < dungeon->size.w; ++x)
        {
            v2u pos = {x, y};
            
            if(is_pos_pathfind(dungeon, pos) &&
               get_pathfind_value(pathfind_map, pos) == U32_MAX)
            {
                //printf("Pathfind Default Value Pos: %u, %u\n", pos.x, pos.y);
                return(true);
            }
        }
    }
    
    return(false);
}

internal void
update_pathfind_map(Dungeon *dungeon, PathfindMap *pathfind_map, v2u start_pos)
{
    //printf("start_pos: %u, %u\n", start_pos.x, start_pos.y);
    //printf("dungeon->ready_for_pathfinding: %u\n", dungeon->ready_for_pathfinding);
    
    if(dungeon->ready_for_pathfinding &&
           is_dungeon_pos_traversable_or_closed_door(dungeon->tiles, start_pos))
    {
        // Init pathfind map
        for(u32 y = 0; y < dungeon->size.h; ++y)
        {
            for(u32 x = 0; x < dungeon->size.w; ++x)
            {
                v2u pos = {x, y};
                
                set_pathfind_value(pathfind_map, pos, U32_MAX);
            }
        }
        
        // Set pathfind map start_pos value
        set_pathfind_value(pathfind_map, start_pos, 0);
        
        while(pathfind_map_has_default_values(dungeon, pathfind_map))
        {
            for(u32 y = 0; y < dungeon->size.h; ++y)
            {
                for(u32 x = 0; x < dungeon->size.w; ++x)
                {
                    v2u pos = {x, y};
                    
                    if(is_pos_pathfind(dungeon, pos))
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