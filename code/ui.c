internal u32
get_view_range(View view)
{
    u32 result = view.start + view.count;
    return(result);
}

internal b32
is_inventory_element_in_view(View view, u32 element)
{
    b32 result = (!view.count || (element >= view.start &&
                                  element <= get_view_range(view)));
    
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
get_centered_asset_x(v2u window_size, u32 asset_w)
{
    u32 result = ((window_size.w / 2) - (asset_w / 2));
    return(result);
}

internal u32
get_centered_asset_y(v2u window_size, Assets *assets, u32 asset_h)
{
    u32 result = ((window_size.h - assets->stat_and_log_window_h - asset_h) / 2);
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
    u32 message_count = view->count - 1;
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
        update_log_view(&ui->full_log, index);
        update_log_view(&ui->short_log, index);
        
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
          Dungeon *dungeon,
          Entity *player,
          UI *ui,
          Inventory *inventory,
          Assets *assets)
{
    u32 window_y_offset = game->window_size.h - assets->stat_and_log_window_h;
    
    v4u stat_window_dest = {0, window_y_offset, 388, assets->stat_and_log_window_h};
    render_window(game, stat_window_dest, 2);
    
    v4u log_window_dest = {stat_window_dest.w + 4, window_y_offset, game->window_size.w - log_window_dest.x, assets->stat_and_log_window_h};
    render_window(game, log_window_dest, 2);
    
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
    u32 message_x = log_window_dest.x + ui->window_offset;
    u32 message_y = log_window_dest.y + (ui->font->size / 2);
    
#if 0
    printf("full_log.start_index: %u\n", ui->full_log.start_index);
    printf("short_log.start_index: %u\n", ui->short_log.start_index);
#endif
    
    assert(ui->short_log.count);
    assert(ui->full_log.count);
    
    // Short Log
    for(u32 index = ui->short_log.start;
        index < get_view_range(ui->short_log);
        ++index)
    {
        if(ui->log_messages[index].str[0])
        {
            render_text(game, ui->log_messages[index].str, message_x, message_y, ui->font, 0);
            message_y += ui->font_newline;
        }
    }
    
    Examine *examine = &game->examine;
    
    // Full Log
    if(ui->is_full_log_open)
    {
        v4u full_log_window_dest = {0};
        full_log_window_dest.w = 640;
        
        u32 message_x = full_log_window_dest.x + ui->window_offset;
        u32 message_y = full_log_window_dest.y + ui->window_offset;
        
        for(u32 index = ui->full_log.start;
            index < get_view_range(ui->full_log);
            ++index)
        {
            if(ui->log_messages[index].str[0])
            {
                add_render_queue_text(ui->render_queue, ui->log_messages[index].str, message_x, message_y, ui->font);
                message_y += ui->font_newline;
            }
        }
        
        message_y += (ui->font_newline / 2);
        
        full_log_window_dest.h = message_y;
        full_log_window_dest.x = get_centered_asset_x(game->window_size, full_log_window_dest.w);
        full_log_window_dest.y = get_centered_asset_y(game->window_size, assets, full_log_window_dest.h);
        
        render_window(game, full_log_window_dest, 2);
        process_render_queue(game, assets, ui, full_log_window_dest.x, full_log_window_dest.y);
    }
    else if(examine->is_open)
    {
        v4u dest = get_game_dest(game, game->examine.pos);
        SDL_RenderCopy(game->renderer, assets->ui.tex, (SDL_Rect *)&assets->yellow_outline_src, (SDL_Rect *)&dest);
    }
    else if(examine->is_inspecting)
    {
        v4u inspect_window_dest = {0};
        inspect_window_dest.w = 576;
        
        v2u header =
        {
            inspect_window_dest.x + (ui->window_offset * 2),
            inspect_window_dest.y + ui->window_offset
        };
        
        v2u info = header;
        
        Item *item = game->examine.item;
        Entity *entity = game->examine.entity;
        
        // Picture
        switch(examine->type)
        {
            case InspectType_Item:  add_render_queue_texture(ui->render_queue, header, item->tile_pos); break;
            case InspectType_Entity:  add_render_queue_texture(ui->render_queue, header, entity->tile_pos); break;
            case InspectType_Tile: add_render_queue_texture(ui->render_queue, header, get_dungeon_tile_pos(dungeon->tiles, examine->pos)); break;
            
            invalid_default_case
        }
        
        // Name offset
        header.x += ui->window_offset * 4;
        header.y += ui->font->size / 2;
        
        if(examine->type == InspectType_Item)
        {
            if(item->is_identified)
            {
                String128 item_name = full_item_name(item);
                
                if(item->is_cursed)
                {
                    add_render_queue_text(ui->render_queue, "%s (cursed)", header.x, header.y, item_name.str);
                }
                else
                {
                    add_render_queue_text(ui->render_queue, "%s", header.x, header.y, item_name.str);
                }
                
                info.y += (ui->font_newline * 2);
                
                if(item->type == ItemType_Weapon)
                {
                    info.y += ui->font_newline;
                    add_render_queue_text(ui->render_queue, "Damage: %d", info.x, info.y, item->w.damage + item->enchantment_level);
                    
                    info.y += ui->font_newline;
                    add_render_queue_text(ui->render_queue, "Accuracy: %d", info.x, info.y, item->w.accuracy + item->enchantment_level);
                    
                    info.y += ui->font_newline;
                    add_render_queue_text(ui->render_queue, "Attack Speed: %.1f", info.x, info.y, item->w.speed);
                }
                else if(item->type == ItemType_Armor)
                {
                    info.y += ui->font_newline;
                    add_render_queue_text(ui->render_queue, "Defence: %d", info.x, info.y, item->a.defence + item->enchantment_level);
                    
                    info.y += ui->font_newline;
                    add_render_queue_text(ui->render_queue, "Weight: %d", info.x, info.y, item->a.weight);
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
                    info.y += ui->font_newline;
                    add_render_queue_text(ui->render_queue, "Consuming rations will restore some of your hitpoints.", info.x, info.y);
                    
                    info.y += ui->font_newline;
                    add_render_queue_text(ui->render_queue, "The amount restored is between %u and %u.", info.x, info.y, RATION_RANGE_MIN, RATION_RANGE_MAX);
                }
            }
            else
            {
                add_render_queue_text(ui->render_queue, "%s (Unidentified)", header.x, header.y, item_id_text(item->id));
                info.y += (ui->font_newline * 2);
                
                if(item->type == ItemType_Weapon)
                {
                    info.y += ui->font_newline;
                    add_render_queue_text(ui->render_queue, "Base Damage: %u", info.x, info.y, item->w.damage);
                    
                    info.y += ui->font_newline;
                    add_render_queue_text(ui->render_queue, "Base Accuracy: %d", info.x, info.y, item->w.accuracy);
                    
                    info.y += ui->font_newline;
                    add_render_queue_text(ui->render_queue, "Base Attack Speed: %.1f", info.x, info.y, item->w.speed);
                }
                else if(item->type == ItemType_Armor)
                {
                    info.y += ui->font_newline;
                    add_render_queue_text(ui->render_queue, "Base Defence: %d", info.x, info.y, item->a.defence);
                    
                    info.y += ui->font_newline;
                    add_render_queue_text(ui->render_queue, "Base Weight: %d", info.x, info.y, item->a.weight);
                }
                else if(item->type == ItemType_Potion)
                {
                    info.y += ui->font_newline;
                    add_render_queue_text(ui->render_queue, "Consuming potions will bestow you with different effects.", info.x, info.y);
                    
                    info.y += ui->font_newline;
                    add_render_queue_text(ui->render_queue, "Some of these effects will be helpful, while others harmful.", info.x, info.y);
                }
                else if(item->type == ItemType_Scroll)
                {
                    info.y += ui->font_newline;
                    add_render_queue_text(ui->render_queue, "Reading scrolls will bring out different magical effects.", info.x, info.y);
                }
            }
            
            info.y += ui->font_newline * 2;
            
            if(item->is_identified &&
               item->is_cursed)
            {
                add_render_queue_text(ui->render_queue, "It is a cursed item.", info.x, info.y);
                info.y += ui->font_newline;
            }
            
            if(item->type == ItemType_Weapon)
            {
                if(item->rarity == ItemRarity_Common)
                {
                    add_render_queue_text(ui->render_queue, "It is of common rarity.", info.x, info.y);
                }
                else if(item->rarity == ItemRarity_Magical)
                {
                    add_render_queue_text(ui->render_queue, "It is of magical rarity.", info.x, info.y);
                }
                else if(item->rarity == ItemRarity_Mythical)
                {
                    add_render_queue_text(ui->render_queue, "It is of mythical rarity.", info.x, info.y);
                }
                
                info.y += ui->font_newline;
                
                if(item->handedness == ItemHandedness_OneHanded)
                {
                    add_render_queue_text(ui->render_queue, "It is a one-handed weapon.", info.x, info.y);
                }
                else if(item->handedness == ItemHandedness_TwoHanded)
                {
                    add_render_queue_text(ui->render_queue, "It is a two-handed weapon.", info.x, info.y);
                }
                
                info.y += ui->font_newline * 2;
            }
        }
        
#if 0
        else if(examine->type == InspectType_Entity)
        {
            // TODO(rami): Queue
            // TODO(rami): Add text.
            render_text(game, "%s", header.x, header.y, ui->font, 0, entity->name);
            
            info.y += (ui->font_newline * 2);
        }
        else if(examine->type == InspectType_Tile)
        {
            // TODO(rami): Which tile? Add text.
            TileID id = examine->tile_id;
        }
#endif
        
        inspect_window_dest.h = info.y;
        inspect_window_dest.x = get_centered_asset_x(game->window_size, inspect_window_dest.w);
        inspect_window_dest.y = get_centered_asset_y(game->window_size, assets, inspect_window_dest.h);
        
        render_window(game, inspect_window_dest, 2);
        process_render_queue(game, assets, ui, inspect_window_dest.x, inspect_window_dest.y);
    }
    else if(inventory->is_open)
    {
        // TODO(rami): To see items you need to press the key for the letter,
        // this needs to work from the input side.
        
        v4u inventory_rect = {0};
        inventory_rect.w = 640;
        
        v2u pos =
        {
            inventory_rect.x + (ui->window_offset * 2),
            inventory_rect.y + (ui->window_offset * 2)
        };
        
        add_render_queue_text(ui->render_queue, "Inventory", pos.x, pos.y);
        pos.y += ui->font_newline;
        
        u32 element_count = 0;
        u32 element_size = 32;
        char item_letter = 'a';
        
        u32 first_type = ItemType_None + 0;
        for(u32 type = first_type; type < ItemType_Count - 1; ++type)
        {
            b32 should_add_type_header = true;
            
            for(u32 index = 0; index < INVENTORY_SLOT_COUNT; ++index)
            {
                Item *item = inventory->slots[index];
                if(item &&
                   item->in_inventory &&
                   item->type == type)
                {
                    if(should_add_type_header)
                    {
                        should_add_type_header = false;
                        ++element_count;
                        
                        if(is_inventory_element_in_view(inventory->element_view, element_count))
                        {
                            if(pos.y + element_size >= window_y_offset)
                            {
                                inventory->is_using_scrollbar = true;
                                
                                if(!inventory->element_view.count)
                                {
                                    inventory->element_view.count = element_count;
                                }
                            }
                            else
                            {
                                add_render_queue_fill_rect(ui->render_queue, pos.x, pos.y, element_size, element_size, Color_LightGreen);
                                pos.y += element_size;
                            }
                        }
                    }
                    
                    ++element_count;
                    
                    if(is_inventory_element_in_view(inventory->element_view, element_count))
                    {
                        if(pos.y + element_size >= window_y_offset)
                        {
                            inventory->is_using_scrollbar = true;
                            
                            if(!inventory->element_view.count)
                            {
                                // This element would go over window_y_offset
                                // which means it's not applied, which is why we
                                // decrement element_count.
                                inventory->element_view.count = element_count - 1;
                            }
                        }
                        else
                        {
                            add_render_queue_fill_rect(ui->render_queue, pos.x, pos.y, element_size, element_size, Color_LightRed);
                            add_render_queue_text(ui->render_queue, "%c) %s", pos.x, pos.y, item_letter, item->name);
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
        
        // If all the elements are in view then we don't need scrolling.
        if(element_count <= inventory->element_view.count)
        {
            inventory->is_using_scrollbar = false;
        }
        
        pos.y += ui->font_newline;
        
        inventory_rect.h = pos.y;
        inventory_rect.x = get_centered_asset_x(game->window_size, inventory_rect.w);
        inventory_rect.y = get_centered_asset_y(game->window_size, assets, inventory_rect.h);
        render_window(game, inventory_rect, 2);
        
        inventory->rect = inventory_rect;
        inventory->element_count = element_count;
        process_render_queue(game, assets, ui, inventory_rect.x, inventory_rect.y);
        
#if 0
        printf("element_count: %u\n", element_count);
        printf("inventory->element_view.start: %u\n", inventory->element_view.start);
        printf("inventory->element_view.count: %u\n\n", inventory->element_view.count);
        
        printf("inventory.x: %u\n", inventory_rect.x);
        printf("inventory.y: %u\n", inventory_rect.y);
        printf("inventory.w: %u\n", inventory_rect.w);
        printf("inventory.h: %u\n\n", inventory_rect.h);
#endif
        
        // Render scrollbar
        if(inventory->is_using_scrollbar)
        {
            u32 scrollbar_offset = 20;
            
            v4u gutter = {0};
            gutter.x = inventory_rect.x + inventory_rect.w - scrollbar_offset;
            gutter.y = inventory_rect.y;
            gutter.w = 2;
            
            // Get the correct gutter height.
            u32 scrollbar_size = (inventory_rect.h - (ui->font_newline * 4)) / inventory->element_count;
            gutter.h = scrollbar_size * inventory->element_count;
            
            // Center gutter vertically relative to inventory.
            gutter.y += ((inventory_rect.h - gutter.h) / 2);
            render_fill_rect(game, gutter, Color_WindowAccent);
            
            v4u scrollbar = {0};
            scrollbar.x = inventory_rect.x + inventory_rect.w - scrollbar_offset;
            scrollbar.y = gutter.y + (scrollbar_size * (inventory->element_view.start - 1));
            scrollbar.w = gutter.w;
            scrollbar.h = scrollbar_size * inventory->element_view.count;
            render_fill_rect(game, scrollbar, Color_WindowBorder);
        }
    }
}