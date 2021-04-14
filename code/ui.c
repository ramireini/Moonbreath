internal v2u
get_header_pos(UI *ui, u32 y_multiplier)
{
    v2u result =
    {
        ui->defer_rect.x + (ui->window_offset * 2),
        ui->defer_rect.y + (ui->window_offset * y_multiplier)
    };
    
    return(result);
}

internal v2u
start_defer_rect(UI *ui, u32 y_multiplier)
{
    v2u result = get_header_pos(ui, y_multiplier);
    zero_struct(ui->defer_rect);
    
    return(result);
}

internal void
init_defer_rect_window(DeferRectWindow *window, UI *ui)
{
    window->entry_count = 0;
    window->pos = start_defer_rect(ui, 2);
}

internal v2u
get_window_entry_name_pos(v2u pos, UI *ui)
{
    v2u result =
    {
        pos.x + (ui->font_newline * 3),
        pos.y + (ui->font->size / 2)
    };
    
    return(result);
}

internal v2u
render_window_tile(v4u tile_src, v2u pos, UI *ui)
{
    v2u result =
    {
        pos.x + 8,
        pos.y
    };
    
    defer_texture(ui, result, tile_src);
    
    return(result);
}

internal v2u
render_window_separator(v4u rect, v2u pos, UI *ui)
{
    v2u result =
    {
        pos.x,
        pos.y + (ui->font_newline / 2)
    };
    
    defer_fill_rect(ui,
                    result.x,
                    result.y - 4,
                    rect.w - (result.x * 2),
                    1,
                    Color_WindowAccent);
    
    return(result);
}

internal void
render_item_type_header(UI *ui, v4u rect, v2u pos, ItemType type)
{
    v2u header_pos = render_window_separator(rect, pos, ui);
    
    switch(type)
    {
        case ItemType_Weapon: defer_text(ui, "Weapons", header_pos.x, header_pos.y); break;
        case ItemType_Armor: defer_text(ui, "Armors", header_pos.x, header_pos.y); break;
        case ItemType_Potion: defer_text(ui, "Potions", header_pos.x, header_pos.y); break;
        case ItemType_Scroll: defer_text(ui, "Scrolls", header_pos.x, header_pos.y); break;
        case ItemType_Ration: defer_text(ui, "Rations", header_pos.x, header_pos.y); break;
        
        invalid_default_case;
    }
}

internal void
render_defer_rect_item(UI *ui, DeferRectWindow *window, Item *item, ItemType type, b32 *needs_header)
                       {
    if(*needs_header)
    {
        *needs_header = false;
        ++window->entry_count;
        
        if(is_entry_in_view(window->view, window->entry_count))
        {
            render_item_type_header(ui, ui->defer_rect, window->pos, type);
            window->pos.y += ui->window_entry_size;
        }
    }
    
    ++window->entry_count;
    
    if(is_entry_in_view(window->view, window->entry_count))
    {
        v2u picture_pos = render_window_tile(item->tile_src, window->pos, ui);
        
        v2u name_pos = get_window_entry_name_pos(picture_pos, ui);
        String8 letter = get_item_letter_string(item);
        String128 mark = get_item_mark_string(item);
        
        if(is_item_consumable(item->type))
        {
            if(is_set(item->flags, ItemFlag_IsIdentified))
            {
                defer_text(ui, "%s%s%s%s", name_pos.x, name_pos.y, letter.s, item->name.s, get_item_stack_string(item).s, mark.s);
            }
            else
            {
                defer_text(ui, "%s%s%s%s%s", name_pos.x, name_pos.y, letter.s, item->c.depiction.s, get_item_id_text(item->id), get_item_stack_string(item).s, mark.s);
            }
        }
        else
        {
            if(is_set(item->flags, ItemFlag_IsIdentified))
            {
                String16 equipped_string = {0};
                
                if(is_set(item->flags, ItemFlag_IsEquipped))
                {
                    sprintf(equipped_string.s, " (equipped)");
                }
                
                defer_text(ui, "%s%s%s%s%s%s",
                           name_pos.x, name_pos.y,
                           get_item_status_color(item),
                           letter.s,
                           get_item_status_prefix(item),
                           get_full_item_name(item).s,
                           equipped_string.s,
                           mark.s);
            }
            else
            {
                defer_text(ui, "%s%s%s", name_pos.x, name_pos.y, letter.s, get_item_id_text(item->id), mark.s);
            }
        }
        
        window->pos.y += ui->window_entry_size;
    }
                       }

internal u32
get_defer_rect_set_width()
{
    u32 result = 512;
    return(result);
}

internal LetterParent
get_letter_parent(Letter *letters, char c)
{
    assert(c);
    
    LetterParent result = {0};
    
    for(u32 i = 0; i < MAX_SELECT_LETTER_COUNT; ++i)
    {
        Letter *letter = &letters[i];
        
        if(letter->parent_type &&
           letter->c == c)
        {
            result.type = letter->parent_type;
            
            switch(letter->parent_type)
            {
                case LetterParentType_Entity:
                {
                    assert(0);
                } break;
                
                case LetterParentType_Item:
                {
                    result.item = letter->item;
                } break;
                
                case LetterParentType_Trap:
                {
                    result.trap = letter->trap;
                } break;
                
                invalid_default_case;
            }
            
            break;
        }
    }
    
    return(result);
    }

internal void
reset_letters(Letter *letters)
{
    for(u32 i = 0; i < MAX_SELECT_LETTER_COUNT; ++i)
    {
        Letter *letter = &letters[i];
        
        if(letter->parent_type)
        {
            assert(letter->c);
            
            switch(letter->parent_type)
            {
                case LetterParentType_Entity:
                {
                    assert(letter->entity->select_letter);
                    
                    letter->entity->select_letter = 0;
                } break;
                
                case LetterParentType_Item:
                {
                    assert(letter->item->select_letter);
                    
                    letter->item->select_letter = 0;
                } break;
                
                case LetterParentType_Trap:
                {
                    assert(letter->trap->select_letter);
                    
                    letter->trap->select_letter = 0;
                } break;
                
                invalid_default_case;
            }
            
            letter->parent_type = LetterParentType_None;
            }
    }
}

