internal u32
get_centered_asset_x(v2u window_size, u32 asset_src_w)
{
    u32 result = ((window_size.w / 2) - (asset_src_w / 2));
    return(result);
}

internal u32
get_centered_asset_y(Assets *assets, v2u window_size, u32 asset_src_h)
{
    u32 result = ((window_size.h - assets->bottom_window_src.h - asset_src_h) / 2);
    return(result);
}

internal void
ui_newline(ItemWindow *window)
{
    window->at.y += window->next_line_advance;
}

internal void
render_item_window(GameState *game,
                   Inventory *inventory,
                   Assets *assets,
                   ItemWindow window,
                   u32 slot_index)
{
    // Background
    v4u window_rect_dest = {window.x, window.y, window.w, window.h};
    SDL_RenderCopy(game->renderer, assets->ui.tex, (SDL_Rect *)&assets->item_window_src, (SDL_Rect *)&window_rect_dest);
    
    Item *item = inventory->slots[slot_index];
    u32 window_edge_offset = 12;
    
    // Item Name
    window.at.x += window_edge_offset;
    window.at.y += window_edge_offset;
    
    if(item->is_identified)
    {
        String128 item_name = full_item_name(item);
        render_text(game, "%s%s", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, item_rarity_color_code(item->rarity), item_name.str);
    }
    else
    {
        render_text(game, "%s%s", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, item_rarity_color_code(item->rarity), item_id_text(item->id));
    }
    
    ui_newline(&window);
    
    // Item Stats
    if(item->is_identified)
    {
        if(is_item_consumable(item->type))
        {
            render_text(game, "%sConsumable", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, start_color(Color_LightGray));
            ui_newline(&window);
        }
        else if(item->type == ItemType_Armor)
        {
            render_text(game, "%sArmor", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, start_color(Color_LightGray));
        }
        else
        {
            render_text(game, "%s%s, %s", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, start_color(Color_LightGray), item_rarity_text(item->rarity), item_handedness_text(item->handedness));
        }
        
        ui_newline(&window);
        
        if(item->type == ItemType_Weapon)
        {
            ui_newline(&window);
            if(item->secondary_damage_type)
            {
                render_text(game, "Damage Type: %s (%s)", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, item_damage_type_text(item->primary_damage_type), item_damage_type_text(item->secondary_damage_type));
            }
            else
            {
                render_text(game, "Damage Type: %s", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, item_damage_type_text(item->primary_damage_type));
            }
            
            ui_newline(&window);
            render_text(game, "Damage: %d", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, item->w.damage +  + item->enchantment_level);
            
            ui_newline(&window);
            render_text(game, "Accuracy: %d", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, item->w.accuracy +  + item->enchantment_level);
            
            ui_newline(&window);
            render_text(game, "Attack Speed: %.1f", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, item->w.speed);
        }
        else if(item->type == ItemType_Armor)
        {
            ui_newline(&window);
            render_text(game, "Defence: %d", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, item->a.defence + item->enchantment_level);
            
            ui_newline(&window);
            render_text(game, "Weight: %d", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, item->a.weight);
        }
        else if(is_item_consumable(item->type))
        {
            render_text(game, "%s", window.at.x, window.at.y, assets->fonts[FontName_DosVga], (window.x + window.w) - window_edge_offset, item->description);
        }
    }
    else
    {
        render_text(game, "%sUnidentified", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, start_color(Color_LightGray));
    }
    
    window.at.y = window.offset_to_actions;
    
    // Window Actions
    if(window.is_comparing)
    {
        render_text(game, "%sCurrently Equipped", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, start_color(Color_LightGray));
    }
    else
    {
        if(item->type == ItemType_Weapon ||
           item->type == ItemType_Armor)
        {
            if(inventory->item_use_type == ItemUseType_Move)
            {
                render_text(game, "%s[%c] %s", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, start_color(Color_DarkGray), game->Key_InventoryAction, item->is_equipped ? "Unequip" : "Equip");
            }
            else if(inventory->item_use_type == ItemUseType_Identify)
            {
                if(item->is_identified)
                {
                    render_text(game, "%s[%c] %s", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, start_color(Color_DarkGray), game->Key_InventoryAction, item->is_equipped ? "Unequip" : "Equip");
                }
                else
                {
                    render_text(game, "[%c] Identify", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, game->Key_InventoryAction);
                }
            }
            else if(inventory->item_use_type == ItemUseType_EnchantWeapon)
            {
                if(item->type == ItemType_Weapon)
                {
                    render_text(game, "[%c] Enchant", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, game->Key_InventoryAction);
                }
                else
                {
                    render_text(game, "%s[%c] %s", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, start_color(Color_DarkGray), game->Key_InventoryAction, item->is_equipped ? "Unequip" : "Equip");
                }
            }
            else if(inventory->item_use_type == ItemUseType_EnchantArmor)
            {
                if(item->type == ItemType_Armor)
                {
                    render_text(game, "[%c] Enchant", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, game->Key_InventoryAction);
                }
                else
                {
                    render_text(game, "%s[%c] %s", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, start_color(Color_DarkGray), game->Key_InventoryAction, item->is_equipped ? "Unequip" : "Equip");
                }
            }
            else
            {
                if(inventory->item_use_type == ItemUseType_EnchantArmor &&
                   item->is_identified)
                {
                    render_text(game, "%s[%c] %s", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, start_color(Color_DarkGray), game->Key_InventoryAction, item->is_equipped ? "Unequip" : "Equip");
                }
                else
                {
                    render_text(game, "[%c] %s", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, game->Key_InventoryAction, item->is_equipped ? "Unequip" : "Equip");
                }
            }
        }
        else if(item->type == ItemType_Potion)
        {
            if(inventory->item_use_type == ItemUseType_Move ||
               is_player_enchanting(inventory->item_use_type))
            {
                render_text(game, "%s[%c] Drink", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, start_color(Color_DarkGray), game->Key_InventoryAction);
            }
            else if(inventory->item_use_type == ItemUseType_Identify)
            {
                if(item->is_identified)
                {
                    render_text(game, "%s[%c] Drink", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, start_color(Color_DarkGray), game->Key_InventoryAction);
                }
                else
                {
                    render_text(game, "[%c] Identify", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, game->Key_InventoryAction);
                }
            }
            else
            {
                render_text(game, "[%c] Drink", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, game->Key_InventoryAction);
            }
        }
        else if(item->type == ItemType_Scroll)
        {
            if(inventory->item_use_type == ItemUseType_Move)
            {
                render_text(game, "%s[%c] Read", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, start_color(Color_DarkGray), game->Key_InventoryAction);
            }
            else if(inventory->item_use_type == ItemUseType_Identify)
            {
                if(slot_index == inventory->use_item_src_index)
                {
                    render_text(game, "[%c] Cancel Identify", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, game->Key_InventoryAction);
                }
                else
                {
                    if(item->is_identified)
                    {
                        render_text(game, "%s[%c] Read", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, start_color(Color_DarkGray), game->Key_InventoryAction);
                    }
                    else
                    {
                        render_text(game, "[%c] Identify", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, game->Key_InventoryAction);
                    }
                }
            }
            else if(is_player_enchanting(inventory->item_use_type))
            {
                if(slot_index == inventory->use_item_src_index)
                {
                    render_text(game, "[%c] Cancel Enchant", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, game->Key_InventoryAction);
                }
                else
                {
                    render_text(game, "%s[%c] Read", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, start_color(Color_DarkGray), game->Key_InventoryAction);
                }
            }
            else
            {
                render_text(game, "[%c] Read", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, game->Key_InventoryAction);
            }
        }
        else if(item->type == ItemType_Ration)
        {
            if(inventory->item_use_type)
            {
                render_text(game, "%s[%c] Eat", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, start_color(Color_DarkGray), game->Key_InventoryAction);
            }
            else
            {
                render_text(game, "[%c] Eat", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, game->Key_InventoryAction);
            }
        }
        
        ui_newline(&window);
        if(inventory->item_use_type == ItemUseType_Identify ||
           is_player_enchanting(inventory->item_use_type))
        {
            render_text(game, "%s[%c] Move", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, start_color(Color_DarkGray), game->Key_InventoryMove);
        }
        else
        {
            render_text(game, "[%c] Move", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, game->Key_InventoryMove);
        }
        
        ui_newline(&window);
        if(inventory->item_use_type)
        {
            render_text(game, "%s[%c] Drop", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, start_color(Color_DarkGray), game->Key_PickupDrop);
        }
        else
        {
            render_text(game, "[%c] Drop", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, game->Key_PickupDrop);
        }
    }
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
log_add(Log *log, char *text, ...)
{
    String128 formatted = {0};
    
    va_list arg_list;
    va_start(arg_list, text);
    vsnprintf(formatted.str, sizeof(formatted), text, arg_list);
    va_end(arg_list);
    
    // Copy the new text to a vacant log index if there is one.
    for(u32 index = 0; index < MAX_LOG_MESSAGE_COUNT; ++index)
    {
        update_log_view(&log->full_log, index);
        update_log_view(&log->short_log, index);
        
        if(!log->messages[index].str[0])
        {
            strcpy(log->messages[index].str, formatted.str);
            return;
        }
    }
    
    // Move all texts up by one.
    for(u32 index = 1; index < MAX_LOG_MESSAGE_COUNT; ++index)
    {
        strcpy(log->messages[index - 1].str, log->messages[index].str);
    }
    
    // Copy the new text to the bottom.
    strcpy(log->messages[MAX_LOG_MESSAGE_COUNT - 1].str, formatted.str);
}

internal void
render_ui(GameState *game,
          Dungeon *dungeon,
          Entity *player,
          Log *log,
          Inventory *inventory,
          Assets *assets)
{
    v4u log_window_dest = {0, game->window_size.h - assets->bottom_window_src.h, assets->bottom_window_src.w, assets->bottom_window_src.h};
    SDL_RenderCopy(game->renderer, assets->ui.tex, (SDL_Rect *)&assets->bottom_window_src, (SDL_Rect *)&log_window_dest);
    
    // Render Player Stats
    v2u stat_pos = {12, game->window_size.h - assets->bottom_window_src.h};
    render_text(game, player->name, stat_pos.x, stat_pos.y + 12, assets->fonts[FontName_DosVga], 0);
    render_text(game, "Health:    %u/%u", stat_pos.x, stat_pos.y + 30, assets->fonts[FontName_DosVga], 0, player->hp, player->max_hp);
    
    // Left Side
    render_text(game, "Strength:     %u", stat_pos.x, stat_pos.y + 48, assets->fonts[FontName_DosVga], 0, player->p.strength);
    render_text(game, "Intelligence: %u", stat_pos.x, stat_pos.y + 66, assets->fonts[FontName_DosVga], 0, player->p.intelligence);
    render_text(game, "Dexterity:    %u", stat_pos.x, stat_pos.y + 84, assets->fonts[FontName_DosVga], 0, player->p.dexterity);
    render_text(game, "Defence:      %u", stat_pos.x, stat_pos.y + 102, assets->fonts[FontName_DosVga], 0, player->defence);
    render_text(game, "Evasion:      %u", stat_pos.x, stat_pos.y + 120, assets->fonts[FontName_DosVga], 0, player->evasion);
    
    // Right Side
    u32 right_side_offset = 160;
    render_text(game, "Time:          %.01f", stat_pos.x + right_side_offset, stat_pos.y + 48, assets->fonts[FontName_DosVga], 0, game->time);
    render_text(game, "Action time:   %.01f", stat_pos.x + right_side_offset, stat_pos.y + 66, assets->fonts[FontName_DosVga], 0, player->action_time);
    render_text(game, "Dungeon level: %u", stat_pos.x + right_side_offset, stat_pos.y + 84, assets->fonts[FontName_DosVga], 0, dungeon->level);
    
    // Render Player HP Bar
    v4u health_bar_outside_dest = {stat_pos.x + right_side_offset, stat_pos.y + 29, assets->health_bar_outside_src.w, assets->health_bar_outside_src.h};
    SDL_RenderCopy(game->renderer, assets->ui.tex, (SDL_Rect *)&assets->health_bar_outside_src, (SDL_Rect *)&health_bar_outside_dest);
    
    u32 health_bar_inside_width = 0;
    if(player->hp > 0)
    {
        health_bar_inside_width = get_ratio(player->hp, player->max_hp, assets->health_bar_inside_src.w);
    }
    
    v4u health_bar_inside_src = {assets->health_bar_inside_src.x, assets->health_bar_inside_src.y, health_bar_inside_width, assets->health_bar_inside_src.h};
    v4u health_bar_inside_dest = {health_bar_outside_dest.x + 2, health_bar_outside_dest.y + 2, health_bar_inside_width, assets->health_bar_inside_src.h};
    SDL_RenderCopy(game->renderer, assets->ui.tex, (SDL_Rect *)&health_bar_inside_src,  (SDL_Rect *)&health_bar_inside_dest);
    
    // Render Log
    u32 string_window_offset = 12;
    u32 string_y_offset = 20;
    u32 bottom_window_separator_left_edge = log_window_dest.x + 385;
    
    u32 string_x = bottom_window_separator_left_edge + string_window_offset;
    u32 string_y = log_window_dest.y + string_window_offset;
    
#if 0
    printf("full_log.start_index: %u\n", log->full_log.start_index);
    printf("short_log.start_index: %u\n", log->short_log.start_index);
#endif
    
    // Render Short Log
    for(u32 index = log->short_log.start_index;
        index < (log->short_log.start_index + log->short_log.message_count);
        ++index)
    {
        if(log->messages[index].str[0])
        {
            render_text(game, log->messages[index].str, string_x, string_y, assets->fonts[FontName_DosVga], 0);
            string_y += string_y_offset;
        }
    }
    
    if(log->is_full_log_open)
    {
        v4u full_log_window_dest =
        {
            get_centered_asset_x(game->window_size, assets->full_log_window_src.w),
            get_centered_asset_y(assets, game->window_size, assets->full_log_window_src.h),
            assets->full_log_window_src.w,
            assets->full_log_window_src.h
        };
        
        SDL_RenderCopy(game->renderer, assets->ui.tex, (SDL_Rect *)&assets->full_log_window_src, (SDL_Rect *)&full_log_window_dest);
        
        u32 string_x = full_log_window_dest.x + string_window_offset;
        u32 string_y = full_log_window_dest.y + string_window_offset;
        
        for(u32 index = log->full_log.start_index;
            index < (log->full_log.start_index + log->full_log.message_count);
            ++index)
        {
            if(log->messages[index].str[0])
            {
                render_text(game, log->messages[index].str, string_x, string_y, assets->fonts[FontName_DosVga], 0);
                string_y += string_y_offset;
            }
        }
    }
    else if(game->examine.is_open)
    {
        v4u dest = get_game_dest(game, game->examine.pos);
        SDL_RenderCopy(game->renderer, assets->ui.tex, (SDL_Rect *)&assets->yellow_outline_src, (SDL_Rect *)&dest);
    }
    else if(game->inspect.is_open)
    {
        v4u inspect_window_dest =
        {
            get_centered_asset_x(game->window_size, assets->inspect_window_src.w),
            get_centered_asset_y(assets, game->window_size, assets->inspect_window_src.h),
            assets->inspect_window_src.w,
            assets->inspect_window_src.h
        };
        
        SDL_RenderCopy(game->renderer, assets->ui.tex, (SDL_Rect *)&assets->inspect_window_src, (SDL_Rect *)&inspect_window_dest);
    }
    else if(inventory->is_open)
    {
        v4u inventory_window_dest = {0};
        inventory_window_dest.w = assets->inventory_window_src.w;
        inventory_window_dest.h = assets->inventory_window_src.h;
        inventory_window_dest.x = game->window_size.w - inventory_window_dest.w;
        inventory_window_dest.y = game->window_size.h - inventory_window_dest.h - assets->bottom_window_src.h - 4;
        SDL_RenderCopy(game->renderer, assets->ui.tex, (SDL_Rect *)&assets->inventory_window_src, (SDL_Rect *)&inventory_window_dest);
        
        // Set src and dest values of the inventory equip slot icons.
        v4u head_src = get_tile_rect(make_v2u(14, 2));
        v4u head_dest = {inventory_window_dest.x + 133, inventory_window_dest.y + 7, 32, 32};
        
        v4u body_src = get_tile_rect(make_v2u(15, 2));
        v4u body_dest = {inventory_window_dest.x + 133, inventory_window_dest.y + 79, 32, 32};
        
        v4u legs_src = get_tile_rect(make_v2u(16, 2));
        v4u legs_dest = {inventory_window_dest.x + 133, inventory_window_dest.y + 115, 32, 32};
        
        v4u feet_src = get_tile_rect(make_v2u(17, 2));
        v4u feet_dest = {inventory_window_dest.x + 133, inventory_window_dest.y + 151, 32, 32};
        
        v4u first_hand_src = get_tile_rect(make_v2u(18, 2));
        v4u first_hand_dest = {inventory_window_dest.x + 97, inventory_window_dest.y + 79, 32, 32};
        
        v4u second_hand_src = get_tile_rect(make_v2u(19, 2));
        v4u second_hand_dest = {inventory_window_dest.x + 169, inventory_window_dest.y + 79, 32, 32};
        
        v4u amulet_src = get_tile_rect(make_v2u(20, 2));
        v4u amulet_dest = {inventory_window_dest.x + 133, inventory_window_dest.y + 43, 32, 32};
        
        v4u ring_src = get_tile_rect(make_v2u(21, 2));
        v4u ring_dest = {inventory_window_dest.x + 97, inventory_window_dest.y + 151, 32, 32};
        
        // If an item is equipped, replace its slot source with the item tile.
        for(u32 index = 0; index < INVENTORY_SLOT_COUNT; ++index)
        {
            Item *item = inventory->slots[index];
            if(item && item->is_equipped)
            {
                switch(item->slot)
                {
                    case ItemSlot_Head: head_src = get_tile_rect(item->tile_pos); break;
                    case ItemSlot_Body: body_src = get_tile_rect(item->tile_pos); break;
                    case ItemSlot_Legs: legs_src = get_tile_rect(item->tile_pos); break;
                    case ItemSlot_Feet: feet_src = get_tile_rect(item->tile_pos); break;
                    case ItemSlot_Amulet: amulet_src = get_tile_rect(item->tile_pos); break;
                    case ItemSlot_SecondHand: second_hand_src = get_tile_rect(item->tile_pos); break;
                    case ItemSlot_FirstHand: first_hand_src = get_tile_rect(item->tile_pos); break;
                    case ItemSlot_Ring: ring_src = get_tile_rect(item->tile_pos); break;
                    
                    invalid_default_case;
                }
            }
        }
        
        // Render Equipped Inventory Items
        SDL_RenderCopy(game->renderer, assets->tileset.tex, (SDL_Rect *)&head_src, (SDL_Rect *)&head_dest);
        SDL_RenderCopy(game->renderer, assets->tileset.tex, (SDL_Rect *)&body_src, (SDL_Rect *)&body_dest);
        SDL_RenderCopy(game->renderer, assets->tileset.tex, (SDL_Rect *)&legs_src, (SDL_Rect *)&legs_dest);
        SDL_RenderCopy(game->renderer, assets->tileset.tex, (SDL_Rect *)&feet_src, (SDL_Rect *)&feet_dest);
        SDL_RenderCopy(game->renderer, assets->tileset.tex, (SDL_Rect *)&first_hand_src, (SDL_Rect *)&first_hand_dest);
        SDL_RenderCopy(game->renderer, assets->tileset.tex, (SDL_Rect *)&second_hand_src, (SDL_Rect *)&second_hand_dest);
        SDL_RenderCopy(game->renderer, assets->tileset.tex, (SDL_Rect *)&amulet_src, (SDL_Rect *)&amulet_dest);
        SDL_RenderCopy(game->renderer, assets->tileset.tex, (SDL_Rect *)&ring_src, (SDL_Rect *)&ring_dest);
        
        // Render Inventory Items
        u32 slot_padding = 4;
        v2u first_slot = {inventory_window_dest.x + 7, inventory_window_dest.y + 194};
        
        for(u32 index = 0; index < INVENTORY_SLOT_COUNT; ++index)
        {
            Item *item = inventory->slots[index];
            if(item)
            {
                v2u offset = v2u_from_index(index, INVENTORY_WIDTH);
                
                v4u src = get_tile_rect(item->tile_pos);
                v4u dest = get_tile_rect(offset);
                dest.x += first_slot.x + (offset.x * slot_padding);
                dest.y += first_slot.y + (offset.y * slot_padding);
                
                // Render item at full or half opacity.
                if(inventory->item_use_type == ItemUseType_Move &&
                   inventory->use_item_src_index == index)
                {
                    SDL_SetTextureAlphaMod(assets->tileset.tex, 127);
                    SDL_RenderCopy(game->renderer, assets->tileset.tex, (SDL_Rect *)&src, (SDL_Rect *)&dest);
                    SDL_SetTextureAlphaMod(assets->tileset.tex, 255);
                }
                else
                {
                    SDL_RenderCopy(game->renderer, assets->tileset.tex, (SDL_Rect *)&src, (SDL_Rect *)&dest);
                }
                
                // Render item at equip slot.
                if(inventory->slots[index]->is_equipped)
                {
                    SDL_RenderCopy(game->renderer, assets->ui.tex, (SDL_Rect *)&assets->inventory_equipped_slot_src, (SDL_Rect *)&dest);
                }
                
                if(index == inventory_slot_index(inventory->pos))
                {
                    ItemWindow item_window = {0};
                    item_window.is_comparing = false;
                    item_window.w = assets->item_window_src.w;
                    item_window.h = assets->item_window_src.h;
                    item_window.x = inventory_window_dest.x - item_window.w - 6;
                    item_window.y = inventory_window_dest.y;
                    item_window.at.x = item_window.x;
                    item_window.at.y = item_window.y;
                    item_window.next_line_advance = 20;
                    item_window.offset_to_actions = item_window.y + 274;
                    
                    render_item_window(game, inventory, assets, item_window, index);
                    
                    InventorySlot slot = equipped_inventory_slot_from_item_slot(item->slot, inventory);
                    if(slot.item && (slot.index != index))
                    {
                        item_window.is_comparing = true;
                        item_window.x = item_window.x - item_window.w - 4;
                        item_window.at.x = item_window.x;
                        item_window.at.y = item_window.y;
                        item_window.offset_to_actions = item_window.y + 314;
                        
                        render_item_window(game, inventory, assets, item_window, slot.index);
                    }
                }
            }
        }
        
        // Render the selected inventory slot highlight.
        v4u slot_src = get_tile_rect(inventory->pos);
        slot_src.x += first_slot.x + (inventory->pos.x * slot_padding);
        slot_src.y += first_slot.y + (inventory->pos.y * slot_padding);
        
        v4u slot_dest = {slot_src.x, slot_src.y, assets->inventory_selected_slot_src.w, assets->inventory_selected_slot_src.h};
        SDL_RenderCopy(game->renderer, assets->ui.tex, (SDL_Rect *)&assets->inventory_selected_slot_src, (SDL_Rect *)&slot_dest);
        
        // Render the moving item at the current inventory slot.
        if(inventory->item_use_type == ItemUseType_Move)
        {
            Item *item = inventory->slots[inventory->use_item_src_index];
            if(item)
            {
                v4u slot_src = get_tile_rect(item->tile_pos);
                SDL_RenderCopy(game->renderer, assets->tileset.tex, (SDL_Rect *)&slot_src, (SDL_Rect *)&slot_dest);
            }
        }
    }
}