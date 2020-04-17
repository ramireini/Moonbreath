internal void
render_window_item_stats(item_window_t window, item_t *item)
{
    if(item->is_identified)
    {
        render_text("##2 %s%s", window.at.x, window.at.y, fonts[font_dos_vga], 0, get_item_rarity_text(item), get_item_id_text(item));
        window.at.y += window.offset_per_row;
        
        render_text("##2 %s", window.at.x, window.at.y, fonts[font_dos_vga], 0, get_item_handedness_text(item));
        window.at.y += window.offset_per_row;
        
        if(item->type == item_type_weapon)
        {
            window.at.y += window.offset_per_row;
            if(item->secondary_damage_type)
            {
                render_text("Damage Type: %s (%s)", window.at.x, window.at.y, fonts[font_dos_vga], 0, get_item_damage_type_text(item->primary_damage_type), get_item_damage_type_text(item->secondary_damage_type));
            }
            else
            {
                render_text("Damage Type: %s", window.at.x, window.at.y, fonts[font_dos_vga], 0, get_item_damage_type_text(item->primary_damage_type));
            }
            
            window.at.y += window.offset_per_row;
            render_text("Base Damage: %d", window.at.x, window.at.y, fonts[font_dos_vga], 0, item->w.damage);
            
            window.at.y += window.offset_per_row;
            render_text("Base Accuracy: %d", window.at.x, window.at.y, fonts[font_dos_vga], 0, item->w.accuracy);
            
            window.at.y += window.offset_per_row;
            render_text("Speed: %.1f", window.at.x, window.at.y, fonts[font_dos_vga], 0, item->w.speed);
        }
        else if(item->type == item_type_armor)
        {
            window.at.y += window.offset_per_row;
            render_text("Base Defence: %d", window.at.x, window.at.y, fonts[font_dos_vga], 0, item->a.defence);
            
            window.at.y += window.offset_per_row;
            render_text("Weight: %s", window.at.x, window.at.y, fonts[font_dos_vga], 0, item->a.weight);
        }
        else if(item->type == item_type_potion ||
                item->type == item_type_scroll)
        {
            render_text("##A %s", window.at.x, window.at.y, fonts[font_dos_vga], 0, item->description);
        }
    }
    else
    {
        render_text("##4 Unidentified", window.at.x, window.at.y, fonts[font_dos_vga], 0);
    }
}

internal void
render_window_actions(item_window_t window, item_t *item)
{
    if(window.is_comparing_items)
    {
        window.at.y = window.offset_to_actions;
        render_text("##2 Currently Equipped", window.at.x, window.at.y, fonts[font_dos_vga], 0);
    }
    else
    {
        window.at.y = window.offset_to_actions;
        
        if(item->type == item_type_weapon ||
           item->type == item_type_armor)
        {
            render_text("[%c] %s", window.at.x, window.at.y, fonts[font_dos_vga], 0, game.keybinds[key_equip_item], item->is_equipped ? "Unequip" : "Equip");
        }
        else if(item->type == item_type_potion ||
                item->type == item_type_scroll)
        {
            render_text("[%c] Consume", window.at.x, window.at.y, fonts[font_dos_vga], 0, game.keybinds[key_consume_item]);
        }
        
        window.at.y += window.offset_per_row;
        render_text("[%c] Move", window.at.x, window.at.y, fonts[font_dos_vga], 0, game.keybinds[key_move_item]);
        
        window.at.y += window.offset_per_row;
        render_text("[%c] Drop", window.at.x, window.at.y, fonts[font_dos_vga], 0, game.keybinds[key_drop_item]);
    }
}

internal void
render_window_background(item_window_t window)
{
    v4u window_rect = {window.x, window.y, window.w, window.h};
    SDL_RenderCopy(game.renderer, textures.ui, (SDL_Rect *)&textures.item_window, (SDL_Rect *)&window_rect);
}

internal v2u
render_window_item_name(item_window_t window, item_t *item)
{
    v2u result = V2u(window.x + 12, window.y + 12);
    
    if(item->is_identified)
    {
        string_t full_item_name = get_full_item_name(item);
        render_text("%s%s", result.x, result.y, fonts[font_dos_vga], 0, get_item_rarity_color_code(item->rarity), full_item_name.str);
    }
    else
    {
        render_text("%s%s", result.x, result.y, fonts[font_dos_vga], 0, get_item_rarity_color_code(item->rarity), get_item_id_text(item));
    }
    
    result.y += window.offset_per_row;
    return(result);
}

internal void
render_item_window(item_window_t window, u32 slot_index)
{
    render_window_background(window);
    window.at = render_window_item_name(window, inventory.slots[slot_index]);
    
    render_window_item_stats(window, inventory.slots[slot_index]);
    render_window_actions(window, inventory.slots[slot_index]);
}

internal void
add_log_string(char *string, ...)
{
    char formatted_string[128] = {0};
    
    va_list arg_list;
    va_start(arg_list, string);
    vsnprintf(formatted_string, sizeof(formatted_string), string, arg_list);
    va_end(arg_list);
    
    for(u32 string_index = 0;
        string_index < array_count(log_strings);
        ++string_index)
    {
        if(!log_strings[string_index].str[0])
        {
            strcpy(log_strings[string_index].str, formatted_string);
            return;
        }
    }
    
    log_strings[0].str[0] = 0;
    
    for(u32 string_index = 1;
        string_index < array_count(log_strings);
        ++string_index)
    {
        strcpy(log_strings[string_index - 1].str, log_strings[string_index].str);
    }
    
    strcpy(log_strings[array_count(log_strings) - 1].str, formatted_string);
}