internal void
set_letter(Letter *letters, void *parent, LetterParentType parent_type)
{
    for(u32 i = 0; i < MAX_SELECT_LETTER_COUNT; ++i)
    {
        Letter *letter = &letters[i];
        
        if(!letter->parent_type)
        {
            if(!letter->c)
            {
                u32 alphabet_size = 26;
                
                for(u32 i = 0; i < alphabet_size * 2; ++i)
                {
                    if(i < alphabet_size)
                    {
                        letter->c = 97 + i;
                    }
                    else
                    {
                        letter->c = 65 + (i - alphabet_size);
                    }
                }
            }
            assert(is_alpha(letter->c));
            
            letter->parent_type = parent_type;
            switch(letter->parent_type)
            {
                
                #if 0
                case SelectLetterParent_Entity:
                {
                    assert(0);
                } break;
                #endif
                
                case LetterParentType_Item:
                {
                    Item *item = parent;
                    
                    letter->item = item;
                    item->select_letter = letter->c;
                    
                    #if 0
                    printf("Select letter char: %c\n", select_letter->c);
                    printf("Item char: %c\n\n", item->select_letter);
                    #endif
                    
                    } break;
                
                case LetterParentType_Trap:
                {
                     DungeonTrap *trap = parent;
                    
                    letter->trap = trap;
                    trap->select_letter = letter->c;
                } break;
                
                invalid_default_case;
            }
            
            return;
        }
        }
    
    assert(0);
}

internal void
ui_print_view(char *name, View view)
{
    printf("%s - count: %u\n", name, view.count);
    printf("%s - start: %u\n", name, view.start);
    printf("%s - end: %u\n\n", name, view.end);
}

internal void
ui_next_line(UI *ui, v2u *pos, u32 multiplier)
{
    pos->y += (ui->font_newline * multiplier);
}

internal b32
can_render_window_item(Item *item, ItemType type, u32 dungeon_level, v2u pos)
{
    b32 result = false;
    
    if(is_item_valid_and_not_in_inventory(item, dungeon_level) &&
       item->type == type &&
       is_v2u_equal(item->pos, pos))
    {
        result = true;
    }
    
    return(result);
}

internal void
log_add_okay(UI *ui)
{
    log_add(ui, "%sOkay.", start_color(Color_Yellow));
}

internal void
render_window_option(UI *ui, char *text, v2u *pos)
{
    defer_text(ui, text, pos->x, pos->y);
    pos->x += get_text_width(ui->font, text) + 10;
    }

internal u32
get_centering_offset(u32 total_size, u32 part_size)
{
    assert(total_size > part_size);
    
    u32 result = (total_size - part_size) / 2;
    return(result);
}

internal u32
get_ui_padding()
{
    u32 result = 10;
    return(result);
}

internal u32
get_view_range(View view)
{
    u32 result = view.start + view.end;
    return(result);
}

internal b32
is_view_scrolling(View view, u32 count)
{
    b32 result = view.end && (count > view.end);
    return(result);
}

internal void
set_view_at_start(View *view)
{
    view->start = 1;
}

internal void
set_view_at_end(View *view)
{
    view->start = view->count - view->end + 1;
}

internal void
init_view_end(View *view, u32 entry_count)
{
    if(!view->end)
    {
        view->end = entry_count;
    }
}

internal b32
item_fits_using_item_type(ItemUseType type, Item *item)
{
    if(type == UsingItemType_EnchantArmor && item->type == ItemType_Armor) return(true);
    else if(type == UsingItemType_EnchantWeapon && item->type == ItemType_Weapon) return(true);
    else if(type == UsingItemType_Identify && !is_set(item->flags, ItemFlag_IsIdentified)) return(true);
    else if(type == UsingItemType_Uncurse && is_set(item->flags, ItemFlag_IsIdentified | ItemFlag_IsCursed)) return(true);
    
    return(false);
}

internal v2u
get_examine_window_header_pos(UI *ui, v2u pos)
{
    v2u result = pos;
    
    result.x += ui->window_offset * 4;
    result.y += ui->font->size / 2;
    
    return(result);
}

internal void
move_view_towards_start(View *view)
{
    if(!is_zero(view->start - 1))
    {
        --view->start;
    }
}

internal void
update_view_scrolling(View *view, Input *input)
{
    if(was_pressed(&input->Button_ScrollUp))
    {
        move_view_towards_start(view);
    }
    else if(was_pressed(&input->Button_ScrollDown))
    {
        if(get_view_range(*view) <= view->count)
        {
            ++view->start;
        }
    }
    else if(was_pressed(&input->Key_Home))
    {
        if(is_view_scrolling(*view, view->count))
        {
        set_view_at_start(view);
        }
    }
    else if(was_pressed(&input->Key_End))
    {
        if(is_view_scrolling(*view, view->count))
        {
        set_view_at_end(view);
        }
    }
}

internal b32
can_render_multiple_examine_item(Item *item, ItemType type, v2u examine_pos, u32 dungeon_level)
{
    b32 result = (is_item_valid_and_not_in_inventory(item, dungeon_level) &&
     item->type == type &&
                      is_v2u_equal(item->pos, examine_pos));
    
    return(result);
}

internal b32
can_render_inventory_item(Item *item, ItemType type, u32 dungeon_level)
{
    b32 result = (is_item_valid_and_in_inventory(item, dungeon_level) && item->type == type);
    return(result);
}

internal b32
can_add_defer_rect_entry(v2u pos, u32 window_entry_size, u32 window_scroll_start_y)
{
    b32 result = (pos.y + (window_entry_size * 2) < window_scroll_start_y);
    return(result);
}

internal void
add_defer_rect_window_entry(DeferRectWindow *window, UI *ui)
{
    if(can_add_defer_rect_entry(window->pos, ui->window_entry_size, ui->window_scroll_start_y))
    {
        window->pos.y += ui->window_entry_size;
    }
    else
    {
        init_view_end(&window->view, window->entry_count);
    }
    
    ++window->entry_count;
}

internal void
render_scrollbar_on_rect(Game *game, UI *ui, v4u rect, View *view)
{
    assert(game);
    assert(ui);
    assert(rect.w);
    assert(rect.h);
    
    if(view && is_view_scrolling(*view, view->count))
    {
        u32 rect_gutter_x = rect.x + rect.w - get_ui_padding();
        
        v4u gutter = {0};
        gutter.x = rect_gutter_x;
        gutter.y = rect.y;
        gutter.w = 2;
        
        // Get the correct gutter height
        u32 scrollbar_size = (rect.h - (ui->font_newline * 4)) / view->count;
        gutter.h = scrollbar_size * view->count;
        
        // Center gutter vertically relative to rect
        gutter.y += get_centering_offset(rect.h, gutter.h);
        render_fill_rect(game, gutter, Color_WindowAccent);
        
        v4u scrollbar = {0};
        scrollbar.x = rect_gutter_x;
        scrollbar.y = gutter.y + (scrollbar_size * (view->start - 1));
        scrollbar.w = gutter.w;
        scrollbar.h = scrollbar_size * view->end;
        render_fill_rect(game, scrollbar, Color_WindowBorder);
    }
}

