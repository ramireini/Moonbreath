#if MOONBREATH_SLOW
internal void
print_pathfind_pos_info(PathfindPosInfo info)
{
 printf("Pos: %u, %u - Value: %u - Direction: %s\n",
        info.pos.x, info.pos.y,
        info.pathfind_value,
        get_direction_string(info.direction));
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
 result.pathfind_value = get_pathfind_value(map, result.pos);
 result.direction = direction;
 
 #if 0
 printf("result.pos: %u, %u\n", result.pos.x, result.pos.y);
  printf("result.pathfind_value: %u\n", result.pathfind_value);
  printf("result.direction: %u\n", direction);
#endif
 
    return(result);
}

internal void
set_pathfind_map_value(PathfindMap *pathfind_map, v2u pos, u32 value)
{
    pathfind_map->array[(pos.y * pathfind_map->width) + pos.x] = value;
}

internal b32
can_entity_move_to_pos(Entity *entity, Dungeon *dungeon, v2u pos, b32 came_from_entity_pathfind)
{
 assert(is_entity_valid(entity));
 assert(dungeon);
 
 b32 result = false;
 
    if(entity->type == EntityType_Player &&
    is_dungeon_pos_traversable_or_closed_door(dungeon->tiles, pos) &&
    !get_dungeon_pos_occupier(dungeon->tiles, pos))
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
         !get_dungeon_pos_occupier(dungeon->tiles, pos))
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

internal PathfindResult
get_pathfind_result(Entity *entity, Dungeon *dungeon, PathfindMap *map, PathfindMethod method)
{
 assert(is_entity_valid(entity));
 assert(!is_v2u_zero(entity->pos));
 assert(dungeon);
 assert(map);
 assert(method);
 
#if 0
 printf("map: %p\n", map);
 printf("entity->pathfind_map: %p\n", entity->pathfind_map);
#endif
 
 #if 0
 printf("entity->pos: %u, %u\n", entity->pos.x, entity->pos.y);
 printf("val: %u\n", get_pathfind_value(entity->explore_map, entity->pos));
 #endif
 
 assert(get_pathfind_value(map, entity->pos)); // Map has to be initted
 //printf("Start Pos Value: %u\n", get_pathfind_value(pathfind_map, entity->pos));
 
 b32 is_result_set = false;
 PathfindResult result = {0};
 
    // Save positions
 PathfindPosInfo info_array[Direction_Count] = {0};
 for(Direction direction = Direction_Up; direction <= Direction_DownRight; ++direction)
 {
  PathfindPosInfo info = get_pathfind_pos_info(map, entity->pos, direction);
  info_array[direction] = info;
  }
 
 // Find the best position out of the directions
        for(Direction direction = Direction_Up; direction <= Direction_DownRight; ++direction)
 {
  PathfindPosInfo *info = &info_array[direction];
  if(is_result_set)
  {
    b32 is_info_better = false;
    
    switch(method)
    {
     case PathfindMethod_Away:
     {
      if(info->pathfind_value > result.info.pathfind_value)
      {
       is_info_better = true;
      }
     } break;
     
     case PathfindMethod_Toward:
     {
      if(info->pathfind_value < result.info.pathfind_value)
      {
       is_info_better = true;
      }
     } break;
     
     invalid_default_case;
    }
    
   // Set better result info
    if(is_info_better)
    {
     result.info = *info;
    }
        }
        else
            {
   // Set first result info
   is_result_set = true;
   result.info = *info;
   }
 }
 
 result.can_move = can_entity_move_to_pos(entity, dungeon, result.info.pos, true);
 
 #if 0
 // Print array positions
 for(Direction direction = Direction_Up; direction <= Direction_DownRight; ++direction)
 {
  print_pathfind_pos_info(info_array[direction]);
 }
 //assert(0);
 
 printf("\nResult Pos Info\n");
 print_pathfind_pos_info(result.info);
 
 printf("\n\n");
    //assert(0);
 
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
update_pathfind_map(Entity *entity, PathfindMap *map, Dungeon *dungeon, v2u start)
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
    
    if(dungeon->can_pathfind && is_dungeon_pos_traversable_or_closed_door(dungeon->tiles, start))
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
  set_pathfind_map_value(map, start, 0);
  
        while(pathfind_map_has_default_values(entity, dungeon, map))
  {
   assert_loop_count();
   
            for(u32 y = 0; y < dungeon->size.h; ++y)
            {
                for(u32 x = 0; x < dungeon->size.w; ++x)
                {
     v2u pos = {x, y};
     
     // This does not care about what the entity can traverse on, this is just so we can
     // set the pathfind map value, whether the entity can move to a pathfind map pos
     // pos is decided elsewhere.
     if(is_dungeon_pos_traversable_or_closed_door(dungeon->tiles, pos))
                    {
                        assert(is_pos_inside_dungeon(dungeon->size, pos));
                        u32 lowest_target_dist = get_pathfind_value(map, pos);
                            
                            for(Direction direction = Direction_Up; direction <= Direction_DownRight; ++direction)
                        {
       PathfindPosInfo neighbour = get_pathfind_pos_info(map, pos, direction);
                            assert(is_pos_inside_dungeon(dungeon->size, neighbour.pos));
                            
                            // See if any of the neighbouring positions has a smaller value than current position.
       if(neighbour.pathfind_value < lowest_target_dist)
                                {
        lowest_target_dist = neighbour.pathfind_value;
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