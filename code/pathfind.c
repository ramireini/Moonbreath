internal u32
pathfind_value(PathfindData pathfind, v2u pos)
{
    u32 result = pathfind.array[(pos.y * pathfind.width) + pos.x];
    return(result);
}

internal void
set_pathfind_value(u32 *map, u32 width, v2u pos, u32 value)
{
    map[(pos.y * width) + pos.x] = value;
}

internal v2u
next_pathfind_pos(Dungeon *dungeon, Entity *player, Entity *enemy)
{
    v2u result = {0};
    
    u32 closest_distance = pathfind_value(dungeon->pathfind, enemy->pos);
    v2u pos = {0, 0};
    
    for(u32 direction = Direction_Up; direction <= Direction_DownRight; ++direction)
    {
        switch(direction)
        {
            case Direction_Up: pos = V2u(enemy->pos.x, enemy->pos.y - 1); break;
            case Direction_Down: pos = V2u(enemy->pos.x, enemy->pos.y + 1); break;
            case Direction_Left: pos = V2u(enemy->pos.x - 1, enemy->pos.y); break;
            case Direction_Right: pos = V2u(enemy->pos.x + 1, enemy->pos.y); break;
            
            case Direction_UpLeft: pos = V2u(enemy->pos.x - 1, enemy->pos.y - 1); break;
            case Direction_UpRight: pos = V2u(enemy->pos.x + 1, enemy->pos.y - 1); break;
            case Direction_DownLeft: pos = V2u(enemy->pos.x - 1, enemy->pos.y + 1); break;
            case Direction_DownRight: pos = V2u(enemy->pos.x + 1, enemy->pos.y + 1); break;
            
            invalid_default_case;
        }
        
        u32 pos_distance = pathfind_value(dungeon->pathfind, pos);
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
update_pathfind_map(Dungeon *dungeon, Entity *player)
{
    if(is_tile_traversable(dungeon->tiles, player->pos))
    {
        // Initialize to a high value.
        u32 map_default_value = 1024;
        for(u32 y = 0; y < dungeon->height; ++y)
        {
            for(u32 x = 0; x < dungeon->width; ++x)
            {
                set_pathfind_value(dungeon->pathfind.array, dungeon->width, V2u(x, y), map_default_value);
            }
        }
        
        // This is the lowest number, the goal.
        set_pathfind_value(dungeon->pathfind.array, dungeon->width, player->pos, 0);
        
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
                       is_tile_id(dungeon->tiles, current, Tile_StoneDoorClosed))
                    {
                        if(pos_in_dungeon(dungeon, current))
                        {
                            u32 closest_distance = pathfind_value(dungeon->pathfind, current);
                            v2u pos = {0, 0};
                            
                            for(u32 direction = Direction_Up; direction <= Direction_DownRight; ++direction)
                            {
                                switch(direction)
                                {
                                    case Direction_Up: pos = V2u(current.x, current.y - 1); break;
                                    case Direction_Down: pos = V2u(current.x, current.y + 1); break;
                                    case Direction_Left: pos = V2u(current.x - 1, current.y); break;
                                    case Direction_Right: pos = V2u(current.x + 1, current.y); break;
                                    
                                    case Direction_UpLeft: pos = V2u(current.x - 1, current.y - 1); break;
                                    case Direction_UpRight: pos = V2u(current.x + 1, current.y - 1); break;
                                    case Direction_DownLeft: pos = V2u(current.x - 1, current.y + 1); break;
                                    case Direction_DownRight: pos = V2u(current.x + 1, current.y + 1); break;
                                    
                                    invalid_default_case;
                                }
                                
                                u32 pos_distance = pathfind_value(dungeon->pathfind, pos);
                                if(pos_distance < closest_distance)
                                {
                                    closest_distance = pos_distance;
                                    set_pathfind_value(dungeon->pathfind.array, dungeon->width, current, closest_distance + 1);
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
                    if(pathfind_value(dungeon->pathfind_map, dungeon->w, current) != 1024)
                    {
                        printf("%u ", pathfind_value(dungeon->pathfind_map, dungeon->w, current));
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
                    if(is_tile_traversable(dungeon->tiles, current) &
                       pathfind_value(dungeon->pathfind, current) == map_default_value)
                    {
                        goto next_iteration;
                    }
                }
            }
            
            return;
        }
    }
}