internal v4u
get_border_adjusted_rect(v4u rect, u32 border_size)
{
    v4u result =
    {
        rect.x + border_size,
        rect.y + border_size,
        rect.w - (border_size * 2),
        rect.h - (border_size * 2)
    };
    
    return(result);
}

internal void
center_rect_to_window(v2u window_size, Assets *assets, v4u *rect)
{
    if(rect->w && rect->w)
    {
    rect->x = (window_size.w / 2) - (rect->w / 2);
        rect->y = get_centering_offset(window_size.h - assets->stat_and_log_window_h, rect->h);
        
        // This is so the rectangle can still be seen even if its wrong
        if(rect->y > window_size.h)
        {
            rect->y = 0;
        }
    }
}

internal void
render_centered_rect(Game *game, Assets *assets, v4u *rect)
{
    center_rect_to_window(game->window_size, assets, rect);
    render_rect(game, *rect, 2);
}

internal void
end_defer_rect(Game *game, Assets *assets, UI *ui, View *view, v2u pos)
{
    ui->defer_rect.h = pos.y;
    
    render_centered_rect(game, assets, &ui->defer_rect);
    render_scrollbar_on_rect(game, ui, ui->defer_rect, view);
    
    render_defer(game, ui, assets->tileset);
}

internal void
end_defer_rect_window(Game *game, Assets *assets, UI *ui, DeferRectWindow *window)
{
    window->view.count = window->entry_count;
    ui_next_line(ui, &window->pos, 1);
    end_defer_rect(game, assets, ui, &window->view, window->pos);
}

internal void
render_entity_examine_window(Game *game, Entity *entity, Assets *assets, UI *ui)
{
    assert(is_entity_valid_and_enemy(entity));
    
    v2u pos = start_defer_rect(ui, 1);
    
    // Render entity stats
    v2u name = get_examine_window_header_pos(ui, pos);
    
    if(is_set(entity->flags, EntityFlag_IsInvisible))
    {
        defer_texture(ui, pos, get_dungeon_tileset_rect(DungeonTileID_EntityUnknown));
        
        defer_text(ui, "Something", name.x, name.y);
        ui_next_line(ui, &pos, 2);
        
        ui_next_line(ui, &pos, 1);
        defer_text(ui, "An unknown being.", pos.x, pos.y);
        
        ui_next_line(ui, &pos, 2);
    }
    else
    {
        defer_texture(ui, pos, entity->tile_src);
        
        defer_text(ui, "%s", name.x, name.y, entity->name.s);
        ui_next_line(ui, &pos, 2);
        
        ui_next_line(ui, &pos, 1);
        defer_text(ui, "Max HP: %u", pos.x, pos.y, entity->max_hp);
        
        ui_next_line(ui, &pos, 1);
        defer_text(ui, "Damage: %u", pos.x, pos.y, entity->e.damage);
        
        ui_next_line(ui, &pos, 1);
        defer_text(ui, "Defence: %u", pos.x, pos.y, entity->defence);
        
        char evasion_text[24] = {0};
        if(entity->evasion <= 3)
        {
            strcpy(evasion_text, "Very Low");
        }
        else if(entity->evasion <= 7)
        {
            strcpy(evasion_text, "Low");
        }
        else if(entity->evasion <= 13)
        {
            strcpy(evasion_text, "Average");
        }
        else if(entity->evasion <= 17)
        {
            strcpy(evasion_text, "High");
        }
        else
        {
            strcpy(evasion_text, "Very High");
        }
        
        ui_next_line(ui, &pos, 1);
        defer_text(ui, "Evasion: %s (%u)", pos.x, pos.y, evasion_text, entity->evasion);
        
        ui_next_line(ui, &pos, 1);
        defer_text(ui, "Speed: %.01f", pos.x, pos.y, entity->action_time);
        
        // Find out the longest damage type length and damage type resistance so that
        // we can align text correctly.
        u32 longest_damage_type_length = 0;
        s32 highest_damage_type_resistance = 0;
        for(u32 damage_type_index = 0; damage_type_index < DamageType_Count; ++damage_type_index)
        {
            s32 resistance = entity->resistances[damage_type_index];
            if(resistance != 0)
            {
                char *damage_type_text = get_damage_type_text(damage_type_index);
                u32 current_damage_type_length = get_string_length(damage_type_text);
                
                if(longest_damage_type_length < current_damage_type_length)
                {
                    longest_damage_type_length = current_damage_type_length;
                }
                
                u32 absolute_resistance = get_absolute(resistance);
                if(highest_damage_type_resistance < absolute_resistance)
                {
                    highest_damage_type_resistance = absolute_resistance;
                }
            }
        }
        
#if 0
        printf("longest_damage_type_length: %u\n", longest_damage_type_length);
        printf("highest_damage_type_resistance: %u\n", highest_damage_type_resistance);
#endif
        
        // Render entity damage type resistances
        b32 render_prefix_text = true;
        for(u32 damage_type_index = 0; damage_type_index < DamageType_Count; ++damage_type_index)
        {
            s32 resistance = entity->resistances[damage_type_index];
            if(resistance != 0)
            {
                if(render_prefix_text)
                {
                    ui_next_line(ui, &pos, 2);
                    defer_text(ui, "It has the following resistances:", pos.x, pos.y);
                    
                    render_prefix_text = false;
                }
                
                // This sets the padding after the damage_type_text, we do this so that
                // the resistance_text_value's start aligned.
                char *damage_type_text = get_damage_type_text(damage_type_index);
                
                String32 damage_type_text_padding = {0};
                u32 damage_type_text_padding_length = longest_damage_type_length - get_string_length(damage_type_text);
                
                for(u32 index = 0; index < damage_type_text_padding_length; ++index)
                {
                    damage_type_text_padding.s[index] = ' ';
                }
                
                // Set the character we use based on if value is positive or negative.
                b32 add_extra_resistance_space = false;
                char resistance_char = '-';
                
                if(resistance > 0)
                {
                    add_extra_resistance_space = true;
                    resistance_char = '+';
                }
                
                u32 absolute_resistance = get_absolute(resistance);
                
                // Copy as many of the correct characters as we need into resistance_text_value.
                String8 resistance_value_string = {0};
                
                for(u32 string_index = 0; string_index < absolute_resistance; ++string_index)
                {
                    resistance_value_string.s[string_index] = resistance_char;
                }
                
                // This sets the padding at the end of resistance_text_value, we do this so that
                // the rendered resistance values start aligned.
                u32 length = highest_damage_type_resistance - absolute_resistance;
                u32 padding_length = absolute_resistance + length;
                for(u32 padding_index = absolute_resistance; padding_index < padding_length; ++padding_index)
                {
                    resistance_value_string.s[padding_index] = ' ';
                }
                
                // Positive values need an extra space, hence the boolean for it.
                if(add_extra_resistance_space)
                {
                    resistance_value_string.s[padding_length] = ' ';
                }
                
                ui_next_line(ui, &pos, 1);
                defer_text(ui, "%s: %s%s (%d)",
                           pos.x, pos.y,
                           damage_type_text,
                           damage_type_text_padding.s,
                           resistance_value_string.s,
                           resistance);
            }
        }
        
        // Render entity status effects
        if(entity_has_any_status_effect(entity))
        {
            ui_next_line(ui, &pos, 2);
            defer_text(ui, "It is under the following status effects:", pos.x, pos.y);
            
            for(u32 index = StatusEffectType_None + 1; index < StatusEffectType_Count; ++index)
            {
                if(entity_has_status_effect(entity, index))
                {
                    ui_next_line(ui, &pos, 1);
                    defer_text(ui, "%s", pos.x, pos.y, get_status_effect_name(index));
                }
            }
        }
        
        // Render entity spells
        char spell_letter = 'a';
        if(is_set(entity->flags, EntityFlag_UsesMagicAttacks))
        {
            ui_next_line(ui, &pos, 2);
            defer_text(ui, "It has the following spells:", pos.x, pos.y);
            
            for(u32 spell_index = 0; spell_index < MAX_ENTITY_SPELL_COUNT; ++spell_index)
            {
                Spell *spell = &entity->e.spells[spell_index];
                if(spell->id)
                {
                    ui_next_line(ui, &pos, 1);
                    defer_text(ui, "%c - %s", pos.x, pos.y, spell_letter, get_spell_name(spell->id));
                    
                    ++spell_letter;
                }
            }
            
            ui_next_line(ui, &pos, 2);
            defer_text(ui, "Press the key shown to read a description.", pos.x, pos.y);
        }
        
        ui_next_line(ui, &pos, 2);
    }
    
    end_defer_rect(game, assets, ui, 0, pos);
        }

