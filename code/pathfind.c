internal u32
pathfind_value(u32 *map, u32 width, v2u pos)
{
    u32 result = map[(pos.y * width) + pos.x];
    return(result);
}

internal void
set_pathfind_value(u32 *map, u32 width, v2u pos, u32 value)
{
    map[(pos.y * width) + pos.x] = value;
}

internal v2u
next_pathfind_pos(dungeon_t *dungeon, u32 *map, u32 width, entity_t *enemy, entity_t *player)
{
    v2u result = {0};
    
    u32 closest_distance = pathfind_value(map, width, enemy->pos);
    v2u pos = {0, 0};
    
    for(u32 dir = dir_up; dir <= dir_down_right; ++dir)
    {
        switch(dir)
        {
            case dir_up: pos = V2u(enemy->pos.x, enemy->pos.y - 1); break;
            case dir_down: pos = V2u(enemy->pos.x, enemy->pos.y + 1); break;
            case dir_left: pos = V2u(enemy->pos.x - 1, enemy->pos.y); break;
            case dir_right: pos = V2u(enemy->pos.x + 1, enemy->pos.y); break;
            
            case dir_up_left: pos = V2u(enemy->pos.x - 1, enemy->pos.y - 1); break;
            case dir_up_right: pos = V2u(enemy->pos.x + 1, enemy->pos.y - 1); break;
            case dir_down_left: pos = V2u(enemy->pos.x - 1, enemy->pos.y + 1); break;
            case dir_down_right: pos = V2u(enemy->pos.x + 1, enemy->pos.y + 1); break;
            
            invalid_default_case;
        }
        
        u32 pos_distance = pathfind_value(map, width, pos);
        if(pos_distance < closest_distance &&
           (!is_tile_occupied(dungeon->tiles, pos) || V2u_equal(pos, player->pos)))
        {
            closest_distance = pos_distance;
            result = pos;
        }
    }
    
#if 0
    printf("closest_distance: %u\n", closest_distance);
    printf("pos.x: %u\n", pos.x);
    printf("pos.y: %u\n\n", pos.y);
#endif
    
    return(result);
}

internal void
update_pathfind_map(dungeon_t *dungeon, entity_t *player)
{
    if(is_tile_traversable(dungeon->tiles, player->pos))
    {
        // Initialize to a high value.
        u32 map_default_value = 1024;
        for(u32 y = 0; y < dungeon->h; ++y)
        {
            for(u32 x = 0; x < dungeon->w; ++x)
            {
                set_pathfind_value(dungeon->pathfind_map, dungeon->w, V2u(x, y), map_default_value);
            }
        }
        
        // This is the lowest number, the goal.
        set_pathfind_value(dungeon->pathfind_map, dungeon->w, player->pos, 0);
        
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
                       is_tile_value(dungeon->tiles, current, tile_stone_door_closed))
                    {
                        if(is_inside_rectangle(current, V4u(0, 0, dungeon->w, dungeon->h)))
                        {
                            u32 closest_distance = pathfind_value(dungeon->pathfind_map, dungeon->w, current);
                            v2u pos = {0, 0};
                            
                            for(u32 dir = dir_up; dir <= dir_down_right; ++dir)
                            {
                                switch(dir)
                                {
                                    case dir_up: pos = V2u(current.x, current.y - 1); break;
                                    case dir_down: pos = V2u(current.x, current.y + 1); break;
                                    case dir_left: pos = V2u(current.x - 1, current.y); break;
                                    case dir_right: pos = V2u(current.x + 1, current.y); break;
                                    
                                    case dir_up_left: pos = V2u(current.x - 1, current.y - 1); break;
                                    case dir_up_right: pos = V2u(current.x + 1, current.y - 1); break;
                                    case dir_down_left: pos = V2u(current.x - 1, current.y + 1); break;
                                    case dir_down_right: pos = V2u(current.x + 1, current.y + 1); break;
                                    
                                    invalid_default_case;
                                }
                                
                                u32 pos_distance = pathfind_value(dungeon->pathfind_map, dungeon->w, pos);
                                if(pos_distance < closest_distance)
                                {
                                    closest_distance = pos_distance;
                                    set_pathfind_value(dungeon->pathfind_map, dungeon->w, current, closest_distance + 1);
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
                    if(get_pathfind_value(dungeon->pathfind_map, dungeon->w, current) != 1024)
                    {
                        printf("%u ", get_pathfind_value(dungeon->pathfind_map, dungeon->w, current));
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
                    if(is_tile_traversable(dungeon->tiles, current) &
                       pathfind_value(dungeon->pathfind_map, dungeon->w, current) == map_default_value)
                    {
                        goto next_iteration;
                    }
                }
            }
            
            return;
        }
    }
}