internal void
render_ui()
{
    v4u log_window = {0, game.window_size.h - textures.log_window.h, textures.log_window.w, textures.log_window.h};
    SDL_RenderCopy(game.renderer, textures.ui, (SDL_Rect *)&textures.log_window, (SDL_Rect *)&log_window);
    
    // Render Player Stats
    v2u stat_start = {12, game.window_size.h - textures.log_window.h};
    
    render_text(player->name, stat_start.x, stat_start.y + 12, fonts[font_dos_vga], 0);
    render_text("Health: %u/%u", stat_start.x, stat_start.y + 30, fonts[font_dos_vga], 0, player->hp, player->max_hp);
    render_text("Str: %u", stat_start.x, stat_start.y + 48, fonts[font_dos_vga], 0, player->strength);
    render_text("Int: %u", stat_start.x, stat_start.y + 66, fonts[font_dos_vga], 0, player->intelligence);
    render_text("Dex: %u", stat_start.x, stat_start.y + 84, fonts[font_dos_vga], 0, player->dexterity);
    render_text("Gold: %u", stat_start.x, stat_start.y + 102, fonts[font_dos_vga], 0, player->p.gold);
    render_text("Defence: %u", stat_start.x + 128, stat_start.y + 48, fonts[font_dos_vga], 0, player->p.defence);
    render_text("Evasion: %u", stat_start.x + 128, stat_start.y + 66, fonts[font_dos_vga], 0, player->evasion);
    render_text("Time: %.01f", stat_start.x + 128, stat_start.y + 84, fonts[font_dos_vga], 0, game.time);
    render_text("Location: Dungeon: %u", stat_start.x + 128, stat_start.y + 102, fonts[font_dos_vga], 0, dungeon.level);
    
    // Render Player HP Bar
    v4u health_bar_outside = {stat_start.x + 126, stat_start.y + 29, textures.health_bar_outside.w, textures.health_bar_outside.h};
    SDL_RenderCopy(game.renderer, textures.ui, (SDL_Rect *)&textures.health_bar_outside, (SDL_Rect *)&health_bar_outside);
    
    u32 health_bar_inside_w = 0;
    if(player->hp > 0)
    {
        health_bar_inside_w = get_ratio(player->hp, player->max_hp, textures.health_bar_inside.w);
    }
    
    v4u health_bar_inside_src = {textures.health_bar_inside.x, textures.health_bar_inside.y, health_bar_inside_w, textures.health_bar_inside.h};
    v4u health_bar_inside_dest = {health_bar_outside.x + 2, health_bar_outside.y + 2, health_bar_inside_w, textures.health_bar_inside.h};
    SDL_RenderCopy(game.renderer, textures.ui, (SDL_Rect *)&health_bar_inside_src,  (SDL_Rect *)&health_bar_inside_dest);
    
    // Render Log Strings
    u32 str_x = 362;
    u32 str_y = log_window.y + 12;
    u32 str_offset = 20;
    
    for(u32 string_index = 0;
        string_index < array_count(log_strings);
        ++string_index)
    {
        if(log_strings[string_index].str[0])
        {
            render_text(log_strings[string_index].str, str_x, str_y, fonts[font_dos_vga], 0);
            str_y += str_offset;
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
        
        { // Render Inventory Slot Shadows
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
                item_t *item = inventory.slots[slot_index];
                if(item && item->is_equipped)
                {
                    switch(item->slot)
                    {
                        case item_slot_head:
                        {
                            head_src.x = tile_mul(item->tile.x);
                            head_src.y = tile_mul(item->tile.y);
                        } break;
                        
                        case item_slot_body:
                        {
                            body_src.x = tile_mul(item->tile.x);
                            body_src.y = tile_mul(item->tile.y);
                        } break;
                        
                        case item_slot_legs:
                        {
                            legs_src.x = tile_mul(item->tile.x);
                            legs_src.y = tile_mul(item->tile.y);
                        } break;
                        
                        case item_slot_feet:
                        {
                            feet_src.x = tile_mul(item->tile.x);
                            feet_src.y = tile_mul(item->tile.y);
                        } break;
                        
                        case item_slot_amulet:
                        {
                            amulet_src.x = tile_mul(item->tile.x);
                            amulet_src.y = tile_mul(item->tile.y);
                        } break;
                        
                        case item_slot_off_hand:
                        {
                            second_hand_src.x = tile_mul(item->tile.x);
                            second_hand_src.y = tile_mul(item->tile.y);
                        } break;
                        
                        case item_slot_main_hand:
                        {
                            first_hand_src.x = tile_mul(item->tile.x);
                            first_hand_src.y = tile_mul(item->tile.y);
                        } break;
                        
                        case item_slot_ring:
                        {
                            ring_src.x = tile_mul(item->tile.x);
                            ring_src.y = tile_mul(item->tile.y);
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
            item_t *item = inventory.slots[slot_index];
            if(item)
            {
                ++new_item_count;
                
                v2u offset = v2u_from_index(slot_index, inventory.w);
                v4u src = {tile_mul(item->tile.x), tile_mul(item->tile.y), 32, 32};
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
                    
                    u32_t slot = get_equipped_item_slot_index(item->slot);
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
            item_t *item = inventory.slots[inventory.moving_item_src_index];
            v4u slot_src = {tile_mul(item->tile.x), tile_mul(item->tile.y), 32, 32};
            SDL_RenderCopy(game.renderer, textures.item_tileset.tex, (SDL_Rect *)&slot_src, (SDL_Rect *)&slot_dest);
        }
    }
}