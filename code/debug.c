internal b32
is_var_group(DebugVariableType type)
{
    b32 result = (type == DebugVariableType_Group);
    return(result);
}

internal DebugVariable *
get_debug_tree_root(DebugTree *tree)
{
    DebugVariable *result = tree->root;
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
get_debug_tree_root_var_count(DebugTree *tree)
{
    u32 result = 0;
    
    DebugVariable *root = get_debug_tree_root(tree);
    DebugVariable *var = root->group.first_child;
    
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
    pop_memory_(&debug->memory_arena, sizeof(DebugVariable) * var_count);
    
    // Zero the root so when a new one is created it will replace this one
    zero_struct(*get_debug_tree_root(tree));
}

#define add_debug_variable(tree_type, name, value, variable_type) add_debug_variable_(&game->debug, tree_type, name, &value, 0, 0, 0, variable_type)
internal DebugVariable *
add_debug_variable_(DebugState *debug, DebugTree *tree, char *name, void *value,
                    Color color, char *enum_to_string_callback(u32),
                    u32 *flags, DebugVariableType variable_type)
{
    DebugVariable *var = push_memory_struct(&debug->memory_arena, DebugVariable);
    
    var->type = variable_type;
    strcpy(var->name.s, name);
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
    
    DebugVariable *root = get_debug_tree_root(tree);
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
    add_debug_variable_(debug, tree, name, &flag, 0, 0, flags, DebugVariableType_Flag);
}

#define add_debug_enum(tree, name, value, enum_to_string_callback) add_debug_enum_(debug, tree, name, (u32 *)&value, enum_to_string_callback)
internal void
add_debug_enum_(DebugState *debug,
                DebugTree *tree,
               char *name,
               u32 *value,
               char *enum_to_string_callback(u32))
{
    add_debug_variable_(debug, tree, name, value, 0,
                        enum_to_string_callback, 0, DebugVariableType_Enum);
}

internal void
add_debug_newline(DebugState *debug, DebugTree *tree)
{
    add_debug_variable_(debug, tree, "", 0, 0, 0, 0, DebugVariableType_Newline);
}

internal DebugVariable *
start_debug_group(DebugState *debug, DebugTree *tree, char *name, b32 is_expanded)
{
    DebugVariable *var = add_debug_variable_(debug, tree, name, 0, 0, 0, 0, DebugVariableType_Group);
    
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
            
            tree->move_color.active = Color_Yellow;
            tree->move_color.inactive = Color_White;
            
            tree->delete_color.active = Color_LightRed;
            tree->delete_color.inactive = Color_DarkRed;
            
            tree->group_text_color.active = Color_White;
            tree->group_text_color.inactive = Color_White;
            
            tree->text_color_active = Color_Yellow;
            
            tree->root = start_debug_group(debug, tree, "Tree Root", true);
            
            return(tree);
        }
    }
    
    assert(0);
}

#define add_debug_text(text, color, ...) add_debug_text_(&game->debug, text, color, ##__VA_ARGS__)
internal void
add_debug_text_(DebugState *debug, DebugTree *tree, char *text, Color color, ...)
{
    String256 text_final = {0};
    
    va_list arg_list;
    va_start(arg_list, color);
    vsnprintf(text_final.s, sizeof(text_final), text, arg_list);
    va_end(arg_list);
    
    add_debug_variable_(debug, tree, text_final.s, 0, color, 0, 0, DebugVariableType_Text);
}

internal void
end_debug_group(DebugTree *tree)
{
    assert(tree->root);
    tree->root = tree->root->parent_group;
}

