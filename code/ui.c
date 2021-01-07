internal u32
get_ui_padding()
{
    u32 result = 10;
    return(result);
}

internal b32
view_needs_scrollbar(View view)
{
    b32 result = (view.end && (view.entry_count > view.end));
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
    view->start = view->entry_count - view->end + 1;
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
item_fits_using_item_type(UsingItemType type, Item *item)
{
    assert(type);
    
    b32 result = false;
    
    if((type == UsingItemType_Identify && !item->is_identified) ||
       (type == UsingItemType_EnchantWeapon && item->type == ItemType_Weapon) ||
       (type == UsingItemType_EnchantArmor && item->type == ItemType_Armor) ||
       (type == UsingItemType_Uncurse && is_item_cursed_and_identified(item)))
    {
        result = true;
    }
    
    return(result);
}

internal void
render_item_type_header(UI *ui, v4u rect, v2u pos, ItemType type)
{
    v2u header_pos = {pos.x, pos.y + (ui->font_newline / 2)};
    defer_fill_rect(ui->defer,
                    header_pos.x, header_pos.y - 4,
                    rect.w - header_pos.x - (ui->window_offset * 2),
                    1,
                    Color_WindowAccent);
    
    switch(type)
    {
        
        case ItemType_Weapon: defer_text(ui->defer, "Weapon", header_pos.x, header_pos.y); break;
        case ItemType_Armor: defer_text(ui->defer, "Armor", header_pos.x, header_pos.y); break;
        case ItemType_Potion: defer_text(ui->defer, "Potion", header_pos.x, header_pos.y); break;
        case ItemType_Scroll: defer_text(ui->defer, "Scroll", header_pos.x, header_pos.y); break;
        case ItemType_Ration: defer_text(ui->defer, "Ration", header_pos.x, header_pos.y); break;
        
        invalid_default_case;
    }
}

internal v2u
get_header_text_pos(UI *ui, v2u header)
{
    v2u result = header;
    
    result.x += ui->window_offset * 4;
    result.y += ui->font->size / 2;
    
    return(result);
}

internal v2u
get_inventory_header_pos(UI *ui, v4u rect)
{
    v2u result =
    {
        rect.x + (ui->window_offset * 2),
        rect.y + (ui->window_offset * 2)
    };
    
    return(result);
}

internal void
update_view_scrollbar(View *view, Input *input)
{
    if(input->mouse_scroll == MouseScroll_Up)
    {
        if(!is_zero_or_underflow(view->start - 1))
        {
            --view->start;
        }
    }
    else if(input->mouse_scroll == MouseScroll_Down)
    {
        if(get_view_range(*view) <= view->entry_count)
        {
            ++view->start;
        }
    }
    else if(input->page_move == PageMove_PageUp)
    {
        if(view_needs_scrollbar(*view))
        {
            view->start -= view->end;
            if(is_zero_or_underflow(view->start - 1))
            {
                set_view_at_start(view);
            }
        }
    }
    else if(input->page_move == PageMove_PageDown)
    {
        view->start += view->end;
        if(get_view_range(*view) > view->entry_count)
        {
            set_view_at_end(view);
        }
    }
}

internal b32
entry_has_space(v2u pos, u32 entry_size, u32 window_asset_y)
{
    b32 result = (pos.y + (entry_size * 2) < window_asset_y);
    return(result);
}

internal void
render_scrollbar(Game *game, UI *ui, v4u rect, View *view)
{
    if(view_needs_scrollbar(*view))
    {
        u32 rect_gutter_x = rect.x + rect.w - 10;
        
        v4u gutter = {0};
        gutter.x = rect_gutter_x;
        gutter.y = rect.y;
        gutter.w = 2;
        
        // Get the correct gutter height.
        u32 scrollbar_size = (rect.h - (ui->font_newline * 4)) / view->entry_count;
        gutter.h = scrollbar_size * view->entry_count;
        
        // Center gutter vertically relative to rect.
        gutter.y += (rect.h - gutter.h) / 2;
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
render_window(Game *game, v4u rect, u32 border_size)
{
    // Window border
    render_fill_rect(game, rect, Color_WindowBorder);
    
    // Window background
    v4u background_rect = get_border_adjusted_rect(rect, border_size);
    render_fill_rect(game, background_rect, Color_Window);
    
    // Window accent
    render_draw_rect(game, background_rect, Color_WindowAccent);
}

internal void
center_window_to_available_screen(v2u game_window_size, Assets *assets, v4u *rect)
{
    assert(rect->w);
    assert(rect->h);
    
    rect->x = (game_window_size.w / 2) - (rect->w / 2);
    rect->y = (game_window_size.h - assets->stat_and_log_window_h - rect->h) / 2;
}

internal void
center_and_render_window_to_available_screen(Game *game, Assets *assets, v4u *rect, u32 border_size)
{
    center_window_to_available_screen(game->window_size, assets, rect);
    render_window(game, *rect, border_size);
}

internal v2u
get_header_pos(UI *ui, v4u rect)
{
    v2u result =
    {
        rect.x + (ui->window_offset * 2),
        rect.y + ui->window_offset
    };
    
    return(result);
}

internal v4u
get_window_rect()
{
    v4u result = {0};
    result.w = 800;
    
    return(result);
}

internal v4u
get_inspect_rect()
{
    v4u result = {0};
    result.w = 608;
    
    return(result);
}

internal v2u
render_inspect_item_info(Game *game, UI *ui, Item *item, ItemInfo *item_info, v2u header, v2u info, b32 inspecting_from_inventory)
{
    v2u result = info;
    String128 letter = get_item_letter_string(item->inventory_letter);
    
    defer_texture(ui->defer, header, item->tile_pos);
    header = get_header_text_pos(ui, header);
    
    String128 item_name = full_item_name(item);
    defer_text(ui->defer, "%s%s%s%s",
               header.x, header.y,
               item_status_color(item),
               letter.str,
               item_status_prefix(item),
               item_name.str);
    
    result.y += ui->font_newline * 2;
    
    if(item->is_identified)
    {
        if(item->type == ItemType_Weapon)
        {
            result.y += ui->font_newline;
            defer_text(ui->defer, "Damage: %d", result.x, result.y, item->w.damage + item->enchantment_level);
            
            result.y += ui->font_newline;
            defer_text(ui->defer, "Accuracy: %d", result.x, result.y, item->w.accuracy + item->enchantment_level);
            
            result.y += ui->font_newline;
            defer_text(ui->defer, "Attack Speed: %.1f", result.x, result.y, item->w.speed);
        }
        else if(item->type == ItemType_Armor)
        {
            result.y += ui->font_newline;
            defer_text(ui->defer, "Defence: %d", result.x, result.y, item->a.defence + item->enchantment_level);
            
            result.y += ui->font_newline;
            defer_text(ui->defer, "Weight: %d", result.x, result.y, item->a.weight);
        }
        else if(is_item_consumable(item->type))
        {
            result.y += ui->font_newline;
            defer_text(ui->defer, "%s", result.x, result.y, item->description);
        }
    }
    else
    {
        if(item->type == ItemType_Weapon)
        {
            result.y += ui->font_newline;
            defer_text(ui->defer, "Base Damage: %u", result.x, result.y, item->w.damage);
            
            result.y += ui->font_newline;
            defer_text(ui->defer, "Base Accuracy: %d", result.x, result.y, item->w.accuracy);
            
            result.y += ui->font_newline;
            defer_text(ui->defer, "Base Attack Speed: %.1f", result.x, result.y, item->w.speed);
        }
        else if(item->type == ItemType_Armor)
        {
            result.y += ui->font_newline;
            defer_text(ui->defer, "Base Defence: %d", result.x, result.y, item->a.defence);
            
            result.y += ui->font_newline;
            defer_text(ui->defer, "Base Weight: %d", result.x, result.y, item->a.weight);
        }
        else if(item->type == ItemType_Potion)
        {
            result.y += ui->font_newline;
            defer_text(ui->defer, "Consuming potions will bestow you with different effects.", result.x, result.y);
            
            result.y += ui->font_newline;
            defer_text(ui->defer, "Some of these effects will be helpful, while others harmful.", result.x, result.y);
        }
        else if(item->type == ItemType_Scroll)
        {
            result.y += ui->font_newline;
            defer_text(ui->defer, "Reading scrolls will bring out different magical effects.", result.x, result.y);
        }
    }
    
    result.y += ui->font_newline * 2;
    
    if(item->is_identified &&
       item->is_cursed)
    {
        defer_text(ui->defer, "It is a cursed item.", result.x, result.y);
        result.y += ui->font_newline;
    }
    
    if(is_item_equipment(item->type))
    {
        if(item->rarity == ItemRarity_Common)
        {
            defer_text(ui->defer, "It is of common rarity.", result.x, result.y);
        }
        else if(item->rarity == ItemRarity_Magical)
        {
            defer_text(ui->defer, "It is of magical rarity.", result.x, result.y);
        }
        else if(item->rarity == ItemRarity_Mythical)
        {
            defer_text(ui->defer, "It is of mythical rarity.", result.x, result.y);
        }
        
        if(item->type == ItemType_Weapon)
        {
            result.y += ui->font_newline;
            
            if(item->handedness == ItemHandedness_OneHanded)
            {
                defer_text(ui->defer, "It is a one-handed weapon.", result.x, result.y);
            }
            else if(item->handedness == ItemHandedness_TwoHanded)
            {
                defer_text(ui->defer, "It is a two-handed weapon.", result.x, result.y);
            }
        }
        
        result.y += ui->font_newline * 2;
    }
    
    if(inspecting_from_inventory)
    {
        u32 padding = get_ui_padding();
        
        char *adjust = "(a)djust";
        defer_text(ui->defer, adjust, result.x, result.y);
        result.x += get_text_width(ui->font, adjust) + padding;
        
        if(is_item_equipment(item->type))
        {
            if(item->is_equipped)
            {
                char *unequip = "(u)nequip";
                defer_text(ui->defer, unequip, result.x, result.y);
                result.x += get_text_width(ui->font, unequip) + padding;
            }
            else
            {
                char *equip = "(e)quip";
                defer_text(ui->defer, equip, result.x, result.y);
                result.x += get_text_width(ui->font, equip) + padding;
            }
        }
        else if(item->type == ItemType_Potion ||
                item->type == ItemType_Ration)
        {
            char *consume = "(c)onsume";
            defer_text(ui->defer, consume, result.x, result.y);
            result.x += get_text_width(ui->font, consume) + padding;
        }
        else if(item->type == ItemType_Scroll)
        {
            char *read = "(r)ead";
            defer_text(ui->defer, read, result.x, result.y);
            result.x += get_text_width(ui->font, read) + padding;
        }
        
        char *drop = "(d)rop";
        defer_text(ui->defer, drop, result.x, result.y);
        result.x += get_text_width(ui->font, drop) + padding;
        
        char *mark = "(m)ark";
        defer_text(ui->defer, mark, result.x, result.y);
        result.x += get_text_width(ui->font, mark) + padding;
        
        result.y += ui->font_newline * 2;
    }
    
    return(result);
}

internal u32
get_view_range(View view)
{
    u32 result = view.start + view.end;
    return(result);
}

internal b32
is_entry_in_view(View view, u32 entry)
{
    b32 result = (!view.end || (entry >= view.start &&
                                entry < get_view_range(view)));
    
    return(result);
}

internal v2u
render_identified_weapon_stats(Game *game, UI *ui, Item *item, v2u pos)
{
    pos.y += ui->font_newline;
    render_text(game, "Damage: %d", pos.x, pos.y, ui->font, 0, item->w.damage + item->enchantment_level);
    
    pos.y += ui->font_newline;
    render_text(game, "Accuracy: %d", pos.x, pos.y, ui->font, 0, item->w.accuracy + item->enchantment_level);
    
    pos.y += ui->font_newline;
    render_text(game, "Attack Speed: %.1f", pos.x, pos.y, ui->font, 0, item->w.speed);
    
    return(pos);
}

internal v2u
render_identified_armor_stats(Game *game, UI *ui, Item *item, v2u pos)
{
    pos.y += ui->font_newline;
    render_text(game, "Defence: %d", pos.x, pos.y, ui->font, 0, item->a.defence + item->enchantment_level);
    
    pos.y += ui->font_newline;
    render_text(game, "Weight: %d", pos.x, pos.y, ui->font, 0, item->a.weight);
    
    return(pos);
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
    String128 formatted = {0};
    
    va_list arg_list;
    va_start(arg_list, text);
    vsnprintf(formatted.str, sizeof(formatted), text, arg_list);
    va_end(arg_list);
    
    // Copy the new text to a vacant log index if there is one.
    for(u32 index = 0; index < MAX_LOG_MESSAGE_COUNT; ++index)
    {
        update_log_view(&ui->full_log_view, index);
        update_log_view(&ui->short_log_view, index);
        
        if(!ui->log_messages[index].str[0])
        {
            strcpy(ui->log_messages[index].str, formatted.str);
            return;
        }
    }
    
    // Move all texts up by one.
    for(u32 index = 1; index < MAX_LOG_MESSAGE_COUNT; ++index)
    {
        strcpy(ui->log_messages[index - 1].str, ui->log_messages[index].str);
    }
    
    // Copy the new text to the bottom.
    strcpy(ui->log_messages[MAX_LOG_MESSAGE_COUNT - 1].str, formatted.str);
}

internal void
render_ui(Game *game,
          Input *input,
          Dungeon *dungeon,
          Entity *player,
          UI *ui,
          Inventory *inventory,
          ItemInfo *item_info,
          Assets *assets)
{
    Examine *examine = &game->examine;
    u32 window_asset_y = game->window_size.h - assets->stat_and_log_window_h;
    
    v4u stat_window = {0, window_asset_y, 388, assets->stat_and_log_window_h};
    render_window(game, stat_window, 2);
    
    v4u short_log_rect = {stat_window.w + 4, window_asset_y, game->window_size.w - short_log_rect.x, assets->stat_and_log_window_h};
    render_window(game, short_log_rect, 2);
    
    // Render Stats
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
        render_text(game, player->name, left.x, left.y, ui->font, 0);
        
        left.y += ui->font_newline;
        render_text(game, "Health:    %u/%u", left.x, left.y, ui->font, 0, player->hp, player->max_hp);
        
        left.y += ui->font_newline;
        render_text(game, "Strength:     %u", left.x, left.y, ui->font, 0, player->strength);
        
        left.y += ui->font_newline;
        render_text(game, "Intelligence: %u", left.x, left.y, ui->font, 0, player->intelligence);
        
        left.y += ui->font_newline;
        render_text(game, "Dexterity:    %u", left.x, left.y, ui->font, 0, player->dexterity);
        
        left.y += ui->font_newline;
        render_text(game, "Evasion:      %u", left.x, left.y, ui->font, 0, player->evasion);
        
        left.y += ui->font_newline;
        render_text(game, "Defence:      %u", left.x, left.y, ui->font, 0, player->defence);
        
        left.y += ui->font_newline;
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
        
        right.y += ui->font_newline;
        render_text(game, "Time:          %.01f", right.x, right.y, ui->font, 0, game->time);
        
        right.y += ui->font_newline;
        render_text(game, "Action count:  %.01f", right.x, right.y, ui->font, 0, player->action_count);
        
        right.y += ui->font_newline;
        render_text(game, "Dungeon level: %u", right.x, right.y, ui->font, 0, dungeon->level);
    }
    
    // Short Log
    assert(ui->short_log_view.end);
    
    v2u full_log_message_pos =
    {
        short_log_rect.x + ui->window_offset,
        short_log_rect.y + (ui->font->size / 2)
    };
    
    for(u32 index = ui->short_log_view.start;
        index < get_view_range(ui->short_log_view);
        ++index)
    {
        if(ui->log_messages[index].str[0])
        {
            render_text(game, ui->log_messages[index].str,
                        full_log_message_pos.x, full_log_message_pos.y,
                        ui->font, 0);
            
            full_log_message_pos.y += ui->font_newline;
        }
    }
    
    // Full Log
    if(ui->is_full_log_open)
    {
        v4u full_log_rect = get_window_rect();
        
        v2u full_log_message_pos =
        {
            full_log_rect.x + ui->window_offset,
            full_log_rect.y + ui->window_offset
        };
        
        v2u test_message_pos = full_log_message_pos;
        ui->full_log_view.entry_count = 0;
        
        for(u32 index = 0; index < MAX_LOG_MESSAGE_COUNT; ++index)
        {
            if(ui->log_messages[index].str[0])
            {
                ++ui->full_log_view.entry_count;
                
                if(entry_has_space(test_message_pos, ui->font_newline * 2, window_asset_y))
                {
                    test_message_pos.y += ui->font_newline;
                }
                else
                {
                    init_view_end(&ui->full_log_view, index + 1);
                }
            }
        }
        
        if(view_needs_scrollbar(ui->full_log_view) &&
           !ui->is_full_log_view_set_at_end)
        {
            ui->is_full_log_view_set_at_end = true;
            set_view_at_end(&ui->full_log_view);
        }
        
        for(u32 index = 0; index < MAX_LOG_MESSAGE_COUNT; ++index)
        {
            if(ui->log_messages[index].str[0])
            {
                if(is_entry_in_view(ui->full_log_view, index + 1))
                {
                    defer_text(ui->defer, ui->log_messages[index].str, full_log_message_pos.x, full_log_message_pos.y);
                    full_log_message_pos.y += ui->font_newline;
                }
            }
        }
        
        full_log_message_pos.y += (ui->font_newline / 2);
        full_log_rect.h = full_log_message_pos.y;
        ui->full_log_rect = full_log_rect;
        
        center_and_render_window_to_available_screen(game, assets, &full_log_rect, 2);
        process_defer(game, assets, ui, full_log_rect.x, full_log_rect.y);
        render_scrollbar(game, ui, full_log_rect, &ui->full_log_view);
        
#if 0
        printf("full_log.entry_count: %u\n", ui->full_log_view.entry_count);
        printf("full_log.start: %u\n", ui->full_log_view.start);
        printf("full_log.end: %u\n\n", ui->full_log_view.end);
#endif
        
    }
    else if(examine->is_open)
    {
        if(examine->type)
        {
            v4u inspect_rect = get_inspect_rect();
            v2u header = get_header_pos(ui, inspect_rect);
            v2u info = header;
            
            Item *item = game->examine.item;
            Entity *entity = game->examine.entity;
            
            if(examine->type == ExamineType_Item)
            {
                info = render_inspect_item_info(game, ui, item, item_info, header, info, false);
            }
            else if(examine->type == ExamineType_Entity)
            {
                assert(entity->type == EntityType_Enemy);
                
                defer_texture(ui->defer, header, entity->tile_pos);
                
                header = get_header_text_pos(ui, header);
                defer_text(ui->defer, "%s", header.x, header.y, entity->name);
                info.y += ui->font_newline * 2;
                
                info.y += ui->font_newline;
                defer_text(ui->defer, "Max HP: %u", info.x, info.y, entity->max_hp);
                
                info.y += ui->font_newline;
                defer_text(ui->defer, "Damage: %u", info.x, info.y, entity->e.damage);
                
                info.y += ui->font_newline;
                defer_text(ui->defer, "Defence: %u", info.x, info.y, entity->defence);
                
                char evasion_description[24] = {0};
                if(entity->evasion <= 3)
                {
                    strcpy(evasion_description, "(very low evasion)");
                }
                else if(entity->evasion <= 7)
                {
                    strcpy(evasion_description, "(low evasion)");
                }
                else if(entity->evasion <= 13)
                {
                    strcpy(evasion_description, "(average evasion)");
                }
                else if(entity->evasion <= 17)
                {
                    strcpy(evasion_description, "(high evasion)");
                }
                else
                {
                    strcpy(evasion_description, "(very high evasion)");
                }
                
                info.y += ui->font_newline;
                defer_text(ui->defer, "Evasion: %u %s", info.x, info.y, entity->evasion, evasion_description);
                
                info.y += ui->font_newline;
                defer_text(ui->defer, "Speed: %.01f", info.x, info.y, entity->action_count);
                
                char letter = 'a';
                
                // Render entity spells
                if(is_flag_set(entity, EntityFlags_MagicAttacks))
                {
                    info.y += ui->font_newline * 2;
                    defer_text(ui->defer, "It has the following spells:", info.x, info.y);
                    
                    for(u32 spell_index = 0; spell_index < MAX_ENTITY_SPELL_COUNT; ++spell_index)
                    {
                        Spell *spell = &entity->e.spells[spell_index];
                        if(spell->id)
                        {
                            info.y += ui->font_newline;
                            defer_text(ui->defer, "%c - %s", info.x, info.y, letter, get_spell_name(spell->id));
                            
                            ++letter;
                        }
                    }
                }
                
                // Render entity status effects
                b32 render_status_effect_start = true;
                
                for(u32 status_index = 0; status_index < StatusEffectType_Count; ++status_index)
                {
                    if(is_entity_under_status_effect(entity, status_index))
                    {
                        if(render_status_effect_start)
                        {
                            info.y += ui->font_newline * 2;
                            defer_text(ui->defer, "It is under the following status effects:", info.x, info.y);
                            
                            render_status_effect_start = false;
                        }
                        
                        info.y += ui->font_newline;
                        defer_text(ui->defer, "%s", info.x, info.y, get_status_effect_name(status_index));
                    }
                }
                
                if(is_flag_set(entity, EntityFlags_MagicAttacks))
                {
                    info.y += ui->font_newline * 2;
                    defer_text(ui->defer, "Press the key next to the spell to show its information.", info.x, info.y);
                }
                
                info.y += ui->font_newline * 2;
            }
            else if(examine->type == ExamineType_EntitySpell)
            {
                Entity *enemy = examine->entity;
                Spell *spell = examine->spell;
                assert(spell->type);
                
                defer_text(ui->defer, get_spell_name(spell->id), info.x, info.y);
                info.y += ui->font_newline * 2;
                
                defer_text(ui->defer, get_spell_description(spell->id), info.x, info.y);
                info.y += ui->font_newline * 2;
                
                switch(spell->type)
                {
                    case SpellType_Offensive:
                    {
                        defer_text(ui->defer, "Damage Type: %s", info.x, info.y, get_damage_type_text(spell->damage_type));
                        info.y += ui->font_newline;
                        
                        defer_text(ui->defer, "Damage: %u", info.x, info.y, spell->effect.value);
                        info.y += ui->font_newline;
                    } break;
                    
                    case SpellType_Healing:
                    {
                        defer_text(ui->defer, "Healing: %u", info.x, info.y, spell->effect.value);
                        info.y += ui->font_newline;
                    } break;
                    
                    case SpellType_Buff:
                    {
                        defer_text(ui->defer, "Increase: %u", info.x, info.y, spell->effect.value);
                        info.y += ui->font_newline;
                        
                        defer_text(ui->defer, "Duration: %u", info.x, info.y, spell->effect.duration);
                        info.y += ui->font_newline;
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
                if(in_spell_range(spell->range, enemy->pos, player->pos))
                {
                    sprintf(in_spell_range_text, "(you are in range)");
                }
                
                defer_text(ui->defer, "%s %s", info.x, info.y, spell_range_text, in_spell_range_text);
                info.y += ui->font_newline * 2;
            }
            else if(examine->type == ExamineType_Tile)
            {
                TileID id = examine->tile_id;
                defer_texture(ui->defer, header, get_dungeon_tile_pos(dungeon->tiles, examine->pos));
                
                header = get_header_text_pos(ui, header);
                defer_text(ui->defer, "%s", header.x, header.y, get_tile_name(id));
                info.y += ui->font_newline * 3;
                
                char *tile_info_text = get_tile_info_text(id);
                if(*tile_info_text)
                {
                    defer_text(ui->defer, tile_info_text, info.x, info.y);
                    info.y += ui->font_newline * 2;
                }
            }
            
            inspect_rect.h = info.y;
            center_and_render_window_to_available_screen(game, assets, &inspect_rect, 2);
            process_defer(game, assets, ui, inspect_rect.x, inspect_rect.y);
        }
        else
        {
            v4u dest = get_game_dest(game, game->examine.pos);
            SDL_RenderCopy(game->renderer, assets->ui.tex, (SDL_Rect *)&assets->yellow_outline_src, (SDL_Rect *)&dest);
        }
    }
    else if(inventory->is_inspecting)
    {
        Item *item = inventory->slots[inventory->inspect_index];
        
        v4u inspect_rect = get_inspect_rect();
        v2u header = get_header_pos(ui, inspect_rect);
        v2u info = header;
        
        info = render_inspect_item_info(game, ui, item, item_info, header, info, true);
        
        inspect_rect.h = info.y;
        center_and_render_window_to_available_screen(game, assets, &inspect_rect, 2);
        process_defer(game, assets, ui, inspect_rect.x, inspect_rect.y);
    }
    else if(inventory->is_open)
    {
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
                inventory->view.entry_count -= 1;
            }
            else
            {
                // Since the item is the only one of its type, we take
                // away the item and its inventory item header from
                // the inventory entry count.
                inventory->view.entry_count -= 2;
            }
            
            inventory->view_update_item_type = ItemType_None;
        }
        
        
        if(view_needs_scrollbar(inventory->view))
        {
            // If the inventory view is at the bottom and something was
            // deleted then the view is adjusted.
            if(get_view_range(inventory->view) > inventory->view.entry_count)
            {
                set_view_at_end(&inventory->view);
            }
        }
        else
        {
            set_view_at_start(&inventory->view);
        }
        
        v4u inventory_rect = get_window_rect();
        v2u header = get_inventory_header_pos(ui, inventory_rect);
        
        v2u pos = header;
        pos.y += ui->font_newline;
        
        u32 entry_count = 0;
        u32 item_count = 0;
        v2u test_pos = pos;
        
        // This sets up the end value for the inventory view.
        if(!inventory->view.end)
        {
            for(u32 type = ItemType_Weapon; type < ItemType_Count; ++type)
            {
                b32 needs_item_type_header = true;
                
                for(u32 index = 0; index < MAX_INVENTORY_SLOT_COUNT; ++index)
                {
                    Item *item = inventory->slots[index];
                    if(is_item_valid_and_in_inventory(item) &&
                       item->type == type)
                    {
                        if(needs_item_type_header)
                        {
                            needs_item_type_header = false;
                            ++entry_count;
                            
                            if(entry_has_space(test_pos, inventory->entry_size, window_asset_y))
                            {
                                test_pos.y += inventory->entry_size;
                            }
                            else
                            {
                                init_view_end(&inventory->view, entry_count);
                            }
                        }
                        
                        if(entry_has_space(test_pos, inventory->entry_size, window_asset_y))
                        {
                            test_pos.y += inventory->entry_size;
                        }
                        else
                        {
                            init_view_end(&inventory->view, entry_count);
                        }
                        
                        ++entry_count;
                    }
                }
            }
        }
        
        entry_count = 0;
        for(u32 type = ItemType_Weapon; type < ItemType_Count; ++type)
        {
            b32 needs_item_type_header = true;
            
            for(u32 index = 0; index < MAX_INVENTORY_SLOT_COUNT; ++index)
            {
                Item *item = inventory->slots[index];
                if(is_item_valid_and_in_inventory(item) && item->type == type)
                {
                    // If using_item_type is valid, we only want to show items that fit it.
                    if(inventory->using_item_type &&
                       !item_fits_using_item_type(inventory->using_item_type, item))
                    {
                        continue;
                    }
                    
                    ++item_count;
                    
                    if(needs_item_type_header)
                    {
                        needs_item_type_header = false;
                        ++entry_count;
                        
                        if(is_entry_in_view(inventory->view, entry_count))
                        {
                            render_item_type_header(ui, inventory_rect, pos, type);
                            pos.y += inventory->entry_size;
                        }
                    }
                    
                    ++entry_count;
                    
                    if(is_entry_in_view(inventory->view, entry_count))
                    {
                        v2u picture_pos = {pos.x + 8, pos.y};
                        defer_texture(ui->defer, picture_pos, item->tile_pos);
                        
                        v2u name_pos =
                        {
                            picture_pos.x + (ui->font_newline * 3),
                            picture_pos.y + (ui->font->size / 2)
                        };
                        
                        String128 letter_string = get_item_letter_string(item->inventory_letter);
                        
                        if(is_item_consumable(item->type))
                        {
                            char stack_count_text[16] = {0};
                            if(item->c.stack_count > 1)
                            {
                                sprintf(stack_count_text, " (%u)", item->c.stack_count);
                            }
                            
                            if(item->is_identified)
                            {
                                defer_text(ui->defer, "%s%s%s", name_pos.x, name_pos.y, letter_string.str, item->name, stack_count_text);
                            }
                            else
                            {
                                defer_text(ui->defer, "%s%s%s%s", name_pos.x, name_pos.y, letter_string.str, item->c.depiction, item_id_text(item->id), stack_count_text);
                            }
                        }
                        else
                        {
                            if(item->is_identified)
                            {
                                char equipped_text[16] = {0};
                                if(item->is_equipped)
                                {
                                    sprintf(equipped_text, " (equipped)");
                                }
                                
                                String128 item_name = full_item_name(item);
                                defer_text(ui->defer, "%s%s%s%s%s",
                                           name_pos.x, name_pos.y,
                                           item_status_color(item),
                                           letter_string.str,
                                           item_status_prefix(item),
                                           item_name.str,
                                           equipped_text);
                            }
                            else
                            {
                                defer_text(ui->defer, "%s%s", name_pos.x, name_pos.y, letter_string.str, item_id_text(item->id));
                            }
                        }
                        
                        pos.y += inventory->entry_size;
                    }
                }
            }
        }
        
        pos.y += ui->font_newline;
        
        inventory_rect.h = pos.y;
        inventory->rect = inventory_rect;
        inventory->view.entry_count = entry_count;
        
        if(inventory->using_item_type)
        {
            switch(inventory->using_item_type)
            {
                case UsingItemType_Identify: defer_text(ui->defer, "Identify which item?", header.x, header.y); break;
                case UsingItemType_EnchantWeapon: defer_text(ui->defer, "Enchant which weapon?", header.x, header.y); break;
                case UsingItemType_EnchantArmor: defer_text(ui->defer, "Enchant which armor?", header.x, header.y); break;
                case UsingItemType_Uncurse: defer_text(ui->defer, "Uncurse which item?", header.x, header.y); break;
                
                invalid_default_case;
            }
        }
        else
        {
            defer_text(ui->defer, "Inventory: %u/%u", header.x, header.y, item_count, MAX_INVENTORY_SLOT_COUNT);
        }
        
        center_and_render_window_to_available_screen(game, assets, &inventory_rect, 2);
        process_defer(game, assets, ui, inventory_rect.x, inventory_rect.y);
        render_scrollbar(game, ui, inventory_rect, &inventory->view);
        
#if 0
        printf("inventory->view.entry_count: %u\n", inventory->view.entry_count);
        printf("inventory->view.start: %u\n", inventory->view.start);
        printf("inventory->view.end: %u\n\n", inventory->view.end);
#endif
        
    }
}