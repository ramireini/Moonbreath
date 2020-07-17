internal void
next_ui_line(ItemWindow *window)
{
    window->at.y += window->next_line_advance;
}

internal void
render_item_window(GameState *game,
                   ItemWindow window,
                   u32 slot_index,
                   Inventory *inventory,
                   Assets *assets)
{
    // Background
    v4u window_rect = {window.x, window.y, window.w, window.h};
    SDL_RenderCopy(game->renderer, assets->ui.tex, (SDL_Rect *)&assets->item_window, (SDL_Rect *)&window_rect);
    
    Item *item = inventory->slots[slot_index];
    u32 item_window_edge_offset = 12;
    
    // Item Name
    window.at.x += item_window_edge_offset;
    window.at.y += item_window_edge_offset;
    
    if(item->is_identified)
    {
        String128 item_name = full_item_name(item);
        render_text(game, "%s%s", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, item_rarity_color_code(item->rarity), item_name.str);
    }
    else
    {
        render_text(game, "%s%s", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, item_rarity_color_code(item->rarity), item_id_text(item->id));
    }
    
    next_ui_line(&window);
    
    // Item Stats
    if(item->is_identified)
    {
        if(is_item_consumable(item->type))
        {
            render_text(game, "%sConsumable", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, start_color(Color_LightGray));
            next_ui_line(&window);
        }
        else if(item->type == ItemType_Armour)
        {
            render_text(game, "%sArmour", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, start_color(Color_LightGray));
        }
        else
        {
            render_text(game, "%s%s%s", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, start_color(Color_LightGray), item_rarity_text(item->rarity), item_id_text(item->id));
            next_ui_line(&window);
            
            render_text(game, "%s%s", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, start_color(Color_LightGray), item_handedness_text(item->handedness));
        }
        
        next_ui_line(&window);
        
        if(item->type == ItemType_Weapon)
        {
            next_ui_line(&window);
            if(item->secondary_damage_type)
            {
                render_text(game, "Damage Type: %s (%s)", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, item_damage_type_text(item->primary_damage_type), item_damage_type_text(item->secondary_damage_type));
            }
            else
            {
                render_text(game, "Damage Type: %s", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, item_damage_type_text(item->primary_damage_type));
            }
            
            // TODO(rami): I was thinking I would prefer rendering the actual
            // item damage, accuracy etc. instead of the base values.
            // This would take some work but I prefer that.
            
            next_ui_line(&window);
            render_text(game, "Base Damage: %d", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, item->w.damage);
            
            next_ui_line(&window);
            render_text(game, "Base Accuracy: %d", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, item->w.accuracy);
            
            next_ui_line(&window);
            render_text(game, "Attack Speed: %.1f", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, item->w.speed);
        }
        else if(item->type == ItemType_Armour)
        {
            next_ui_line(&window);
            render_text(game, "Base Defence: %d", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, item->a.defence);
            
            next_ui_line(&window);
            render_text(game, "Weight: %d", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, item->a.weight);
        }
        else if(is_item_consumable(item->type))
        {
            render_text(game, "%s%s", window.at.x, window.at.y, assets->fonts[FontName_DosVga], (window.x + window.w) - item_window_edge_offset, start_color(Color_LightGray), item->description);
        }
    }
    else
    {
        render_text(game, "%sUnidentified", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, start_color(Color_LightGray));
    }
    
    window.at.y = window.window_actions_advance;
    
    // Window Actions
    if(window.is_comparing_items)
    {
        render_text(game, "%sCurrently Equipped", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, start_color(Color_LightGray));
    }
    else
    {
        if(item->type == ItemType_Weapon ||
           item->type == ItemType_Armour)
        {
            if(inventory->item_use_type == ItemUseType_Move)
            {
                render_text(game, "%s[%c] %s", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, start_color(Color_DarkGray), game->keybinds[Key_InventoryAction], item->is_equipped ? "Unequip" : "Equip");
            }
            else if(inventory->item_use_type == ItemUseType_Identify)
            {
                if(item->is_identified)
                {
                    render_text(game, "%s[%c] %s", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, start_color(Color_DarkGray), game->keybinds[Key_InventoryAction], item->is_equipped ? "Unequip" : "Equip");
                }
                else
                {
                    render_text(game, "[%c] Identify", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, game->keybinds[Key_InventoryAction]);
                }
            }
            else if(inventory->item_use_type == ItemUseType_EnchantWeapon)
            {
                if(item->type == ItemType_Weapon)
                {
                    render_text(game, "[%c] Enchant", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, game->keybinds[Key_InventoryAction]);
                }
                else
                {
                    render_text(game, "%s[%c] %s", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, start_color(Color_DarkGray), game->keybinds[Key_InventoryAction], item->is_equipped ? "Unequip" : "Equip");
                }
            }
            else if(inventory->item_use_type == ItemUseType_EnchantArmour)
            {
                if(item->type == ItemType_Armour)
                {
                    render_text(game, "[%c] Enchant", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, game->keybinds[Key_InventoryAction]);
                }
                else
                {
                    render_text(game, "%s[%c] %s", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, start_color(Color_DarkGray), game->keybinds[Key_InventoryAction], item->is_equipped ? "Unequip" : "Equip");
                }
            }
            else
            {
                if(inventory->item_use_type == ItemUseType_EnchantArmour &&
                   item->is_identified)
                {
                    render_text(game, "%s[%c] %s", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, start_color(Color_DarkGray), game->keybinds[Key_InventoryAction], item->is_equipped ? "Unequip" : "Equip");
                }
                else
                {
                    render_text(game, "[%c] %s", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, game->keybinds[Key_InventoryAction], item->is_equipped ? "Unequip" : "Equip");
                }
            }
        }
        else if(item->type == ItemType_Potion)
        {
            if(inventory->item_use_type == ItemUseType_Move ||
               is_player_enchanting(inventory->item_use_type))
            {
                render_text(game, "%s[%c] Drink", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, start_color(Color_DarkGray), game->keybinds[Key_InventoryAction]);
            }
            else if(inventory->item_use_type == ItemUseType_Identify)
            {
                if(item->is_identified)
                {
                    render_text(game, "%s[%c] Drink", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, start_color(Color_DarkGray), game->keybinds[Key_InventoryAction]);
                }
                else
                {
                    render_text(game, "[%c] Identify", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, game->keybinds[Key_InventoryAction]);
                }
            }
            else
            {
                render_text(game, "[%c] Drink", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, game->keybinds[Key_InventoryAction]);
            }
        }
        else if(item->type == ItemType_Scroll)
        {
            if(inventory->item_use_type == ItemUseType_Move)
            {
                render_text(game, "%s[%c] Read", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, start_color(Color_DarkGray), game->keybinds[Key_InventoryAction]);
            }
            else if(inventory->item_use_type == ItemUseType_Identify)
            {
                if(slot_index == inventory->use_item_src_index)
                {
                    render_text(game, "[%c] Cancel Identify", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, game->keybinds[Key_InventoryAction]);
                }
                else
                {
                    if(item->is_identified)
                    {
                        render_text(game, "%s[%c] Read", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, start_color(Color_DarkGray), game->keybinds[Key_InventoryAction]);
                    }
                    else
                    {
                        render_text(game, "[%c] Identify", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, game->keybinds[Key_InventoryAction]);
                    }
                }
            }
            else if(is_player_enchanting(inventory->item_use_type))
            {
                if(slot_index == inventory->use_item_src_index)
                {
                    render_text(game, "[%c] Cancel Enchant", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, game->keybinds[Key_InventoryAction]);
                }
                else
                {
                    render_text(game, "%s[%c] Read", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, start_color(Color_DarkGray), game->keybinds[Key_InventoryAction]);
                }
            }
            else
            {
                render_text(game, "[%c] Read", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, game->keybinds[Key_InventoryAction]);
            }
        }
        
        next_ui_line(&window);
        if(inventory->item_use_type == ItemUseType_Identify ||
           is_player_enchanting(inventory->item_use_type))
        {
            render_text(game, "%s[%c] Move", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, start_color(Color_DarkGray), game->keybinds[Key_InventoryMove]);
        }
        else
        {
            render_text(game, "[%c] Move", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, game->keybinds[Key_InventoryMove]);
        }
        
        next_ui_line(&window);
        if(inventory->item_use_type)
        {
            render_text(game, "%s[%c] Drop", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, start_color(Color_DarkGray), game->keybinds[Key_PickupOrDrop]);
        }
        else
        {
            render_text(game, "[%c] Drop", window.at.x, window.at.y, assets->fonts[FontName_DosVga], 0, game->keybinds[Key_PickupOrDrop]);
        }
    }
}

