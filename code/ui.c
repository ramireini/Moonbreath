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
item_fits_inventory_using_item_type(Inventory *inventory, Item *item)
{
    assert(inventory->using_item_type);
    
    b32 result = false;
    
    if((inventory->using_item_type == UsingItemType_Identify && !item->is_identified) ||
       (inventory->using_item_type == UsingItemType_EnchantWeapon && item->type == ItemType_Weapon) ||
       (inventory->using_item_type == UsingItemType_EnchantArmor && item->type == ItemType_Armor) ||
       (inventory->using_item_type == UsingItemType_Uncurse && is_item_cursed_and_identified(item)))
    {
        result = true;
    }
    
    return(result);
}

internal void
render_item_type_header(UI *ui, v4u rect, v2u pos, ItemType type)
{
    v2u header_pos = {pos.x, pos.y + (ui->font_newline / 2)};
    add_render_queue_fill_rect(ui->render_queue,
                               header_pos.x, header_pos.y - 4,
                               rect.w - header_pos.x - (ui->window_offset * 2),
                               1,
                               Color_WindowAccent);
    
    switch(type)
    {
        
        case ItemType_Weapon: add_render_queue_text(ui->render_queue, "Weapon", header_pos.x, header_pos.y); break;
        case ItemType_Armor: add_render_queue_text(ui->render_queue, "Armor", header_pos.x, header_pos.y); break;
        case ItemType_Potion: add_render_queue_text(ui->render_queue, "Potion", header_pos.x, header_pos.y); break;
        case ItemType_Scroll: add_render_queue_text(ui->render_queue, "Scroll", header_pos.x, header_pos.y); break;
        case ItemType_Ration: add_render_queue_text(ui->render_queue, "Ration", header_pos.x, header_pos.y); break;
        
        invalid_default_case;
    }
}

internal v2u
get_inventory_text_start_pos(UI *ui, v4u rect)
{
    v2u result =
    {
        rect.x + (ui->window_offset * 2),
        rect.y + (ui->window_offset * 2)
    };
    
    return(result);
}