internal void
render_entity_spell_examine_window(Game *game,
                                   Entity *entity,
                                   Spell *spell,
                                   Assets *assets,
                                   UI *ui,
                                   v2u player_pos)
{
    assert(is_entity_valid_and_enemy(entity));
    assert(spell->type);
    
    v2u pos = start_defer_rect(ui, 1);
    
    defer_text(ui, get_spell_name(spell->id), pos.x, pos.y);
    ui_next_line(ui, &pos, 2);
    
    defer_text(ui, get_spell_description(spell->id), pos.x, pos.y);
    ui_next_line(ui, &pos, 2);
    
    switch(spell->type)
    {
        case SpellType_Offensive:
        {
            defer_text(ui, "Damage Type: %s", pos.x, pos.y, get_damage_type_text(spell->damage_type));
            ui_next_line(ui, &pos, 1);
            
            defer_text(ui, "Damage: %u", pos.x, pos.y, spell->value);
            ui_next_line(ui, &pos, 1);
        } break;
        
        case SpellType_Healing:
        {
            defer_text(ui, "Healing: %u", pos.x, pos.y, spell->value);
            ui_next_line(ui, &pos, 1);
        } break;
        
        case SpellType_Buff:
        {
            defer_text(ui, "Increase: %u", pos.x, pos.y, spell->value);
            ui_next_line(ui, &pos, 1);
            
            defer_text(ui, "Duration: %u", pos.x, pos.y, spell->duration);
            ui_next_line(ui, &pos, 1);
        } break;
        
        invalid_default_case;
    }
    
    char spell_range_text[24] = {0};
    if(spell->range)
    {
        sprintf(spell_range_text, "Range: %u", spell->range);
    }
    else
    {
        sprintf(spell_range_text, "Range: Line of sight");
    }
    
    char in_spell_range_text[24] = {0};
    if(in_spell_range(spell->range, entity->pos, player_pos))
    {
        sprintf(in_spell_range_text, "(You are in range)");
    }
    
    defer_text(ui, "%s %s", pos.x, pos.y, spell_range_text, in_spell_range_text);
    ui_next_line(ui, &pos, 2);
    
    end_defer_rect(game, assets, ui, 0, pos);
}

internal void
render_tile_examine_window(Game *game, DungeonTileID tile, Assets *assets, UI *ui)
{
    assert(tile);
    
    v2u pos = start_defer_rect(ui, 1);
    
    defer_texture(ui, pos, get_dungeon_tileset_rect(tile));
    
    v2u header = get_examine_window_header_pos(ui, pos);
    defer_text(ui, "%s", header.x, header.y, get_dungeon_tile_name(tile));
    ui_next_line(ui, &pos, 3);
    
    char *text = get_dungeon_tile_info_text(tile);
    if(text)
    {
        defer_text(ui, text, pos.x, pos.y);
        ui_next_line(ui, &pos, 2);
    }
    
    end_defer_rect(game, assets, ui, 0, pos);
    }

internal void
render_trap_examine_window(Game *game, DungeonTrap *trap, Assets *assets, UI *ui)
{
    assert(trap);
    
    v2u pos = start_defer_rect(ui, 1);
    
    defer_texture(ui, pos, trap->tile_src);
    
    v2u header = get_examine_window_header_pos(ui, pos);
    defer_text(ui, trap->name.s, header.x, header.y);
    ui_next_line(ui, &pos, 3);
    
    defer_text(ui, trap->description.s, pos.x, pos.y);
    ui_next_line(ui, &pos, 2);
    
    end_defer_rect(game, assets, ui, 0, pos);
}

