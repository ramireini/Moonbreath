internal void
add_console_text(char *str, v4f color, ...)
{
    char str_final[128] = {0};
    
    va_list arg_list;
    va_start(arg_list, color);
    vsnprintf(str_final, sizeof(str_final), str, arg_list);
    va_end(arg_list);
    
    for(u32 i = 0; i < array_count(console_texts); ++i)
    {
        if(!console_texts[i].str[0])
        {
            strcpy(console_texts[i].str, str_final);
            console_texts[i].color = color;
            return;
        }
    }
    
    console_texts[0].str[0] = 0;
    console_texts[0].color = color_black;
    
    for(u32 i = 1; i < array_count(console_texts); ++i)
    {
        strcpy(console_texts[i - 1].str, console_texts[i].str);
        console_texts[i - 1].color = console_texts[i].color;
    }
    
    strcpy(console_texts[array_count(console_texts) - 1].str, str_final);
    console_texts[array_count(console_texts) - 1].color = color;
}

internal v2u
render_window_item_name(item_window_t window, char *item_name)
{
    v2u result = V2u(window.x + 12, window.y + 12);
    render_text(item_name, result, color_white, 0, fonts[font_dos_vga]);
    result.y += window.small_offset;
    
    return(result);
}

internal u32
next_window_stat_row(item_window_t window)
{
    u32 result = window.pos.y += window.small_offset;
    return(result);
}

internal void
render_window_background(item_window_t window)
{
    v4u window_rect = {window.x, window.y, window.w, window.h};
    SDL_RenderCopy(game.renderer, textures.ui, (SDL_Rect *)&textures.item_window, (SDL_Rect *)&window_rect);
}

internal void
render_item_window(item_window_t window, u32 equipped_item_inventory_index, b32 comparing_items)
{
    item_t *equipped_item_inventory_slot = &inventory.slots[equipped_item_inventory_index];
    u32 equipped_item_info_index = get_inventory_info_index(equipped_item_inventory_index);
    item_info_t *equipped_item_info = &item_info[equipped_item_info_index];
    
    render_window_background(window);
    window.pos = render_window_item_name(window, equipped_item_info->name);
    
    // Render Item Stats
    {
        if(equipped_item_info->type == type_weapon ||
           equipped_item_info->type == type_armor)
        {
            if(equipped_item_info->stats.min_damage &&
               equipped_item_info->stats.max_damage)
            {
                window.pos.y = next_window_stat_row(window);
                render_text("%u - %u Damage", window.pos, color_white, 0, fonts[font_dos_vga], equipped_item_info->stats.min_damage, equipped_item_info->stats.max_damage);
            }
            
            if(equipped_item_info->stats.strength)
            {
                window.pos.y = next_window_stat_row(window);
                render_text("+%u Strength", window.pos, color_green, 0, fonts[font_dos_vga], equipped_item_info->stats.strength);
            }
            
            if(equipped_item_info->stats.defence)
            {
                window.pos.y = next_window_stat_row(window);
                render_text("+%u Defence", window.pos, color_green, 0, fonts[font_dos_vga], equipped_item_info->stats.defence);
            }
            
            if(equipped_item_info->stats.vitality)
            {
                window.pos.y = next_window_stat_row(window);
                render_text("+%u Vitality", window.pos, color_green, 0, fonts[font_dos_vga], equipped_item_info->stats.vitality);
            }
        }
        else if(equipped_item_info->type == type_consumable)
        {
            window.pos.y = next_window_stat_row(window);
            render_text(equipped_item_info->consumable.effect_text, window.pos, color_green, 0, fonts[font_dos_vga]);
        }
        
        window.pos.y = next_window_stat_row(window);
        render_text(equipped_item_info->description, window.pos, color_light_brown, window.x + window.w - 20, fonts[font_alkhemikal]);
    }
    
    // Render Item Options
    {
        window.pos.y = window.actions_offset;
        
        if(equipped_item_info->type == type_weapon ||
           equipped_item_info->type == type_armor)
        {
            
#if MOONBREATH_SLOW
            render_text("Unique ID: %u", V2u(window.pos.x, window.pos.y - window.small_offset), color_orange, 0, fonts[font_dos_vga], equipped_item_inventory_slot->unique_id);
#endif
            
            if(equipped_item_inventory_slot->is_equipped)
            {
                render_text("[E] Unequip", window.pos, color_white, 0, fonts[font_dos_vga]);
            }
            else
            {
                render_text("[E] Equip", window.pos, color_white, 0, fonts[font_dos_vga]);
            }
        }
        else if(equipped_item_info->type == type_consumable)
        {
            render_text("[C]onsume", window.pos, color_white, 0, fonts[font_dos_vga]);
        }
        
        window.pos.y = next_window_stat_row(window);
        render_text("[M] Move", window.pos, color_white, 0, fonts[font_dos_vga]);
        
        window.pos.y = next_window_stat_row(window);
        render_text("[.] Drop", window.pos, color_white, 0, fonts[font_dos_vga]);
    }
}

