internal DebugVariable *
get_debug_context_root(DebugState *debug, DebugContextType type)
{
    DebugVariable *result = debug->contexts[type].root;
    return(result);
}
internal DebugContext *
get_debug_context(DebugState *debug, DebugContextType type)
{
    DebugContext *result = &debug->contexts[type];
    return(result);
}

internal DebugVariable *
get_next_debug_variable(DebugVariable *var)
{
    if(var->type == DebugVariableType_Group)
    {
        var = var->group.first_child;
    }
    else
    {
        while(var)
        {
            if(var->next)
            {
                var = var->next;
                break;
            }
            else
            {
                var = var->parent_group;
            }
        }
    }
    
    return(var);
}

internal u32
get_debug_root_variable_count(DebugState *debug, DebugContextType type)
{
    u32 result = 0;
    
    DebugVariable *root = get_debug_context_root(debug, type);
    DebugVariable *var = root->group.first_child;
    
    while(var)
    {
        ++result;
        
        var = get_next_debug_variable(var);
    }
    
    return(result);
}

internal void
zero_debug_root_variables(DebugState *debug, DebugContextType type)
{
    // Pop all the variables after root
    u32 var_count = get_debug_root_variable_count(debug, type);
    pop_memory_(&debug->memory_arena, sizeof(DebugVariable) * var_count);
    
    // Zero the root so when a new one is created it will replace this one
    zero_struct(*get_debug_context_root(debug, type));
}

#define add_debug_variable(context_type, name, value, variable_type) add_debug_variable_(&game->debug, context_type, name, &value, 0, variable_type)
internal DebugVariable *
add_debug_variable_(DebugState *debug, DebugContextType context_type, char *name, void *value, Color color, DebugVariableType variable_type)
{
    DebugVariable *var = push_memory_struct(&debug->memory_arena, DebugVariable);
    var->type = variable_type;
    strcpy(var->name, name);
    var->color = color;
    
    var->next = 0;
    
    switch(var->type)
    {
        case DebugVariableType_Newline: break;
        case DebugVariableType_Group: break;
        case DebugVariableType_Text: break;
        
        case DebugVariableType_U32: var->u32 = (u32 *)value; break;
        case DebugVariableType_B32: var->b32 = (b32 *)value; break;
        case DebugVariableType_F32: var->f32 = (f32 *)value; break;
        case DebugVariableType_V2U: var->v2u = (v2u *)value; break;
        
        invalid_default_case;
    }
    
    DebugVariable *root = get_debug_context_root(debug, context_type);
    var->parent_group = root;
    
    if(root)
    {
        if(root->group.last_child)
        {
            // Add to the end of list
            root->group.last_child->next = var;
            root->group.last_child = var;
            }
        else
        {
            // Set the first entry in the list
            root->group.first_child = var;
            root->group.last_child = var;
        }
    }
    
    return(var);
}

#define add_debug_text(text, color, ...) add_debug_text_(&game->debug, text, color, ##__VA_ARGS__)
internal void
add_debug_text_(DebugState *debug, DebugContextType type, char *text, Color color, ...)
{
    String128 text_final = {0};
    
    va_list arg_list;
    va_start(arg_list, color);
    vsnprintf(text_final.str, sizeof(text_final), text, arg_list);
    va_end(arg_list);
    
    add_debug_variable_(debug, type, text_final.str, 0, color, DebugVariableType_Text);
}

internal void
add_debug_newline(DebugState *debug, DebugContextType type)
{
    add_debug_variable_(debug, type, "", 0, 0, DebugVariableType_Newline);
}

internal DebugVariable *
start_debug_group(DebugState *debug, DebugContextType type, char *name, b32 is_expanded)
{
    DebugVariable *group = add_debug_variable_(debug, type, name, 0, 0, DebugVariableType_Group);
    
    group->group.is_expanded = is_expanded;
    
    group->group.first_child = 0;
    group->group.last_child = 0;
    
    debug->contexts[type].root = group;
    return(group);
}

internal void
end_debug_group(DebugState *debug, DebugContextType type)
{
    assert(debug->contexts[type].root);
    
    debug->contexts[type].root = debug->contexts[type].root->parent_group;
}

internal void
set_debug_root(DebugState *debug, DebugContextType type, u32 x, u32 y)
{
    DebugContext *context = get_debug_context(debug, type);
    context->pos = make_v2u(x, y);
    
    switch(type)
    {
        case DebugContextType_Default: context->root = start_debug_group(debug, type, "Default Group", true); break;
        case DebugContextType_Active: context->root = start_debug_group(debug, type, "Active Group", true); break;
        
        invalid_default_case;
    }
}