internal void
render_item_examine_window(Game *game, Item *item, Assets *assets, UI *ui, b32 show_inventory_options)
{
    assert(item);
    
    v2u pos = start_defer_rect(ui, 1);
    
    // Render item picture and name
    defer_texture(ui, pos, item->tile_src);
    
    v2u header = get_examine_window_header_pos(ui, pos);
    defer_text(ui, "%s%s%s%s%s%s",
               header.x, header.y,
               get_item_status_color(item),
                   get_item_letter_string(item).s,
               get_item_status_prefix(item),
                   get_full_item_name(item).s,
                   get_item_stack_string(item).s,
               get_item_mark_string(item).s);
    
    ui_next_line(ui, &pos, 2);
    
    if(is_set(item->flags, ItemFlag_IsIdentified))
    {
        if(item->type == ItemType_Weapon)
        {
            ui_next_line(ui, &pos, 1);
            defer_text(ui, "Damage: %d", pos.x, pos.y, item->w.damage + item->enchantment_level);
            
            ui_next_line(ui, &pos, 1);
            defer_text(ui, "Accuracy: %d", pos.x, pos.y, item->w.accuracy + item->enchantment_level);
            
            ui_next_line(ui, &pos, 1);
            defer_text(ui, "Attack Speed: %.1f", pos.x, pos.y, item->w.speed);
        }
        else if(item->type == ItemType_Armor)
        {
            ui_next_line(ui, &pos, 1);
            defer_text(ui, "Defence: %d", pos.x, pos.y, item->a.defence + item->enchantment_level);
            
            ui_next_line(ui, &pos, 1);
            defer_text(ui, "Weight: %d", pos.x, pos.y, item->a.weight);
        }
        else if(is_item_consumable(item->type))
        {
            ui_next_line(ui, &pos, 1);
            defer_text(ui, "%s", pos.x, pos.y, item->description.s);
        }
    }
    else
    {
        if(item->type == ItemType_Weapon)
        {
            ui_next_line(ui, &pos, 1);
            defer_text(ui, "Base Damage: %u", pos.x, pos.y, item->w.damage);
            
            ui_next_line(ui, &pos, 1);
            defer_text(ui, "Base Accuracy: %d", pos.x, pos.y, item->w.accuracy);
            
            ui_next_line(ui, &pos, 1);
            defer_text(ui, "Base Attack Speed: %.1f", pos.x, pos.y, item->w.speed);
        }
        else if(item->type == ItemType_Armor)
        {
            ui_next_line(ui, &pos, 1);
            defer_text(ui, "Base Defence: %d", pos.x, pos.y, item->a.defence);
            
            ui_next_line(ui, &pos, 1);
            defer_text(ui, "Base Weight: %d", pos.x, pos.y, item->a.weight);
        }
        else if(item->type == ItemType_Potion)
        {
            ui_next_line(ui, &pos, 1);
            defer_text(ui, "Consuming potions will bestow you with different effects.", pos.x, pos.y);
            
            ui_next_line(ui, &pos, 1);
            defer_text(ui, "Some of these effects will be helpful, while others harmful.", pos.x, pos.y);
        }
        else if(item->type == ItemType_Scroll)
        {
            ui_next_line(ui, &pos, 1);
            defer_text(ui, "Reading scrolls will bring out different magical effects.", pos.x, pos.y);
        }
    }
    
    ui_next_line(ui, &pos, 2);
    
    if(is_set(item->flags, ItemFlag_IsIdentified | ItemFlag_IsCursed))
    {
        defer_text(ui, "It is a cursed item.", pos.x, pos.y);
        ui_next_line(ui, &pos, 1);
    }
    
    if(is_item_equipment(item->type))
    {
        defer_text(ui, "It is of %s rarity.", pos.x, pos.y, get_item_rarity_text(item->rarity));
        
            if(item->type == ItemType_Weapon)
        {
            ui_next_line(ui, &pos, 1);
            defer_text(ui, "It is a %s weapon.", pos.x, pos.y, get_item_handedness_text(item->w.handedness));
        }
        
        ui_next_line(ui, &pos, 2);
    }
    
    if(show_inventory_options)
    {
        u32 padding = get_ui_padding();
        render_window_option(ui, "(a)djust", &pos);
        
        if(is_item_equipment(item->type))
        {
            if(is_set(item->flags, ItemFlag_IsEquipped))
            {
                render_window_option(ui, "(u)nequip", &pos);
            }
            else
            {
                render_window_option(ui, "(e)quip", &pos);
            }
        }
        else if(item->type == ItemType_Potion ||
                item->type == ItemType_Ration)
        {
            render_window_option(ui, "(c)onsume", &pos);
        }
        else if(item->type == ItemType_Scroll)
        {
            render_window_option(ui, "(r)ead", &pos);
        }
        
        render_window_option(ui, "(d)rop", &pos);
        render_window_option(ui, "(m)ark", &pos);
        
        ui_next_line(ui, &pos, 2);
    }
    
    end_defer_rect(game, assets, ui, 0, pos);
}

internal b32
is_entry_in_view(View view, u32 entry)
{
    b32 result = (!view.end || (entry >= view.start && entry < get_view_range(view)));
    return(result);
}

internal u32
get_font_newline(u32 font_size)
{
    u32 result = (u32)(font_size * 1.15f);
    return(result);
}

internal void
update_log_view(View *view, u32 index)
{
    u32 message_count = (view->end - 1);
    if(index > message_count)
    {
        view->start = (index - message_count);
    }
}

internal void
log_add(UI *ui, char *text, ...)
{
    String128 text_format = {0};
    
    va_list arg_list;
    va_start(arg_list, text);
    vsnprintf(text_format.s, sizeof(text_format), text, arg_list);
    va_end(arg_list);
    
    // Set the new log message to an unused index if possible
    for(u32 index = 0; index < MAX_LOG_MESSAGE_COUNT; ++index)
    {
        update_log_view(&ui->full_log_view, index);
        update_log_view(&ui->short_log_view, index);
        
        LogMessage *message = &ui->log_messages[index];
        if(!message->is_set)
        {
            message->is_set = true;
            strcpy(message->string, text_format.s);
            
            return;
        }
    }
    
    // Move all log messages up by one
    for(u32 index = 1; index < MAX_LOG_MESSAGE_COUNT; ++index)
    {
        LogMessage *current_message = &ui->log_messages[index];
        LogMessage *last_message = &ui->log_messages[index - 1];
        
        assert(current_message->is_set);
        assert(last_message->is_set);
        
        strcpy(last_message->string, current_message->string);
    }
    
    // Set the new log message to the bottom
    LogMessage *new_message = &ui->log_messages[MAX_LOG_MESSAGE_COUNT - 1];
    strcpy(new_message->string, text_format.s);
}