internal b32
view_needs_scrollbar(View view)
{
    b32 result = (view.end && (view.entry_count > view.end));
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
render_scrollbar(Game *game, UI *ui, v4u rect, View view)
{
    u32 rect_gutter_x = rect.x + rect.w - 10;
    
    v4u gutter = {0};
    gutter.x = rect_gutter_x;
    gutter.y = rect.y;
    gutter.w = 2;
    
    // Get the correct gutter height.
    u32 scrollbar_size = (rect.h - (ui->font_newline * 4)) / view.entry_count;
    gutter.h = scrollbar_size * view.entry_count;
    
    // Center gutter vertically relative to rect.
    gutter.y += ((rect.h - gutter.h) / 2);
    render_fill_rect(game, gutter, Color_WindowAccent);
    
    v4u scrollbar = {0};
    scrollbar.x = rect_gutter_x;
    scrollbar.y = gutter.y + (scrollbar_size * (view.start - 1));
    scrollbar.w = gutter.w;
    scrollbar.h = scrollbar_size * view.end;
    render_fill_rect(game, scrollbar, Color_WindowBorder);
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
    result.w = 640;
    
    return(result);
}

internal v4u
get_inspect_rect()
{
    v4u result = {0};
    result.w = 576;
    
    return(result);
}

internal v2u
render_inspect_item_information(Game *game, UI *ui, Item *item, ItemInfo *item_info, v2u header, v2u info, b32 inspecting_from_inventory)
{
    v2u result = info;
    String128 letter = get_item_letter_string(item->inventory_letter);
    
    // Picture and name offset
    add_render_queue_texture(ui->render_queue, header, item->tile_pos);
    header.x += ui->window_offset * 4;
    header.y += ui->font->size / 2;
    
    if(item->is_identified)
    {
        String128 item_name = full_item_name(item);
        add_render_queue_text(ui->render_queue, "%s%s%s%s",
                              header.x, header.y,
                              item_status_color(item),
                              letter.str,
                              item_status_prefix(item->is_cursed),
                              item_name.str);
        
        result.y += (ui->font_newline * 2);
        
        if(item->type == ItemType_Weapon)
        {
            result.y += ui->font_newline;
            add_render_queue_text(ui->render_queue, "Damage: %d", result.x, result.y, item->w.damage + item->enchantment_level);
            
            result.y += ui->font_newline;
            add_render_queue_text(ui->render_queue, "Accuracy: %d", result.x, result.y, item->w.accuracy + item->enchantment_level);
            
            result.y += ui->font_newline;
            add_render_queue_text(ui->render_queue, "Attack Speed: %.1f", result.x, result.y, item->w.speed);
        }
        else if(item->type == ItemType_Armor)
        {
            result.y += ui->font_newline;
            add_render_queue_text(ui->render_queue, "Defence: %d", result.x, result.y, item->a.defence + item->enchantment_level);
            
            result.y += ui->font_newline;
            add_render_queue_text(ui->render_queue, "Weight: %d", result.x, result.y, item->a.weight);
        }
        else if(is_item_consumable(item->type))
        {
            result.y += ui->font_newline;
            add_render_queue_text(ui->render_queue, "%s", result.x, result.y, item->description);
        }
    }
    else
    {
        add_render_queue_text(ui->render_queue, "%s%s", header.x, header.y, letter.str, item_id_text(item->id));
        result.y += (ui->font_newline * 2);
        
        if(item->type == ItemType_Weapon)
        {
            result.y += ui->font_newline;
            add_render_queue_text(ui->render_queue, "Base Damage: %u", result.x, result.y, item->w.damage);
            
            result.y += ui->font_newline;
            add_render_queue_text(ui->render_queue, "Base Accuracy: %d", result.x, result.y, item->w.accuracy);
            
            result.y += ui->font_newline;
            add_render_queue_text(ui->render_queue, "Base Attack Speed: %.1f", result.x, result.y, item->w.speed);
        }
        else if(item->type == ItemType_Armor)
        {
            result.y += ui->font_newline;
            add_render_queue_text(ui->render_queue, "Base Defence: %d", result.x, result.y, item->a.defence);
            
            result.y += ui->font_newline;
            add_render_queue_text(ui->render_queue, "Base Weight: %d", result.x, result.y, item->a.weight);
        }
        else if(item->type == ItemType_Potion)
        {
            result.y += ui->font_newline;
            add_render_queue_text(ui->render_queue, "Consuming potions will bestow you with different effects.", result.x, result.y);
            
            result.y += ui->font_newline;
            add_render_queue_text(ui->render_queue, "Some of these effects will be helpful, while others harmful.", result.x, result.y);
        }
        else if(item->type == ItemType_Scroll)
        {
            result.y += ui->font_newline;
            add_render_queue_text(ui->render_queue, "Reading scrolls will bring out different magical effects.", result.x, result.y);
        }
    }
    
    result.y += ui->font_newline * 2;
    
    if(item->is_identified &&
       item->is_cursed)
    {
        add_render_queue_text(ui->render_queue, "It is a cursed item.", result.x, result.y);
        result.y += ui->font_newline;
    }
    
    if(is_item_equipment(item->type))
    {
        if(item->rarity == ItemRarity_Common)
        {
            add_render_queue_text(ui->render_queue, "It is of common rarity.", result.x, result.y);
        }
        else if(item->rarity == ItemRarity_Magical)
        {
            add_render_queue_text(ui->render_queue, "It is of magical rarity.", result.x, result.y);
        }
        else if(item->rarity == ItemRarity_Mythical)
        {
            add_render_queue_text(ui->render_queue, "It is of mythical rarity.", result.x, result.y);
        }
        
        if(item->type == ItemType_Weapon)
        {
            result.y += ui->font_newline;
            
            if(item->handedness == ItemHandedness_OneHanded)
            {
                add_render_queue_text(ui->render_queue, "It is a one-handed weapon.", result.x, result.y);
            }
            else if(item->handedness == ItemHandedness_TwoHanded)
            {
                add_render_queue_text(ui->render_queue, "It is a two-handed weapon.", result.x, result.y);
            }
        }
        
        result.y += ui->font_newline * 2;
    }
    
    if(inspecting_from_inventory)
    {
        u32 padding = 10;
        
        char *adjust = "(a)djust";
        add_render_queue_text(ui->render_queue, adjust, result.x, result.y);
        result.x += get_text_width(ui->font, adjust) + padding;
        
        if(is_item_equipment(item->type))
        {
            if(item->is_equipped)
            {
                char *unequip = "(u)nequip";
                add_render_queue_text(ui->render_queue, unequip, result.x, result.y);
                result.x += get_text_width(ui->font, unequip) + padding;
            }
            else
            {
                char *equip = "(e)quip";
                add_render_queue_text(ui->render_queue, equip, result.x, result.y);
                result.x += get_text_width(ui->font, equip) + padding;
            }
        }
        else if(item->type == ItemType_Potion ||
                item->type == ItemType_Ration)
        {
            char *consume = "(c)onsume";
            add_render_queue_text(ui->render_queue, consume, result.x, result.y);
            result.x += get_text_width(ui->font, consume) + padding;
        }
        else if(item->type == ItemType_Scroll)
        {
            char *read = "(r)ead";
            add_render_queue_text(ui->render_queue, read, result.x, result.y);
            result.x += get_text_width(ui->font, read) + padding;
        }
        
        char *drop = "(d)rop";
        add_render_queue_text(ui->render_queue, drop, result.x, result.y);
        result.x += get_text_width(ui->font, drop) + padding;
        
        char *mark = "(m)ark";
        add_render_queue_text(ui->render_queue, mark, result.x, result.y);
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
        render_text(game, "Strength:     %u", left.x, left.y, ui->font, 0, player->p.strength);
        
        left.y += ui->font_newline;
        render_text(game, "Intelligence: %u", left.x, left.y, ui->font, 0, player->p.intelligence);
        
        left.y += ui->font_newline;
        render_text(game, "Dexterity:    %u", left.x, left.y, ui->font, 0, player->p.dexterity);
        
        left.y += ui->font_newline;
        render_text(game, "Defence:      %u", left.x, left.y, ui->font, 0, player->defence);
        
        left.y += ui->font_newline;
        render_text(game, "Evasion:      %u", left.x, left.y, ui->font, 0, player->evasion);
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
        render_text(game, "Action time:   %.01f", right.x, right.y, ui->font, 0, player->action_time);
        
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
                    add_render_queue_text(ui->render_queue, ui->log_messages[index].str, full_log_message_pos.x, full_log_message_pos.y);
                    full_log_message_pos.y += ui->font_newline;
                }
            }
        }
        
        full_log_message_pos.y += (ui->font_newline / 2);
        
        full_log_rect.h = full_log_message_pos.y;
        center_and_render_window_to_available_screen(game, assets, &full_log_rect, 2);
        
