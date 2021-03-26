internal b32
is_var_group(DebugVariableType type)
{
    b32 result = (type == DebugVariableType_Group);
    return(result);
}

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
    if(is_var_group(var->type))
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
zero_debug_context_variables(DebugState *debug, DebugContextType type)
{
    // Pop all the variables after root
    u32 var_count = get_debug_root_variable_count(debug, type);
    pop_memory_(&debug->memory_arena, sizeof(DebugVariable) * var_count);
    
    // Zero the root so when a new one is created it will replace this one
    zero_struct(*get_debug_context_root(debug, type));
}

#define add_debug_variable(context_type, name, value, variable_type) add_debug_variable_(&game->debug, context_type, name, &value, 0, 0, 0, variable_type)
internal DebugVariable *
add_debug_variable_(DebugState *debug,
                    DebugContextType context_type,
                    char *name,
                    void *value,
                    Color color,
                    char *enum_to_string_callback(u32),
                    u32 *flags,
                    DebugVariableType variable_type)
{
    DebugVariable *var = push_memory_struct(&debug->memory_arena, DebugVariable);
    
    var->type = variable_type;
    strcpy(var->name, name);
    var->color = color;
    
    var->enum_to_string_callback = enum_to_string_callback;
    var->flags = flags;
    
    var->next = 0;
    
    switch(var->type)
    {
        case DebugVariableType_Newline: break;
        case DebugVariableType_Group: break;
        case DebugVariableType_Text: break;
        
        case DebugVariableType_S32: var->s32 = (s32 *)value; break;
        case DebugVariableType_U32: var->u32 = (u32 *)value; break;
        case DebugVariableType_B32: var->b32 = (b32 *)value; break;
        case DebugVariableType_F32: var->f32 = (f32 *)value; break;
        case DebugVariableType_V2U: var->v2u = (v2u *)value; break;
        case DebugVariableType_V4U: var->v4u = (v4u *)value; break;
        case DebugVariableType_String: var->string = (char *)value; break;
        case DebugVariableType_Enum: var->u32 = (u32 *)value; break;
        case DebugVariableType_Flag: var->flag = *(u32 *)value; break;
        
        invalid_default_case;
            };
    
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

#define add_debug_flag(type, name, flags, flag) add_debug_flag_(debug, type, name, &flags, flag)
internal void
add_debug_flag_(DebugState *debug, DebugContextType type, char *name, u32 *flags, u32 flag)
{
    add_debug_variable_(debug, type, name, &flag, 0, 0, flags, DebugVariableType_Flag);
}

#define add_debug_enum(type, name, value, enum_to_string_callback) add_debug_enum_(debug, type, name, (u32 *)&value, enum_to_string_callback)
internal void
add_debug_enum_(DebugState *debug,
               DebugContextType type,
               char *name,
               u32 *value,
               char *enum_to_string_callback(u32))
{
    add_debug_variable_(debug, type, name, value, 0,
                        enum_to_string_callback, 0, DebugVariableType_Enum);
}

internal void
add_debug_newline(DebugState *debug, DebugContextType type)
{
    add_debug_variable_(debug, type, "", 0, 0, 0, 0, DebugVariableType_Newline);
}

internal DebugVariable *
start_debug_group(DebugState *debug, DebugContextType type, char *name, b32 is_expanded)
{
    DebugVariable *group = add_debug_variable_(debug, type, name, 0, 0, 0, 0, DebugVariableType_Group);
    
    group->group.is_expanded = is_expanded;
    
    group->group.first_child = 0;
    group->group.last_child = 0;
    
    debug->contexts[type].root = group;
    return(group);
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
    
    add_debug_variable_(debug, type, text_final.str, 0, color, 0, 0, DebugVariableType_Text);
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
    assert(type > DebugContextType_None);
    assert(type < DebugContextType_Count);
    
    DebugContext *context = get_debug_context(debug, type);
    
    context->pos = make_v2u(x, y);
    context->move_rect_size = 8;
    context->move_rect.w = context->move_rect_size;
    context->move_rect.h = context->move_rect_size;
    
    context->group_move_color_active = Color_Yellow;
    context->group_move_color_inactive = Color_White;
    
    context->group_text_color_active = Color_White;
    context->group_text_color_inactive = Color_AlmostWhite;
    
    context->text_color_active = Color_Yellow;
    
    context->root = start_debug_group(debug, type, "Group", true);
}

internal void
update_and_render_debug_state(Game *game,
                    Input *input,
                    EntityState *entities,
                    ItemState *items,
                    DungeonState *dungeons)
{
    DebugState *debug = &game->debug;
    
    debug->hot.type = DebugHotType_None;
    debug->hot.var = 0;
    
    DebugInteraction new_interaction = {0};
    
    // TODO(rami): Add debug hot item data
    
    // TODO(rami): In the future, it would be cool to be able to push new debug contexts
    // instead of having a set amount.
    
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
                
                case DebugVariableType_Group:
                case DebugVariableType_Text: snprintf(text.str, sizeof(text), "%s", var->name); break;
                
                case DebugVariableType_S32: snprintf(text.str, sizeof(text), "%s: %d", var->name, *var->s32); break;
                case DebugVariableType_U32: snprintf(text.str, sizeof(text), "%s: %u", var->name, *var->u32); break;
                case DebugVariableType_B32: snprintf(text.str, sizeof(text), "%s: %s", var->name, (*var->b32 == true) ? "true" : "false"); break;
                case DebugVariableType_F32: snprintf(text.str, sizeof(text), "%s: %.01f", var->name, *var->f32); break;
                case DebugVariableType_V2U: snprintf(text.str, sizeof(text), "%s: %u, %u", var->name, var->v2u->x, var->v2u->y); break;
                case DebugVariableType_V4U: snprintf(text.str, sizeof(text), "%s: %u, %u, %u, %u", var->name, var->v4u->x, var->v4u->y, var->v4u->w, var->v4u->h); break;
                case DebugVariableType_String: snprintf(text.str, sizeof(text), "%s: %s", var->name, var->string); break;
                
                case DebugVariableType_Enum:
                {
                    if(var->enum_to_string_callback)
                    {
                        snprintf(text.str, sizeof(text), "%s: %s",
                                     var->name, var->enum_to_string_callback(*var->u32));
                    }
                    else
                    {
                        snprintf(text.str, sizeof(text), "%s: %u", var->name, *var->u32);
                    }
                } break;
                
                case DebugVariableType_Flag: snprintf(text.str, sizeof(text), "%s: %s", var->name, is_set(*var->flags, var->flag) ? "true" : "false"); break;
                
                invalid_default_case;
            }
            
            // Set move rect
            Color move_rect_color = context->group_move_color_inactive;
            if(context->is_moving)
            {
                move_rect_color = context->group_move_color_active;
            }
            
            context->move_rect.x = context->pos.x + (group_depth * debug->text_offset.x);
            context->move_rect.y = context->pos.y + (var_count * debug->text_offset.y);
            
            // Set text rect
            Font *font = debug->font;
            assert(font->type == FontType_TTF);
            
            Color text_background_color = Color_DebugBackgroundDark;
            Color text_rect_color = context->group_text_color_inactive;
            if(var->group.is_expanded)
            {
                text_background_color = Color_DebugBackgroundLight;
                text_rect_color = context->group_text_color_active;
            }
            
            v4u text_rect =
            {
                context->move_rect.x,
                context->move_rect.y,
                get_text_width(font, text.str),
                font->size
            };
            
            // This offsets the text from the move rect
            if(is_var_group(var->type))
            {
                text_rect.x += 24;
                text_rect.y -= (text_rect.h / 2) - (context->move_rect_size / 2);
            }
            
            // Set interaction
            if(is_pos_inside_rect(context->move_rect, input->mouse_pos))
            {
                new_interaction.type = DebugInteractionType_Move;
                new_interaction.context = context;
                
                move_rect_color = context->group_move_color_active;
            }
            else if(is_pos_inside_rect(text_rect, input->mouse_pos))
            {
                debug->hot.var = var;
                
                if(is_var_group(var->type))
                {
                    text_background_color = Color_DebugBackgroundLight;
                    text_rect_color = context->group_text_color_active;
                }
                else
                {
                    text_rect_color = context->text_color_active;
                }
            }
            
            if(is_var_group(var->type))
            {
                // Render move rect
                render_fill_rect(game, context->move_rect, move_rect_color);
                
                // Render text background
                
                v2u text_background_offset = {6, 1};
                v4u text_background_rect =
                {
                    text_rect.x - text_background_offset.x,
                    text_rect.y - text_background_offset.y,
                    text_rect.w + (text_background_offset.x * 1.5f),
                    text_rect.h + (text_background_offset.y * 1.5f)
                };
                
                render_fill_rect(game, text_background_rect, text_background_color);
            }
            
            // Render text
            if(var->color)
            {
                text_rect_color = var->color;
            }
            
            render_text(game, "%s%s", text_rect.x, text_rect.y, font, 0,
                        start_color(text_rect_color),
                            text.str);
            
            // Get next variable
            if(is_var_group(var->type) && var->group.is_expanded)
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
    
    DebugVariable *hot_var = debug->hot.var;
    DebugInteraction *hot_interaction = &debug->hot_interaction;
    
    if(was_pressed(&input->Button_Left))
    {
        if(new_interaction.type)
        {
            // Start debug interaction
            
            switch(new_interaction.type)
            {
                case DebugInteractionType_Move:
                {
                    new_interaction.pos = &input->mouse_pos;
                    debug->hot_interaction = new_interaction;
                } break;
                
                invalid_default_case;
            }
        }
        else if(hot_var)
        {
                switch(hot_var->type)
                {
                    case DebugVariableType_Newline: break;
                    case DebugVariableType_Group: hot_var->group.is_expanded = !hot_var->group.is_expanded; break;
                    case DebugVariableType_Text: break;
                    
                    case DebugVariableType_S32: break;
                    case DebugVariableType_U32: break;
                    case DebugVariableType_B32: break;
                    case DebugVariableType_F32: break;
                    case DebugVariableType_V2U: break;
                    case DebugVariableType_V4U: break;
                    case DebugVariableType_String: break;
                    case DebugVariableType_Enum: break;
                    case DebugVariableType_Flag: break;
                    
                    invalid_default_case;
                }
        }
        else
        {
            DebugContextType type = DebugContextType_Hot;
            zero_debug_context_variables(debug, type);
            
            DebugHot *hot = &debug->hot;
            Dungeon *dungeon = get_dungeon_from_level(dungeons, dungeons->current_level);
            
            Entity *entity = get_entity_on_pos(entities, dungeon->level, input->mouse_tile_pos);
            if(entity)
            {
                hot->type = DebugHotType_Entity;
                hot->pos = entity->pos;
                hot->generic = entity;
                
                start_debug_group(debug, type, "Entity", true);
                {
                    add_debug_newline(debug, type);
                    
                    start_debug_group(debug, type, "Flags", false);
                    {
                        add_debug_flag(type, "Notify About Multiple Items", entity->flags, EntityFlags_NotifyAboutMultipleItems);
                        add_debug_flag(type, "Uses Physical Attacks", entity->flags, EntityFlags_UsesPhysicalAttacks);
                        add_debug_flag(type, "Uses Ranged Attacks", entity->flags, EntityFlags_UsesRangedAttacks);
                        add_debug_flag(type, "Uses Magic Attacks", entity->flags, EntityFlags_UsesMagicAttacks);
                        add_debug_flag(type, "Has Been Seen", entity->flags, EntityFlags_HasBeenSeen);
                        add_debug_flag(type, "Is Flipped", entity->flags, EntityFlags_IsFlipped);
                        add_debug_flag(type, "In Combat", entity->flags, EntityFlags_InCombat);
                        add_debug_flag(type, "Is Pathfinding", entity->flags, EntityFlags_IsPathfinding);
                        add_debug_flag(type, "Is Ghost Enabled", entity->flags, EntityFlags_IsGhostEnabled);
                        add_debug_flag(type, "Is Ghost Flipped", entity->flags, EntityFlags_IsGhostFlipped);
                        add_debug_flag(type, "Is Invisible", entity->flags, EntityFlags_IsInvisible);
                    }
                    end_debug_group(debug, type);
                    add_debug_newline(debug, type);
                    
                    add_debug_variable(type, "ID", entity->id, DebugVariableType_U32);
                    add_debug_enum(type, "Type", entity->type, get_entity_type_text);
                    add_debug_variable(type, "Name", entity->name, DebugVariableType_String);
                    add_debug_variable(type, "Max HP", entity->max_hp, DebugVariableType_U32);
                    add_debug_variable(type, "HP", entity->hp, DebugVariableType_U32);
                    add_debug_newline(debug, type);
                    
                    add_debug_variable(type, "Dungeon Level", entity->dungeon_level, DebugVariableType_U32);
                    add_debug_variable(type, "New Pos", entity->new_pos, DebugVariableType_V2U);
                    add_debug_variable(type, "Pos", entity->pos, DebugVariableType_V2U);
                    add_debug_enum(type, "New Direction", entity->new_direction, get_direction_string);
                    
                    add_debug_newline(debug, type);
                    
                    add_debug_variable(type, "Width", entity->width, DebugVariableType_U32);
                    add_debug_variable(type, "Height", entity->height, DebugVariableType_U32);
                    add_debug_variable(type, "Tile Source", entity->tile_src, DebugVariableType_V4U);
                    add_debug_enum(type, "Remains", entity->remains, get_entity_remains_text);
                    add_debug_variable(type, "pathfind Target", entity->pathfind_target_pos, DebugVariableType_V2U);
                    add_debug_newline(debug, type);
                    
                    start_debug_group(debug, type, "Stats", false);
                    {
                        add_debug_variable(type, "FOV", entity->fov, DebugVariableType_U32);
                        add_debug_variable(type, "Action Count", entity->action_count, DebugVariableType_F32);
                        add_debug_newline(debug, type);
                        
                        add_debug_variable(type, "Hit Chance", entity->hit_chance, DebugVariableType_U32);
                        add_debug_variable(type, "Strength", entity->strength, DebugVariableType_U32);
                        add_debug_variable(type, "Intelligence", entity->intelligence, DebugVariableType_U32);
                        add_debug_variable(type, "Dexterity", entity->dexterity, DebugVariableType_U32);
                        add_debug_variable(type, "Defence", entity->defence, DebugVariableType_U32);
                        add_debug_variable(type, "Evasion", entity->evasion, DebugVariableType_U32);
                    }
                    end_debug_group(debug, type);
                    
                    start_debug_group(debug, type, "Resistances", false);
                    {
                        for(DamageType damage_type = DamageType_None + 1;
                            damage_type < DamageType_Count;
                            ++damage_type)
                        {
                            add_debug_variable(type, get_damage_type_text(damage_type), entity->resistances[damage_type], DebugVariableType_S32);
                        }
                    }
                    end_debug_group(debug, type);
                    
                    start_debug_group(debug, type, "Status Effects", false);
                    {
                        for(StatusEffectType status_type = StatusEffectType_None + 1;
                            status_type < StatusEffectType_Count;
                            ++status_type)
                        {
                            if(entity_has_status_effect(entity, status_type))
                            {
                                add_debug_text(type, "%s", Color_White, get_status_effect_text(status_type));
                            }
                        }
                    }
                    end_debug_group(debug, type);
                    
                }
                end_debug_group(debug, type);
                
                return;
            }
            
            Item *item = get_item_on_pos(items, dungeon->level, input->mouse_tile_pos, 0);
            if(item)
            {
                hot->type = DebugHotType_Item;
                hot->pos = item->pos;
                hot->generic = item;
                
                start_debug_group(debug, type, "Item", true);
                {
                }
                end_debug_group(debug, type);
                
                return;
            }
            }
    }
    else if(hot_interaction->type)
    {
        // Update debug interaction
        
        switch(hot_interaction->type)
        {
            case DebugInteractionType_Move:
            {
                DebugContext *context = hot_interaction->context;
                context->is_moving = true;
                
                if(input->Button_Left.is_down)
                {
                    assert(context);
                    context->pos = *hot_interaction->pos;
                    
                    // Center move rect to cursor
                    u32 move_rect_half_size = context->move_rect_size / 2;
                    context->pos.x -= move_rect_half_size;
                    context->pos.y -= move_rect_half_size;
                }
                else
                {
                    context->is_moving = false;
                    zero_struct(*hot_interaction);
                }
                } break;
        }
    }
}