internal void
render_inventory_window(Game *game,
                   ItemState *items,
                       Inventory *inventory,
                       u32 dungeon_level,
                        Assets *assets,
                        UI *ui)
{
    Examine *examine = &game->examine;
    DeferRectWindow *window = &inventory->window;
    
    assert(window->view.start);
    
    { // Update inventory view if an item was used
        if(inventory->view_update_item_type)
        {
            b32 item_with_same_type_exists = false;
            
            for(u32 index = 0; index < MAX_INVENTORY_SLOT_COUNT; ++index)
            {
                Item *inventory_item = inventory->slots[index];
                if(inventory_item &&
                   inventory_item->type == inventory->view_update_item_type)
                {
                    item_with_same_type_exists = true;
                    break;
                }
            }
            
            if(item_with_same_type_exists)
            {
                --window->view.count;
            }
            else
            {
                // Take away the item and the item header from view count
                window->view.count -= 2;
            }
            
            inventory->view_update_item_type = ItemType_None;
        }
        
        if(is_view_scrolling(window->view, window->view.count))
        {
            // Set the view at end if something was removed from the bottom of the inventory
            if(get_view_range(window->view) > window->view.count)
            {
                set_view_at_end(&window->view);
            }
        }
        else
        {
            set_view_at_start(&window->view);
        }
    }
    
    // Find and set the end for the inventory view
    if(!window->view.end)
    {
        v2u test_pos = {0};
        u32 test_entry_count = 0;
        
        for(u32 type = ItemType_Weapon; type < ItemType_Count; ++type)
        {
            b32 needs_header = true;
            
            for(u32 index = 0; index < MAX_INVENTORY_SLOT_COUNT; ++index)
            {
                Item *item = inventory->slots[index];
                
                    if(can_render_inventory_item(item, type, dungeon_level))
                    {
                    if(needs_header)
                        {
                        needs_header = false;
                        add_defer_rect_window_entry(window, ui);
                    }
                    
                    add_defer_rect_window_entry(window, ui);
                    }
            }
        }
    }
    
    init_defer_rect_window(window, ui);
    ui->defer_rect.w = get_defer_rect_set_width();
    
    // Render inventory header
        if(inventory->item_use_type)
        {
            switch(inventory->item_use_type)
            {
            case UsingItemType_Identify: defer_text(ui, "Identify which item?", window->pos.x, window->pos.y); break;
            case UsingItemType_EnchantWeapon: defer_text(ui, "Enchant which weapon?", window->pos.x, window->pos.y); break;
            case UsingItemType_EnchantArmor: defer_text(ui, "Enchant which armor?", window->pos.x, window->pos.y); break;
            case UsingItemType_Uncurse: defer_text(ui, "Uncurse which item?", window->pos.x, window->pos.y); break;
                
                invalid_default_case;
            }
        }
        else
        {
        defer_text(ui, "Inventory: %u/%u slots", window->pos.x, window->pos.y, get_inventory_item_count(inventory), MAX_INVENTORY_SLOT_COUNT);
        }
    
    ui_next_line(ui, &window->pos, 1);
    
    // Render inventory items
            for(u32 type = ItemType_Weapon; type < ItemType_Count; ++type)
            {
        b32 needs_header = true;
        
        for(u32 index = 0; index < MAX_INVENTORY_SLOT_COUNT; ++index)
        {
            Item *item = inventory->slots[index];
            
                   if(can_render_inventory_item(item, type, dungeon_level) &&
                   (!inventory->item_use_type || item_fits_using_item_type(inventory->item_use_type, item)))
            {
                render_defer_rect_item(ui, window, item, type, &needs_header);
                }
                }
            }
    
    end_defer_rect_window(game, assets, ui, window);
    
#if 0
    ui_print_view("Inventory View", *view);
#endif
    
}

internal void
render_examine_window(Game *game, Assets *assets, UI *ui, v2u player_pos)
{
    Examine *examine = &game->examine;
    
    if(examine->type)
    {
        switch(examine->type)
        {
            case ExamineType_Item: render_item_examine_window(game, examine->item, assets, ui, false); break;
            case ExamineType_Entity: render_entity_examine_window(game, examine->entity, assets, ui); break;
            case ExamineType_Spell: render_entity_spell_examine_window(game, examine->entity, examine->spell, assets, ui, player_pos); break;
            case ExamineType_Trap: render_trap_examine_window(game, examine->trap, assets, ui); break;
            case ExamineType_Tile: render_tile_examine_window(game, examine->tile, assets, ui); break;
            
            invalid_default_case;
        }
    }
    else
    {
        v4u dest = get_game_dest(game, examine->pos);
        SDL_RenderCopy(game->renderer, assets->ui.tex, (SDL_Rect *)&assets->yellow_outline_src, (SDL_Rect *)&dest);
    }
}

