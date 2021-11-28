#if MOONBREATH_SLOW
internal void
print_pathfind_pos_info(PathfindPosInfo info)
{
    printf("Pos: %u, %u - Val: %u - Dir: %s - CanMove: %s\n",
           info.pos.x, info.pos.y,
           info.value,
           get_direction_string(info.direction),
           get_bool_string(info.can_move));
}

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
is_new_pathfind_value_better(u32 current_value, u32 new_value, PathfindMethod method)
{
    assert(method == PathfindMethod_Toward ||
           method == PathfindMethod_Away);
    
    b32 result = false;
    
    if(method == PathfindMethod_Toward &&
       new_value < current_value)
    {
        result = true;
    }
    else if(method == PathfindMethod_Away &&
            new_value > current_value)
    {
        result = true;
    }
    
    return(result);
}

internal u32
get_pathfind_value(PathfindMap *map, v2u pos)
{
    assert(map);
    
    u32 result = map->array[(pos.y * map->width) + pos.x];
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
    result.value = get_pathfind_value(map, result.pos);
    result.direction = direction;
    
    return(result);
}

internal void
set_pathfind_map_value(PathfindMap *map, v2u pos, u32 value)
{
    map->array[(pos.y * map->width) + pos.x] = value;
}

internal b32
can_entity_move_to_pos(Entity *entity, Dungeon *dungeon, v2u pos, b32 came_from_entity_pathfind)
{
    assert(is_entity_valid(entity));
    assert(dungeon);
    
    b32 result = false;
    
    if(entity->type == EntityType_Player &&
       is_dungeon_pos_traversable_or_closed_door(dungeon->tiles, pos) &&
       !get_dungeon_pos_entity(dungeon->tiles, pos))
    {
        if(came_from_entity_pathfind && get_dungeon_pos_trap(&dungeon->traps, pos))
        {
        }
        else
        {
            result = true;
        }
    }
    else if(entity->type == EntityType_Enemy &&
            is_dungeon_pos_traversable_or_closed_door(dungeon->tiles, pos) &&
            !get_dungeon_pos_entity(dungeon->tiles, pos))
    {
        if(came_from_entity_pathfind && get_dungeon_pos_trap(&dungeon->traps, pos))
        {
        }
        else if(is_dungeon_pos_closed_door(dungeon->tiles, pos))
        {
            if(is_set(entity->flags, EntityFlag_CanOpenDoors))
            {
                result = true;
            }
        }
        else
        {
            result = true;
        }
    }
    
    return(result);
}

internal b32
is_pathfind_map_pos_default(PathfindMap *map, v2u pos)
{
    assert(map);
    
    b32 result = (get_pathfind_value(map, pos) == U32_MAX);
    return(result);
}

internal PathfindPosInfo
get_pathfind_result(Entity *entity,
                    Dungeon *dungeon,
                    PathfindMap *map,
                    PathfindMethod method,
                    b32 best_pathfind_value_only)
{
    assert(is_entity_valid(entity));
    assert(!is_v2u_zero(entity->pos));
    assert(dungeon);
    assert(map);
    assert(method == PathfindMethod_Toward ||
           method == PathfindMethod_Away);
    
#if MOONBREATH_SLOW
    if(!get_pathfind_value(map, entity->pos)) // Map has to be initted
    {
        printf("map: %p\n", map);
        printf("name: %s (index: %u)\n", entity->name.s, entity->index);
        printf("entity pos: %u, %u\n", entity->pos.x, entity->pos.y);
        printf("entity pos pathfind value: %u\n\n", get_pathfind_value(map, entity->pos));
        
        assert(0);
    }
#endif
    
    b32 is_result_set = false;
    PathfindPosInfo result = {0};
    
    // Set direction positions
    PathfindPosInfo info_array[Direction_Count] = {0};
    for(Direction direction = Direction_Up; direction <= Direction_DownRight; ++direction)
    {
        PathfindPosInfo *info = &info_array[direction];
        *info = get_pathfind_pos_info(map, entity->pos, direction);
        
        info->can_move = can_entity_move_to_pos(entity, dungeon, info->pos, true);
    }
    
    // Find the best direction position
    for(Direction direction = Direction_Up; direction <= Direction_DownRight; ++direction)
    {
        PathfindPosInfo *info = &info_array[direction];
        if(is_result_set)
        {
            b32 is_better = false;
            
            // Sometimes the function caller doesn't want to get the 2nd best etc. position when
            // pathfinding, this is for that situation.
            if(best_pathfind_value_only)
            {
                is_better = is_new_pathfind_value_better(result.value, info->value, method);
            }
            else
            {
                if(result.can_move)
                {
                    if(info->can_move)
                    {
                        is_better = is_new_pathfind_value_better(result.value, info->value, method);
                    }
                }
                else
                {
                    if(info->can_move)
                    {
                        u32 entity_pos_value = get_pathfind_value(map, entity->pos);
                        
                        if(method == PathfindMethod_Toward &&
                           info->value < entity_pos_value)
                        {
                            is_better = true;
                        }
                        else if(method == PathfindMethod_Away &&
                                info->value > entity_pos_value)
                        {
                            is_better = true;
                        }
                    }
                }
            }
            
            // Set better result
            if(is_better)
            {
                result = *info;
            }
        }
        else
        {
            // Set first result
            is_result_set = true;
            result = *info;
        }
    }
    
#if 0
    //if(entity->index == 12)
    {
        // Print direction info
        for(Direction direction = Direction_Up; direction <= Direction_DownRight; ++direction)
        {
            print_pathfind_pos_info(info_array[direction]);
        }
        
        printf("\nResult Pos Info\n");
        print_pathfind_pos_info(result);
        
        printf("\n\n");
    }
#endif
    
    assert(is_result_set);
    return(result);
}

