internal b32
view_needs_scrollbar(View view)
{
    b32 result = (view.entry_count > view.end);
    return(result);
}

internal void
update_scroll_view(MouseScrollMove scroll_move, View *view)
{
    if(scroll_move == MouseScrollMove_Up)
    {
        if((view->start - 1) > 0)
        {
            --view->start;
        }
    }
    else if(scroll_move == MouseScrollMove_Down)
    {
        if(get_view_range(*view) <= view->entry_count)
        {
            ++view->start;
        }
    }
}

internal void
set_scroll_view_at_start(View *view)
{
    view->start = 1;
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

internal String128
get_item_letter_string(char letter)
{
    String128 result = {0};
    sprintf(result.str, "%c) ", letter);
    
    return(result);
}

internal u32
get_text_width(Font *font, char *text)
{
    u32 result = 0;
    
    for(char *c = text; *c; ++c)
    {
        u32 index = get_metric_index(*c);
        result += font->metrics[index].advance;
    }
    
    assert(result);
    return(result);
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

internal v4u
center_rect_to_screen(v2u window_size, Assets *assets, v4u *rect)
{
    assert(rect->w);
    assert(rect->h);
    
    rect->x = (window_size.w / 2) - (rect->w / 2);
    rect->y = (window_size.h - assets->stat_and_log_window_h - rect->h) / 2;
}

internal v2u
get_initialized_header(v4u rect, UI *ui)
{
    v2u result =
    {
        rect.x + (ui->window_offset * 2),
        rect.y + ui->window_offset
    };
    
    return(result);
}

internal v4u
get_initialized_inspect_rect()
{
    v4u result = {0};
    result.w = 576;
    
    return(result);
}

internal v2u
render_item_information(Game *game, UI *ui, Item *item, v2u header, v2u info, b32 inspecting_from_inventory)
{
    v2u result = info;
    String128 letter = get_item_letter_string(item->letter);
    
    // Picture and name offset
    add_render_queue_texture(ui->render_queue, header, item->tile_pos);
    header.x += ui->window_offset * 4;
    header.y += ui->font->size / 2;
    
    if(item->is_identified)
    {
        String128 item_name = full_item_name(item);
        add_render_queue_text(ui->render_queue, "%s%s%s%s",
                              header.x, header.y,
                              item_status_color(item->is_cursed),
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
        else if(item->type == ItemType_Potion)
        {
            // TODO(rami): Add text.
            switch(item->id)
            {
                case ItemID_MightPotion: break;
                case ItemID_WisdomPotion: break;
                case ItemID_AgilityPotion: break;
                case ItemID_ElusionPotion: break;
                case ItemID_HealingPotion: break;
                case ItemID_DecayPotion: break;
                case ItemID_ConfusionPotion: break;
                
                invalid_default_case;
            }
        }
        else if(item->type == ItemType_Scroll)
        {
            // TODO(rami): Add text.
            switch(item->id)
            {
                case ItemID_IdentifyScroll: break;
                case ItemID_EnchantWeaponScroll: break;
                case ItemID_EnchantArmorScroll: break;
                case ItemID_MagicMappingScroll: break;
                case ItemID_TeleportationScroll: break;
                
                invalid_default_case;
            }
        }
        else if(item->id == ItemID_Ration)
        {
            result.y += ui->font_newline;
            add_render_queue_text(ui->render_queue, "Consuming rations will restore some of your hitpoints.", result.x, result.y);
            
            result.y += ui->font_newline;
            add_render_queue_text(ui->render_queue, "The amount restored is between %u and %u.", result.x, result.y, RATION_RANGE_MIN, RATION_RANGE_MAX);
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
    
    if(item->type == ItemType_Weapon)
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
        
        result.y += ui->font_newline;
        
        if(item->handedness == ItemHandedness_OneHanded)
        {
            add_render_queue_text(ui->render_queue, "It is a one-handed weapon.", result.x, result.y);
        }
        else if(item->handedness == ItemHandedness_TwoHanded)
        {
            add_render_queue_text(ui->render_queue, "It is a two-handed weapon.", result.x, result.y);
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
        else if(item->type == ItemType_Potion)
        {
            char *drink = "(d)rink";
            add_render_queue_text(ui->render_queue, drink, result.x, result.y);
            result.x += get_text_width(ui->font, drink) + padding;
        }
        else if(item->type == ItemType_Scroll)
        {
            char *read = "(r)ead";
            add_render_queue_text(ui->render_queue, read, result.x, result.y);
            result.x += get_text_width(ui->font, read) + padding;
        }
        else if(item->type == ItemType_Ration)
        {
            char *eat = "(e)at";
            add_render_queue_text(ui->render_queue, eat, result.x, result.y);
            result.x += get_text_width(ui->font, eat) + padding;
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

internal void
set_inventory_scrolling(Inventory *inventory, u32 scroll_entry_count)
{
    if(!inventory->scroll_view.end)
    {
        inventory->scroll_view.end = scroll_entry_count;
    }
}

internal u32
get_view_range(View view)
{
    u32 result = (view.start + view.end);
    return(result);
}

internal b32
is_inventory_element_in_view(View view, u32 element)
{
    b32 result = (!view.end || (element >= view.start &&
                                element <= get_view_range(view)));
    
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
          Assets *assets)
{
    Examine *examine = &game->examine;
    u32 window_y_offset = game->window_size.h - assets->stat_and_log_window_h;
    
    v4u stat_window = {0, window_y_offset, 388, assets->stat_and_log_window_h};
    render_window(game, stat_window, 2);
    
    v4u log_rect = {stat_window.w + 4, window_y_offset, game->window_size.w - log_rect.x, assets->stat_and_log_window_h};
    render_window(game, log_rect, 2);
    
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
    
    // Render Log
    u32 message_x = log_rect.x + ui->window_offset;
    u32 message_y = log_rect.y + (ui->font->size / 2);
    
#if 0
    printf("full_log.start_index: %u\n", ui->full_log.start_index);
    printf("short_log.start_index: %u\n", ui->short_log.start_index);
#endif
    
    assert(ui->short_log_view.end &&
           ui->full_log_view.end);
    
    // Short Log
    for(u32 index = ui->short_log_view.start;
        index < get_view_range(ui->short_log_view);
        ++index)
    {
        if(ui->log_messages[index].str[0])
        {
            render_text(game, ui->log_messages[index].str, message_x, message_y, ui->font, 0);
            message_y += ui->font_newline;
        }
    }
    
    // Full Log
    if(ui->is_full_log_open)
    {
        v4u full_log_rect = {0};
        full_log_rect.w = 640;
        
        u32 message_x = full_log_rect.x + ui->window_offset;
        u32 message_y = full_log_rect.y + ui->window_offset;
        
        // Get entry count.
        ui->full_log_view.entry_count = 0;
        
        for(u32 index = 0; index < MAX_LOG_MESSAGE_COUNT; ++index)
        {
            if(ui->log_messages[index].str[0])
            {
                ++ui->full_log_view.entry_count;
            }
        }
        
        // Set view at end if we need a scrollbar.
        if(view_needs_scrollbar(ui->full_log_view) &&
           !ui->is_full_log_view_at_end)
        {
            ui->is_full_log_view_at_end = true;
            ui->full_log_view.start = ui->full_log_view.entry_count - (ui->full_log_view.end - 1);
        }
        
        // Render full log messages.
        for(u32 index = 0; index < MAX_LOG_MESSAGE_COUNT; ++index)
        {
            if(ui->log_messages[index].str[0])
            {
                if((index + 1) >= ui->full_log_view.start &&
                   index <= ui->full_log_view.start + (ui->full_log_view.end - 2))
                {
                    add_render_queue_text(ui->render_queue, ui->log_messages[index].str, message_x, message_y, ui->font);
                    message_y += ui->font_newline;
                }
            }
        }
        
        message_y += (ui->font_newline / 2);
        
        full_log_rect.h = message_y;
        center_rect_to_screen(game->window_size, assets, &full_log_rect);
        render_window(game, full_log_rect, 2);
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
    else if(examine->is_inspecting)
    {
        v4u inspect_rect = get_initialized_inspect_rect();
        v2u header = get_initialized_header(inspect_rect, ui);
        v2u info = header;
        
        Item *item = game->examine.item;
        Entity *entity = game->examine.entity;
        
        if(examine->type == InspectType_Item)
        {
            info = render_item_information(game, ui, item, header, info, false);
        }
        else if(examine->type == InspectType_Entity)
        {
            add_render_queue_texture(ui->render_queue, header, entity->tile_pos);
            
            // TODO(rami): Queue
            // TODO(rami): Add text.
            render_text(game, "%s", header.x, header.y, ui->font, 0, entity->name);
            info.y += (ui->font_newline * 2);
        }
        else if(examine->type == InspectType_Tile)
        {
            add_render_queue_texture(ui->render_queue, header, get_dungeon_tile_pos(dungeon->tiles, examine->pos));
            
            // TODO(rami): Which tile? Add text.
            TileID id = examine->tile_id;
        }
        
        inspect_rect.h = info.y;
        center_rect_to_screen(game->window_size, assets, &inspect_rect);
        render_window(game, inspect_rect, 2);
        
        process_render_queue(game, assets, ui, inspect_rect.x, inspect_rect.y);
    }
    else if(inventory->is_inspecting)
    {
        Item *item = inventory->slots[inventory->inspect_index];
        
        v4u inspect_rect = get_initialized_inspect_rect();
        v2u header = get_initialized_header(inspect_rect, ui);
        v2u info = header;
        
        info = render_item_information(game, ui, item, header, info, true);
        
        inspect_rect.h = info.y;
        center_rect_to_screen(game->window_size, assets, &inspect_rect);
        render_window(game, inspect_rect, 2);
        
        process_render_queue(game, assets, ui, inspect_rect.x, inspect_rect.y);
    }
    else if(inventory->is_open)
    {
        v4u inventory_rect = {0};
        inventory_rect.w = 640;
        
        v2u pos =
        {
            inventory_rect.x + (ui->window_offset * 2),
            inventory_rect.y + (ui->window_offset * 2)
        };
        
        add_render_queue_text(ui->render_queue, "Inventory", pos.x, pos.y);
        pos.y += ui->font_newline;
        
        inventory->scroll_view.entry_count = 0;
        u32 element_size = 32;
        char item_letter = 'a';
        
        for(u32 type = ItemType_Weapon; type < ItemType_Count - 1; ++type)
        {
            b32 should_add_type_header = true;
            
            for(u32 index = 0; index < INVENTORY_SLOT_COUNT; ++index)
            {
                Item *item = inventory->slots[index];
                if(is_item_valid_and_in_inventory(item) &&
                   item->type == type)
                {
                    if(should_add_type_header)
                    {
                        should_add_type_header = false;
                        ++inventory->scroll_view.entry_count;
                        
                        if(is_inventory_element_in_view(inventory->scroll_view, inventory->scroll_view.entry_count))
                        {
                            if(pos.y + element_size >= window_y_offset)
                            {
                                set_inventory_scrolling(inventory, inventory->scroll_view.entry_count);
                            }
                            else
                            {
                                //add_render_queue_fill_rect(ui->render_queue, pos.x, pos.y, element_size, element_size, Color_LightGreen);
                                
                                v2u header =
                                {
                                    pos.x,
                                    pos.y + (ui->font_newline / 2)
                                };
                                
                                add_render_queue_fill_rect(ui->render_queue,
                                                           header.x, header.y - 4,
                                                           (inventory_rect.w - header.x - ui->window_offset * 2), 1,
                                                           Color_WindowAccent);
                                
                                switch(type)
                                {
                                    
                                    case ItemType_Weapon: add_render_queue_text(ui->render_queue, "Weapon", header.x, header.y, ui->font); break;
                                    case ItemType_Armor: add_render_queue_text(ui->render_queue, "Armor", header.x, header.y, ui->font); break;
                                    case ItemType_Potion: add_render_queue_text(ui->render_queue, "Potion", header.x, header.y, ui->font); break;
                                    case ItemType_Scroll: add_render_queue_text(ui->render_queue, "Scroll", header.x, header.y, ui->font); break;
                                    case ItemType_Ration: add_render_queue_text(ui->render_queue, "Ration", header.x, header.y, ui->font); break;
                                    
                                    invalid_default_case;
                                }
                                
                                pos.y += element_size;
                            }
                        }
                    }
                    
                    ++inventory->scroll_view.entry_count;
                    
                    if(is_inventory_element_in_view(inventory->scroll_view, inventory->scroll_view.entry_count))
                    {
                        item->letter = item_letter;
                        
                        if(pos.y + element_size >= window_y_offset)
                        {
                            // Element count was incremented above but since this went over
                            // window_y_offset, we don't count it so we decrement the element count.
                            set_inventory_scrolling(inventory, inventory->scroll_view.entry_count - 1);
                        }
                        else
                        {
                            //add_render_queue_fill_rect(ui->render_queue, pos.x, pos.y, element_size, element_size, Color_LightRed);
                            
                            v2u picture =
                            {
                                pos.x + 8,
                                pos.y
                            };
                            
                            add_render_queue_texture(ui->render_queue, picture, item->tile_pos);
                            
                            String128 letter = get_item_letter_string(item->letter);
                            v2u name =
                            {
                                picture.x + (ui->font_newline * 3),
                                picture.y + (ui->font->size / 2)
                            };
                            
                            if(is_item_consumable(item->type) && item->c.stack_count > 1)
                            {
                                if(item->is_identified)
                                {
                                    add_render_queue_text(ui->render_queue, "%s%s (%u)", name.x, name.y, letter.str, item->name, item->c.stack_count);
                                }
                                else
                                {
                                    add_render_queue_text(ui->render_queue, "%s%s %s (%u)", name.x, name.y, letter.str, item->c.visual_text, item_id_text(item->id), item->c.stack_count);
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
                                                          name.x, name.y,
                                                          item_status_color(item->is_cursed),
                                                          letter.str,
                                                          item_status_prefix(item->is_cursed),
                                                          item_name.str,
                                                          equipped_text);
                                }
                                else
                                {
                                    add_render_queue_text(ui->render_queue, "%s%s", name.x, name.y, letter.str, item_id_text(item->id));
                                }
                            }
                            
                            pos.y += element_size;
                        }
                    }
                    
                    if(item_letter == 'z')
                    {
                        item_letter = 'A';
                    }
                    else
                    {
                        ++item_letter;
                    }
                }
            }
        }
        
        pos.y += ui->font_newline;
        
        inventory_rect.h = pos.y;
        center_rect_to_screen(game->window_size, assets, &inventory_rect);
        render_window(game, inventory_rect, 2);
        process_render_queue(game, assets, ui, inventory_rect.x, inventory_rect.y);
        
        inventory->rect = inventory_rect;
        
#if 0
        printf("inventory->scroll_view.entry_count: %u\n", inventory->scroll_view.entry_count);
        printf("inventory->scroll_view.start: %u\n", inventory->scroll_view.start);
        printf("inventory->scroll_view.end: %u\n\n", inventory->scroll_view.end);
        
        printf("inventory.x: %u\n", inventory_rect.x);
        printf("inventory.y: %u\n", inventory_rect.y);
        printf("inventory.w: %u\n", inventory_rect.w);
        printf("inventory.h: %u\n\n", inventory_rect.h);
#endif
        
        if(inventory->scroll_view.end &&
           view_needs_scrollbar(inventory->scroll_view))
        {
            render_scrollbar(game, ui, inventory_rect, inventory->scroll_view);
        }
    }
}