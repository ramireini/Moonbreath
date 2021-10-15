#define add_editor_source_tile(group, source_type, tile_id) add_editor_source(group, source_type, tile_id, 0, 0, 0)
#define add_editor_source_trap(group, trap_type) add_editor_source(group, EditorSourceType_Trap, 0, trap_type, 0, 0)
#define add_editor_source_entity(group, entity_id) add_editor_source(group, EditorSourceType_Entity, 0, 0, entity_id, get_entity_name(entity_id))
internal void
add_editor_source(EditorGroup *group,
                  EditorSourceType type,
                  DungeonTileID tile_id,
                  DungeonTrapType trap_type,
                  EntityID entity_id,
                  char *name)
{
 assert(group);
 assert(type);
 
 for(u32 index = 0; index < MAX_EDITOR_SOURCE_COUNT; ++index)
 {
  EditorSource *source = &group->sources[index];
  if(!source->id)
  {
   source->id = index + 1;
   source->type = type;
   
   source->name = name;
   source->pos = group->pos;
   source->tile_dest = get_dungeon_tile_rect(source->pos);
   
   switch(type)
   {
    case EditorSourceType_Wall:
    case EditorSourceType_Floor:
    case EditorSourceType_Water:
    {
     assert(tile_id);
     source->tile_src = get_dungeon_tileset_rect(tile_id);
    } break;
    
    case EditorSourceType_DungeonTile:
    {
     assert(tile_id);
     source->tile_id = tile_id;
     source->tile_src = get_dungeon_tileset_rect(tile_id);
    } break;
    
    case EditorSourceType_Trap:
    {
     assert(trap_type);
     source->subtype = trap_type;
     source->tile_src = get_dungeon_trap_tile_src(trap_type);
    } break;
    
    case EditorSourceType_Entity:
    {
     assert(entity_id);
     source->subtype = entity_id;
     source->tile_src = get_dungeon_tile_rect_from_entity_id(entity_id);
    } break;
    
    invalid_default_case;
   }
   
   // Update pos
   ++group->pos.x;
   if(group->pos.x >= group->start_pos.x + group->sources_per_row)
   {
    group->pos.x = group->start_pos.x;
    ++group->pos.y;
   }
   
   return;
  }
 }
 
 assert(0);
}

internal void
zero_editor_target(EditorTarget *target)
{
 assert(target);
 zero_struct(*target);
}

internal void
zero_editor_active_source_info(ActiveEditorSourceInfo *info)
{
 assert(info);
 zero_struct(*info);
}

internal void
remove_editor_actives(EditorMode *editor)
{
 assert(editor);
 
 deselect_editor_mark(editor->groups);
 zero_editor_target(&editor->active_target);
 zero_editor_active_source_info(&editor->active_source_info);
}

internal EditorSource *
get_editor_source(EditorMode *editor)
{
 assert(editor);
 
 EditorSource *result = 0;
 
 ActiveEditorSourceInfo info = editor->active_source_info;
 if(info.group_id && info.source_id)
 {
  u32 group_index = info.group_id - 1;
 EditorGroup *group = &editor->groups[group_index];
 
  u32 source_index = info.source_id - 1;
 result = &group->sources[source_index];
 }
 
 return(result);
 }

internal EditorGroup *
add_editor_group(EditorMode *editor, char *name, u32 x, u32 y, u32 sources_per_row, b32 is_searchable)
{
 for(u32 index = 0; index < MAX_EDITOR_GROUP_COUNT; ++index)
 {
  EditorGroup *group = &editor->groups[index];
  if(!group->id)
  {
   group->is_searchable = is_searchable;
   group->id = index + 1;
   group->name = name;
   group->pos.x = x;
   group->pos.y = y;
   group->start_pos = group->pos;
   group->sources_per_row = sources_per_row;
   
   if(group->is_searchable)
   {
    Mark *mark = &group->mark;
    mark->is_active = false;
    mark->cursor_blink_duration = 800;
    mark->view.end = 24;
    set_mark_cursor_at_start(&group->mark);
   }
   
   return(group);
  }
 }
 
 assert(0);
}