internal void
process_debug_state(Game *game,
                    Input *input,
                    EntityState *entities,
                    ItemState *items,
                    DungeonState *dungeons)
{
    DebugState *debug = &game->debug;
    
    debug->hot.var = 0;
    
    for(DebugContextType type = DebugContextType_None + 1;
            type < DebugContextType_Count;
            ++type)
    {
        u32 var_count = 0;
        u32 group_depth = 0;
        
        DebugContext *context = get_debug_context(debug, type);
        DebugVariable *var = context->root->group.first_child;
        while(var)
        {
            // Set text
            String128 text = {0};
            
            switch(var->type)
            {
                case DebugVariableType_Newline: break;
                case DebugVariableType_Group: snprintf(text.str, sizeof(text), "%s %s", var->group.is_expanded ? "-" : "+", var->name); break;
                case DebugVariableType_Text: snprintf(text.str, sizeof(text), "%s", var->name); break;
                
                case DebugVariableType_U32: snprintf(text.str, sizeof(text), "%s: %u", var->name, *var->u32); break;
                case DebugVariableType_B32: snprintf(text.str, sizeof(text), "%s: %s", var->name, (*var->b32 == true) ? "true" : "false"); break;
                case DebugVariableType_F32: snprintf(text.str, sizeof(text), "%s: %.02f", var->name, *var->f32); break;
                case DebugVariableType_V2U: snprintf(text.str, sizeof(text), "%s: %u, %u", var->name, var->v2u->x, var->v2u->y); break;
                
                invalid_default_case;
            }
            
            assert(debug->x_offset);
            assert(debug->y_offset);
            
            // Render text
            v4u text_rect =
            {
                context->pos.x + (group_depth * debug->x_offset),
                context->pos.y + (var_count * debug->y_offset),
            };
            
            Font *font = debug->font;
            assert(font->type == FontType_TTF);
            text_rect.w += get_text_width(font, text.str);
            text_rect.h = font->size;
            
            Color text_color = Color_White;
            if(is_pos_inside_rect(text_rect, input->mouse_pos))
            {
                debug->hot.var = var;
                text_color = Color_LightBlue;
            }
            
            // If the variable has a color value, we use it
            // If instead it's 0, we use defaults
            if(var->color)
            {
                text_color = var->color;
            }
            
#if 0
            printf("%s (count: %u, depth: %u)\n", var->name, var_count, group_depth);
            printf("%u, %u\n\n", text_rect.x, text_rect.y);
#endif
            
            render_text(game, "%s%s", text_rect.x, text_rect.y, font, 0, start_color(text_color), text.str);
            
            // Get next variable
            if(var->type == DebugVariableType_Group && var->group.is_expanded)
            {
                var = var->group.first_child;
                ++group_depth;
            }
            else
            {
                while(var)
                {
                    if(var->next)
                    {
                        var = var->next;
                        break;
                    }
                    else
                    {
                        var = var->parent_group;
                        --group_depth;
                    }
                }
            }
            
            ++var_count;
        }
    }
    
    // Process debug interactions
        if(was_pressed(&input->Button_Left))
    {
        DebugVariable *hot_var = debug->hot.var;
        if(hot_var)
        {
            switch(hot_var->type)
            {
                case DebugVariableType_Newline: break;
                case DebugVariableType_Group: hot_var->group.is_expanded = !hot_var->group.is_expanded; break;
                case DebugVariableType_Text: break;
                
                case DebugVariableType_U32: break;
                case DebugVariableType_B32: break;
                case DebugVariableType_F32: break;
                case DebugVariableType_V2U: break;
                
                invalid_default_case;
            }
        }
        else
        {
            // DebugContextType_Active root should only have one group
            DebugContextType type = DebugContextType_Active;
            zero_debug_root_variables(debug, type);
            
             DebugHot *hot = &debug->hot;
            
            Dungeon *dungeon = get_dungeon_from_level(dungeons, dungeons->current_level);
            Entity *entity = get_entity_on_pos(entities, dungeon->level, input->mouse_tile_pos);
            if(entity)
            {
                hot->type = DebugHotType_Entity;
                hot->pos = entity->pos;
                hot->entity = entity;
                
                start_debug_group(debug, type, "Entity", true);
                {
                    add_debug_text(type, "%s", Color_White, "Entity Data");
                }
                end_debug_group(debug, type);
                
                return;
            }
            
            Item *item = get_item_on_pos(items, dungeon->level, input->mouse_tile_pos, 0);
            if(item)
            {
                hot->type = DebugHotType_Item;
                hot->pos = item->pos;
                hot->item = item;
                
                start_debug_group(debug, type, "Item", true);
                {
                    add_debug_text(type, "%s", Color_White, "Item Data");
                }
                end_debug_group(debug, type);
                
                return;
            }
            }
    }
}