internal b32
pathfind_map_has_default_values(Entity *entity, Dungeon *dungeon, PathfindMap *map)
{
    assert(is_entity_valid(entity));
    assert(dungeon);
    assert(map);
    
    for(u32 y = 0; y < dungeon->size.h; ++y)
    {
        for(u32 x = 0; x < dungeon->size.w; ++x)
        {
            v2u pos = {x, y};
            
            if(can_entity_move_to_pos(entity, dungeon, pos, false) &&
               is_pathfind_map_pos_default(map, pos))
            {
                //printf("Pathfind Default Value Pos: %u, %u\n", pos.x, pos.y);
                return(true);
            }
        }
    }
    
    return(false);
}

internal void
update_pathfind_map(Entity *entity, PathfindMap *map, Dungeon *dungeon, v2u start_pos)
{
    assert(is_entity_valid(entity));
    assert(map);
    assert(dungeon);
    
    map->width = dungeon->size.w;
    
#if 0
    printf("Name: %s\n", entity->name.s);
    printf("Start Pos: %u, %u\n", start_pos.x, start_pos.y);
    printf("Dungeon Can Pathfind: %u\n\n", dungeon->can_pathfind);
#endif
    
    if(dungeon->can_pathfind && is_dungeon_pos_traversable_or_closed_door(dungeon->tiles, start_pos))
    {
        // Init pathfind map
        for(u32 y = 0; y < dungeon->size.h; ++y)
        {
            for(u32 x = 0; x < dungeon->size.w; ++x)
            {
                v2u pos = {x, y};
                set_pathfind_map_value(map, pos, U32_MAX);
            }
        }
        
        // Set pathfind map start_pos value
        set_pathfind_map_value(map, start_pos, 0);
        
        while(pathfind_map_has_default_values(entity, dungeon, map))
        {
            for(u32 y = 0; y < dungeon->size.h; ++y)
            {
                for(u32 x = 0; x < dungeon->size.w; ++x)
                {
                    v2u pos = {x, y};
                    
                    // This doesn't care if the entity can move to the pos. It just wants to set the
                    // pathfind values and nothing more.
                    if(is_dungeon_pos_traversable_or_closed_door(dungeon->tiles, pos))
                    {
                        assert(is_pos_inside_dungeon(dungeon->size, pos));
                        u32 lowest_target_dist = get_pathfind_value(map, pos);
                        
                        for(Direction direction = Direction_Up; direction <= Direction_DownRight; ++direction)
                        {
                            PathfindPosInfo neighbour = get_pathfind_pos_info(map, pos, direction);
                            assert(is_pos_inside_dungeon(dungeon->size, neighbour.pos));
                            
                            // See if any of the neighbouring positions has a smaller value than current position.
                            if(neighbour.value < lowest_target_dist)
                            {
                                lowest_target_dist = neighbour.value;
                                set_pathfind_map_value(map, pos, lowest_target_dist + 1);
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