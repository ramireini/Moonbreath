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
    set_render_color(game, Color_WindowBorder);
    SDL_RenderFillRect(game->renderer, (SDL_Rect *)&rect);
    
    // Window background
    v4u inner_rect = get_border_adjusted_rect(rect, border_size);
    set_render_color(game, Color_Window);
    SDL_RenderFillRect(game->renderer, (SDL_Rect *)&inner_rect);
    
    // Window accent
    set_render_color(game, Color_WindowAccent);
    SDL_RenderDrawRect(game->renderer, (SDL_Rect *)&inner_rect);
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
update_log_view(LogView *view, u32 index)
{
    u32 message_count = view->message_count - 1;
    if(index > message_count)
    {
        view->start_index = (index - message_count);
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
            set_render_color(game, Color_WindowBorder);
            v4u healthbar_outside = {right.x, right.y, 204, 16};
            SDL_RenderFillRect(game->renderer, (SDL_Rect *)&healthbar_outside);
            
            // Healthbar background
            set_render_color(game, Color_WindowAccent);
            v4u healthbar_inside = get_border_adjusted_rect(healthbar_outside, 1);
            SDL_RenderFillRect(game->renderer, (SDL_Rect *)&healthbar_inside);
            
            if(player->hp > 0)
            {
                healthbar_inside.w = get_ratio(player->hp, player->max_hp, healthbar_inside.w);
                set_render_color(game, Color_DarkRed);
                SDL_RenderFillRect(game->renderer, (SDL_Rect *)&healthbar_inside);
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
    
    // Short Log
    for(u32 index = ui->short_log.start_index;
        index < (ui->short_log.start_index + ui->short_log.message_count);
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
        
        for(u32 index = ui->full_log.start_index;
            index < (ui->full_log.start_index + ui->full_log.message_count);
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
        // TODO(rami): One would think that the width would also be decided by the
        // width of the longest text, but I guess you could also have a set value for it.
        v4u inventory_dest = {0};
        inventory_dest.w = 640;
        
        char item_letter = 'a';
        v2u at =
        {
            inventory_dest.x + (ui->window_offset * 2),
            inventory_dest.y + (ui->window_offset * 2)
        };
        
        add_render_queue_text(ui->render_queue, "Inventory", at.x, at.y);
        at.y += ui->font_newline;
        
        // TODO(rami): To see items you need to press the key for the letter,
        // this needs to work from the input side.
        
        u32 first_type = ItemType_None + 1;
        for(u32 type = first_type; type < ItemType_Count - 1; ++type)
        {
            b32 is_first_item_of_type = true;
            
            for(u32 index = 0; index < INVENTORY_SLOT_COUNT; ++index)
            {
                Item *item = inventory->slots[index];
                if(item &&
                   item->in_inventory &&
                   item->type == type)
                {
                    if(is_first_item_of_type)
                    {
                        is_first_item_of_type = false;
                        at.y += ui->font_newline;
                        
                        add_render_queue_rect(ui->render_queue,
                                              at.x, at.y - 8,
                                              (inventory_dest.w - at.x) - (ui->font_newline * 2), 1,
                                              Color_WindowAccent);
                        
                        switch(type)
                        {
                            case ItemType_Weapon: add_render_queue_text(ui->render_queue, "Weapon", at.x, at.y, ui->font); break;
                            case ItemType_Armor: add_render_queue_text(ui->render_queue, "Armor", at.x, at.y, ui->font); break;
                            case ItemType_Potion: add_render_queue_text(ui->render_queue, "Potion", at.x, at.y, ui->font); break;
                            case ItemType_Scroll: add_render_queue_text(ui->render_queue, "Scroll", at.x, at.y, ui->font); break;
                            case ItemType_Ration: add_render_queue_text(ui->render_queue, "Ration", at.x, at.y, ui->font); break;
                            
                            invalid_default_case;
                        }
                        
                        at.y += ui->font_newline;
                    }
                    
                    // Picture and name
                    add_render_queue_texture(ui->render_queue, at, item->tile_pos);
                    
                    u32 name_x = at.x + (ui->font_newline * 3);
                    u32 name_y = at.y + (ui->font->size / 2);
                    if(is_item_consumable(item->type) && item->c.stack_count) // TODO(rami): if stack_count > 1
                    {
                        if(item->is_identified)
                        {
                            add_render_queue_text(ui->render_queue, "%c) %s (%u)", name_x, name_y, item_letter, item->name, item->c.stack_count);
                        }
                        else
                        {
                            add_render_queue_text(ui->render_queue, "%c) %s %s (%u)", name_x, name_y, item_letter, item->c.visual_text, item_id_text(item->id), item->c.stack_count);
                        }
                    }
                    else
                    {
                        if(item->is_identified)
                        {
                            String128 item_name = full_item_name(item);
                            add_render_queue_text(ui->render_queue, "%c) %s", name_x, name_y, item_letter, item_name.str);
                        }
                        else
                        {
                            add_render_queue_text(ui->render_queue, "%c) %s", name_x, name_y, item_letter, item_id_text(item->id));
                        }
                    }
                    
                    ++item_letter;
                    at.y += ui->font_newline * 1.5f;
                }
            }
        }
        
        at.y += ui->font_newline;
        
        inventory_dest.h = at.y;
        inventory_dest.x = get_centered_asset_x(game->window_size, inventory_dest.w);
        inventory_dest.y = get_centered_asset_y(game->window_size, assets, inventory_dest.h);
        
        render_window(game, inventory_dest, 2);
        process_render_queue(game, assets, ui, inventory_dest.x, inventory_dest.y);
    }
}