internal void
log_text(String128 *log, char *text, ...)
{
    String128 formatted_text = {0};
    
    va_list arg_list;
    va_start(arg_list, text);
    vsnprintf(formatted_text.str, sizeof(formatted_text), text, arg_list);
    va_end(arg_list);
    
    // Copy the new text to a vacant log index if there is one.
    for(u32 index = 0; index < MAX_LOG_ENTRY_COUNT; ++index)
    {
        if(!log[index].str[0])
        {
            strcpy(log[index].str, formatted_text.str);
            return;
        }
    }
    
    // Move all texts up by one.
    for(u32 index = 1; index < MAX_LOG_ENTRY_COUNT; ++index)
    {
        strcpy(log[index - 1].str, log[index].str);
    }
    
    // Copy the new text to the bottom.
    strcpy(log[MAX_LOG_ENTRY_COUNT - 1].str, formatted_text.str);
}

internal void
render_ui(GameState *game,
          Dungeon *dungeon,
          Entity *player,
          String128 *log,
          Inventory *inventory,
          Assets *assets)
{
    v4u log_window = {0, game->window_size.h - assets->log_window.h, assets->log_window.w, assets->log_window.h};
    SDL_RenderCopy(game->renderer, assets->ui.tex, (SDL_Rect *)&assets->log_window, (SDL_Rect *)&log_window);
    
    // Render Player Stats
    v2u stat_start = {12, game->window_size.h - assets->log_window.h};
    
    render_text(game, player->name, stat_start.x, stat_start.y + 12, assets->fonts[FontName_DosVga], 0);
    render_text(game, "Health: %u/%u", stat_start.x, stat_start.y + 30, assets->fonts[FontName_DosVga], 0, player->hp, player->max_hp);
    render_text(game, "Str: %u", stat_start.x, stat_start.y + 48, assets->fonts[FontName_DosVga], 0, player->strength);
    render_text(game, "Int: %u", stat_start.x, stat_start.y + 66, assets->fonts[FontName_DosVga], 0, player->intelligence);
    render_text(game, "Dex: %u", stat_start.x, stat_start.y + 84, assets->fonts[FontName_DosVga], 0, player->dexterity);
    render_text(game, "Gold: %u", stat_start.x, stat_start.y + 102, assets->fonts[FontName_DosVga], 0, player->p.gold);
    render_text(game, "Defence: %u", stat_start.x + 128, stat_start.y + 48, assets->fonts[FontName_DosVga], 0, player->defence);
    render_text(game, "Evasion: %u", stat_start.x + 128, stat_start.y + 66, assets->fonts[FontName_DosVga], 0, player->evasion);
    render_text(game, "Time: %.01f", stat_start.x + 128, stat_start.y + 84, assets->fonts[FontName_DosVga], 0, game->time);
    render_text(game, "Location: Dungeon %u", stat_start.x + 128, stat_start.y + 102, assets->fonts[FontName_DosVga], 0, dungeon->level);
    
    // Render Player HP Bar
    v4u health_bar_outside = {stat_start.x + 126, stat_start.y + 29, assets->health_bar_outside.w, assets->health_bar_outside.h};
    SDL_RenderCopy(game->renderer, assets->ui.tex, (SDL_Rect *)&assets->health_bar_outside, (SDL_Rect *)&health_bar_outside);
    
    u32 health_bar_inside_w = 0;
    if(player->hp > 0)
    {
        health_bar_inside_w = ratio(player->hp, player->max_hp, assets->health_bar_inside.w);
    }
    
    v4u health_bar_inside_src = {assets->health_bar_inside.x, assets->health_bar_inside.y, health_bar_inside_w, assets->health_bar_inside.h};
    v4u health_bar_inside_dest = {health_bar_outside.x + 2, health_bar_outside.y + 2, health_bar_inside_w, assets->health_bar_inside.h};
    SDL_RenderCopy(game->renderer, assets->ui.tex, (SDL_Rect *)&health_bar_inside_src,  (SDL_Rect *)&health_bar_inside_dest);
    
    // Render Log
    u32 str_x = 362;
    u32 str_y = log_window.y + 12;
    u32 str_offset = 20;
    
    for(u32 index = 0; index < MAX_LOG_ENTRY_COUNT; ++index)
    {
        if(log[index].str[0])
        {
            render_text(game, log[index].str, str_x, str_y, assets->fonts[FontName_DosVga], 0);
            str_y += str_offset;
        }
    }
    
    if(inventory->is_open)
    {
        v4u inventory_window = {0};
        inventory_window.w = assets->inventory_window.w;
        inventory_window.h = assets->inventory_window.h;
        inventory_window.x = game->window_size.w - inventory_window.w;
        inventory_window.y = game->window_size.h - inventory_window.h - assets->log_window.h - 4;
        SDL_RenderCopy(game->renderer, assets->ui.tex, (SDL_Rect *)&assets->inventory_window, (SDL_Rect *)&inventory_window);
        
        // Set src and dest values of the inventory equip slot icons.
        v4u head_src = {0, 0, 32, 32};
        v4u head_dest = {inventory_window.x + 133, inventory_window.y + 7, 32, 32};
        
        v4u body_src = {32, 0, 32, 32};
        v4u body_dest = {inventory_window.x + 133, inventory_window.y + 79, 32, 32};
        
        v4u legs_src = {64, 0, 32, 32};
        v4u legs_dest = {inventory_window.x + 133, inventory_window.y + 115, 32, 32};
        
        v4u feet_src = {96, 0, 32, 32};
        v4u feet_dest = {inventory_window.x + 133, inventory_window.y + 151, 32, 32};
        
        v4u first_hand_src = {128, 0, 32, 32};
        v4u first_hand_dest = {inventory_window.x + 97, inventory_window.y + 79, 32, 32};
        
        v4u second_hand_src = {160, 0, 32, 32};
        v4u second_hand_dest = {inventory_window.x + 169, inventory_window.y + 79, 32, 32};
        
        v4u amulet_src = {192, 0, 32, 32};
        v4u amulet_dest = {inventory_window.x + 133, inventory_window.y + 43, 32, 32};
        
        v4u ring_src = {224, 0, 32, 32};
        v4u ring_dest = {inventory_window.x + 97, inventory_window.y + 151, 32, 32};
        
        // If an item is equipped, replace its slot source with the items tile.
        for(u32 slot_index = 0; slot_index < INVENTORY_AREA; ++slot_index)
        {
            Item *item = inventory->slots[slot_index];
            if(item && item->is_equipped)
            {
                switch(item->slot)
                {
                    case ItemSlot_Head: head_src = get_tile_rect(item->tile); break;
                    case ItemSlot_Body: body_src = get_tile_rect(item->tile); break;
                    case ItemSlot_Legs: legs_src = get_tile_rect(item->tile); break;
                    case ItemSlot_Feet: feet_src = get_tile_rect(item->tile); break;
                    case ItemSlot_Amulet: amulet_src = get_tile_rect(item->tile); break;
                    case ItemSlot_SecondHand: second_hand_src = get_tile_rect(item->tile); break;
                    case ItemSlot_FirstHand: first_hand_src = get_tile_rect(item->tile); break;
                    case ItemSlot_Ring: ring_src = get_tile_rect(item->tile); break;
                    
                    invalid_default_case;
                }
            }
            
            SDL_RenderCopy(game->renderer, assets->item_tileset.tex, (SDL_Rect *)&head_src, (SDL_Rect *)&head_dest);
            SDL_RenderCopy(game->renderer, assets->item_tileset.tex, (SDL_Rect *)&body_src, (SDL_Rect *)&body_dest);
            SDL_RenderCopy(game->renderer, assets->item_tileset.tex, (SDL_Rect *)&legs_src, (SDL_Rect *)&legs_dest);
            SDL_RenderCopy(game->renderer, assets->item_tileset.tex, (SDL_Rect *)&feet_src, (SDL_Rect *)&feet_dest);
            SDL_RenderCopy(game->renderer, assets->item_tileset.tex, (SDL_Rect *)&first_hand_src, (SDL_Rect *)&first_hand_dest);
            SDL_RenderCopy(game->renderer, assets->item_tileset.tex, (SDL_Rect *)&second_hand_src, (SDL_Rect *)&second_hand_dest);
            SDL_RenderCopy(game->renderer, assets->item_tileset.tex, (SDL_Rect *)&amulet_src, (SDL_Rect *)&amulet_dest);
            SDL_RenderCopy(game->renderer, assets->item_tileset.tex, (SDL_Rect *)&ring_src, (SDL_Rect *)&ring_dest);
        }
        
        u32 padding = 4;
        v2u first_slot = {inventory_window.x + 7, inventory_window.y + 192};
        
        for(u32 slot_index = 0; slot_index < INVENTORY_AREA; ++slot_index)
        {
            Item *item = inventory->slots[slot_index];
            if(item)
            {
                v2u offset = v2u_from_index(slot_index, INVENTORY_WIDTH);
                
                v4u src = get_tile_rect(item->tile);
                v4u dest = get_tile_rect(offset);
                dest.x += first_slot.x + (offset.x * padding);
                dest.y += first_slot.y + (offset.y * padding);
                
                // Render item at half opacity.
                if(inventory->item_use_type == ItemUseType_Move &&
                   inventory->use_item_src_index == slot_index)
                {
                    
                    SDL_SetTextureAlphaMod(assets->item_tileset.tex, 127);
                    SDL_RenderCopy(game->renderer, assets->item_tileset.tex, (SDL_Rect *)&src, (SDL_Rect *)&dest);
                    SDL_SetTextureAlphaMod(assets->item_tileset.tex, 255);
                }
                else
                {
                    // Render item at full opacity.
                    SDL_RenderCopy(game->renderer, assets->item_tileset.tex, (SDL_Rect *)&src, (SDL_Rect *)&dest);
                }
                
                // Render item at equip slot.
                if(inventory->slots[slot_index]->is_equipped)
                {
                    SDL_RenderCopy(game->renderer, assets->ui.tex, (SDL_Rect *)&assets->inventory_equipped_slot, (SDL_Rect *)&dest);
                }
                
                // Render item window.
                if(slot_index == get_inventory_slot_index(inventory->pos))
                {
                    ItemWindow item_window = {0};
                    item_window.is_comparing_items = false;
                    item_window.w = assets->item_window.w;
                    item_window.h = assets->item_window.h;
                    item_window.x = inventory_window.x - item_window.w - 6;
                    item_window.y = inventory_window.y;
                    item_window.at.x = item_window.x;
                    item_window.at.y = item_window.y;
                    item_window.next_line_advance = 20;
                    item_window.window_actions_advance = item_window.y + 270;
                    
                    render_item_window(game, item_window, slot_index, inventory, assets);
                    
                    InventorySlot slot = equipped_inventory_slot_from_item_slot(item->slot, inventory);
                    if(slot.item && (slot.index != slot_index))
                    {
                        item_window.is_comparing_items = true;
                        item_window.x = item_window.x - item_window.w - 4;
                        item_window.at.x = item_window.x;
                        item_window.at.y = item_window.y;
                        item_window.window_actions_advance = item_window.y + 310;
                        
                        render_item_window(game, item_window, slot.index, inventory, assets);
                    }
                }
            }
        }
        
        // Render the selected inventory slot highlight.
        v4u slot_src = get_tile_rect(inventory->pos);
        slot_src.x += first_slot.x + (inventory->pos.x * padding);
        slot_src.y += first_slot.y + (inventory->pos.y * padding);
        v4u slot_dest = {slot_src.x, slot_src.y, assets->inventory_selected_slot.w, assets->inventory_selected_slot.h};
        SDL_RenderCopy(game->renderer, assets->ui.tex, (SDL_Rect *)&assets->inventory_selected_slot, (SDL_Rect *)&slot_dest);
        
        // Render the moving item at the current inventory slot.
        if(inventory->item_use_type == ItemUseType_Move)
        {
            Item *item = inventory->slots[inventory->use_item_src_index];
            if(item)
            {
                v4u slot_src = get_tile_rect(item->tile);
                SDL_RenderCopy(game->renderer, assets->item_tileset.tex, (SDL_Rect *)&slot_src, (SDL_Rect *)&slot_dest);
            }
        }
    }
}