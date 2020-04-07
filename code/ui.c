internal void
render_window_item_stats(item_window_t window, item_t *item, item_info_t *item_info)
{
    if(item_info->handedness == handedness_one_handed)
    {
        render_text("1-Handed", window.at.x, window.at.y, color_light_gray, fonts[font_dos_vga], 0);
        window.at.y += window.offset_per_row;
    }
    else if(item_info->handedness == handedness_two_handed)
    {
        render_text("2-Handed", window.at.x, window.at.y, color_light_gray, fonts[font_dos_vga], 0);
        window.at.y += window.offset_per_row;
    }
    
    if(item_info->type == type_weapon)
    {
        window.at.y += window.offset_per_row;
        render_text("Damage: %d", window.at.x, window.at.y, color_white, fonts[font_dos_vga], 0, item_info->damage);
        
        window.at.y += window.offset_per_row;
        render_text("Accuracy: %d", window.at.x, window.at.y, color_white, fonts[font_dos_vga], 0, item_info->accuracy);
    }
    else if(item_info->type == type_armor)
    {
        window.at.y += window.offset_per_row;
        render_text("Defence: %d", window.at.x, window.at.y, color_white, fonts[font_dos_vga], 0, item_info->defence);
        
        window.at.y += window.offset_per_row;
        render_text("Weight: %u", window.at.x, window.at.y, color_white, fonts[font_dos_vga], 0, item_info->weight);
    }
    else if(item_info->type == type_consumable)
    {
        window.at.y += window.offset_per_row;
        render_text(item_info->description, window.at.x, window.at.y, color_green, fonts[font_dos_vga], 0);
    }
}

internal void
render_window_actions(item_window_t window, item_t *item, item_info_t *item_info)
{
    if(window.is_comparing_items)
    {
        window.at.y = window.offset_to_actions;
        render_text("Currently Equipped", window.at.x, window.at.y, color_light_gray, fonts[font_dos_vga], 0);
        
#if MOONBREATH_SLOW
        render_text("Unique ID: %u", window.at.x, window.at.y - window.offset_per_row, color_cyan, fonts[font_dos_vga], 0, item->unique_id);
#endif
    }
    else
    {
        window.at.y = window.offset_to_actions;
        
#if MOONBREATH_SLOW
        render_text("Unique ID: %u", window.at.x, window.at.y - window.offset_per_row, color_cyan, fonts[font_dos_vga], 0, item->unique_id);
#endif
        
        if(item_info->type == type_weapon ||
           item_info->type == type_armor)
        {
            if(item->is_equipped)
            {
                render_text("[%c] Unequip", window.at.x, window.at.y, color_white, fonts[font_dos_vga], 0, game.keybinds[key_equip]);
            }
            else
            {
                render_text("[%c] Equip", window.at.x, window.at.y, color_white, fonts[font_dos_vga], 0, game.keybinds[key_equip]);
            }
        }
        else if(item_info->type == type_consumable)
        {
            render_text("[%c] Consume", window.at.x, window.at.y, color_white, fonts[font_dos_vga], 0, game.keybinds[key_consume]);
        }
        
        window.at.y += window.offset_per_row;
        render_text("[%c] Move", window.at.x, window.at.y, color_white, fonts[font_dos_vga], 0, game.keybinds[key_move]);
        
        window.at.y += window.offset_per_row;
        render_text("[%c] Drop", window.at.x, window.at.y, color_white, fonts[font_dos_vga], 0, game.keybinds[key_drop]);
    }
}

internal void
render_window_background(item_window_t window)
{
    v4u window_rect = {window.x, window.y, window.w, window.h};
    SDL_RenderCopy(game.renderer, textures.ui, (SDL_Rect *)&textures.item_window, (SDL_Rect *)&window_rect);
}

internal v2u
render_window_item_name(item_window_t window, item_t *item, item_info_t *item_info)
{
    v2u result = V2u(window.x + 12, window.y + 12);
    render_text("%c%u %s", result.x, result.y, color_white, fonts[font_dos_vga], 0, (item->enchantment_level >= 0) ? '+' : '-', abs(item->enchantment_level), item_info->name);
    result.y += window.offset_per_row;
    
    return(result);
}