internal b32
update_editor_sources(EditorMode *editor, v2u mouse_pos)
{
  for(u32 group_index = 0; group_index < MAX_EDITOR_GROUP_COUNT; ++group_index)
  {
   EditorGroup *group = &editor->groups[group_index];
   if(group->id)
   {
    for(u32 source_index = 0; source_index < MAX_EDITOR_SOURCE_COUNT; ++source_index)
    {
     EditorSource *source = &group->sources[source_index];
     if(source->type && is_pos_inside_rect(source->tile_dest, mouse_pos))
    {
     ActiveEditorSourceInfo *active_source_info = &editor->active_source_info;
     if(active_source_info->group_id == group->id &&
        active_source_info->source_id == source->id)
     {
      // Deselect
      zero_editor_active_source_info(active_source_info);
      }
      else
     {
      // Select
      active_source_info->group_id = group->id;
      active_source_info->source_id = source->id;
      }
     
     zero_editor_target(&editor->active_target);
     
#if 0
      printf("group[%u]->source[%u].type: %u\n", group_index, source_index, source->type);
      printf("group[%u]->source[%u].subtype: %u\n\n", group_index, source_index, source->subtype);
#endif
      
      return(true);
     }
    }
   }
  }
 
 return(false);
}

internal void
place_editor_source(Random *random,
                           EntityState *entity_state,
                           Dungeon *dungeon,
                           v2u mouse_tile_pos,
                           EditorSource *source)
{
 switch(source->type)
 {
  case EditorSourceType_None: break;
  
  case EditorSourceType_Wall:
  {
   set_dungeon_pos_wall(random, &dungeon->spec, dungeon->tiles, mouse_tile_pos);
  } break;
  
  case EditorSourceType_Floor:
  {
   set_dungeon_pos_floor(random, dungeon->tiles, mouse_tile_pos);
  } break;
  
  case EditorSourceType_Water:
  {
   set_dungeon_pos_water(random, dungeon->tiles, mouse_tile_pos);
  } break;
  
  case EditorSourceType_DungeonTile:
  {
   set_dungeon_pos_tile(dungeon->tiles, mouse_tile_pos, source->tile_id);
  } break;
  
  case EditorSourceType_Trap:
  {
   assert(source->subtype);
   
   if(!get_dungeon_pos_trap(&dungeon->traps, mouse_tile_pos))
   {
    add_dungeon_trap(&dungeon->spec, &dungeon->traps, source->subtype, mouse_tile_pos);
   }
  } break;
  
  case EditorSourceType_Entity:
  {
   assert(source->subtype);
   
   if(!get_dungeon_pos_entity(dungeon->tiles, mouse_tile_pos))
   {
    add_enemy_entity(entity_state, dungeon, source->subtype, mouse_tile_pos.x, mouse_tile_pos.y);
   }
  } break;
  
  invalid_default_case
 }
}

internal v2u
get_editor_target_pos(EditorTarget *target)
{
 v2u result = {0};
 
 switch(target->type)
 {
  case EditorTargetType_Entity: result = target->entity->pos; break;
  case EditorTargetType_Trap: result = target->trap->pos; break;
  case EditorTargetType_Tile: result = target->tile.pos; break;
  
  invalid_default_case;
 }
 
 return(result);
}

internal void
zero_editor_target_if_on_pos(EditorTarget *target, v2u pos)
{
 if(target->type)
 {
  v2u target_pos = get_editor_target_pos(target);
  if(is_v2u_equal(target_pos, pos))
  {
   zero_editor_target(target);
  }
 }
}

internal void
delete_editor_target(Random *random,
                     EntityState *entity_state,
                     Dungeon *dungeon,
                     v2u mouse_tile_pos,
                     EditorTarget *target)
{
 Entity *entity = get_dungeon_pos_entity(dungeon->tiles, mouse_tile_pos);
 if(entity && entity->type != EntityType_Player)
 {
  zero_editor_target_if_on_pos(target, entity->pos);
  remove_entity(entity, dungeon->tiles);
 }
 else
 {
  DungeonTrap *trap = get_dungeon_pos_trap(&dungeon->traps, mouse_tile_pos);
  if(trap)
  {
   zero_editor_target_if_on_pos(target, trap->pos);
   remove_dungeon_trap(trap);
  }
  else
  {
   EditorDungeonTile *tile = &target->tile;
   if(tile->id)
   {
     zero_editor_target_if_on_pos(target, mouse_tile_pos);
    set_dungeon_pos_floor(random, dungeon->tiles, mouse_tile_pos);
   }
   else
   {
    // Set non selected dungeon tile as floor
    if(!is_dungeon_pos_floor(dungeon->tiles, mouse_tile_pos))
    {
     set_dungeon_pos_floor(random, dungeon->tiles, mouse_tile_pos);
    }
   }
   }
 }
}

