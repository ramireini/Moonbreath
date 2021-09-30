#define add_editor_source_tile(group, source_type, tile_id) add_editor_source(group, source_type, tile_id, 0, 0)
#define add_editor_source_trap(group, trap_type) add_editor_source(group, EditorSourceType_Trap, 0, trap_type, 0)
#define add_editor_source_entity(group, entity_id) add_editor_source(group, EditorSourceType_Entity, 0, 0, entity_id)
internal void
add_editor_source(EditorGroup *group,
                  EditorSourceType type,
                  DungeonTileID tile_id,
                  DungeonTrapType trap_type,
                  EntityID entity_id)
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
reset_editor_active_source(EditorMode *editor)
{
 editor->active_group_id = 0;
 editor->active_source_id = 0;
}

internal EditorSource *
get_editor_active_source(EditorMode *editor)
{
 u32 group_index = editor->active_group_id - 1;
 EditorGroup *group = &editor->groups[group_index];
 
 u32 source_index = editor->active_source_id - 1;
 EditorSource *result = &group->sources[source_index];
 
 return(result);
 }

internal EditorGroup *
add_editor_group(EditorMode *editor, char *name, u32 x, u32 y, u32 sources_per_row)
{
 for(u32 index = 0; index < MAX_EDITOR_GROUP_COUNT; ++index)
 {
  EditorGroup *group = &editor->groups[index];
  if(!group->id)
  {
   group->id = index + 1;
   group->name = name;
   
   group->pos.x = x;
   group->pos.y = y;
   
   group->start_pos = group->pos;
   group->sources_per_row = sources_per_row;
   
   return(group);
  }
 }
 
 assert(0);
}

internal b32
update_editor_active_source(EditorMode *editor, v2u mouse_pos)
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
     // Deselect if what we clicked is already active.
     if(editor->active_group_id == group->id &&
        editor->active_source_id == source->id)
     {
      reset_editor_active_source(editor);
     }
     else
     {
      editor->active_group_id = group->id;
      editor->active_source_id = source->id;
     }
     
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
place_editor_active_source(Random *random,
                           EntityState *entity_state,
                           Dungeon *dungeon,
                           v2u mouse_tile_pos,
                           EditorSource *active)
{
 switch(active->type)
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
  
  case EditorSourceType_Trap:
  {
   assert(active->subtype);
   
   if(!get_dungeon_pos_trap(&dungeon->traps, mouse_tile_pos))
   {
    add_dungeon_trap(&dungeon->spec, &dungeon->traps, active->subtype, mouse_tile_pos);
   }
  } break;
  
  case EditorSourceType_Entity:
  {
   assert(active->subtype);
   
   if(!get_dungeon_pos_entity(entity_state, dungeon->level, mouse_tile_pos))
   {
    add_enemy_entity(entity_state, dungeon, active->subtype, mouse_tile_pos.x, mouse_tile_pos.y);
   }
  } break;
  
  invalid_default_case
 }
}

internal void
delete_editor_target(Random *random, EntityState *entity_state, Dungeon *dungeon, v2u mouse_tile_pos)
{
 Entity *entity = get_dungeon_pos_entity(entity_state, dungeon->level, mouse_tile_pos);
 if(entity && entity->type != EntityType_Player)
 {
  remove_entity(entity, dungeon->tiles);
 }
 else
 {
  DungeonTrap *trap = get_dungeon_pos_trap(&dungeon->traps, mouse_tile_pos);
  if(trap)
  {
   remove_dungeon_trap(trap);
  }
  else if(!is_dungeon_pos_floor(dungeon->tiles, mouse_tile_pos))
  {
   set_dungeon_pos_floor(random, dungeon->tiles, mouse_tile_pos);
  }
 }
}

