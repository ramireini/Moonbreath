internal void
render_window_option(UI *ui, char *text, v2u *pos)
{
    defer_text(ui, text, pos->x, pos->y);
    pos->x += get_text_width(ui->font, text) + 10;
    }

internal v4u
get_window_rect()
{
    v4u rect = {0, 0, 600, 0};
    return(rect);
}

internal v4u
get_inspect_rect()
{
    v4u rect = {0, 0, 400, 0};
    return(rect);
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
item_fits_using_item_type(UsingItemType type, Item *item)
{
    assert(type);
    
    b32 result = false;
    
    if((type == UsingItemType_Identify && !is_set(item->flags, ItemFlags_Identified)) ||
       (type == UsingItemType_EnchantWeapon && item->type == ItemType_Weapon) ||
       (type == UsingItemType_EnchantArmor && item->type == ItemType_Armor) ||
       (type == UsingItemType_Uncurse && is_set(item->flags, ItemFlags_Identified | ItemFlags_Cursed)))
    {
        result = true;
    }
    
    return(result);
}

internal void
render_item_type_header(UI *ui, v4u rect, v2u pos, ItemType type)
{
    v2u header = {pos.x, pos.y + (ui->font_newline / 2)};
    defer_fill_rect(ui,
                        header.x,
                        header.y - 4,
                        rect.w - (header.x * 2),
                        1,
                    Color_WindowAccent);
    
    switch(type)
    {
        
        case ItemType_Weapon: defer_text(ui, "Weapon", header.x, header.y); break;
        case ItemType_Armor: defer_text(ui, "Armor", header.x, header.y); break;
        case ItemType_Potion: defer_text(ui, "Potion", header.x, header.y); break;
        case ItemType_Scroll: defer_text(ui, "Scroll", header.x, header.y); break;
        case ItemType_Ration: defer_text(ui, "Ration", header.x, header.y); break;
        
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
    else if(was_pressed(&input->Key_PageUp))
    {
        if(is_view_scrolling(*view, view->count))
        {
            view->start -= view->end;
            if(is_zero(view->start - 1))
            {
                set_view_at_start(view);
            }
        }
    }
    else if(was_pressed(&input->Key_PageDown))
    {
        view->start += view->end;
        if(get_view_range(*view) > view->count)
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
    if(is_view_scrolling(*view, view->count))
    {
        assert(rect.w && rect.h);
        
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
center_window_to_available_screen(v2u game_window_size, Assets *assets, v4u *rect)
{
    if(rect->w && rect->w)
    {
    rect->x = (game_window_size.w / 2) - (rect->w / 2);
    rect->y = get_centering_offset(game_window_size.h - assets->stat_and_log_window_h, rect->h);
           }
}

internal void
center_and_render_window_to_available_screen(Game *game, Assets *assets, v4u *rect)
{
    center_window_to_available_screen(game->window_size, assets, rect);
    render_window(game, *rect, 2);
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

internal void
set_view_end_value(Item *items, View *view, u32 entry_size,
                            v2u pos, u32 window_asset_y, CameFrom came_from)
{
    assert(view->start == 1);
    u32 entry_count = 0;
    
    if(!view->end)
    {
        for(u32 type = ItemType_Weapon; type < ItemType_Count; ++type)
        {
            b32 needs_item_type_header = true;
            
            for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
            {
                Item *item = &items[index];
                
                b32 can_process = false;
                if(came_from == CameFrom_Inventory && is_item_valid_and_in_inventory(item))
                {
                    can_process = true;
                }
                else if(came_from == CameFrom_Pickup && is_item_valid_and_not_in_inventory(item))
                {
                    can_process = true;
                }
                
                if(can_process)
                {
                    if(item->type == type)
                    {
                        if(needs_item_type_header)
                        {
                            needs_item_type_header = false;
                            ++entry_count;
                            
                            if(entry_has_space(pos, entry_size, window_asset_y))
                            {
                                pos.y += entry_size;
                            }
                            else
                            {
                                init_view_end(view, entry_count);
                            }
                        }
                        
                        if(entry_has_space(pos, entry_size, window_asset_y))
                        {
                            pos.y += entry_size;
                        }
                        else
                        {
                            init_view_end(view, entry_count);
                        }
                        
                        ++entry_count;
                    }
            }
            }
        }
    }
}

internal void
process_window_end(Game *game, Assets *assets, UI *ui, View *view, v2u pos)
{
    ui->defer_rect.h = pos.y;
    
    center_and_render_window_to_available_screen(game, assets, &ui->defer_rect);
    
    if(view)
    {
    render_scrollbar(game, ui, ui->defer_rect, view);
    }
    
    process_defer(game, assets, ui);
}

internal void
render_inspect_item_info(Game *game, UI *ui, Item *item, ItemInfo *item_info, v2u *pos, CameFrom came_from)
{
    // Render item picture and name
    defer_texture(ui, *pos, item->tile_pos);
    v2u header = get_header_text_pos(ui, *pos);
    defer_text(ui, "%s%s%s%s%s",
               header.x, header.y,
               get_item_status_color(item),
                   get_item_letter_string(item).str,
               get_item_status_prefix(item),
               get_full_item_name(item).str,
               get_item_mark_string(item).str);
    
    pos->y += ui->font_newline * 2;
    
    if(is_set(item->flags, ItemFlags_Identified))
    {
        if(item->type == ItemType_Weapon)
        {
            pos->y += ui->font_newline;
            defer_text(ui, "Damage: %d", pos->x, pos->y, item->w.damage + item->enchantment_level);
            
            pos->y += ui->font_newline;
            defer_text(ui, "Accuracy: %d", pos->x, pos->y, item->w.accuracy + item->enchantment_level);
            
            pos->y += ui->font_newline;
            defer_text(ui, "Attack Speed: %.1f", pos->x, pos->y, item->w.speed);
        }
        else if(item->type == ItemType_Armor)
        {
            pos->y += ui->font_newline;
            defer_text(ui, "Defence: %d", pos->x, pos->y, item->a.defence + item->enchantment_level);
            
            pos->y += ui->font_newline;
            defer_text(ui, "Weight: %d", pos->x, pos->y, item->a.weight);
        }
        else if(is_item_consumable(item->type))
        {
            pos->y += ui->font_newline;
            defer_text(ui, "%s", pos->x, pos->y, item->description);
        }
    }
    else
    {
        if(item->type == ItemType_Weapon)
        {
            pos->y += ui->font_newline;
            defer_text(ui, "Base Damage: %u", pos->x, pos->y, item->w.damage);
            
            pos->y += ui->font_newline;
            defer_text(ui, "Base Accuracy: %d", pos->x, pos->y, item->w.accuracy);
            
            pos->y += ui->font_newline;
            defer_text(ui, "Base Attack Speed: %.1f", pos->x, pos->y, item->w.speed);
        }
        else if(item->type == ItemType_Armor)
        {
            pos->y += ui->font_newline;
            defer_text(ui, "Base Defence: %d", pos->x, pos->y, item->a.defence);
            
            pos->y += ui->font_newline;
            defer_text(ui, "Base Weight: %d", pos->x, pos->y, item->a.weight);
        }
        else if(item->type == ItemType_Potion)
        {
            pos->y += ui->font_newline;
            defer_text(ui, "Consuming potions will bestow you with different effects.", pos->x, pos->y);
            
            pos->y += ui->font_newline;
            defer_text(ui, "Some of these effects will be helpful, while others harmful.", pos->x, pos->y);
        }
        else if(item->type == ItemType_Scroll)
        {
            pos->y += ui->font_newline;
            defer_text(ui, "Reading scrolls will bring out different magical effects.", pos->x, pos->y);
        }
    }
    
    pos->y += ui->font_newline * 2;
    
    if(is_set(item->flags, ItemFlags_Identified | ItemFlags_Cursed))
    {
        defer_text(ui, "It is a cursed item.", pos->x, pos->y);
        pos->y += ui->font_newline;
    }
    
    if(is_item_equipment(item->type))
    {
        if(item->rarity == ItemRarity_Common)
        {
            defer_text(ui, "It is of common rarity.", pos->x, pos->y);
        }
        else if(item->rarity == ItemRarity_Magical)
        {
            defer_text(ui, "It is of magical rarity.", pos->x, pos->y);
        }
        else if(item->rarity == ItemRarity_Mythical)
        {
            defer_text(ui, "It is of mythical rarity.", pos->x, pos->y);
        }
        
        if(item->type == ItemType_Weapon)
        {
            pos->y += ui->font_newline;
            
            if(item->handedness == ItemHandedness_OneHanded)
            {
                defer_text(ui, "It is a one-handed weapon.", pos->x, pos->y);
            }
            else if(item->handedness == ItemHandedness_TwoHanded)
            {
                defer_text(ui, "It is a two-handed weapon.", pos->x, pos->y);
            }
        }
        
        pos->y += ui->font_newline * 2;
    }
    
    if(came_from == CameFrom_Inventory)
    {
        u32 padding = get_ui_padding();
        render_window_option(ui, "(a)djust", pos);
        
        if(is_item_equipment(item->type))
        {
            if(is_set(item->flags, ItemFlags_Equipped))
            {
                render_window_option(ui, "(u)nequip", pos);
            }
            else
            {
                render_window_option(ui, "(e)quip", pos);
            }
        }
        else if(item->type == ItemType_Potion ||
                item->type == ItemType_Ration)
        {
            render_window_option(ui, "(c)onsume", pos);
        }
        else if(item->type == ItemType_Scroll)
        {
            render_window_option(ui, "(r)ead", pos);
        }
        
        render_window_option(ui, "(d)rop", pos);
        render_window_option(ui, "(m)ark", pos);
        
        pos->y += ui->font_newline * 2;
    }
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
    
    // Copy the new text to a vacant log index if there is one
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
    
    // Move all texts up by one
    for(u32 index = 1; index < MAX_LOG_MESSAGE_COUNT; ++index)
    {
        strcpy(ui->log_messages[index - 1].str, ui->log_messages[index].str);
    }
    
    // Copy the new text to the bottom
    strcpy(ui->log_messages[MAX_LOG_MESSAGE_COUNT - 1].str, formatted.str);
}

internal void
render_ui(Game *game,
          Input *input,
          Dungeon *dungeon,
          Entity *player,
          Item *items,
          Inventory *inventory,
          ItemInfo *item_info,
          Assets *assets,
          UI *ui)
{
    Examine *examine = &game->examine;
    u32 window_asset_y = game->window_size.h - assets->stat_and_log_window_h;
    
    v4u stat_rect = {0, window_asset_y, 388, assets->stat_and_log_window_h};
    render_window(game, stat_rect, 2);
    
    v4u short_log_rect = {stat_rect.w + 4, window_asset_y, game->window_size.w - short_log_rect.x, assets->stat_and_log_window_h};
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
    
    for(u32 index = ui->short_log_view.start; index < get_view_range(ui->short_log_view); ++index)
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
        ui->defer_rect = get_window_rect();
        
        v2u full_log_message_pos =
        {
            ui->defer_rect.x + ui->window_offset,
            ui->defer_rect.y + ui->window_offset
        };
        
        v2u test_message_pos = full_log_message_pos;
        ui->full_log_view.count = 0;
        
        for(u32 index = 0; index < MAX_LOG_MESSAGE_COUNT; ++index)
        {
            if(ui->log_messages[index].str[0])
            {
                ++ui->full_log_view.count;
                
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
        
        if(is_view_scrolling(ui->full_log_view, ui->full_log_view.count) && !ui->is_full_log_view_set_at_end)
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
                    defer_text(ui, ui->log_messages[index].str, full_log_message_pos.x, full_log_message_pos.y);
                    full_log_message_pos.y += ui->font_newline;
                }
            }
        }
        
        full_log_message_pos.y += (ui->font_newline / 2);
        process_window_end(game, assets, ui, &ui->full_log_view, full_log_message_pos);
        
#if 0
        printf("full_log.count: %u\n", ui->full_log_view.count);
        printf("full_log.start: %u\n", ui->full_log_view.start);
        printf("full_log.end: %u\n\n", ui->full_log_view.end);
#endif
        
    }
    else if(is_set(game->examine.flags, ExamineFlags_Open))
    {
        if(examine->type)
        {
            ui->defer_rect = get_inspect_rect();
            v2u pos = get_header_pos(ui, ui->defer_rect);
            
            Item *item = game->examine.item;
            Entity *entity = game->examine.entity;
            
            if(examine->type == ExamineType_Item)
            {
                render_inspect_item_info(game, ui, item, item_info, &pos, CameFrom_Examine);
            }
            else if(examine->type == ExamineType_Entity)
            {
                assert(entity->type == EntityType_Enemy);
                
                defer_texture(ui, pos, entity->tile_pos);
                
                v2u name = get_header_text_pos(ui, pos);
                defer_text(ui, "%s", name.x, name.y, entity->name);
                pos.y += ui->font_newline * 2;
                
                pos.y += ui->font_newline;
                defer_text(ui, "Max HP: %u", pos.x, pos.y, entity->max_hp);
                
                pos.y += ui->font_newline;
                defer_text(ui, "Damage: %u", pos.x, pos.y, entity->e.damage);
                
                pos.y += ui->font_newline;
                defer_text(ui, "Defence: %u", pos.x, pos.y, entity->defence);
                
                char evasion_text[24] = {0};
                if(entity->evasion <= 3)
                {
                    strcpy(evasion_text, "(very low evasion)");
                }
                else if(entity->evasion <= 7)
                {
                    strcpy(evasion_text, "(low evasion)");
                }
                else if(entity->evasion <= 13)
                {
                    strcpy(evasion_text, "(average evasion)");
                }
                else if(entity->evasion <= 17)
                {
                    strcpy(evasion_text, "(high evasion)");
                }
                else
                {
                    strcpy(evasion_text, "(very high evasion)");
                }
                
                pos.y += ui->font_newline;
                defer_text(ui, "Evasion: %u %s", pos.x, pos.y, entity->evasion, evasion_text);
                
                pos.y += ui->font_newline;
                defer_text(ui, "Speed: %.01f", pos.x, pos.y, entity->action_count);
                
                char letter = 'a';
                
                // Render entity spells
                if(is_set(entity->flags, EntityFlags_MagicAttacks))
                {
                    pos.y += ui->font_newline * 2;
                    defer_text(ui, "It has the following spells:", pos.x, pos.y);
                    
                    for(u32 spell_index = 0; spell_index < MAX_ENTITY_SPELL_COUNT; ++spell_index)
                    {
                        Spell *spell = &entity->e.spells[spell_index];
                        if(spell->id)
                        {
                            pos.y += ui->font_newline;
                            defer_text(ui, "%c - %s", pos.x, pos.y, letter, get_spell_name(spell->id));
                            
                            ++letter;
                        }
                    }
                }
                
                // Render entity status effects
                if(is_entity_under_any_status_effect(entity))
                {
                    pos.y += ui->font_newline * 2;
                    defer_text(ui, "It is under the following status effects:", pos.x, pos.y);
                    
                    for(u32 status_index = 0; status_index < StatusEffectType_Count; ++status_index)
                    {
                        if(is_entity_under_status_effect(entity, status_index))
                        {
                            pos.y += ui->font_newline;
                            defer_text(ui, "%s", pos.x, pos.y, get_status_effect_name(status_index));
                        }
                    }
                }
                
                if(is_set(entity->flags, EntityFlags_MagicAttacks))
                {
                    pos.y += ui->font_newline * 2;
                    defer_text(ui, "Press the key next to the spell to show its information.", pos.x, pos.y);
                }
                
                pos.y += ui->font_newline * 2;
            }
            else if(examine->type == ExamineType_EntitySpell)
            {
                Entity *enemy = examine->entity;
                Spell *spell = examine->spell;
                assert(spell->type);
                
                defer_text(ui, get_spell_name(spell->id), pos.x, pos.y);
                pos.y += ui->font_newline * 2;
                
                defer_text(ui, get_spell_description(spell->id), pos.x, pos.y);
                pos.y += ui->font_newline * 2;
                
                switch(spell->type)
                {
                    case SpellType_Offensive:
                    {
                        defer_text(ui, "Damage Type: %s", pos.x, pos.y, get_damage_type_text(spell->damage_type));
                        pos.y += ui->font_newline;
                        
                        defer_text(ui, "Damage: %u", pos.x, pos.y, spell->effect.value);
                        pos.y += ui->font_newline;
                    } break;
                    
                    case SpellType_Healing:
                    {
                        defer_text(ui, "Healing: %u", pos.x, pos.y, spell->effect.value);
                        pos.y += ui->font_newline;
                    } break;
                    
                    case SpellType_Buff:
                    {
                        defer_text(ui, "Increase: %u", pos.x, pos.y, spell->effect.value);
                        pos.y += ui->font_newline;
                        
                        defer_text(ui, "Duration: %u", pos.x, pos.y, spell->effect.duration);
                        pos.y += ui->font_newline;
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
                
                defer_text(ui, "%s %s", pos.x, pos.y, spell_range_text, in_spell_range_text);
                pos.y += ui->font_newline * 2;
            }
            else if(examine->type == ExamineType_Tile)
            {
                TileID id = examine->tile_id;
                defer_texture(ui, pos, get_dungeon_tile_pos(dungeon->tiles, examine->pos));
                
                v2u header = get_header_text_pos(ui, pos);
                defer_text(ui, "%s", header.x, header.y, get_tile_name(id));
                pos.y += ui->font_newline * 3;
                
                char *tile_info_text = get_tile_info_text(id);
                if(*tile_info_text)
                {
                    defer_text(ui, tile_info_text, pos.x, pos.y);
                    pos.y += ui->font_newline * 2;
                }
            }
            
            process_window_end(game, assets, ui, 0, pos);
        }
        else
        {
            v4u dest = get_game_dest(game, game->examine.pos);
            SDL_RenderCopy(game->renderer, assets->ui.tex, (SDL_Rect *)&assets->yellow_outline_src, (SDL_Rect *)&dest);
        }
    }
    else if(is_set(inventory->flags, InventoryFlags_Marking))
    {
        Item *inspect_item = inventory->slots[inventory->inspect_index];
        
        // Mark box
        v4u mark_rect = {0, 0, 250, 100};
        center_and_render_window_to_available_screen(game, assets, &mark_rect);
        
        // Header text
        char *header_text = "Mark with what?";
        if(is_set(inspect_item->flags, ItemFlags_Marked))
        {
            header_text = "Replace mark with what?";
        }
        
        v2u header =
        {
            mark_rect.x + get_centering_offset(mark_rect.w, get_text_width(ui->font, header_text)),
            mark_rect.y + 25
        };
        
        render_text(game, header_text, header.x, header.y, ui->font, 0);
        
        // Input box
        u32 height_padding = 4;
        v4u input_rect = {mark_rect.x, mark_rect.y, ui->font->size * 14, ui->font->size + height_padding};
        
        input_rect.x += get_centering_offset(mark_rect.w, input_rect.w);
        input_rect.y += ui->font_newline * 3;
        
        render_fill_rect(game, input_rect, Color_WindowAccent);
        render_draw_rect(game, input_rect, Color_WindowBorder);
        
        // Update Cursor
        Mark *mark = &ui->mark;
        if(!mark->timer)
        {
            mark->timer = SDL_GetTicks();
        }
        
        if((SDL_GetTicks() - mark->timer) >= 500)
        {
            mark->timer = 0;
            mark->render = !mark->render;
        }
        
        // Render input
        v2u text =
        {
            input_rect.x + 4,
            input_rect.y + get_centering_offset(input_rect.h, ui->font->size) + 1
        };
        
        u32 cursor_x = text.x;
        v2u character = text;
        for(u32 index = mark->view.start; index < get_view_range(mark->view); ++index)
        {
            u32 mark_index = index - 1;
            
            render_text(game, "%c", character.x, character.y, ui->font, 0, mark->array[mark_index]);
            character.x += get_glyph_width(ui->font, mark->array[mark_index]);
            
            if(mark->render && (index == mark->index))
            {
                cursor_x = character.x;
            }
        }
        
        // Render cursor
        if(mark->render)
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
        printf("mark->index: %u\n", mark->index);
        printf("mark->view.count: %u\n", mark->view.count);
        printf("mark->view.start: %u\n", mark->view.start);
        printf("mark->view.end: %u\n\n", mark->view.end);
#endif
        
    }
    else if(is_set(inventory->flags, InventoryFlags_Inspecting))
    {
        Item *item = inventory->slots[inventory->inspect_index];
        
        ui->defer_rect = get_inspect_rect();
        v2u pos = get_header_pos(ui, ui->defer_rect);
        render_inspect_item_info(game, ui, item, item_info, &pos, CameFrom_Inventory);
        
        process_window_end(game, assets, ui, 0, pos);
    }
    else if(is_set(inventory->flags, InventoryFlags_Open))
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
                inventory->view.count -= 1;
            }
            else
            {
                // Since the item is the only one of its type, we take
                // away the item and its inventory item header from
                // the inventory entry count.
                inventory->view.count -= 2;
            }
            
            inventory->view_update_item_type = ItemType_None;
        }
        
        if(is_view_scrolling(inventory->view, inventory->view.count))
        {
            // If the inventory view is at the bottom and something was
            // deleted then the view is adjusted.
            if(get_view_range(inventory->view) > inventory->view.count)
            {
                set_view_at_end(&inventory->view);
            }
        }
        else
        {
            set_view_at_start(&inventory->view);
        }
        
        ui->defer_rect = get_window_rect();
        v2u header = get_inventory_header_pos(ui, ui->defer_rect);
        
        v2u pos = header;
        pos.y += ui->font_newline;
        
        set_view_end_value(items, &inventory->view, inventory->entry_size, pos, window_asset_y, CameFrom_Inventory);
        
        // Render inventory header
        if(inventory->using_item_type)
        {
            switch(inventory->using_item_type)
            {
                case UsingItemType_Identify: defer_text(ui, "Identify which item?", header.x, header.y); break;
                case UsingItemType_EnchantWeapon: defer_text(ui, "Enchant which weapon?", header.x, header.y); break;
                case UsingItemType_EnchantArmor: defer_text(ui, "Enchant which armor?", header.x, header.y); break;
                case UsingItemType_Uncurse: defer_text(ui, "Uncurse which item?", header.x, header.y); break;
                
                invalid_default_case;
            }
        }
        else
        {
            defer_text(ui, "Inventory: %u/%u slots", header.x, header.y, get_inventory_item_count(inventory), MAX_INVENTORY_SLOT_COUNT);
        }
        
        // Render inventory items
        u32 entry_count = 0;
        for(u32 type = ItemType_Weapon; type < ItemType_Count; ++type)
        {
            b32 needs_item_type_header = true;
            
            for(u32 index = 0; index < MAX_INVENTORY_SLOT_COUNT; ++index)
            {
                Item *item = inventory->slots[index];
                if(is_item_valid(item) && item->type == type)
                {
                    if(inventory->using_item_type &&
                       !item_fits_using_item_type(inventory->using_item_type, item))
                    {
                        continue;
                    }
                    
                    if(needs_item_type_header)
                    {
                        needs_item_type_header = false;
                        ++entry_count;
                        
                        if(is_entry_in_view(inventory->view, entry_count))
                        {
                            render_item_type_header(ui, ui->defer_rect, pos, type);
                            pos.y += inventory->entry_size;
                        }
                    }
                    
                    ++entry_count;
                    
                    if(is_entry_in_view(inventory->view, entry_count))
                    {
                        v2u picture_pos = {pos.x + 8, pos.y};
                        defer_texture(ui, picture_pos, item->tile_pos);
                        
                        v2u name_pos =
                        {
                            picture_pos.x + (ui->font_newline * 3),
                            picture_pos.y + (ui->font->size / 2)
                        };
                        
                        String128 letter = get_item_letter_string(item);
                        String128 mark_text = get_item_mark_string(item);
                        
                        if(is_item_consumable(item->type))
                        {
                            char stack_count_text[16] = {0};
                            if(item->c.stack_count > 1)
                            {
                                sprintf(stack_count_text, " (%u)", item->c.stack_count);
                            }
                            
                            if(is_set(item->flags, ItemFlags_Identified))
                            {
                                defer_text(ui, "%s%s%s%s", name_pos.x, name_pos.y, letter.str, item->name, stack_count_text, mark_text.str);
                            }
                            else
                            {
                                defer_text(ui, "%s%s%s%s%s", name_pos.x, name_pos.y, letter.str, item->c.depiction, get_item_id_text(item->id), stack_count_text, mark_text.str);
                            }
                        }
                        else
                        {
                            if(is_set(item->flags, ItemFlags_Identified))
                            {
                                char equipped_text[16] = {0};
                                if(is_set(item->flags, ItemFlags_Equipped))
                                {
                                    sprintf(equipped_text, " (equipped)");
                                }
                                
                                defer_text(ui, "%s%s%s%s%s%s",
                                           name_pos.x, name_pos.y,
                                           get_item_status_color(item),
                                           letter.str,
                                           get_item_status_prefix(item),
                                           get_full_item_name(item).str,
                                           equipped_text,
                                           mark_text.str);
                            }
                            else
                            {
                                defer_text(ui, "%s%s%s", name_pos.x, name_pos.y, letter.str, get_item_id_text(item->id), mark_text.str);
                            }
                        }
                        
                        pos.y += inventory->entry_size;
                    }
                }
            }
        }
        
        pos.y += ui->font_newline;
        inventory->view.count = entry_count;
        process_window_end(game, assets, ui, &inventory->view, pos);
        
#if 0
        printf("inventory->view.count: %u\n", inventory->view.count);
        printf("inventory->view.start: %u\n", inventory->view.start);
        printf("inventory->view.end: %u\n\n", inventory->view.end);
#endif
        
    }
    else if(is_set(inventory->flags, InventoryFlags_PickupOpen))
    {
        // TODO(rami):
        // When picking up multiple items, only pick up items that the inventory has
        // room for and notify the player if we run out of inventory room.
        
        // TODO(rami):
        // Inventory and pickup window item rendering do mostly the same thing, unify and
        // pass an enum which tells us where we got called from. That way we can run the
        // code that is relevant.
        
        ui->defer_rect = get_window_rect();
        v2u header = get_inventory_header_pos(ui, ui->defer_rect);
        
        char select_text[16] = {0};
        
        { // Set the select_text buffer with the amount of items selected
            u32 select_item_count = 0;
            
            for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
            {
                Item *item = &items[index];
                if(is_item_valid(item) && is_set(item->flags, ItemFlags_Select))
                {
                    ++select_item_count;
                }
            }
            
            if(select_item_count > 1)
            {
                sprintf(select_text, " (%u items)", select_item_count);
            }
            else if(select_item_count)
            {
                sprintf(select_text, " (%u item)", select_item_count);
            }
            
            //printf("select_text: %s\n", select_text);
        }
        
        defer_text(ui, "Pick up what? %u/%u slots%s", header.x, header.y, get_inventory_item_count(inventory), MAX_INVENTORY_SLOT_COUNT, select_text);
        header.y += ui->font_newline;
        
        v2u pos = header;
        set_view_end_value(items, &inventory->pickup_view, inventory->entry_size, pos, window_asset_y, CameFrom_Pickup);
        
        u32 entry_count = 0;
        for(u32 type = ItemType_Weapon; type < ItemType_Count; ++type)
        {
            b32 needs_item_type_header = true;
            
            for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
            {
                Item *item = &items[index];
                if(is_item_valid_and_not_in_inventory(item) &&
                       item->type == type &&
                       equal_v2u(item->pos, player->pos))
                {
                    if(needs_item_type_header)
                    {
                        needs_item_type_header = false;
                        ++entry_count;
                        
                        if(is_entry_in_view(inventory->pickup_view, entry_count))
                        {
                            render_item_type_header(ui, ui->defer_rect, pos, type);
                            pos.y += inventory->entry_size;
                        }
                    }
                    
                    ++entry_count;
                    
                    if(is_entry_in_view(inventory->pickup_view, entry_count))
                    {
                        v2u picture_pos = {pos.x + 8, pos.y};
                        defer_texture(ui, picture_pos, item->tile_pos);
                        
                        v2u name_pos =
                        {
                            picture_pos.x + (ui->font_newline * 3),
                            picture_pos.y + (ui->font->size / 2)
                        };
                        
                        if(!item->temp_letter)
                        {
                            item->temp_letter = get_free_item_letter(items, ItemLetterType_TempLetter);
                        }
                        
                        String128 letter = get_item_letter_string(item);
                        String128 mark_text = get_item_mark_string(item);
                        
                        if(is_item_consumable(item->type))
                        {
                            char stack_count_text[16] = {0};
                            if(item->c.stack_count > 1)
                            {
                                sprintf(stack_count_text, " (%u)", item->c.stack_count);
                            }
                            
                            if(is_set(item->flags, ItemFlags_Identified))
                            {
                                defer_text(ui, "%s%s%s%s", name_pos.x, name_pos.y, letter.str, item->name, stack_count_text, mark_text.str);
                            }
                            else
                            {
                                defer_text(ui, "%s%s%s%s%s", name_pos.x, name_pos.y, letter.str, item->c.depiction, get_item_id_text(item->id), stack_count_text, mark_text.str);
                            }
                        }
                        else
                        {
                            if(is_set(item->flags, ItemFlags_Identified))
                            {
                                char equipped_text[16] = {0};
                                if(is_set(item->flags, ItemFlags_Equipped))
                                {
                                    sprintf(equipped_text, " (equipped)");
                                }
                                
                                defer_text(ui, "%s%s%s%s%s%s",
                                           name_pos.x, name_pos.y,
                                           get_item_status_color(item),
                                           letter.str,
                                           get_item_status_prefix(item),
                                           get_full_item_name(item).str,
                                           equipped_text,
                                           mark_text.str);
                            }
                            else
                            {
                                defer_text(ui, "%s%s%s", name_pos.x, name_pos.y, letter.str, get_item_id_text(item->id), mark_text.str);
                            }
                        }
                        
                        pos.y += inventory->entry_size;
                    }
                }
            }
        }
        
        pos.y += ui->font_newline;
        inventory->pickup_view.count = entry_count;
        process_window_end(game, assets, ui, &inventory->pickup_view, pos);
        
        #if 0
        printf("inventory->pickup_view.count: %u\n", inventory->pickup_view.count);
        printf("inventory->pickup_view.start: %u\n", inventory->pickup_view.start);
        printf("inventory->pickup_view.end: %u\n\n", inventory->pickup_view.end);
#endif
        
        }
}