#if 0
        print_v4u("full_log_rect", full_log_rect);
#endif
        
        process_render_queue(game, assets, ui, full_log_rect.x, full_log_rect.y);
        
        ui->full_log_rect = full_log_rect;
        
        if(view_needs_scrollbar(ui->full_log_view))
        {
            render_scrollbar(game, ui, full_log_rect, ui->full_log_view);
        }
        
#if 0
        printf("full_log.entry_count: %u\n", ui->full_log_view.entry_count);
        printf("full_log.start: %u\n", ui->full_log_view.start);
        printf("full_log.end: %u\n\n", ui->full_log_view.end);
#endif
    }
    else if(examine->is_open)
    {
        v4u dest = get_game_dest(game, game->examine.pos);
        SDL_RenderCopy(game->renderer, assets->ui.tex, (SDL_Rect *)&assets->yellow_outline_src, (SDL_Rect *)&dest);
    }
    else if(examine->inspect_type)
    {
        v4u inspect_rect = get_inspect_rect();
        v2u header = get_header_pos(ui, inspect_rect);
        v2u info = header;
        
        Item *item = game->examine.item;
        Entity *entity = game->examine.entity;
        
        if(examine->inspect_type == InspectType_Item)
        {
            info = render_inspect_item_information(game, ui, item, item_info, header, info, false);
        }
        else if(examine->inspect_type == InspectType_Entity)
        {
            add_render_queue_texture(ui->render_queue, header, entity->tile_pos);
            
            // TODO(rami): Queue
            // TODO(rami): Add text.
            render_text(game, "%s", header.x, header.y, ui->font, 0, entity->name);
            info.y += (ui->font_newline * 2);
        }
        else if(examine->inspect_type == InspectType_Tile)
        {
            add_render_queue_texture(ui->render_queue, header, get_dungeon_tile_pos(dungeon->tiles, examine->pos));
            
            // TODO(rami): Which tile? Add text.
            TileID id = examine->tile_id;
        }
        
        inspect_rect.h = info.y;
        center_and_render_window_to_available_screen(game, assets, &inspect_rect, 2);
        process_render_queue(game, assets, ui, inspect_rect.x, inspect_rect.y);
    }
    else if(inventory->is_inspecting)
    {
        Item *item = inventory->slots[inventory->inspect_index];
        
        v4u inspect_rect = get_inspect_rect();
        v2u header = get_header_pos(ui, inspect_rect);
        v2u info = header;
        
        info = render_inspect_item_information(game, ui, item, item_info, header, info, true);
        
        inspect_rect.h = info.y;
        center_and_render_window_to_available_screen(game, assets, &inspect_rect, 2);
        process_render_queue(game, assets, ui, inspect_rect.x, inspect_rect.y);
    }
    else if(inventory->is_open)
    {
        if(inventory->using_item_type)
        {
            v4u item_use_rect = get_window_rect();
            
            v2u pos = get_inventory_text_start_pos(ui, item_use_rect);
            switch(inventory->using_item_type)
            {
                case UsingItemType_Identify: add_render_queue_text(ui->render_queue, "Identify which item?", pos.x, pos.y); break;
                case UsingItemType_EnchantWeapon: add_render_queue_text(ui->render_queue, "Enchant which weapon?", pos.x, pos.y); break;
                case UsingItemType_EnchantArmor: add_render_queue_text(ui->render_queue, "Enchant which armor?", pos.x, pos.y); break;
                case UsingItemType_Uncurse: add_render_queue_text(ui->render_queue, "Uncurse which item?", pos.x, pos.y); break;
                
                invalid_default_case;
            }
            
            pos.y += ui->font_newline;
            
            for(u32 type = ItemType_Weapon; type < ItemType_Count; ++type)
            {
                b32 needs_item_type_header = true;
                
                for(u32 index = 0; index < INVENTORY_SLOT_COUNT; ++index)
                {
                    Item *item = inventory->slots[index];
                    if(is_item_valid_and_in_inventory(item) &&
                       item->type == type &&
                       item_fits_inventory_using_item_type(inventory, item))
                    {
                        if(needs_item_type_header)
                        {
                            needs_item_type_header = false;
                            render_item_type_header(ui, item_use_rect, pos, type);
                            pos.y += inventory->entry_size;
                        }
                        
                        v2u picture_pos = {pos.x + 8, pos.y};
                        add_render_queue_texture(ui->render_queue, picture_pos, item->tile_pos);
                        
                        v2u name_pos =
                        {
                            picture_pos.x + (ui->font_newline * 3),
                            picture_pos.y + (ui->font->size / 2)
                        };
                        
                        String128 letter_string = get_item_letter_string(item->inventory_letter);
                        
                        if(is_item_consumable(item->type) && item->c.stack_count > 1)
                        {
                            if(item->is_identified)
                            {
                                add_render_queue_text(ui->render_queue, "%s%s (%u)", name_pos.x, name_pos.y, letter_string.str, item->name, item->c.stack_count);
                            }
                            else
                            {
                                add_render_queue_text(ui->render_queue, "%s%s %s (%u)", name_pos.x, name_pos.y, letter_string.str, item->c.visual_text, item_id_text(item->id), item->c.stack_count);
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
                                add_render_queue_text(ui->render_queue, "%s%s%s%s%s",
                                                      name_pos.x, name_pos.y,
                                                      item_status_color(item),
                                                      letter_string.str,
                                                      item_status_prefix(item->is_cursed),
                                                      item_name.str,
                                                      equipped_text);
                            }
                            else
                            {
                                add_render_queue_text(ui->render_queue, "%s%s", name_pos.x, name_pos.y, letter_string.str, item_id_text(item->id));
                            }
                        }
                        
                        pos.y += inventory->entry_size;
                    }
                }
            }
            
            pos.y += ui->font_newline;
            
            item_use_rect.h = pos.y;
            center_and_render_window_to_available_screen(game, assets, &item_use_rect, 2);
            process_render_queue(game, assets, ui, item_use_rect.x, item_use_rect.y);
        }
        else
        {
            v4u inventory_rect = get_window_rect();
            
            v2u pos = get_inventory_text_start_pos(ui, inventory_rect);
            add_render_queue_text(ui->render_queue, "Inventory", pos.x, pos.y);
            pos.y += ui->font_newline;
            
            u32 entry_count = 0;
            v2u test_pos = pos;
            for(u32 type = ItemType_Weapon; type < ItemType_Count; ++type)
            {
                b32 needs_item_type_header = true;
                
                for(u32 index = 0; index < INVENTORY_SLOT_COUNT; ++index)
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
                        
                        ++entry_count;
                        
                        if(entry_has_space(test_pos, inventory->entry_size, window_asset_y))
                        {
                            test_pos.y += inventory->entry_size;
                        }
                        else
                        {
                            init_view_end(&inventory->view, entry_count - 1);
                        }
                    }
                }
            }
            
            entry_count = 0;
            for(u32 type = ItemType_Weapon; type < ItemType_Count; ++type)
            {
                b32 needs_item_type_header = true;
                
                for(u32 index = 0; index < INVENTORY_SLOT_COUNT; ++index)
                {
                    Item *item = inventory->slots[index];
                    if(is_item_valid_and_in_inventory(item) &&
                       item->type == type)
                    {
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
                            add_render_queue_texture(ui->render_queue, picture_pos, item->tile_pos);
                            
                            v2u name_pos =
                            {
                                picture_pos.x + (ui->font_newline * 3),
                                picture_pos.y + (ui->font->size / 2)
                            };
                            
                            String128 letter_string = get_item_letter_string(item->inventory_letter);
                            
                            if(is_item_consumable(item->type) && item->c.stack_count > 1)
                            {
                                if(item->is_identified)
                                {
                                    add_render_queue_text(ui->render_queue, "%s%s (%u)", name_pos.x, name_pos.y, letter_string.str, item->name, item->c.stack_count);
                                }
                                else
                                {
                                    add_render_queue_text(ui->render_queue, "%s%s %s (%u)", name_pos.x, name_pos.y, letter_string.str, item->c.visual_text, item_id_text(item->id), item->c.stack_count);
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
                                    add_render_queue_text(ui->render_queue, "%s%s%s%s%s",
                                                          name_pos.x, name_pos.y,
                                                          item_status_color(item),
                                                          letter_string.str,
                                                          item_status_prefix(item->is_cursed),
                                                          item_name.str,
                                                          equipped_text);
                                }
                                else
                                {
                                    add_render_queue_text(ui->render_queue, "%s%s", name_pos.x, name_pos.y, letter_string.str, item_id_text(item->id));
                                }
                            }
                            
                            pos.y += inventory->entry_size;
                        }
                    }
                }
            }
            
            pos.y += ui->font_newline;
            
            inventory_rect.h = pos.y;
            center_and_render_window_to_available_screen(game, assets, &inventory_rect, 2);
            process_render_queue(game, assets, ui, inventory_rect.x, inventory_rect.y);
            
            inventory->rect = inventory_rect;
            inventory->view.entry_count = entry_count;
            
#if 0
            printf("inventory->view.entry_count: %u\n", inventory->view.entry_count);
            printf("inventory->view.start: %u\n", inventory->view.start);
            printf("inventory->view.end: %u\n\n", inventory->view.end);
#endif
            
            if(view_needs_scrollbar(inventory->view))
            {
                render_scrollbar(game, ui, inventory_rect, inventory->view);
            }
        }
    }
}