internal void
render_comparison_item_window(item_window_t window, u32 selected_item_inventory_index, u32 equipped_item_inventory_index)
{
    item_t *equipped_item_inventory_slot = &inventory.slots[equipped_item_inventory_index];
    u32 equipped_item_info_index = get_inventory_info_index(equipped_item_inventory_index);
    item_info_t *equipped_item_info = &item_info[equipped_item_info_index];
    
    u32 selected_item_info_index = get_inventory_info_index(selected_item_inventory_index);
    item_info_t *selected_item_info = &item_info[selected_item_info_index];
    
    render_window_background(window);
    window.pos = render_window_item_name(window, equipped_item_info->name);
    
    if(selected_item_info->type == type_weapon ||
       selected_item_info->type == type_armor)
    {
        if(selected_item_info->stats.min_damage &&
           selected_item_info->stats.max_damage)
        {
            window.pos.y = next_window_stat_row(window);
            render_text("%u - %u Damage", window.pos, color_white, 0, fonts[font_dos_vga], equipped_item_info->stats.min_damage, equipped_item_info->stats.max_damage);
        }
        
        // TODO(rami): We don't do the comparisons in the
        // render_item_window function. Do them there as well or
        // don't do them at all.
        v4f stat_color = color_yellow;
        
        if(equipped_item_info->stats.strength)
        {
            if(selected_item_info->stats.strength < equipped_item_info->stats.strength)
            {
                stat_color = color_green;
            }
            else if(selected_item_info->stats.strength > equipped_item_info->stats.strength)
            {
                stat_color = color_red;
            }
            else
            {
                stat_color = color_yellow;
            }
            
            window.pos.y = next_window_stat_row(window);
            render_text("+%u Strength", window.pos, stat_color, 0, fonts[font_dos_vga], equipped_item_info->stats.strength);
        }
        
        if(equipped_item_info->stats.defence)
        {
            if(selected_item_info->stats.defence < equipped_item_info->stats.defence)
            {
                stat_color = color_green;
            }
            else if(selected_item_info->stats.defence > equipped_item_info->stats.defence)
            {
                stat_color = color_red;
            }
            else
            {
                stat_color = color_yellow;
            }
            
            window.pos.y = next_window_stat_row(window);
            render_text("+%u Defence", window.pos, stat_color, 0, fonts[font_dos_vga], equipped_item_info->stats.defence);
        }
        
        if(equipped_item_info->stats.vitality)
        {
            if(selected_item_info->stats.vitality < equipped_item_info->stats.vitality)
            {
                stat_color = color_green;
            }
            else if(selected_item_info->stats.vitality > equipped_item_info->stats.vitality)
            {
                stat_color = color_red;
            }
            else
            {
                stat_color = color_yellow;
            }
            
            window.pos.y = next_window_stat_row(window);
            render_text("+%u Vitality", window.pos, stat_color, 0, fonts[font_dos_vga], equipped_item_info->stats.vitality);
        }
    }
    
    window.pos.y = next_window_stat_row(window);
    render_text(equipped_item_info->description, window.pos, color_light_brown, window.x + window.w - 20, fonts[font_alkhemikal]);
    
    window.pos.y = window.actions_offset;
    render_text("Currently Equipped", window.pos, color_gray, 0, fonts[font_dos_vga]);
    
#if MOONBREATH_SLOW
    render_text("Unique ID: %u", V2u(window.pos.x, window.pos.y - window.small_offset), color_orange, 0, fonts[font_dos_vga], equipped_item_inventory_slot->unique_id);
#endif
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
    
    for(u32 i = 0; i < (inventory.w * inventory.h); ++i)
    {
        if(inventory.slots[i].id && inventory.slots[i].is_equipped)
        {
            u32 info_index = get_inventory_info_index(i);
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
    v4u bottom_window = {0, game.window_size.h - game.console_size.h, textures.bottom_window.w, textures.bottom_window.h};
    SDL_RenderCopy(game.renderer, textures.ui, (SDL_Rect *)&textures.bottom_window, (SDL_Rect *)&bottom_window);
    
    { // Render Player HP Bar
        v4u health_bar_outside = {42, game.window_size.h - 132, textures.health_bar_outside.w, textures.health_bar_outside.h};
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
        render_text(player.name, V2u(10, game.window_size.h - 152), color_white, 0, fonts[font_dos_vga]);
        render_text("HP", V2u(10, game.window_size.h - 128), color_white, 0, fonts[font_dos_vga], player.hp, player.max_hp);
        render_text("%u (%u)", V2u(118, game.window_size.h - 126), color_white, 0, fonts[font_dos_vga], player.hp, player.max_hp);
        render_text("Strength: %u", V2u(10, game.window_size.h - 100), color_white, 0, fonts[font_dos_vga], player.strength);
        render_text("Defence: %u", V2u(10, game.window_size.h - 82), color_white, 0, fonts[font_dos_vga], player.defence);
        render_text("Level: %u", V2u(10, game.window_size.h - 64), color_white, 0, fonts[font_dos_vga], player.level);
        render_text("Turn: %u", V2u(10, game.window_size.h - 26), color_white, 0, fonts[font_dos_vga], game.turn);
    }
    
    { // Render Console Texts
        v2u text_pos = {396, game.window_size.h - 152};
        u32 text_offset = 18;
        
        for(u32 i = 0; i < array_count(console_texts); ++i)
        {
            if(console_texts[i].str[0])
            {
                render_text(console_texts[i].str, text_pos, console_texts[i].color, 0, fonts[font_dos_vga]);
                text_pos.y += text_offset;
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
                    u32 inventory_item_info_index = get_inventory_info_index(inventory_index);
                    
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
                        item_window.w = textures.item_window.w;
                        item_window.h = textures.item_window.h;
                        item_window.x = inventory_window.x - item_window.w - 6;
                        item_window.y = inventory_window.y;
                        item_window.pos.x = item_window.x;
                        item_window.pos.y = item_window.y;
                        item_window.small_offset = 20;
                        item_window.actions_offset = item_window.y + 270;
                        
                        u32_t slot_item_inventory_index = get_inventory_index_of_item_in_inventory_slot(inventory_index);
                        if(slot_item_inventory_index.success)
                        {
                            render_item_window(item_window, inventory_index, true);
                            
                            item_window.x = item_window.x - item_window.w - 6;
                            item_window.pos.x = item_window.x;
                            item_window.pos.y = item_window.y;
                            item_window.actions_offset = item_window.y + 310;
                            render_comparison_item_window(item_window, inventory_index, slot_item_inventory_index.value);
                        }
                        else
                        {
                            render_item_window(item_window, inventory_index, false);
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
                u32 info_index = get_inventory_info_index(inventory.moved_item_src_index);
                v4u slot_src = {tile_mul(item_info[info_index].tile.x), tile_mul(item_info[info_index].tile.y), 32, 32};
                SDL_RenderCopy(game.renderer, textures.item_tileset.tex, (SDL_Rect *)&slot_src, (SDL_Rect *)&slot_dest);
            }
            
            inventory.item_count = new_item_count;
        }
    }
}