internal void
set_editor_target(EditorMode *editor,
                            EntityState *entity_state,
                            Dungeon *dungeon,
                            v2u mouse_tile_pos)
{
 EditorTarget *target = &editor->active_target;
 
 Entity *entity = get_dungeon_pos_entity(dungeon->tiles, mouse_tile_pos);
 if(entity)
 {
  target->type = EditorTargetType_Entity;
  target->entity = entity;
 }
 else
 {
  DungeonTrap *trap = get_dungeon_pos_trap(&dungeon->traps, mouse_tile_pos);
  if(trap)
  {
   target->type = EditorTargetType_Trap;
   target->trap = trap;
  }
  else if(!is_dungeon_pos_floor(dungeon->tiles, mouse_tile_pos))
  {
   target->type = EditorTargetType_Tile;
   target->tile.pos = mouse_tile_pos;
   target->tile.id = get_dungeon_pos_tile_id(dungeon->tiles, mouse_tile_pos);
  }
 }
}

internal void
update_editor_target(Random *random,
                     EntityState *entity_state,
                             Dungeon *dungeon,
                             v2u mouse_tile_pos,
                     EditorTarget *target,
                        EditorMode *editor)
{
 EditorDungeonTile *tile = &target->tile;
 
 if(target->entity)
 {
  Entity *entity = get_dungeon_pos_entity(dungeon->tiles, mouse_tile_pos);
  if(entity)
  {
   zero_editor_target_if_on_pos(target, entity->pos);
  }
  else
  {
   entity_move_force(target->entity, dungeon, mouse_tile_pos, dungeon->level);
  }
 }
 else if(target->trap)
 {
  DungeonTrap *trap = get_dungeon_pos_trap(&dungeon->traps, mouse_tile_pos);
  if(trap)
  {
   zero_editor_target_if_on_pos(target, trap->pos);
  }
  else
  {
   target->trap->pos = mouse_tile_pos;
  }
 }
 else if(tile->id)
 {
  if(is_v2u_equal(tile->pos, mouse_tile_pos))
  {
   zero_editor_target_if_on_pos(target, tile->pos);
   }
  else
  {
   set_dungeon_pos_floor(random, dungeon->tiles, tile->pos);
   set_dungeon_pos_tile(dungeon->tiles, mouse_tile_pos, tile->id);
   tile->pos = mouse_tile_pos;
  }
 }
 }

internal void
deselect_editor_mark(EditorGroup *groups)
                            {
 for(u32 group_index = 0; group_index < MAX_EDITOR_GROUP_COUNT; ++group_index)
 {
  EditorGroup *group = &groups[group_index];
  if(group->id &&
     group->is_searchable)
  {
   Mark *mark = &group->mark;
   if(mark->is_active)
   {
   deselect_mark(mark);
   break;
   }
  }
 }
 }
                            
internal Mark *
get_editor_active_mark(EditorGroup *groups)
{
 Mark *result = 0;
 
 for(u32 group_index = 0; group_index < MAX_EDITOR_GROUP_COUNT; ++group_index)
 {
  EditorGroup *group = &groups[group_index];
  if(group->id &&
     group->is_searchable &&
     group->mark.is_active)
  {
   result = &group->mark;
   break;
  }
 }
 
 return(result);
}

internal b32
update_editor_mark(Input *input, EditorMode *editor, v2u mouse_pos)
{
 b32 result = false;
 
 for(u32 group_index = 0; group_index < MAX_EDITOR_GROUP_COUNT; ++group_index)
 {
  EditorGroup *group = &editor->groups[group_index];
  if(group->id &&
     group->is_searchable)
  {
   Mark *mark = &group->mark;
   
   if(mark->is_active)
   {
    if(!is_pos_inside_rect(mark->input_rect, mouse_pos))
       {
     deselect_mark(mark);
     break;
       }
   }
   else if(is_pos_inside_rect(mark->input_rect, mouse_pos))
   {
    if(is_mark_array_valid(mark->array))
    {
     set_mark_cursor_at_end(mark);
    }
    else
    {
     set_mark_cursor_at_start(mark);
    }
    
    zero_editor_target(&editor->active_target);
    if(get_editor_source(editor))
    {
     zero_editor_active_source_info(&editor->active_source_info);
    }
    
    mark->is_active = true;
    result = true;
    break;
   }
   }
 }
 
 return(result);
}

