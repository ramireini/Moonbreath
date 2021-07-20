typedef enum
{
    PathfindResultType_None,
    
    PathfindResultType_Move,
    PathfindResultType_BlockedByDungeonTrap
} PathfindResultType;

typedef struct
{
    b32 is_valid;
    v2u pos;
    u32 target_dist;
    Direction direction;
} PathfindPosInfo;

#if MOONBREATH_SLOW
internal void
print_pathfind_pos_info(PathfindPosInfo info)
{
    printf("is_valid: %s\n", info.is_valid ? "true" : "false");
    printf("pos: %u, %u\n", info.pos.x, info.pos.y);
    printf("target_dist: %u\n", info.target_dist);
    printf("Direction: %s\n\n", get_direction_string(info.direction));
}
#endif

internal u32
get_pathfind_value(PathfindMap *pathfind_map, v2u pos)
{
    assert(pathfind_map);
    
    u32 result = pathfind_map->array[(pos.y * pathfind_map->width) + pos.x];
    return(result);
}

internal PathfindPosInfo
get_pathfind_pos_info(PathfindMap *map, v2u pos, Direction direction)
{
    assert(map);
    assert(!is_v2u_zero(pos));
    assert(direction);
    
    PathfindPosInfo result = {0};
    result.pos = get_direction_pos(pos, direction);
    result.target_dist = get_pathfind_value(map, result.pos);
    result.direction = direction;
    
    return(result);
}

internal void
set_pathfind_value(PathfindMap *pathfind_map, v2u pos, u32 value)
{
    assert(pathfind_map);
    pathfind_map->array[(pos.y * pathfind_map->width) + pos.x] = value;
}

internal PathfindPosInfo
get_pathfind_result(PathfindMap *map, Dungeon *dungeon, v2u start_pos, v2u target_pos)
{
    assert(map);
    assert(dungeon);
    assert(!is_v2u_zero(start_pos));
    assert(!is_v2u_zero(target_pos));
    assert(get_pathfind_value(map, start_pos));
    
    PathfindPosInfo result = {0};
    result.pos = start_pos;
    //printf("Distance from start: %u\n", get_pathfind_value(map, start_pos));
    
    // Save positions
    PathfindPosInfo info_array[Direction_Count] = {0};
    
    for(Direction direction = Direction_Up; direction <= Direction_DownRight; ++direction)
    {
        PathfindPosInfo info = get_pathfind_pos_info(map, start_pos, direction);
        
        if((is_dungeon_pos_traversable_and_unoccupied(dungeon->tiles, info.pos) ||
                   is_v2u_equal(info.pos, target_pos)) &&
               !is_dungeon_pos_trap(&dungeon->traps, info.pos))
        {
            PathfindPosInfo *array_pos = &info_array[direction];
            
            *array_pos = info;
            array_pos->is_valid = true;
            }
    }
    
    #if 0
    // Print array positions
    for(Direction direction = Direction_Up; direction <= Direction_DownRight; ++direction)
    {
        PathfindPosInfo *info = &info_array[direction];
        if(info->is_valid)
        {
            print_pathfind_pos_info(*info);
        }
    }
    
    //assert(0);
    #endif
    
    // Find the closest position
        for(Direction direction = Direction_Up; direction <= Direction_DownRight; ++direction)
        {
        PathfindPosInfo *array_pos = &info_array[direction];
        if(array_pos->is_valid)
        {
        if(result.is_valid)
            {
                // Set better result
                if(array_pos->target_dist < result.target_dist)
                {
                    result = *array_pos;
                }
        }
        else
            {
                // Set first time result
                result = *array_pos;
            }
        }
        }
    
#if 0
    printf("Closest Position\n");
    print_pathfind_pos_info(result);
    //assert(0);
    #endif
    
    return(result);
}

#if MOONBREATH_SLOW
internal void
print_pathfind_map(Dungeon *dungeon, PathfindMap *pathfind_map)
{
    assert(dungeon);
    assert(pathfind_map);
    
    printf("\n\nPathfind Map\n");
    
    for(u32 y = 0; y < dungeon->size.h; ++y)
    {
        for(u32 x = 0; x < dungeon->size.w; ++x)
        {
            v2u pos = {x, y};
            u32 value = get_pathfind_value(pathfind_map, pos);
            
            if(value != U32_MAX) printf("%u ", value);
        }
        
        printf("\n");
    }
}
#endif

internal b32
is_pos_pathfindable(Dungeon *dungeon, v2u pos)
{
    assert(dungeon);
    
    b32 result = (is_dungeon_pos_traversable_or_closed_door(dungeon->tiles, pos) &&
                      !is_dungeon_pos_trap(&dungeon->traps, pos));
    
    return(result);
}

internal b32
pathfind_map_has_default_values(Dungeon *dungeon, PathfindMap *map)
{
    assert(dungeon);
    assert(map);
    
    for(u32 y = 0; y < dungeon->size.h; ++y)
    {
        for(u32 x = 0; x < dungeon->size.w; ++x)
        {
            v2u pos = {x, y};
            
            if(is_pos_pathfindable(dungeon, pos) &&
               get_pathfind_value(map, pos) == U32_MAX)
            {
                //printf("Pathfind Default Value Pos: %u, %u\n", pos.x, pos.y);
                return(true);
            }
        }
    }
    
    return(false);
}

internal void
update_pathfind_map(Dungeon *dungeon, PathfindMap *map, v2u start_pos)
{
    map->width = dungeon->size.w;
    
    assert(dungeon);
    assert(map);
    assert(!is_v2u_zero(start_pos));
    
    //printf("start_pos: %u, %u\n", start_pos.x, start_pos.y);
    //printf("dungeon->ready_for_pathfinding: %u\n", dungeon->ready_for_pathfinding);
    
    if(dungeon->can_pathfind &&
           is_dungeon_pos_traversable_or_closed_door(dungeon->tiles, start_pos))
    {
        // Init pathfind map
        for(u32 y = 0; y < dungeon->size.h; ++y)
        {
            for(u32 x = 0; x < dungeon->size.w; ++x)
            {
                v2u pos = {x, y};
                set_pathfind_value(map, pos, U32_MAX);
            }
        }
        
        // Set pathfind map start_pos value
        set_pathfind_value(map, start_pos, 0);
        
        while(pathfind_map_has_default_values(dungeon, map))
        {
            for(u32 y = 0; y < dungeon->size.h; ++y)
            {
                for(u32 x = 0; x < dungeon->size.w; ++x)
                {
                    v2u pos = {x, y};
                    
                    if(is_pos_pathfindable(dungeon, pos))
                    {
                        assert(is_pos_inside_dungeon(dungeon->size, pos));
                        u32 lowest_target_dist = get_pathfind_value(map, pos);
                            
                            for(Direction direction = Direction_Up; direction <= Direction_DownRight; ++direction)
                        {
                            PathfindPosInfo neighbour = get_pathfind_pos_info(map, pos, direction);
                            assert(is_pos_inside_dungeon(dungeon->size, neighbour.pos));
                            
                            // See if any of the neighbouring positions has a smaller value than current position.
                            if(neighbour.target_dist < lowest_target_dist)
                                {
                                lowest_target_dist = neighbour.target_dist;
                                set_pathfind_value(map, pos, lowest_target_dist + 1);
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