internal u32
get_pathfind_value(u32 *map, u32 width, v2u pos)
{
    u32 result = map[(pos.y * width) + pos.x];
    return(result);
}

internal void
set_pathfind_value(u32 *map, u32 width, v2u pos, u32 value)
{
    map[(pos.y * width) + pos.x] = value;
}

#if 0
internal void
print_map(u32 *map, u32 width, u32 height)
{
    for(u32 y = 0; y < height; ++y)
    {
        for(u32 x = 0; x < width; ++x)
        {
            v2u current = {x, y};
            if(get_pathfind_value(map, width, current) != 1024)
            {
                printf("%u ", get_pathfind_value(map, width, current));
            }
        }
        
        printf("\n");
    }
}
#endif

internal void
update_pathfind_map(u32 *map, u32 width, u32 height)
{
    assert(width != 0 && height != 0);
    
    if(is_dungeon_traversable(player->pos))
    {
        u32 map_default_value = 1024;
        
        // NOTE(rami): Initialize to a high value.
        for(u32 y = 0; y < height; ++y)
        {
            for(u32 x = 0; x < width; ++x)
            {
                set_pathfind_value(map, width, V2u(x, y), map_default_value);
            }
        }
        
        // NOTE(rami): This is the lowest number, the goal.
        set_pathfind_value(map, width, player->pos, 0);
        
        for(;;)
        {
            next_iteration:
            
            for(u32 y = 0; y < height; ++y)
            {
                for(u32 x = 0; x < width; ++x)
                {
                    v2u current = {x, y};
                    
                    // NOTE(rami): We need to be able to go through closed doors
                    // with this so we don't infinite loop. If we were to have
                    // different doors in the future, we would need something like
                    // a is_door() function to be used here instead.
                    if(is_dungeon_traversable(current) ||
                       is_dungeon_tile(current, tile_stone_door_closed))
                    {
                        u32 lowest_neighbour = get_pathfind_value(map, width, current);
                        
                        // NOTE(rami): Up
                        if(y >= 1)
                        {
                            v2u up = {x, y - 1};
                            if(get_pathfind_value(map, width, up) < lowest_neighbour)
                            {
                                lowest_neighbour = get_pathfind_value(map, width, up);
                            }
                        }
                        
                        // NOTE(rami): Down
                        if(y < (height - 1))
                        {
                            v2u down = {x, y + 1};
                            if(get_pathfind_value(map, width, down) < lowest_neighbour)
                            {
                                lowest_neighbour = get_pathfind_value(map, width, down);
                            }
                        }
                        
                        // NOTE(rami): Left
                        if(x >= 1)
                        {
                            v2u left = {x - 1, y};
                            if(get_pathfind_value(map, width, left) < lowest_neighbour)
                            {
                                lowest_neighbour = get_pathfind_value(map, width, left);
                            }
                        }
                        
                        // NOTE(rami): Right
                        if(x < (width - 1))
                        {
                            v2u right = {x + 1, y};
                            if(get_pathfind_value(map, width, right) < lowest_neighbour)
                            {
                                lowest_neighbour = get_pathfind_value(map, width, right);
                            }
                        }
                        
                        if(lowest_neighbour < map[(y * width) + x])
                        {
                            set_pathfind_value(map, width, current, lowest_neighbour + 1);
                        }
                    }
                }
            }
            
#if 0
            printf("\n\nResult Map\n");
            print_map(map, width, height);
#endif
            
            for(u32 y = 0; y < height; ++y)
            {
                for(u32 x = 0; x < width; ++x)
                {
                    v2u current = {x, y};
                    if(is_dungeon_traversable(current) && get_pathfind_value(map, width, current) == map_default_value)
                    {
                        goto next_iteration;
                    }
                }
            }
            
            break;
        }
    }
}

internal v2u
next_pathfind_pos(u32 *map, u32 width, entity_t *enemy)
{
    v2u result = {0};
    
    u32 lowest_distance = get_pathfind_value(map, width, enemy->pos);
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
        
        u32 pos_distance = get_pathfind_value(map, width, pos);
        if(pos_distance <= lowest_distance && (!is_dungeon_occupied(pos) || V2u_equal(pos, player->pos)))
        {
            lowest_distance = pos_distance;
            result = pos;
        }
    }
    
    return(result);
}