internal void
add_status_debug_variables(Game *game, DebugTree *tree, Status *status)
{
    DebugState *debug = &game->debug;
    
    add_debug_enum(tree, "Type", status->type, get_status_type_text);
    
    if(status->type == StatusType_Damage)
    {
        add_debug_enum(tree, "Damage Type", status->damage_type, get_damage_type_text);
    }
    
        add_debug_enum(tree, "Stat Type", status->stat_type, get_stat_type_text);
    
    add_debug_variable(tree, "Was Value Used", status->was_value_used, DebugVariableType_B32);
    add_debug_variable(tree, "Value", status->value, DebugVariableType_U32);
    add_debug_variable(tree, "Chance", status->chance, DebugVariableType_U32);
    add_debug_variable(tree, "Duration", status->duration, DebugVariableType_U32);
}

internal void
update_and_render_debug_state(Game *game,
                    Input *input,
                    EntityState *entities,
                    ItemState *items,
                              Dungeons *dungeons)
{
    DebugState *debug = &game->debug;
    if(debug->is_shown)
    {
    zero_struct(debug->hot);
    DebugInteraction new_interaction = {0};
    
        for(u32 index = 0; index < MAX_DEBUG_TREE_COUNT; ++index)
        {
        DebugTree *tree = &debug->trees[index];
        if(tree->is_set)
            {
                b32 is_first_group = true;
                
        u32 var_count = 0;
        u32 group_depth = 0;
                
        DebugVariable *var = tree->root->group.first_child;
        while(var)
                {
            // Set text
                    String256 text = {0};
            
            switch(var->type)
            {
                case DebugVariableType_Newline: break;
                
                case DebugVariableType_Group:
                        case DebugVariableType_Text: snprintf(text.s, sizeof(text), "%s", var->name.s); break;
                
                        case DebugVariableType_S32: snprintf(text.s, sizeof(text), "%s: %d", var->name.s, *var->s32); break;
                        case DebugVariableType_U32: snprintf(text.s, sizeof(text), "%s: %u", var->name.s, *var->u32); break;
                        case DebugVariableType_B32: snprintf(text.s, sizeof(text), "%s: %s", var->name.s, (*var->b32 == true) ? "true" : "false"); break;
                        case DebugVariableType_F32: snprintf(text.s, sizeof(text), "%s: %.01f", var->name.s, *var->f32); break;
                        case DebugVariableType_V2U: snprintf(text.s, sizeof(text), "%s: %u, %u", var->name.s, var->v2u->x, var->v2u->y); break;
                        case DebugVariableType_V4U: snprintf(text.s, sizeof(text), "%s: %u, %u, %u, %u", var->name.s, var->v4u->x, var->v4u->y, var->v4u->w, var->v4u->h); break;
                        case DebugVariableType_String: snprintf(text.s, sizeof(text), "%s: %s", var->name.s, var->string); break;
                
                case DebugVariableType_Enum:
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
                
                        case DebugVariableType_Flag: snprintf(text.s, sizeof(text), "%s: %s", var->name.s, is_set(*var->flags, var->flag) ? "true" : "false"); break;
                
                invalid_default_case;
            }
                    
                    Color delete_color = tree->delete_color.inactive;
                    Color move_color = tree->move_color.inactive;
                    if(tree->is_moving)
            {
                        move_color = tree->move_color.active;
                    }
                    
                    // Set move_rect pos
            tree->move_rect.x = tree->pos.x + (group_depth * debug->text_offset.x);
            tree->move_rect.y = tree->pos.y + (var_count * debug->text_offset.y);
                    
                    // Set text rect
                    Font *font = debug->font;
                    assert(font->type == FontType_TTF);
                    
            Color background_color = Color_DebugBackgroundDark;
                    Color text_color = tree->group_text_color.inactive;
            if(var->group.is_expanded)
            {
                        background_color = Color_DebugBackgroundLight;
                        text_color = tree->group_text_color.active;
            }
            
            v4u text_rect =
            {
                tree->move_rect.x,
                tree->move_rect.y,
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
                    v4u background_rect =
                    {
                        text_rect.x - text_background_offset.x,
                        text_rect.y - text_background_offset.y,
                        text_rect.w + (text_background_offset.x * 1.5f),
                        text_rect.h + (text_background_offset.y * 1.5f)
                    };
                    
            // Set interaction
            if(is_pos_inside_rect(tree->move_rect, input->mouse_pos))
            {
                new_interaction.type = DebugInteractionType_Move;
                new_interaction.tree = tree;
                
                        move_color = tree->move_color.active;
                    }
                    else if(is_pos_inside_rect(tree->delete_rect, input->mouse_pos))
                    {
                        new_interaction.type = DebugInteractionType_Delete;
                        new_interaction.tree = tree;
                        
                        delete_color = tree->delete_color.active;
                    }
            else if(is_pos_inside_rect(background_rect, input->mouse_pos))
            {
                debug->hot.var = var;
                
                if(is_var_group(var->type))
                {
                            background_color = Color_DebugBackgroundLight;
                            text_color = tree->group_text_color.active;
                }
                else
                {
                            text_color = tree->text_color_active;
                }
            }
            
            if(is_var_group(var->type))
            {
                        // Render text background
                        render_fill_rect(game, background_rect, background_color);
                        
                        if(is_first_group)
                        {
                        // Render move rect
                            render_fill_rect(game, tree->move_rect, move_color);
                            
                            // Render delete rect
                            tree->delete_rect.x = tree->move_rect.x - (tree->move_rect.w * 2);
                            tree->delete_rect.y = tree->move_rect.y;
                            
                            render_fill_rect(game, tree->delete_rect, delete_color);
                        }
            }
            
            // Render text
            if(var->color)
            {
                        text_color = var->color;
                    }
                    
                    v2u text_pos = {text_rect.x, text_rect.y};
                    render_text(game, "%s%s", &text_pos, font, start_color(text_color), text.s);
                    
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
    
    DebugVariable *hot_var = debug->hot.var;
    DebugInteraction *hot_interaction = &debug->hot_interaction;
    
        if(was_pressed_core(&input->Button_Left))
        {
        if(new_interaction.type)
        {
            // Start hot interaction
            
            switch(new_interaction.type)
            {
                case DebugInteractionType_Move:
                {
                    new_interaction.pos = &input->mouse_pos;
                    debug->hot_interaction = new_interaction;
                    } break;
                    
                    case DebugInteractionType_Delete:
                    {
                        zero_struct(*new_interaction.tree);
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
                    case DebugVariableType_B32: *hot_var->b32 = !(*hot_var->b32); break;
                case DebugVariableType_F32: break;
                case DebugVariableType_V2U: break;
                case DebugVariableType_V4U: break;
                case DebugVariableType_String: break;
                case DebugVariableType_Enum: break;
                    case DebugVariableType_Flag:
                    {
                        toggle(*hot_var->flags, hot_var->flag);
                        } break;
                
                invalid_default_case;
            }
        }
        else
        {
            DebugTree *new_tree = add_debug_tree(debug, game->window_size.x / 2, 50);
            Dungeon *dungeon = get_dungeon_from_level(dungeons, dungeons->current_level);
            
                Entity *entity = get_dungeon_pos_entity(entities, dungeon->level, input->mouse_tile_pos, false);
                if(entity && is_tile_seen_or_has_been_seen(dungeon->tiles, entity->pos))
            {
                start_debug_group(debug, new_tree, "Entity", true);
                {
                    start_debug_group(debug, new_tree, "Flags", false);
                    {
                        add_debug_flag(new_tree, "Notify About Multiple Items", entity->flags, EntityFlag_NotifyAboutMultipleItems);
                        add_debug_flag(new_tree, "Uses Physical Attacks", entity->flags, EntityFlag_UsesPhysicalAttacks);
                        add_debug_flag(new_tree, "Uses Ranged Attacks", entity->flags, EntityFlag_UsesRangedAttacks);
                        add_debug_flag(new_tree, "Has Been Seen", entity->flags, EntityFlag_HasBeenSeen);
                        add_debug_flag(new_tree, "Is Flipped", entity->flags, EntityFlag_IsFlipped);
                        add_debug_flag(new_tree, "In Combat", entity->flags, EntityFlag_InCombat);
                        add_debug_flag(new_tree, "Is Pathfinding", entity->flags, EntityFlag_IsPathfinding);
                        add_debug_flag(new_tree, "Is Ghost Enabled", entity->flags, EntityFlag_IsGhostEnabled);
                        add_debug_flag(new_tree, "Is Ghost Flipped", entity->flags, EntityFlag_IsGhostFlipped);
                            add_debug_flag(new_tree, "Is Invisible", entity->flags, EntityFlag_IsInvisible);
                            add_debug_flag(new_tree, "Moves Normally On Water", entity->flags, EntityFlag_MovesNormallyOnWater);
                    }
                    end_debug_group(new_tree);
                    add_debug_newline(debug, new_tree);
                    
                    add_debug_variable(new_tree, "ID", entity->id, DebugVariableType_U32);
                    add_debug_enum(new_tree, "Type", entity->type, get_entity_type_text);
                        add_debug_variable(new_tree, "Name", entity->name.s, DebugVariableType_String);
                    add_debug_variable(new_tree, "Max HP", entity->max_hp, DebugVariableType_U32);
                    add_debug_variable(new_tree, "HP", entity->hp, DebugVariableType_U32);
                    add_debug_newline(debug, new_tree);
                    
                    add_debug_variable(new_tree, "Dungeon Level", entity->dungeon_level, DebugVariableType_U32);
                    add_debug_variable(new_tree, "New Pos", entity->new_pos, DebugVariableType_V2U);
                    add_debug_variable(new_tree, "Pos", entity->pos, DebugVariableType_V2U);
                    
                    add_debug_newline(debug, new_tree);
                    
                    add_debug_variable(new_tree, "Width", entity->width, DebugVariableType_U32);
                    add_debug_variable(new_tree, "Height", entity->height, DebugVariableType_U32);
                    add_debug_variable(new_tree, "Tile Source", entity->tile_src, DebugVariableType_V4U);
                    add_debug_enum(new_tree, "Remains", entity->remains, get_entity_remains_text);
                    add_debug_variable(new_tree, "pathfind Target", entity->pathfind_target_pos, DebugVariableType_V2U);
                    add_debug_newline(debug, new_tree);
                    
                    start_debug_group(debug, new_tree, "Stats", false);
                    {
                        add_debug_variable(new_tree, "FOV", entity->fov, DebugVariableType_U32);
                            add_debug_variable(new_tree, "Action Time", entity->action_time, DebugVariableType_F32);
                        add_debug_newline(debug, new_tree);
                        
                        add_debug_variable(new_tree, "Hit Chance", entity->hit_chance, DebugVariableType_U32);
                        add_debug_variable(new_tree, "Strength", entity->strength, DebugVariableType_U32);
                        add_debug_variable(new_tree, "Intelligence", entity->intelligence, DebugVariableType_U32);
                        add_debug_variable(new_tree, "Dexterity", entity->dexterity, DebugVariableType_U32);
                        add_debug_variable(new_tree, "Defence", entity->defence, DebugVariableType_U32);
                        add_debug_variable(new_tree, "Evasion", entity->evasion, DebugVariableType_U32);
                    }
                    end_debug_group(new_tree);
                    
                    start_debug_group(debug, new_tree, "Resistances", false);
                    {
                        for(DamageType damage_type = DamageType_None + 1;
                            damage_type < DamageType_Count;
                            ++damage_type)
                        {
                            add_debug_variable(new_tree, get_damage_type_text(damage_type), entity->resistances[damage_type], DebugVariableType_S32);
                        }
                    }
                    end_debug_group(new_tree);
                        add_debug_newline(debug, new_tree);
                        
                        for(u32 index = 0; index < MAX_ENTITY_STATUS_COUNT; ++index)
                            {
                                Status *status = &entity->statuses[index];
                                
                                    start_debug_group(debug, new_tree, "Status", false);
                                    {
                                        add_status_debug_variables(game, new_tree, status);
                                    }
                                    end_debug_group(new_tree);
                                }
                    }
                end_debug_group(new_tree);
                
                return;
            }
            
                Item *item = get_dungeon_pos_item(items, dungeon->level, input->mouse_tile_pos, 0);
                if(item && is_tile_seen_or_has_been_seen(dungeon->tiles, item->pos))
            {
                start_debug_group(debug, new_tree, "Item", true);
                {
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
                    
                    add_debug_variable(new_tree, "ID", item->id, DebugVariableType_U32);
                    add_debug_enum(new_tree, "Type", item->type, get_item_type_text);
                        add_debug_variable(new_tree, "Name", item->name.s, DebugVariableType_String);
                    add_debug_variable(new_tree, "Description", item->description, DebugVariableType_String);
                    add_debug_newline(debug, new_tree);
                    
                        add_debug_variable(new_tree, "Inventory Letter", item->inventory_letter, DebugVariableType_String);
                        add_debug_variable(new_tree, "Selection Letter", item->select_letter, DebugVariableType_String);
                    add_debug_variable(new_tree, "Mark", item->mark.array, DebugVariableType_String);
                    add_debug_newline(debug, new_tree);
                    
                    add_debug_variable(new_tree, "Dungeon Level", item->dungeon_level, DebugVariableType_U32);
                    add_debug_variable(new_tree, "Pos", item->pos, DebugVariableType_V2U);
                    add_debug_newline(debug, new_tree);
                    
                    if(item->slot)
                    {
                        add_debug_enum(new_tree, "Slot", item->slot, get_item_slot_text);
                    }
                    
                    add_debug_enum(new_tree, "Rarity", item->rarity, get_item_rarity_text);
                    add_debug_variable(new_tree, "Tile Source", item->tile_src, DebugVariableType_V4U);
                    add_debug_variable(new_tree, "Equip Tile Source", item->equip_tile_src, DebugVariableType_V4U);
                    add_debug_newline(debug, new_tree);
                    
                    if(item->type == ItemType_Weapon)
                    {
                        add_debug_enum(new_tree, "Handedness", item->w.handedness, get_item_handedness_text);
                        add_debug_enum(new_tree, "First Damage Type", item->type, get_damage_type_text);
                        add_debug_enum(new_tree, "Second Damage Type", item->type, get_damage_type_text);
                        add_debug_variable(new_tree, "Enchantment Level", item->enchantment_level, DebugVariableType_S32);
                        add_debug_variable(new_tree, "Extra Stat Count", item->extra_stat_count, DebugVariableType_S32);
                    }
                    else if(item->type == ItemType_Armor)
                    {
                        add_debug_variable(new_tree, "Defence", item->a.defence, DebugVariableType_U32);
                        add_debug_variable(new_tree, "Weight", item->a.weight, DebugVariableType_U32);
                    }
                    else if(is_item_consumable(item->type))
                        {
                            start_debug_group(debug, new_tree, "Status", false);
                            {
                            add_status_debug_variables(game, new_tree, &item->c.status);
                            }
                            end_debug_group(new_tree);
                    }
                    
                    add_debug_newline(debug, new_tree);
                    
                }
                end_debug_group(new_tree);
                
                return;
            }
        }
    }
    else if(hot_interaction->type)
    {
        // Update hot interaction
        
        switch(hot_interaction->type)
        {
            case DebugInteractionType_Move:
            {
                DebugTree *tree = hot_interaction->tree;
                tree->is_moving = true;
                
                if(input->Button_Left.is_down)
                {
                    assert(tree);
                    tree->pos = *hot_interaction->pos;
                    
                    // Center rect to cursor
                        u32 move_rect_half_size = tree->rect_size / 2;
                    tree->pos.x -= move_rect_half_size;
                    tree->pos.y -= move_rect_half_size;
                }
                else
                {
                    tree->is_moving = false;
                    zero_struct(*hot_interaction);
                }
            } break;
        }
    }
    }
}