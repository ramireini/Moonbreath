internal u32
get_pathfind_value(PathfindData *pathfind, v2u pos)
{
    u32 result = pathfind->array[(pos.y * pathfind->width) + pos.x];
    return(result);
}

internal void
set_pathfind_value(PathfindData *pathfind, v2u pos, u32 value)
{
    pathfind->array[(pos.y * pathfind->width) + pos.x] = value;
}

internal v2u
next_pathfind_pos(Dungeon *dungeon, Entity *player, Entity *enemy)
{
    v2u result = {0};
    u32 closest_distance = get_pathfind_value(&dungeon->pathfind, enemy->pos);
    
    for(u32 direction = Direction_Up; direction <= Direction_DownRight; ++direction)
    {
        v2u pos = {0, 0};
        
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
        
        u32 pos_distance = get_pathfind_value(&dungeon->pathfind, pos);
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
        for(u32 y = 0; y < dungeon->height; ++y)
        {
            for(u32 x = 0; x < dungeon->width; ++x)
            {
                set_pathfind_value(&dungeon->pathfind, V2u(x, y), MAX_U32);
            }
        }
        
        // This is the lowest number, the goal.
        set_pathfind_value(&dungeon->pathfind, player->pos, 0);
        
        for(;;)
        {
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
                            u32 closest_distance = get_pathfind_value(&dungeon->pathfind, current);
                            
                            for(u32 direction = Direction_Up; direction <= Direction_DownRight; ++direction)
                            {
                                v2u pos = {0, 0};
                                
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
                                
                                u32 pos_distance = get_pathfind_value(&dungeon->pathfind, pos);
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
            for(u32 y = 0; y < dungeon->height; ++y)
            {
                for(u32 x = 0; x < dungeon->width; ++x)
                {
                    v2u current = {x, y};
                    u32 value = get_pathfind_value(dungeon->pathfind, current);
                    if(value != MAX_U32)
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
                       get_pathfind_value(&dungeon->pathfind, current) == MAX_U32)
                    {
                        break;
                    }
                }
            }
            
            return;
        }
    }
}