internal void
render_item_window(item_window_t window, u32 slot_index)
{
    item_info_t *info = item_info_from_slot_index(slot_index);
    
    render_window_background(window);
    window.at = render_window_item_name(window, inventory.slots[slot_index], info);
    
    render_window_item_stats(window, inventory.slots[slot_index], info);
    render_window_actions(window, inventory.slots[slot_index], info);
}

internal void
add_log_message(char *message, v4f color, ...)
{
    char formatted_message[128] = {0};
    
    va_list arg_list;
    va_start(arg_list, color);
    vsnprintf(formatted_message, sizeof(formatted_message), message, arg_list);
    va_end(arg_list);
    
    for(u32 i = 0; i < array_count(log_messages); ++i)
    {
        if(!log_messages[i].message[0])
        {
            strcpy(log_messages[i].message, formatted_message);
            log_messages[i].color = color;
            return;
        }
    }
    
    log_messages[0].message[0] = 0;
    log_messages[0].color = color_black;
    
    for(u32 i = 1; i < array_count(log_messages); ++i)
    {
        strcpy(log_messages[i - 1].message, log_messages[i].message);
        log_messages[i - 1].color = log_messages[i].color;
    }
    
    strcpy(log_messages[array_count(log_messages) - 1].message, formatted_message);
    log_messages[array_count(log_messages) - 1].color = color;
}