internal void
render_multiple_examine_window(Game *game,
                               EntityState *entities,
                               ItemState *items,
                               Inventory *inventory,
                               Dungeon *dungeon,
                               Assets *assets,
                               UI *ui)
{
    Examine *examine = &game->examine;
    DeferRectWindow *window = &inventory->examine_window;
    
    // Find and set the end for the multiple examine view
    if(!window->view.end)
    {
        v2u test_pos = {0};
        u32 test_entry_count = 0;
        
        for(ExamineType examine_type = ExamineType_None + 1;
            examine_type < ExamineType_Count;
            ++examine_type)
        {
            if(examine_type == ExamineType_Entity)
            {
                // TODO(rami): Probably same as trap branch
            }
            else if(examine_type == ExamineType_Item)
            {
                for(u32 type = ItemType_Weapon; type < ItemType_Count; ++type)
                {
                    b32 needs_header = true;
                    
                    for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
                    {
                        Item *item = &items->array[index];
                        
                            if(can_render_multiple_examine_item(item, type, examine->pos, dungeon->level))
                        {
                            if(needs_header)
                            {
                                needs_header = false;
                                add_defer_rect_window_entry(window, ui);
                            }
                            
                            add_defer_rect_window_entry(window, ui);
                        }
                    }
                }
            }
                else if(examine_type == ExamineType_Trap)
            {
                DungeonTrap *trap = get_dungeon_pos_trap(dungeon->tiles, &dungeon->traps, examine->pos);
                if(trap)
                {
                    add_defer_rect_window_entry(window, ui);
                    add_defer_rect_window_entry(window, ui);
                }
            }
        }
    }
    
    init_defer_rect_window(window, ui);
    ui->defer_rect.w = get_defer_rect_set_width();
    
    defer_text(ui, "Examine what?", window->pos.x, window->pos.y);
    ui_next_line(ui, &window->pos, 1);
    
    for(ExamineType examine_type = ExamineType_None + 1;
        examine_type < ExamineType_Count;
        ++examine_type)
    {
        if(examine_type == ExamineType_Entity)
        {
            // TODO(rami): Needs to check for view and increase entry_count
            
            Entity *enemy = get_dungeon_pos_entity(entities, dungeon->level, examine->pos, true);
            if(enemy)
            {
                v2u header_pos = render_window_separator(ui->defer_rect, window->pos, ui);
                defer_text(ui, "Enemies", header_pos.x, header_pos.y);
                
                window->pos.y += ui->window_entry_size;
                
                v2u picture_pos = render_window_tile(enemy->tile_src, window->pos, ui);
                v2u name_pos = get_window_entry_name_pos(picture_pos, ui);
                defer_text(ui, "%s%s", name_pos.x, name_pos.y, "? - ", enemy->name.s);
                
                window->pos.y += ui->window_entry_size;
            }
        }
        else if(examine_type == ExamineType_Item)
        {
            for(u32 type = ItemType_Weapon; type < ItemType_Count; ++type)
            {
                b32 needs_header = true;
                
                for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
                {
                    Item *item = &items->array[index];
                    
                        if(can_render_multiple_examine_item(item, type, examine->pos, dungeon->level))
                    {
                        render_defer_rect_item(ui, window, item, type, &needs_header);
                    }
                }
            }
        }
        else if(examine_type == ExamineType_Trap)
        {
            DungeonTrap *trap = get_dungeon_pos_trap(dungeon->tiles, &dungeon->traps, examine->pos);
            if(trap)
            {
                ++window->entry_count;
                
                if(is_entry_in_view(window->view, window->entry_count))
                {
                    v2u header_pos = render_window_separator(ui->defer_rect, window->pos, ui);
                    defer_text(ui, "Traps", header_pos.x, header_pos.y);
                    window->pos.y += ui->window_entry_size;
                }
                
                ++window->entry_count;
                
                if(is_entry_in_view(window->view, window->entry_count))
                {
                    v2u picture_pos = render_window_tile(trap->tile_src, window->pos, ui);
                    v2u name_pos = get_window_entry_name_pos(picture_pos, ui);
                    
                    if(!trap->select_letter)
                    {
                        set_letter(ui->letters, trap, LetterParentType_Trap);
                    }
                    
                    // TODO(rami): A function to get the "%c - "
                    defer_text(ui, "%c - %s", name_pos.x, name_pos.y, trap->select_letter, trap->name.s);
                    
                    window->pos.y += ui->window_entry_size;
                }
            }
            }
            }
    
    end_defer_rect_window(game, assets, ui, window);
    
    #if 0
    ui_print_view("Multiple Examine", *view);
#endif
    
    }

internal void
render_item_mark_window(Game *game, Item *item, Assets *assets, UI *ui)
{
    v4u mark_rect = {0, 0, 250, 100};
    render_centered_rect(game, assets, &mark_rect);
    
    { // Render header
        char *header_text = "Mark with what?";
        if(is_set(item->flags, ItemFlag_IsMarked))
        {
            header_text = "Replace mark with what?";
        }
        
        v2u header_pos =
        {
            mark_rect.x + get_centering_offset(mark_rect.w, get_text_width(ui->font, header_text)),
            mark_rect.y + 25
        };
        
        render_text(game, header_text, header_pos.x, header_pos.y, ui->font, 0);
    }
    
    // Render input box
    u32 height_padding = 4;
    v4u input_rect = {mark_rect.x, mark_rect.y, ui->font->size * 14, ui->font->size + height_padding};
    
    input_rect.x += get_centering_offset(mark_rect.w, input_rect.w);
    input_rect.y += ui->font_newline * 3;
    
    render_fill_rect(game, input_rect, Color_WindowAccent);
    render_outline_rect(game, input_rect, Color_WindowBorder);
    
    // Update cursor
    Mark *mark = &ui->mark;
    
    if(!mark->cursor_render_start)
    {
        mark->cursor_render_start = SDL_GetTicks();
    }
    
    if(get_sdl_ticks_difference(mark->cursor_render_start) >= mark->cursor_blink_duration)
    {
        mark->cursor_render_start = 0;
        mark->render_cursor = !mark->render_cursor;
    }
    
    // Render input
    v2u text_pos =
    {
        input_rect.x + 4,
        input_rect.y + get_centering_offset(input_rect.h, ui->font->size) + 1
    };
    
    u32 cursor_x = text_pos.x;
    v2u character_pos = text_pos;
    for(u32 index = mark->view.start; index < get_view_range(mark->view); ++index)
    {
        u32 mark_index = index - 1;
        
        render_text(game, "%c", character_pos.x, character_pos.y, ui->font, 0, mark->array[mark_index]);
        character_pos.x += get_glyph_width(ui->font, mark->array[mark_index]);
        
        if(mark->render_cursor && (index == mark->cursor_index))
        {
            cursor_x = character_pos.x;
        }
    }
    
    // Render cursor
    if(mark->render_cursor)
    {
        v4u cursor_rect =
        {
            cursor_x,
            input_rect.y + (height_padding / 2),
            1,
            input_rect.h - height_padding
        };
        
        render_fill_rect(game, cursor_rect, Color_White);
    }
    
#if 0
    ui_print_view("Mark View", mark->view);
#endif
    
}