internal void
update_editor_mode(Random *random,
              EditorMode *editor,
              Input *input,
              EntityState *entity_state,
              Dungeon *dungeon)
{
 if(editor->is_open)
 {
  b32 updated_mark_input = false;
  Mark *mark = get_editor_active_mark(editor->groups);
  if(mark)
  {
   updated_mark_input = update_mark_input(input, mark);
  }
  
  if(!updated_mark_input)
  {
   EditorTarget *target = &editor->active_target;
   
  if(was_pressed(&input->Button_Left))
  {
   b32 updated_mark = update_editor_mark(input, editor, input->mouse_pos);
   if(!updated_mark)
   {
    b32 updated_sources = update_editor_sources(editor, input->mouse_pos);
    if(!updated_sources)
     {
      EditorSource *source = get_editor_source(editor);
      if(source)
     {
      place_editor_source(random, entity_state, dungeon, input->mouse_tile_pos, source);
     }
     else if(target->type)
     {
      update_editor_target(random, entity_state, dungeon, input->mouse_tile_pos, target, editor);
     }
     else
     {
      set_editor_target(editor, entity_state, dungeon, input->mouse_tile_pos);
     }
    }
    }
   }
  else if(was_pressed(&input->Button_Right))
  {
   delete_editor_target(random, entity_state, dungeon, input->mouse_tile_pos, target);
  }
  }
 }
 }

internal void
render_editor_mode(SDL_Renderer *renderer,
                   v4s camera,
                   Input *input,
                   EditorMode *editor,
                   Texture tileset)
{
 if(editor->is_open)
 {
  // Render group sources
  for(u32 group_index = 0; group_index < MAX_EDITOR_GROUP_COUNT; ++group_index)
  {
   EditorGroup *group = &editor->groups[group_index];
   if(group->id)
   {
    v2u name_pos =
    {
     tile_mul(group->start_pos.x),
     tile_mul(group->start_pos.y)
    };
    
    name_pos.y -= editor->font->size;
    render_string(renderer, group->name, &name_pos, editor->font);
    
    if(group->is_searchable)
    {
     Mark *mark = &group->mark;
     
     v2u mark_input_pos =
     {
      name_pos.x + get_text_width(group->name, editor->font, false) + editor->font->size,
      name_pos.y - 8
     };
     update_and_render_mark_input(renderer, editor->font, mark, mark_input_pos, 0);
     }
    
    // Render sources
    for(u32 source_index = 0; source_index < MAX_EDITOR_SOURCE_COUNT; ++source_index)
    {
     EditorSource *source = &group->sources[source_index];
     if(source->type)
     {
      b32 half_color = false;
      if(source->name)
      {
       if(is_mark_array_valid(group->mark.array) &&
          !string_has_string(source->name, group->mark.array, false))
       {
        half_color = true;
       }
       }
      
      render_texture(renderer, tileset.tex, &source->tile_src, &source->tile_dest, false, half_color);
      }
    }
    
    // Render active source
    EditorSource *source = get_editor_source(editor);
    if(source)
    {
     render_outline_rect(renderer, source->tile_dest, Color_Green);
    }
    }
  }
  
  
  // Render target highlight
  EditorTarget *target = &editor->active_target;
  
  #if 0
  printf("type: %u\n", target->type);
  printf("entity: %p\n", target->entity);
  printf("trap: %p\n", target->trap);
  printf("tile.pos: %u, %u\n", target->tile.pos.x, target->tile.pos.y);
  printf("tile.id: %u\n\n", target->tile.id);
  #endif
  
  if(target->type)
  {
   v2u target_pos = get_editor_target_pos(target);
   v4u rect = get_game_dest(camera, target_pos);
   render_outline_rect(renderer, rect, Color_Green);
   }
 }
}