internal void
render_ui()
{
    v4u log_window = {0, game.window_size.h - textures.log_window.h, textures.log_window.w, textures.log_window.h};
    SDL_RenderCopy(game.renderer, textures.ui, (SDL_Rect *)&textures.log_window, (SDL_Rect *)&log_window);
    
    // Render Player Stats
    u32 stats_x_start = 12;
    u32 stats_y_start = game.window_size.h - textures.log_window.h;
    
    render_text(player.name, stats_x_start, stats_y_start + 12, color_white, fonts[font_dos_vga], 0);
    render_text("Health: %u/%u", stats_x_start, stats_y_start + 28, color_white, fonts[font_dos_vga], 0, player.hp, player.max_hp);
    render_text("Str: %u", stats_x_start, stats_y_start + 44, color_white, fonts[font_dos_vga], 0, player.strength);
    render_text("Int: %u", stats_x_start, stats_y_start + 60, color_white, fonts[font_dos_vga], 0, player.intelligence);
    render_text("Dex: %u", stats_x_start, stats_y_start + 76, color_white, fonts[font_dos_vga], 0, player.dexterity);
    render_text("Gold: %u", stats_x_start, stats_y_start + 92, color_white, fonts[font_dos_vga], 0, player.gold);
    render_text("Defence: %u", stats_x_start + 128, stats_y_start + 44, color_white, fonts[font_dos_vga], 0, player.defence);
    render_text("Evasion: %u", stats_x_start + 128, stats_y_start + 60, color_white, fonts[font_dos_vga], 0, player.evasion);
    render_text("Time: %.01f", stats_x_start + 128, stats_y_start + 76, color_white, fonts[font_dos_vga], 0, game.time);
    render_text("Location: Dungeon: %u", stats_x_start + 128, stats_y_start + 92, color_white, fonts[font_dos_vga], 0, dungeon.level);
    
    // Render Player HP Bar
    v4u health_bar_outside = {stats_x_start + 126, stats_y_start + 27, textures.health_bar_outside.w, textures.health_bar_outside.h};
    SDL_RenderCopy(game.renderer, textures.ui, (SDL_Rect *)&textures.health_bar_outside, (SDL_Rect *)&health_bar_outside);
    
    u32 health_bar_inside_w = 0;
    if(player.hp > 0)
    {
        health_bar_inside_w = get_ratio(player.hp, player.max_hp, textures.health_bar_inside.w);
    }
    
    v4u health_bar_inside_src = {textures.health_bar_inside.x, textures.health_bar_inside.y, health_bar_inside_w, textures.health_bar_inside.h};
    v4u health_bar_inside_dest = {health_bar_outside.x + 2, health_bar_outside.y + 2, health_bar_inside_w, textures.health_bar_inside.h};
    SDL_RenderCopy(game.renderer, textures.ui, (SDL_Rect *)&health_bar_inside_src,  (SDL_Rect *)&health_bar_inside_dest);
    
    // Render Log Messages
    u32 message_x = 362;
    u32 message_y = log_window.y + 12;
    u32 message_offset = 20;
    
    for(u32 i = 0; i < array_count(log_messages); ++i)
    {
        if(log_messages[i].message[0])
        {
            render_text(log_messages[i].message, message_x, message_y, log_messages[i].color, fonts[font_dos_vga], 0);
            message_y += message_offset;
        }
    }
    
    // Render Inventory
    if(inventory.is_open)
    {
        v4u inventory_window = {0};
        inventory_window.w = textures.inventory_window.w;
        inventory_window.h = textures.inventory_window.h;
        inventory_window.x = game.window_size.w - inventory_window.w - 4;
        inventory_window.y = game.window_size.h - inventory_window.h - textures.log_window.h - 4;
        SDL_RenderCopy(game.renderer, textures.ui, (SDL_Rect *)&textures.inventory_window, (SDL_Rect *)&inventory_window);
        
        { // Render Inventory Slot Items
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
            
            for(u32 slot_index = 0;
                slot_index< (inventory.w * inventory.h);
                ++slot_index)
            {
                if(inventory.slots[slot_index] &&
                   inventory.slots[slot_index]->is_equipped)
                {
                    item_info_t *info = item_info_from_slot_index(slot_index);
                    switch(info->slot)
                    {
                        case slot_head:
                        {
                            head_src.x = tile_mul(info->tile.x);
                            head_src.y = tile_mul(info->tile.y);
                        } break;
                        
                        case slot_body:
                        {
                            body_src.x = tile_mul(info->tile.x);
                            body_src.y = tile_mul(info->tile.y);
                        } break;
                        
                        case slot_legs:
                        {
                            legs_src.x = tile_mul(info->tile.x);
                            legs_src.y = tile_mul(info->tile.y);
                        } break;
                        
                        case slot_feet:
                        {
                            feet_src.x = tile_mul(info->tile.x);
                            feet_src.y = tile_mul(info->tile.y);
                        } break;
                        
                        case slot_amulet:
                        {
                            amulet_src.x = tile_mul(info->tile.x);
                            amulet_src.y = tile_mul(info->tile.y);
                        } break;
                        
                        case slot_off_hand:
                        {
                            second_hand_src.x = tile_mul(info->tile.x);
                            second_hand_src.y = tile_mul(info->tile.y);
                        } break;
                        
                        case slot_main_hand:
                        {
                            first_hand_src.x = tile_mul(info->tile.x);
                            first_hand_src.y = tile_mul(info->tile.y);
                        } break;
                        
                        case slot_ring:
                        {
                            ring_src.x = tile_mul(info->tile.x);
                            ring_src.y = tile_mul(info->tile.y);
                        } break;
                        
                        invalid_default_case;
                    }
                }
            }
            
            SDL_RenderCopy(game.renderer, textures.item_tileset.tex, (SDL_Rect *)&head_src, (SDL_Rect *)&head_dest);
            SDL_RenderCopy(game.renderer, textures.item_tileset.tex, (SDL_Rect *)&body_src, (SDL_Rect *)&body_dest);
            SDL_RenderCopy(game.renderer, textures.item_tileset.tex, (SDL_Rect *)&legs_src, (SDL_Rect *)&legs_dest);
            SDL_RenderCopy(game.renderer, textures.item_tileset.tex, (SDL_Rect *)&feet_src, (SDL_Rect *)&feet_dest);
            SDL_RenderCopy(game.renderer, textures.item_tileset.tex, (SDL_Rect *)&first_hand_src, (SDL_Rect *)&first_hand_dest);
            SDL_RenderCopy(game.renderer, textures.item_tileset.tex, (SDL_Rect *)&second_hand_src, (SDL_Rect *)&second_hand_dest);
            SDL_RenderCopy(game.renderer, textures.item_tileset.tex, (SDL_Rect *)&amulet_src, (SDL_Rect *)&amulet_dest);
            SDL_RenderCopy(game.renderer, textures.item_tileset.tex, (SDL_Rect *)&ring_src, (SDL_Rect *)&ring_dest);
        }
        
        u32 padding = 4;
        v2u first_slot = {inventory_window.x + 7, inventory_window.y + 192};
        u32 new_item_count = 0;
        
        for(u32 slot_index = 0;
            slot_index < (inventory.w * inventory.h);
            ++slot_index)
        {
            if(inventory.slots[slot_index])
            {
                ++new_item_count;
                item_info_t *info = item_info_from_slot_index(slot_index);
                
                v2u offset = v2u_from_index(slot_index, inventory.w);
                v4u src = {tile_mul(info->tile.x), tile_mul(info->tile.y), 32, 32};
                v4u dest = {first_slot.x + tile_mul(offset.x) + (offset.x * padding), first_slot.y + tile_mul(offset.y) + (offset.y * padding), 32, 32};
                
                // Item is being moved
                if(inventory.moving_item_src_index != (u32)-1 &&
                   inventory.moving_item_src_index == slot_index)
                {
                    SDL_SetTextureAlphaMod(textures.item_tileset.tex, 127);
                    SDL_RenderCopy(game.renderer, textures.item_tileset.tex, (SDL_Rect *)&src, (SDL_Rect *)&dest);
                    SDL_SetTextureAlphaMod(textures.item_tileset.tex, 255);
                }
                else
                {
                    // Render item in the slot
                    SDL_RenderCopy(game.renderer, textures.item_tileset.tex, (SDL_Rect *)&src, (SDL_Rect *)&dest);
                }
                
                // Item is equipped
                if(inventory.slots[slot_index]->is_equipped)
                {
                    SDL_RenderCopy(game.renderer, textures.ui, (SDL_Rect *)&textures.inventory_equipped_slot, (SDL_Rect *)&dest);
                }
                
                if(slot_index == index_from_v2u(inventory.current, inventory.w))
                {
                    item_window_t item_window = {0};
                    item_window.is_comparing_items = false;
                    item_window.w = textures.item_window.w;
                    item_window.h = textures.item_window.h;
                    item_window.x = inventory_window.x - item_window.w - 6;
                    item_window.y = inventory_window.y;
                    item_window.at.x = item_window.x;
                    item_window.at.y = item_window.y;
                    item_window.offset_per_row = 20;
                    item_window.offset_to_actions = item_window.y + 270;
                    
                    render_item_window(item_window, slot_index);
                    
                    u32_t slot = get_equipped_item_slot_index(info->slot);
                    if(slot.success && slot.value != slot_index)
                    {
                        item_window.is_comparing_items = true;
                        item_window.x = item_window.x - item_window.w - 4;
                        item_window.at.x = item_window.x;
                        item_window.at.y = item_window.y;
                        item_window.offset_to_actions = item_window.y + 310;
                        
                        render_item_window(item_window, slot.value);
                    }
                }
            }
        }
        
        u32 selected_x_offset = tile_mul(inventory.current.x) + (inventory.current.x * padding);
        u32 selected_y_offset = tile_mul(inventory.current.y) + (inventory.current.y * padding);
        v4u slot_dest = {first_slot.x + selected_x_offset, first_slot.y + selected_y_offset, textures.inventory_selected_slot.w, textures.inventory_selected_slot.h};
        SDL_RenderCopy(game.renderer, textures.ui, (SDL_Rect *)&textures.inventory_selected_slot, (SDL_Rect *)&slot_dest);
        
        // Render the item being moved at current slot
        if(inventory.is_item_being_moved)
        {
            item_info_t *info = item_info_from_slot_index(inventory.moving_item_src_index);
            v4u slot_src = {tile_mul(info->tile.x), tile_mul(info->tile.y), 32, 32};
            SDL_RenderCopy(game.renderer, textures.item_tileset.tex, (SDL_Rect *)&slot_src, (SDL_Rect *)&slot_dest);
        }
    }
}