internal void
render_ui(Game *game,
          Input *input,
          EntityState *entities,
          ItemState *items,
          Inventory *inventory,
           Dungeon *dungeon,
          Assets *assets,
          UI *ui)
{
    Examine *examine = &game->examine;
    Entity *player = get_player_entity(entities);
    
    ui->window_scroll_start_y = game->window_size.h - assets->stat_and_log_window_h;
    
    v4u stat_rect =
    {
        0,
        ui->window_scroll_start_y,
        388,
        assets->stat_and_log_window_h
    };
    render_rect(game, stat_rect, 2);
    
    v4u short_log_rect =
    {
        stat_rect.w + 4,
        ui->window_scroll_start_y,
        game->window_size.w - short_log_rect.x,
        assets->stat_and_log_window_h
    };
    render_rect(game, short_log_rect, 2);
    
    // Render stats
    v2u left =
    {
        ui->window_offset,
        (game->window_size.h - assets->stat_and_log_window_h) + ui->window_offset
    };
    
    v2u right =
    {
        left.x + 160,
        left.y
    };
    
    { // Left side
        render_text(game, player->name.s, left.x, left.y, ui->font, 0);
        
        ui_next_line(ui, &left, 1);
        render_text(game, "Health:    %u/%u", left.x, left.y, ui->font, 0, player->hp, player->max_hp);
        
        ui_next_line(ui, &left, 1);
        render_text(game, "Strength:     %u", left.x, left.y, ui->font, 0, player->strength);
        
        ui_next_line(ui, &left, 1);
        render_text(game, "Intelligence: %u", left.x, left.y, ui->font, 0, player->intelligence);
        
        ui_next_line(ui, &left, 1);
        render_text(game, "Dexterity:    %u", left.x, left.y, ui->font, 0, player->dexterity);
        
        ui_next_line(ui, &left, 1);
        render_text(game, "Evasion:      %u", left.x, left.y, ui->font, 0, player->evasion);
        
        ui_next_line(ui, &left, 1);
        render_text(game, "Defence:      %u", left.x, left.y, ui->font, 0, player->defence);
        
        ui_next_line(ui, &left, 1);
        render_text(game, "Weight:       %u", left.x, left.y, ui->font, 0, player->p.weight);
    }
    
    { // Right side
        
        { // Render healthbar
            right.y += ui->font_newline - 1;
            
            // Healthbar border
            v4u healthbar_outside = {right.x, right.y, 204, 16};
            render_fill_rect(game, healthbar_outside, Color_WindowBorder);
            
            // Healthbar background
            v4u healthbar_inside = get_border_adjusted_rect(healthbar_outside, 1);
            render_fill_rect(game, healthbar_inside, Color_WindowAccent);
            
            if(player->hp > 0)
            {
                healthbar_inside.w = get_ratio(player->hp, player->max_hp, healthbar_inside.w);
                render_fill_rect(game, healthbar_inside, Color_DarkRed);
            }
        }
        
        ui_next_line(ui, &right, 1);
        render_text(game, "Time:          %.01f", right.x, right.y, ui->font, 0, game->time);
        
        ui_next_line(ui, &right, 1);
        render_text(game, "Action Time:   %.01f", right.x, right.y, ui->font, 0, player->action_time);
        
        ui_next_line(ui, &right, 1);
        render_text(game, "Dungeon Level: %u", right.x, right.y, ui->font, 0, dungeon->level);
    }
    
    // Render Short Log
    assert(ui->short_log_view.end);
    
    v2u short_log_pos =
    {
        short_log_rect.x + ui->window_offset,
        short_log_rect.y + (ui->font->size / 2)
    };
    
    for(u32 index = ui->short_log_view.start;
            index < get_view_range(ui->short_log_view);
            ++index)
    {
        LogMessage *message = &ui->log_messages[index];
        if(message->is_set)
        {
            render_text(game, message->string, short_log_pos.x, short_log_pos.y, ui->font, 0);
            ui_next_line(ui, &short_log_pos, 1);
        }
    }
    
    // Render Full Log
    if(ui->full_log_open)
    {
        ui->full_log_view.count = 0;
        zero_struct(ui->defer_rect);
        
        v2u full_log_pos =
        {
            ui->defer_rect.x + ui->window_offset,
            ui->defer_rect.y + ui->window_offset
        };
        
        // Set full log view end index
        v2u test_message_pos = full_log_pos;
        
        for(u32 index = 0; index < MAX_LOG_MESSAGE_COUNT; ++index)
        {
            LogMessage *message = &ui->log_messages[index];
            
            if(message->is_set)
            {
                ++ui->full_log_view.count;
                
                if(can_add_defer_rect_entry(test_message_pos, ui->font_newline * 2, ui->window_scroll_start_y))
                {
                    ui_next_line(ui, &test_message_pos, 1);
                }
                else
                {
                    init_view_end(&ui->full_log_view, index + 1);
                }
            }
        }
        
        // Always set the log view at end if we're scrolling
        if(is_view_scrolling(ui->full_log_view, ui->full_log_view.count) &&
               !ui->full_log_at_end)
        {
            ui->full_log_at_end = true;
            set_view_at_end(&ui->full_log_view);
        }
        
        // Render log messages
        for(u32 index = 0; index < MAX_LOG_MESSAGE_COUNT; ++index)
        {
            LogMessage *message = &ui->log_messages[index];
            
            if(message->is_set)
            {
                if(is_entry_in_view(ui->full_log_view, index + 1))
                {
                    defer_text(ui, message->string, full_log_pos.x, full_log_pos.y);
                    ui_next_line(ui, &full_log_pos, 1);
                }
                else
                {
                    // Even if the entry is not in view, update the ui defer rect so that the
                    // full log width is the same as the longest log message.
                    update_defer_rect_width(full_log_pos.x, message->string, ui);
                }
            }
        }
        
        full_log_pos.y += (ui->font_newline / 2);
        end_defer_rect(game, assets, ui, &ui->full_log_view, full_log_pos);
        
        #if 0
        ui_print_view("Full Log View", ui->full_log_view);
#endif
        
    }
    else if(is_set(inventory->flags, InventoryFlag_Mark))
    {
        render_item_mark_window(game, inventory->examine_item, assets, ui);
    }
    else if(is_set(examine->flags, ExamineFlag_Open))
    {
        render_examine_window(game, assets, ui, player->pos);
        }
    else if(is_set(inventory->flags, InventoryFlag_Examine))
    {
        render_item_examine_window(game, inventory->examine_item, assets, ui, true);
        }
    else if(is_set(inventory->flags, InventoryFlag_MultipleExamine))
        {
        render_multiple_examine_window(game, entities, items, inventory, dungeon, assets, ui);
    }
    else if(is_set(inventory->flags, InventoryFlag_Open))
    {
        render_inventory_window(game, items, inventory, dungeon->level, assets, ui);
    }
    else if(is_set(inventory->flags, InventoryFlag_MultiplePickup))
    {
        //render_inventory_items(game, entities, items, inventory, dungeon, &inventory->pickup_view, assets, ui, player->pos, CameFrom_Pickup);
    }
}