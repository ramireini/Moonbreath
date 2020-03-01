internal u32
next_window_row(item_window_t window)
{
    u32 result = window.at.y += window.offset_per_row;
    return(result);
}

internal void
render_window_item_info(item_window_t window, item_info_t *item_info)
{
    if(item_info->type == type_weapon ||
       item_info->type == type_armor)
    {
        if(item_info->stats.min_damage &&
           item_info->stats.max_damage)
        {
            window.at.y = next_window_row(window);
            render_text("%u - %u Damage", window.at.x, window.at.y, color_white, 0, fonts[font_dos_vga], item_info->stats.min_damage, item_info->stats.max_damage);
        }
        
        if(item_info->stats.strength)
        {
            window.at.y = next_window_row(window);
            render_text("+%u Strength", window.at.x, window.at.y, color_white, 0, fonts[font_dos_vga], item_info->stats.strength);
        }
        
        if(item_info->stats.defence)
        {
            window.at.y = next_window_row(window);
            render_text("+%u Defence", window.at.x, window.at.y, color_white, 0, fonts[font_dos_vga], item_info->stats.defence);
        }
        
        if(item_info->stats.vitality)
        {
            window.at.y = next_window_row(window);
            render_text("+%u Vitality", window.at.x, window.at.y, color_white, 0, fonts[font_dos_vga], item_info->stats.vitality);
        }
    }
    else if(item_info->type == type_consumable)
    {
        window.at.y = next_window_row(window);
        render_text(item_info->consumable.effect_text, window.at.x, window.at.y, color_green, 0, fonts[font_dos_vga]);
    }
    
    window.at.y = next_window_row(window);
    render_text(item_info->description, window.at.x, window.at.y, color_light_brown, window.x + window.w - 20, fonts[font_alkhemikal]);
}

internal void
render_window_actions(item_window_t window, item_t *item_slot, item_info_t *item_info)
{
    if(window.is_comparing_items)
    {
        window.at.y = window.offset_to_actions;
        render_text("Currently Equipped", window.at.x, window.at.y, color_gray, 0, fonts[font_dos_vga]);
        
#if MOONBREATH_SLOW
        render_text("Unique ID: %u", window.at.x, window.at.y - window.offset_per_row, color_orange, 0, fonts[font_dos_vga], item_slot->unique_id);
#endif
    }
    else
    {
        window.at.y = window.offset_to_actions;
        
        if(item_info->type == type_weapon ||
           item_info->type == type_armor)
        {
            
#if MOONBREATH_SLOW
            render_text("Unique ID: %u", window.at.x, window.at.y - window.offset_per_row, color_orange, 0, fonts[font_dos_vga], item_slot->unique_id);
#endif
            
            if(item_slot->is_equipped)
            {
                render_text("[E] Unequip", window.at.x, window.at.y, color_white, 0, fonts[font_dos_vga]);
            }
            else
            {
                render_text("[E] Equip", window.at.x, window.at.y, color_white, 0, fonts[font_dos_vga]);
            }
        }
        else if(item_info->type == type_consumable)
        {
            render_text("[C]onsume", window.at.x, window.at.y, color_white, 0, fonts[font_dos_vga]);
        }
        
        window.at.y = next_window_row(window);
        render_text("[M] Move", window.at.x, window.at.y, color_white, 0, fonts[font_dos_vga]);
        
        window.at.y = next_window_row(window);
        render_text("[.] Drop", window.at.x, window.at.y, color_white, 0, fonts[font_dos_vga]);
    }
}

internal void
render_window_background(item_window_t window)
{
    v4u window_rect = {window.x, window.y, window.w, window.h};
    SDL_RenderCopy(game.renderer, textures.ui, (SDL_Rect *)&textures.item_window, (SDL_Rect *)&window_rect);
}

internal v2u
render_window_item_name(item_window_t window, char *item_name)
{
    v2u result = V2u(window.x + 12, window.y + 12);
    render_text(item_name, result.x, result.y, color_white, 0, fonts[font_dos_vga]);
    result.y += window.offset_per_row;
    
    return(result);
}

