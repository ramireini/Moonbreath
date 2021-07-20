internal void
reset_debug_event(DebugEvent *event)
{
    assert(event);
    
    event->tree->is_moving = false;
    zero_struct(*event);
}

internal b32
is_var_group(DebugVarType type)
{
    b32 result = (type == DebugVarType_Group);
    return(result);
}

internal DebugVar *
get_debug_tree_root(DebugTree *tree)
{
    DebugVar *result = tree->root;
    return(result);
}

internal DebugVar *
get_next_debug_variable(DebugVar *var)
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
get_debug_tree_root_var_count(DebugTree *tree)
{
    u32 result = 0;
    
    DebugVar *root = get_debug_tree_root(tree);
    DebugVar *var = root->group.first_child;
    
    while(var)
    {
        ++result;
        
        var = get_next_debug_variable(var);
    }
    
    return(result);
}

internal void
zero_debug_tree_variables(DebugState *debug, DebugTree *tree)
{
    // Pop all the variables after root
    u32 var_count = get_debug_tree_root_var_count(tree);
    pop_memory_(&debug->memory_arena, sizeof(DebugVar) * var_count);
    
    // Zero the root so when a new one is created it will replace this one
    zero_struct(*get_debug_tree_root(tree));
}

#define add_debug_variable(tree_type, name, value, variable_type) add_debug_variable_(debug, tree_type, name, &value, 0, 0, 0, variable_type)
internal DebugVar *
add_debug_variable_(DebugState *debug,
                    DebugTree *tree,
                    char *name,
                    void *value,
                    Color color,
                    char *enum_to_string_callback(u32),
                    u32 *flags, DebugVarType variable_type)
{
    DebugVar *var = push_memory_struct(&debug->memory_arena, DebugVar);
    
    var->type = variable_type;
    strcpy(var->name.s, name);
    var->color = color;
    
    var->enum_to_string_callback = enum_to_string_callback;
    var->flags = flags;
    
    var->next = 0;
    
    switch(var->type)
    {
        case DebugVarType_Newline: break;
        case DebugVarType_Group: break;
        case DebugVarType_Text: break;
        
        case DebugVarType_S32: var->s32 = (s32 *)value; break;
        case DebugVarType_U32: var->u32 = (u32 *)value; break;
        case DebugVarType_B32: var->b32 = (b32 *)value; break;
        case DebugVarType_F32: var->f32 = (f32 *)value; break;
        case DebugVarType_V2U: var->v2u = (v2u *)value; break;
        case DebugVarType_V4U: var->v4u = (v4u *)value; break;
        case DebugVarType_String: var->string = (char *)value; break;
        case DebugVarType_Enum: var->u32 = (u32 *)value; break;
        case DebugVarType_Flag: var->flag = *(u32 *)value; break;
        
        invalid_default_case;
            };
    
    DebugVar *root = get_debug_tree_root(tree);
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

#define add_debug_flag(tree, name, flags, flag) add_debug_flag_(debug, tree, name, &flags, flag)
internal void
add_debug_flag_(DebugState *debug, DebugTree *tree, char *name, u32 *flags, u32 flag)
{
    add_debug_variable_(debug, tree, name, &flag, 0, 0, flags, DebugVarType_Flag);
}

#define add_debug_enum(tree, name, value, enum_to_string_callback) add_debug_enum_(debug, tree, name, (u32 *)&value, enum_to_string_callback)
internal void
add_debug_enum_(DebugState *debug,
                DebugTree *tree,
               char *name,
               u32 *value,
               char *enum_to_string_callback(u32))
{
    add_debug_variable_(debug, tree, name, value, 0, enum_to_string_callback, 0, DebugVarType_Enum);
}

internal void
add_debug_newline(DebugState *debug, DebugTree *tree)
{
    add_debug_variable_(debug, tree, "", 0, 0, 0, 0, DebugVarType_Newline);
}

internal DebugVar *
start_debug_group(DebugState *debug, DebugTree *tree, char *name, b32 is_expanded)
{
    DebugVar *var = add_debug_variable_(debug, tree, name, 0, 0, 0, 0, DebugVarType_Group);
    
    var->group.is_expanded = is_expanded;
    
    var->group.first_child = 0;
    var->group.last_child = 0;
    
    tree->root = var;
    return(var);
}

internal DebugTree *
add_debug_tree(DebugState *debug, u32 x, u32 y)
{
    for(u32 index = 0; index < MAX_DEBUG_TREE_COUNT; ++index)
    {
        DebugTree *tree = &debug->trees[index];
        if(!tree->is_set)
        {
            tree->is_set = true;
            tree->is_moving = false;
            
            tree->pos.x = x;
            tree->pos.y = y;
            
            tree->rect_size = 10;
            
            tree->move_rect.x = 0;
            tree->move_rect.y = 0;
            tree->move_rect.w = tree->rect_size;
            tree->move_rect.h = tree->rect_size;
            
            tree->delete_rect = tree->move_rect;
            tree->delete_string = "Delete";
            
            tree->move_color.active = Color_LightYellow;
            tree->move_color.inactive = Color_White;
            
            tree->delete_color.active = Color_LightRed;
            tree->delete_color.inactive = Color_DarkRed;
            
            tree->group_text_color.active = Color_White;
            tree->group_text_color.inactive = Color_White;
            
            tree->root = start_debug_group(debug, tree, "Tree Root", true);
            
            return(tree);
        }
    }
}

#define add_debug_string(string, color, ...) add_debug_text_(&game->debug, string, color, ##__VA_ARGS__)
internal DebugVar *
add_debug_text_(DebugState *debug, DebugTree *tree, char *string, Color color, ...)
{
    String256 string_final = {0};
    
    va_list arg_list;
    va_start(arg_list, color);
    vsnprintf(string_final.s, sizeof(string_final), string, arg_list);
    va_end(arg_list);
    
    DebugVar *result = add_debug_variable_(debug, tree, string_final.s, 0, color, 0, 0, DebugVarType_Text);
    return(result);
}

internal void
add_debug_delete(Game *game, DebugTree *tree, void *data, DebugVarActionType type)
{
    assert(game);
    assert(tree);
    assert(data);
    assert(type);
    
    DebugVar *var = add_debug_string(tree, tree->delete_string, Color_White);
    var->action = type;
    
    switch(type)
    {
        case DebugVarActionType_DeleteItem: var->item = (Item *)data; break;
        case DebugVarActionType_DeleteEntity: var->entity = (Entity *)data; break;
        
        invalid_default_case;
    }
    
    add_debug_newline(&game->debug, tree);
}

internal void
end_debug_group(DebugTree *tree)
{
    assert(tree && tree->root);
    tree->root = tree->root->parent_group;
}

internal void
add_debug_status_effect(DebugState *debug, DebugTree *tree, Status *status)
{
    assert(debug);
    assert(tree);
    assert(status);
    
    DebugVar *group_var = start_debug_group(debug, tree, "Empty Status", false);
    {
        group_var->status = status;
        
        add_debug_enum(tree, "Type", status->type, get_status_type_string);
        
        if(status->type == StatusType_Damage)
        {
            add_debug_enum(tree, "Damage Type", status->value.type, get_damage_type_string);
        }
        
        add_debug_enum(tree, "Stat Type", status->stat_type, get_stat_type_string);
        
        add_debug_variable(tree, "Stat Value Applied", status->stat_value_applied, DebugVarType_B32);
        add_debug_variable(tree, "Min Value", status->value.min, DebugVarType_S32);
        add_debug_variable(tree, "Max Value", status->value.max, DebugVarType_S32);
        add_debug_variable(tree, "Chance", status->chance, DebugVarType_U32);
        add_debug_variable(tree, "Duration", status->duration, DebugVarType_U32);
    }
    end_debug_group(tree);
}

internal void
update_and_render_debug_state(Game *game,
                    Input *input,
                    EntityState *entities,
                    ItemState *items,
                              Dungeon *dungeon)
{
    DebugState *debug = &game->debug;
    if(debug->is_shown)
    {
    zero_struct(debug->hot);
        DebugEvent new_event = {0};
        
        for(u32 index = 0; index < MAX_DEBUG_TREE_COUNT; ++index)
        {
        DebugTree *tree = &debug->trees[index];
        if(tree->is_set)
            {
                b32 is_first_group = true;
                
        u32 var_count = 0;
        u32 group_depth = 0;
                
        DebugVar *var = tree->root->group.first_child;
        while(var)
                {
            // Set text
                    String256 text = {0};
            
            switch(var->type)
            {
                case DebugVarType_Newline: break;
                
                case DebugVarType_Group:
                        case DebugVarType_Text: snprintf(text.s, sizeof(text), "%s", var->name.s); break;
                
                        case DebugVarType_S32: snprintf(text.s, sizeof(text), "%s: %d", var->name.s, *var->s32); break;
                        case DebugVarType_U32: snprintf(text.s, sizeof(text), "%s: %u", var->name.s, *var->u32); break;
                        case DebugVarType_B32: snprintf(text.s, sizeof(text), "%s: %s", var->name.s, (*var->b32 == true) ? "true" : "false"); break;
                        case DebugVarType_F32: snprintf(text.s, sizeof(text), "%s: %.01f", var->name.s, *var->f32); break;
                        case DebugVarType_V2U: snprintf(text.s, sizeof(text), "%s: %u, %u", var->name.s, var->v2u->x, var->v2u->y); break;
                        case DebugVarType_V4U: snprintf(text.s, sizeof(text), "%s: %u, %u, %u, %u", var->name.s, var->v4u->x, var->v4u->y, var->v4u->w, var->v4u->h); break;
                        case DebugVarType_String: snprintf(text.s, sizeof(text), "%s: %s", var->name.s, var->string); break;
                
                case DebugVarType_Enum:
                {
                    if(var->enum_to_string_callback)
                            {
                                if(*var->u32 == 0)
                                {
                                    snprintf(text.s, sizeof(text), "%s: None", var->name.s);
                                }
                                else
                                {
                                snprintf(text.s, sizeof(text), "%s: %s", var->name.s, var->enum_to_string_callback(*var->u32));
                                }
                    }
                    else
                    {
                                snprintf(text.s, sizeof(text), "%s: %u", var->name.s, *var->u32);
                    }
                } break;
                
                        case DebugVarType_Flag: snprintf(text.s, sizeof(text), "%s: %s", var->name.s, is_set(*var->flags, var->flag) ? "true" : "false"); break;
                
                invalid_default_case;
            }
                    
                    v2u base_pos =
                    {
                        tree->pos.x + (group_depth * debug->text_offset.x),
                        tree->pos.y + (var_count * debug->text_offset.y)
                    };
                    
                    // Set text rect
                    Font *font = debug->font;
                    assert(font->type == FontType_TTF);
                    
            v4u text_rect =
            {
                base_pos.x,
                        base_pos.y,
                        get_text_width(text.s, font, false),
                font->size
                    };
                    
                    // This offsets text to the right of move_rect
            if(is_var_group(var->type) &&
               is_first_group)
            {
                text_rect.x += 24;
                        text_rect.y -= (text_rect.h / 2) - (tree->rect_size / 2);
            }
                    
                    v2u text_background_offset = {6, 1};
                    v4u text_background_rect =
                    {
                        text_rect.x - text_background_offset.x,
                        text_rect.y - text_background_offset.y,
                        text_rect.w + (text_background_offset.x * 1.5f),
                        text_rect.h + (text_background_offset.y * 1.5f)
                    };
                    
                    Color background_color = Color_DebugBackgroundDark;
                    Color text_color = tree->group_text_color.inactive;
                    if(var->group.is_expanded)
                    {
                        background_color = Color_DebugBackgroundLight;
                        text_color = tree->group_text_color.active;
                    }
                    
                    Color delete_color = tree->delete_color.inactive;
                    Color move_color = tree->move_color.inactive;
                    if(tree->is_moving)
                    {
                        move_color = tree->move_color.active;
                    }
                    
                    if(var->color)
                    {
                        text_color = var->color;
                    }
                    
            // Set event
            if(is_pos_inside_rect(tree->move_rect, input->mouse))
                    {
                        new_event.type = DebugEventType_MoveTree;
                        new_event.tree = tree;
                
                        move_color = tree->move_color.active;
                    }
                    else if(is_pos_inside_rect(tree->delete_rect, input->mouse))
                    {
                        new_event.type = DebugEventType_DeleteTree;
                        new_event.tree = tree;
                        
                        delete_color = tree->delete_color.active;
                    }
                    else if(is_pos_inside_rect(text_background_rect, input->mouse))
            {
                debug->hot.var = var;
                
                if(is_var_group(var->type))
                {
                            background_color = Color_DebugBackgroundLight;
                            text_color = tree->group_text_color.active;
                        }
                        else if(strings_match(var->name.s, tree->delete_string))
                        {
                            text_color = tree->delete_color.active;
                        }
                        else
                        {
                            text_color = Color_LightYellow;
                }
            }
                    
            if(is_var_group(var->type))
            {
                        // Render text background
                        render_fill_rect(game->renderer, text_background_rect, background_color, false);
                        
                        if(is_first_group)
                        {
                            // Render move rect
                            tree->move_rect.x = base_pos.x;
                            tree->move_rect.y = base_pos.y;
                            
                            render_fill_rect(game->renderer, tree->move_rect, move_color, false);
                            
                            // Render delete rect
                            tree->delete_rect.x = tree->move_rect.x - (tree->move_rect.w * 2);
                            tree->delete_rect.y = tree->move_rect.y;
                            
                            render_fill_rect(game->renderer, tree->delete_rect, delete_color, false);
                        }
            }
                    
                    // Render text
                    v2u text_pos = {text_rect.x, text_rect.y};
                    render_string(game, "%s%s", &text_pos, font, start_color(text_color), text.s);
                    
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
            is_first_group = false;
            }
    }
    }
    
    DebugVar *hot_var = debug->hot.var;
        DebugEvent *event = &debug->event;
        
        if(debug->should_update)
        {
            debug->should_update = false;
            
            if(new_event.type)
            {
                // Update event
                
                switch(new_event.type)
            {
                    case DebugEventType_MoveTree:
                    {
                        new_event.pos = &input->mouse;
                        debug->event = new_event;
                    } break;
                    
                    case DebugEventType_DeleteTree:
                    {
                        zero_struct(*new_event.tree);
                    } break;
                    
                invalid_default_case;
            }
        }
        else if(hot_var)
        {
            switch(hot_var->type)
            {
                case DebugVarType_Newline: break;
                case DebugVarType_Group: hot_var->group.is_expanded = !hot_var->group.is_expanded; break;
                    
                    case DebugVarType_Text:
                    {
                        switch(hot_var->action)
                            {
                            case DebugVarActionType_DeleteItem: zero_struct(*hot_var->item); break;
                            case DebugVarActionType_DeleteEntity: remove_entity_from_game(hot_var->entity, dungeon->tiles); break;
                            case DebugVarActionType_ShowEntityView: toggle(hot_var->entity->flags, EntityFlag_ShowViewRange); break;
                        }
                    } break;
                
                case DebugVarType_S32: break;
                case DebugVarType_U32: break;
                    case DebugVarType_B32: *hot_var->b32 = !(*hot_var->b32); break;
                case DebugVarType_F32: break;
                case DebugVarType_V2U: break;
                case DebugVarType_V4U: break;
                case DebugVarType_String: break;
                    case DebugVarType_Enum: break;
                    
                    case DebugVarType_Flag:
                    {
                        toggle(*hot_var->flags, hot_var->flag);
                        } break;
                
                invalid_default_case;
            }
        }
        else
        {
            DebugTree *new_tree = add_debug_tree(debug, game->window_size.x / 2, 50);
                
                Entity *entity = get_dungeon_pos_entity(entities, dungeon->level, input->mouse_tile, false);
                if(entity && is_tile_seen_or_has_been_seen(dungeon->tiles, entity->pos))
                {
                    entity->debug_tree = new_tree;
                    
                    start_debug_group(debug, new_tree, entity->name.s, true);
                    {
                        DebugVar *view_var = add_debug_string(new_tree, "View Toggle", Color_White);
                        view_var->action = DebugVarActionType_ShowEntityView;
                        view_var->entity = entity;
                        
                        add_debug_delete(game, new_tree, entity, DebugVarActionType_DeleteEntity);
                        
                    start_debug_group(debug, new_tree, "Flags", false);
                        {
                            add_debug_flag(new_tree, "ShowViewRange", entity->flags, EntityFlag_ShowViewRange);
                            add_debug_flag(new_tree, "NotifyAboutMultipleItems", entity->flags, EntityFlag_NotifyAboutMultipleItems);
                            add_debug_flag(new_tree, "UsesPhysicalAttacks", entity->flags, EntityFlag_UsesPhysicalAttacks);
                            add_debug_flag(new_tree, "UsesRangedAttacks", entity->flags, EntityFlag_UsesRangedAttacks);
                            add_debug_flag(new_tree, "HasBeenSeen", entity->flags, EntityFlag_HasBeenSeen);
                        add_debug_flag(new_tree, "Flipped", entity->flags, EntityFlag_Flipped);
                        add_debug_flag(new_tree, "InCombat", entity->flags, EntityFlag_InCombat);
                        add_debug_flag(new_tree, "Pathfinding", entity->flags, EntityFlag_Pathfinding);
                        add_debug_flag(new_tree, "GhostEnabled", entity->flags, EntityFlag_GhostEnabled);
                        add_debug_flag(new_tree, "GhostFlipped", entity->flags, EntityFlag_GhostFlipped);
                            add_debug_flag(new_tree, "Invisible", entity->flags, EntityFlag_Invisible);
                            add_debug_flag(new_tree, "NormalWaterMovement", entity->flags, EntityFlag_NormalWaterMovement);
                            add_debug_flag(new_tree, "Undead", entity->flags, EntityFlag_Undead);
                    }
                    end_debug_group(new_tree);
                    add_debug_newline(debug, new_tree);
                    
                    add_debug_variable(new_tree, "ID", entity->id, DebugVarType_U32);
                        add_debug_enum(new_tree, "Type", entity->type, get_entity_type_string);
                        add_debug_variable(new_tree, "Name", entity->name.s, DebugVarType_String);
                    add_debug_variable(new_tree, "Max HP", entity->max_hp, DebugVarType_U32);
                    add_debug_variable(new_tree, "HP", entity->hp, DebugVarType_U32);
                    add_debug_newline(debug, new_tree);
                    
                    add_debug_variable(new_tree, "Dungeon Level", entity->dungeon_level, DebugVarType_U32);
                    add_debug_variable(new_tree, "New Pos", entity->new_pos, DebugVarType_V2U);
                    add_debug_variable(new_tree, "Pos", entity->pos, DebugVarType_V2U);
                    
                    add_debug_newline(debug, new_tree);
                    
                    add_debug_variable(new_tree, "Width", entity->width, DebugVarType_U32);
                    add_debug_variable(new_tree, "Height", entity->height, DebugVarType_U32);
                    add_debug_variable(new_tree, "Tile Source", entity->tile_src, DebugVarType_V4U);
                        add_debug_enum(new_tree, "Remains", entity->remains, get_entity_remains_string);
                    add_debug_variable(new_tree, "pathfind Target", entity->pathfind_target_pos, DebugVarType_V2U);
                    add_debug_newline(debug, new_tree);
                    
                    start_debug_group(debug, new_tree, "Stats", false);
                        {
                            add_debug_variable(new_tree, "Hit Chance", entity->hit_chance, DebugVarType_U32);
                            add_debug_variable(new_tree, "View Range", entity->view_range, DebugVarType_U32);
                            add_debug_variable(new_tree, "Action Time", entity->action_time, DebugVarType_F32);
                        add_debug_newline(debug, new_tree);
                        
                        add_debug_variable(new_tree, "Hit Chance", entity->hit_chance, DebugVarType_U32);
                            add_debug_variable(new_tree, "Strength", entity->str, DebugVarType_U32);
                            add_debug_variable(new_tree, "Intelligence", entity->intel, DebugVarType_U32);
                            add_debug_variable(new_tree, "Dexterity", entity->dex, DebugVarType_U32);
                            add_debug_variable(new_tree, "Defence", entity->def, DebugVarType_U32);
                            add_debug_variable(new_tree, "Evasion", entity->ev, DebugVarType_U32);
                    }
                    end_debug_group(new_tree);
                    
                    start_debug_group(debug, new_tree, "Resistances", false);
                    {
                        for(u32 index = DamageType_None + 1; index < DamageType_Count; ++index)
                        {
                                add_debug_variable(new_tree, get_damage_type_string(index), entity->resistances[index], DebugVarType_S32);
                        }
                    }
                    end_debug_group(new_tree);
                        add_debug_newline(debug, new_tree);
                        
                        for(u32 index = 0; index < MAX_ENTITY_STATUS_COUNT; ++index)
                            {
                            add_debug_status_effect(&game->debug, new_tree, &entity->statuses[index]);
                        }
                    }
                end_debug_group(new_tree);
                
                return;
            }
            
                Item *item = get_dungeon_pos_item(items, dungeon->level, input->mouse_tile, 0);
                if(item && is_tile_seen_or_has_been_seen(dungeon->tiles, item->pos))
                {
                    start_debug_group(debug, new_tree, get_item_type_string(item->type), true);
                    {
                        add_debug_delete(game, new_tree, item, DebugVarActionType_DeleteItem);
                        
                    start_debug_group(debug, new_tree, "Flags", false);
                    {
                        add_debug_flag(new_tree, "Is Identified", item->flags, ItemFlag_IsIdentified);
                        add_debug_flag(new_tree, "Is Equipped", item->flags, ItemFlag_IsEquipped);
                        add_debug_flag(new_tree, "Is Cursed", item->flags, ItemFlag_IsCursed);
                        add_debug_flag(new_tree, "Is Marked", item->flags, ItemFlag_IsMarked);
                        add_debug_flag(new_tree, "Is Selected", item->flags, ItemFlag_IsSelected);
                        add_debug_flag(new_tree, "In Inventory", item->flags, ItemFlag_InInventory);
                        add_debug_flag(new_tree, "Has Been Seen", item->flags, ItemFlag_HasBeenSeen);
                    }
                    end_debug_group(new_tree);
                    add_debug_newline(debug, new_tree);
                    
                    add_debug_variable(new_tree, "ID", item->id, DebugVarType_U32);
                        add_debug_enum(new_tree, "Type", item->type, get_item_type_string);
                        
                        if(is_item_consumable(item->type))
                        {
                            add_debug_variable(new_tree, "Stack Count", item->c.stack_count, DebugVarType_U32);
                        }
                        
                            add_debug_variable(new_tree, "Name", item->name.s, DebugVarType_String);
                        add_debug_variable(new_tree, "Description", item->description, DebugVarType_String);
                    add_debug_newline(debug, new_tree);
                        
                        add_debug_variable(new_tree, "Dungeon Level", item->dungeon_level, DebugVarType_U32);
                        add_debug_variable(new_tree, "Inventory Letter", item->inventory_letter, DebugVarType_String);
                        add_debug_variable(new_tree, "Selection Letter", item->select_letter, DebugVarType_String);
                    add_debug_variable(new_tree, "Mark", item->mark.array, DebugVarType_String);
                    add_debug_newline(debug, new_tree);
                    
                    add_debug_variable(new_tree, "Dungeon Level", item->dungeon_level, DebugVarType_U32);
                    add_debug_variable(new_tree, "Pos", item->pos, DebugVarType_V2U);
                    add_debug_newline(debug, new_tree);
                    
                    if(item->slot)
                    {
                            add_debug_enum(new_tree, "Slot", item->slot, get_item_slot_string);
                    }
                    
                        add_debug_enum(new_tree, "Rarity", item->rarity, get_item_rarity_string);
                    add_debug_variable(new_tree, "Tile Source", item->tile_src, DebugVarType_V4U);
                    add_debug_variable(new_tree, "Equip Tile Source", item->equip_tile_src, DebugVarType_V4U);
                    add_debug_newline(debug, new_tree);
                    
                    if(item->type == ItemType_Weapon)
                    {
                            add_debug_enum(new_tree, "Handedness", item->w.handedness, get_item_handedness_string);
                            add_debug_enum(new_tree, "First Damage Type", item->type, get_damage_type_string);
                            add_debug_enum(new_tree, "Second Damage Type", item->type, get_damage_type_string);
                            add_debug_variable(new_tree, "Enchant Level", item->enchant_level, DebugVarType_S32);
                        add_debug_variable(new_tree, "Extra Stat Count", item->extra_stat_count, DebugVarType_S32);
                    }
                    else if(item->type == ItemType_Armor)
                    {
                        add_debug_variable(new_tree, "Defence", item->a.defence, DebugVarType_U32);
                        add_debug_variable(new_tree, "Weight", item->a.weight, DebugVarType_U32);
                    }
                    else if(is_item_consumable(item->type))
                        {
                                add_debug_status_effect(&game->debug, new_tree, &item->c.status);
                    }
                    
                    add_debug_newline(debug, new_tree);
                    
                }
                end_debug_group(new_tree);
                
                return;
            }
        }
    }
    else if(event->type)
    {
            // Handle event
            
            switch(event->type)
        {
                case DebugEventType_MoveTree:
                {
                    DebugTree *tree = event->tree;
                    assert(tree);
                    
                    tree->is_moving = true;
                    
                    u32 half_rect_size = tree->rect_size / 2;
                    v4u move_rect =
                    {
                        event->pos->x - half_rect_size,
                        event->pos->y - half_rect_size,
                        tree->rect_size,
                        tree->rect_size
                    };
                    
                    if(input->Button_Left.is_down)
                    {
                        // Clamp move_rect to window
                        if((s32)move_rect.x < 0) move_rect.x = 0;
                        if((s32)move_rect.y < 0) move_rect.y = 0;
                        if(get_rect_width(move_rect) > game->window_size.w) move_rect.x = game->window_size.w - move_rect.w;
                        if(get_rect_height(move_rect) > game->window_size.h) move_rect.y = game->window_size.h - move_rect.h;
                        
                        tree->pos.x = move_rect.x;
                        tree->pos.y = move_rect.y;
                        }
                else
                    {
                        reset_debug_event(event);
                        }
            } break;
        }
        }
        
        // Update entity status names in debug tree
        for(u32 index = 0; index < MAX_ENTITY_COUNT; ++index)
        {
            Entity *entity = &entities->array[index];
            DebugTree *debug_tree = entity->debug_tree;
            
            if(is_entity_valid(entity) && debug_tree)
            {
                if(debug_tree->is_set)
                {
                    DebugVar *var = debug_tree->root->group.first_child;
                    while(var)
                    {
                        if(var->status &&
                           var->type == DebugVarType_Group)
                        {
                            char *empty_status_string = "Empty Status";
                            
                            if(strings_match(var->name.s, empty_status_string))
                            {
                                StatusType type = *var->group.first_child->u32;
                                if(type)
                                {
                                    sprintf(var->name.s, "%s Status", get_status_type_string(type));
                                }
                            }
                            else if(!var->status->type)
                            {
                                strcpy(var->name.s, empty_status_string);
                            }
                        }
                        
                        // Find next var
                        if(var->next)
                        {
                            var = var->next;
                        }
                        else if(var->group.first_child)
                        {
                            var = var->group.first_child;
                        }
                        else
                        {
                            break;
                        }
                    }
                }
                else
                {
                    // Unset the entity debug tree since it doesn't exist anymore
                    entity->debug_tree = 0;
                }
            }
        }
        }
}