internal void
editor_start_selection_move(EditorMode *editor,
                            EntityState *entity_state,
                            Dungeon *dungeon,
                            v2u mouse_tile_pos)
{
 Entity *entity = get_dungeon_pos_entity(entity_state, dungeon->level, mouse_tile_pos);
 if(entity)
 {
  printf("Editor: Make Selection (Entity)\n");
  
  editor->selection.is_set = true;
  editor->selection.entity = entity;
 }
 else
 {
  DungeonTrap *trap = get_dungeon_pos_trap(&dungeon->traps, mouse_tile_pos);
  if(trap)
  {
   printf("Editor: Make Selection (Trap)\n");
   
   editor->selection.is_set = true;
   editor->selection.trap = trap;
  }
 }
}

internal void
editor_zero_selection(EditorSelection *selection)
{
 zero_struct(*selection);
}

internal void
editor_update_selection_move(EntityState *entity_state,
                             Dungeon *dungeon,
                             v2u mouse_tile_pos,
                             EditorSelection *selection)
{
 if(selection->entity)
 {
  Entity *entity = get_dungeon_pos_entity(entity_state, dungeon->level, mouse_tile_pos);
  if(entity)
  {
   if(is_v2u_equal(selection->entity->pos, entity->pos))
   {
    //printf("Editor: Zero Selection (Entity)\n");
    editor_zero_selection(selection);
   }
  }
  else
  {
   //printf("Editor: Move Selection (Entity)\n");
   entity_move_force(selection->entity, dungeon, mouse_tile_pos, dungeon->level);
  }
 }
 else if(selection->trap)
 {
  DungeonTrap *trap = get_dungeon_pos_trap(&dungeon->traps, mouse_tile_pos);
  if(trap)
  {
   if(is_v2u_equal(selection->trap->pos, trap->pos))
   {
    //printf("Editor: Zero Selection (Trap)\n");
    editor_zero_selection(selection);
   }
  }
  else
  {
   //printf("Editor: Move Selection (Trap)\n");
   selection->trap->pos = mouse_tile_pos;
  }
 }
 }

internal void
update_editor_mode(Random *random,
              EditorMode *editor,
              Input *input,
              EntityState *entity_state,
              Dungeon *dungeon,
              b32 left_click)
{
 if(editor->is_open)
 {
  EditorSource *active_source = 0;
  if(editor->active_group_id && editor->active_source_id)
  {
   active_source = get_editor_active_source(editor);
  }
  
  if(left_click)
  {
   if(!update_editor_active_source(editor, input->mouse_pos))
   {
    if(active_source)
    {
     place_editor_active_source(random, entity_state, dungeon, input->mouse_tile_pos, active_source);
    }
    else if(editor->selection.is_set)
    {
     editor_update_selection_move(entity_state, dungeon, input->mouse_tile_pos, &editor->selection);
    }
    else
    {
     editor_start_selection_move(editor, entity_state, dungeon, input->mouse_tile_pos);
     }
   }
   }
  else // Right click
  {
   delete_editor_target(random, entity_state, dungeon, input->mouse_tile_pos);
  }
 }
 else
 {
  reset_editor_active_source(editor);
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
    
    for(u32 source_index = 0; source_index < MAX_EDITOR_SOURCE_COUNT; ++source_index)
    {
     EditorSource *source = &group->sources[source_index];
     if(source->type)
     {
      render_texture(renderer, tileset.tex, &source->tile_src, &source->tile_dest, false, false);
      
      // Render active source
      if(editor->active_source_id)
      {
       EditorSource *active_source = get_editor_active_source(editor);
       render_outline_rect(renderer, active_source->tile_dest, Color_Green);
      }
     }
    }
   }
  }
  
  // Render selection highlight
  EditorSelection *selection = &editor->selection;
  if(selection->is_set)
  {
   v2u selection_pos = {0};
   if(selection->entity)
   {
    selection_pos = selection->entity->pos;
   }
   else if(selection->trap)
   {
    selection_pos = selection->trap->pos;
   }
   
   v4u rect = get_game_dest(camera, selection_pos);
   render_outline_rect(renderer, rect, Color_Green);
   }
 }
}