internal void
render_item_window(item_window_t window, u32 item_inventory_index)
{
    item_t *item_inventory_slot = &inventory.slots[item_inventory_index];
    u32 item_info_index = item_info_index_from_inventory_index(item_inventory_index);
    item_info_t *info = &item_info[item_info_index];
    
    render_window_background(window);
    window.at = render_window_item_name(window, info->name);
    
    render_window_item_info(window, info);
    render_window_actions(window, item_inventory_slot, info);
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
set_and_render_inventory_slot_items(v4u inventory_win)
{
    v4u head_src = {0, 0, 32, 32};
    v4u head_dest = {inventory_win.x + 133, inventory_win.y + 7, 32, 32};
    
    v4u body_src = {32, 0, 32, 32};
    v4u body_dest = {inventory_win.x + 133, inventory_win.y + 79, 32, 32};
    
    v4u legs_src = {64, 0, 32, 32};
    v4u legs_dest = {inventory_win.x + 133, inventory_win.y + 115, 32, 32};
    
    v4u feet_src = {96, 0, 32, 32};
    v4u feet_dest = {inventory_win.x + 133, inventory_win.y + 151, 32, 32};
    
    v4u first_hand_src = {128, 0, 32, 32};
    v4u first_hand_dest = {inventory_win.x + 97, inventory_win.y + 79, 32, 32};
    
    v4u second_hand_src = {160, 0, 32, 32};
    v4u second_hand_dest = {inventory_win.x + 169, inventory_win.y + 79, 32, 32};
    
    v4u amulet_src = {192, 0, 32, 32};
    v4u amulet_dest = {inventory_win.x + 133, inventory_win.y + 43, 32, 32};
    
    v4u ring_src = {224, 0, 32, 32};
    v4u ring_dest = {inventory_win.x + 97, inventory_win.y + 151, 32, 32};
    
    for(u32 inventory_index = 0;
        inventory_index< (inventory.w * inventory.h);
        ++inventory_index)
    {
        if(inventory.slots[inventory_index].id &&
           inventory.slots[inventory_index].is_equipped)
        {
            u32 info_index = item_info_index_from_inventory_index(inventory_index);
            switch(item_info[info_index].slot)
            {
                case slot_head:
                {
                    head_src.x = tile_mul(item_info[info_index].tile.x);
                    head_src.y = tile_mul(item_info[info_index].tile.y);
                } break;
                
                case slot_body:
                {
                    body_src.x = tile_mul(item_info[info_index].tile.x);
                    body_src.y = tile_mul(item_info[info_index].tile.y);
                } break;
                
                case slot_legs:
                {
                    legs_src.x = tile_mul(item_info[info_index].tile.x);
                    legs_src.y = tile_mul(item_info[info_index].tile.y);
                } break;
                
                case slot_feet:
                {
                    feet_src.x = tile_mul(item_info[info_index].tile.x);
                    feet_src.y = tile_mul(item_info[info_index].tile.y);
                } break;
                
                case slot_first_hand:
                {
                    first_hand_src.x = tile_mul(item_info[info_index].tile.x);
                    first_hand_src.y = tile_mul(item_info[info_index].tile.y);
                } break;
                
                case slot_second_hand:
                {
                    second_hand_src.x = tile_mul(item_info[info_index].tile.x);
                    second_hand_src.y = tile_mul(item_info[info_index].tile.y);
                } break;
                
                case slot_amulet:
                {
                    amulet_src.x = tile_mul(item_info[info_index].tile.x);
                    amulet_src.y = tile_mul(item_info[info_index].tile.y);
                } break;
                
                case slot_ring:
                {
                    ring_src.x = tile_mul(item_info[info_index].tile.x);
                    ring_src.y = tile_mul(item_info[info_index].tile.y);
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

internal void
render_ui()
{
    v4u log_window = {0, game.window_size.h - textures.log_window.h, textures.log_window.w, textures.log_window.h};
    SDL_RenderCopy(game.renderer, textures.ui, (SDL_Rect *)&textures.log_window, (SDL_Rect *)&log_window);
    
    // TODO(rami):
#if 0
    { // Render Player HP Bar
        v4u health_bar_outside = {42, bottom_window.y + 28, textures.health_bar_outside.w, textures.health_bar_outside.h};
        SDL_RenderCopy(game.renderer, textures.ui, (SDL_Rect *)&textures.health_bar_outside, (SDL_Rect *)&health_bar_outside);
        
        u32 health_bar_inside_w = 0;
        if(player.hp > 0)
        {
            health_bar_inside_w = get_ratio(player.hp, player.max_hp, textures.health_bar_inside.w);
        }
        
        v4u health_bar_inside_src = {textures.health_bar_inside.x, textures.health_bar_inside.y, health_bar_inside_w, textures.health_bar_inside.h};
        v4u health_bar_inside_dest = {health_bar_outside.x + 2, health_bar_outside.y + 2, health_bar_inside_w, textures.health_bar_inside.h};
        SDL_RenderCopy(game.renderer, textures.ui, (SDL_Rect *)&health_bar_inside_src,  (SDL_Rect *)&health_bar_inside_dest);
    }
    
    { // Render Player Stats
        render_text(player.name, 10, bottom_window.y + 8, color_white, 0, fonts[font_dos_vga]);
        render_text("HP", 10, bottom_window.y + 32, color_white, 0, fonts[font_dos_vga], player.hp, player.max_hp);
        render_text("%u (%u)", 118, bottom_window.y + 34, color_white, 0, fonts[font_dos_vga], player.hp, player.max_hp);
        render_text("Strength: %u", 10, bottom_window.y + 60, color_white, 0, fonts[font_dos_vga], player.strength);
        render_text("Defence: %u", 10, bottom_window.y + 80, color_white, 0, fonts[font_dos_vga], player.defence);
        render_text("Level: %u", 10, bottom_window.y + 100, color_white, 0, fonts[font_dos_vga], player.level);
        render_text("Turn: %u", 10, bottom_window.y + 120, color_white, 0, fonts[font_dos_vga], game.turn);
    }
#endif
    
    { // Render Log Messages
        u32 message_x = 12;
        u32 message_y = log_window.y + 10;
        u32 message_offset = 20;
        
        for(u32 i = 0; i < array_count(log_messages); ++i)
        {
            if(log_messages[i].message[0])
            {
                render_text(log_messages[i].message, message_x, message_y, log_messages[i].color, 0, fonts[font_dos_vga]);
                message_y += message_offset;
            }
        }
    }
    
    { // Render Inventory
        if(inventory.is_open)
        {
            v4u inventory_window = {game.window_size.w - 324, game.window_size.h - 550, textures.inventory_window.w, textures.inventory_window.h};
            SDL_RenderCopy(game.renderer, textures.ui, (SDL_Rect *)&textures.inventory_window, (SDL_Rect *)&inventory_window);
            
            set_and_render_inventory_slot_items(inventory_window);
            
            u32 padding = 4;
            v2u first_slot = {inventory_window.x + 7, inventory_window.y + 192};
            u32 new_item_count = 0;
            
            for(u32 inventory_index = 0;
                inventory_index < (inventory.w * inventory.h);
                ++inventory_index)
            {
                if(inventory.slots[inventory_index].id)
                {
                    ++new_item_count;
                    u32 inventory_item_info_index = item_info_index_from_inventory_index(inventory_index);
                    
                    v2u offset = v2u_from_index(inventory_index, inventory.w);
                    v4u src = {tile_mul(item_info[inventory_item_info_index].tile.x), tile_mul(item_info[inventory_item_info_index].tile.y), 32, 32};
                    v4u dest = {first_slot.x + tile_mul(offset.x) + (offset.x * padding), first_slot.y + tile_mul(offset.y) + (offset.y * padding), 32, 32};
                    
                    // Item is equipped
                    if(inventory.slots[inventory_index].is_equipped)
                    {
                        SDL_RenderCopy(game.renderer, textures.ui, (SDL_Rect *)&textures.inventory_equipped_slot, (SDL_Rect *)&dest);
                    }
                    
                    // Item is being moved
                    if(inventory.moved_item_src_index != (u32)-1 &&
                       inventory.moved_item_src_index == inventory_index)
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
                    
                    if(inventory_index == index_from_v2u(inventory.current_slot, inventory.w))
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
                        
                        render_item_window(item_window, inventory_index);
                        
                        equip_slot_t slot = get_item_equip_slot_status(inventory_index);
                        if(slot.has_an_item)
                        {
                            item_window.is_comparing_items = true;
                            item_window.x = item_window.x - item_window.w - 6;
                            item_window.at.x = item_window.x;
                            item_window.at.y = item_window.y;
                            item_window.offset_to_actions = item_window.y + 310;
                            
                            render_item_window(item_window, slot.equipped_item_inventory_index);
                        }
                    }
                }
            }
            
            u32 selected_x_offset = tile_mul(inventory.current_slot.x) + (inventory.current_slot.x * padding);
            u32 selected_y_offset = tile_mul(inventory.current_slot.y) + (inventory.current_slot.y * padding);
            v4u slot_dest = {first_slot.x + selected_x_offset, first_slot.y + selected_y_offset, textures.inventory_selected_slot.w, textures.inventory_selected_slot.h};
            SDL_RenderCopy(game.renderer, textures.ui, (SDL_Rect *)&textures.inventory_selected_slot, (SDL_Rect *)&slot_dest);
            
            // Render the item being moved at current slot
            if(inventory.item_is_being_moved)
            {
                u32 info_index = item_info_index_from_inventory_index(inventory.moved_item_src_index);
                v4u slot_src = {tile_mul(item_info[info_index].tile.x), tile_mul(item_info[info_index].tile.y), 32, 32};
                SDL_RenderCopy(game.renderer, textures.item_tileset.tex, (SDL_Rect *)&slot_src, (SDL_Rect *)&slot_dest);
            }
            
            inventory.item_count = new_item_count;
        }
    }
}