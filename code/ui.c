internal void
add_console_text(char *str, v4f color, ...)
{
    char str_final[256] = {0};
    
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

internal v4u
render_item_window(v2u pos, u32 equipped_item_inventory_index, b32 comparing_items)
{
    v4u item_window = {0};
    item_window.w = textures.item_window.w;
    item_window.h = textures.item_window.h;
    item_window.x = pos.x - item_window.w - 6;
    item_window.y = pos.y;
    
    SDL_RenderCopy(game.renderer, textures.ui, (SDL_Rect *)&textures.item_window, (SDL_Rect *)&item_window);
    
    item_t *equipped_item_inventory_slot = &inventory.slots[equipped_item_inventory_index];
    u32 equipped_item_info_index = get_inventory_info_index(equipped_item_inventory_index);
    item_info_t *equipped_item_info = &item_info[equipped_item_info_index];
    
    v2u name_pos = {item_window.x + 12, item_window.y + 12};
    render_text("%s", name_pos, color_white, 0, fonts[font_dos_vga], equipped_item_info->name);
    
    v2u stat_pos = {name_pos.x, name_pos.y + 35};
    u32 stat_offset = 20;
    
    if(equipped_item_info->type != type_consumable)
    {
        if(equipped_item_info->stats.min_damage &&
           equipped_item_info->stats.max_damage)
        {
            render_text("%u - %u Damage", stat_pos, color_white, 0, fonts[font_dos_vga], equipped_item_info->stats.min_damage, equipped_item_info->stats.max_damage);
            stat_pos.y += stat_offset;
        }
        
        if(equipped_item_info->stats.strength)
        {
            render_text("+%u Strength", stat_pos, color_green, 0, fonts[font_dos_vga], equipped_item_info->stats.strength);
            stat_pos.y += stat_offset;
        }
        
        if(equipped_item_info->stats.defence)
        {
            render_text("+%u Defence", stat_pos, color_green, 0, fonts[font_dos_vga], equipped_item_info->stats.defence);
            stat_pos.y += stat_offset;
        }
        
        if(equipped_item_info->stats.vitality)
        {
            render_text("+%u Vitality", stat_pos, color_green, 0, fonts[font_dos_vga], equipped_item_info->stats.vitality);
            stat_pos.y += stat_offset;
        }
        
        if(!comparing_items)
        {
            if(equipped_item_inventory_slot->is_equipped)
            {
                v2u unequip_pos = {name_pos.x, name_pos.y + 260};
                render_text("[E] Unequip", unequip_pos, color_white, 0, fonts[font_dos_vga]);
            }
            else
            {
                v2u equip_pos = {name_pos.x, name_pos.y + 260};
                render_text("[E] Equip", equip_pos, color_white, 0, fonts[font_dos_vga]);
            }
        }
    }
    else
    {
        render_text(equipped_item_info->consumable.effect_text, stat_pos, color_green, 0, fonts[font_dos_vga]);
        stat_pos.y += stat_offset;
        
        v2u consume_pos = {name_pos.x, name_pos.y + 230};
        render_text("[C]onsume", consume_pos, color_white, 0, fonts[font_dos_vga]);
    }
    
    render_text(equipped_item_info->description, stat_pos, color_light_brown, item_window.x + item_window.w - 20, fonts[font_alkhemikal]);
    
    if(comparing_items)
    {
#if MOONBREATH_SLOW
        v2u unique_id_pos = {name_pos.x, name_pos.y + 280};
        render_text("Unique ID: %u", unique_id_pos, color_orange, 0, fonts[font_dos_vga], equipped_item_inventory_slot->unique_id);
#endif
        
        v2u currently_equipped_pos = {name_pos.x, name_pos.y + 300};
        render_text("Currently Equipped", currently_equipped_pos, color_gray, 0, fonts[font_dos_vga]);
    }
    else
    {
#if MOONBREATH_SLOW
        v2u unique_id_pos = {name_pos.x, name_pos.y + 240};
        render_text("Unique ID: %u", unique_id_pos, color_orange, 0, fonts[font_dos_vga], equipped_item_inventory_slot->unique_id);
#endif
        
        v2u move_pos = {name_pos.x, name_pos.y + 280};
        v2u drop_pos = {name_pos.x, name_pos.y + 300};
        render_text("[M] Move", move_pos, color_white, 0, fonts[font_dos_vga]);
        render_text("[.] Drop", drop_pos, color_white, 0, fonts[font_dos_vga]);
    }
    
    return(item_window);
}

internal void
render_comparison_item_window(v2u pos, u32 selected_item_inventory_index, u32 equipped_item_inventory_index)
{
    v4u item_window = {0};
    item_window.w = textures.item_window.w;
    item_window.h = textures.item_window.h;
    item_window.x = pos.x - item_window.w - 6;
    item_window.y = pos.y;
    
    SDL_RenderCopy(game.renderer, textures.ui, (SDL_Rect *)&textures.item_window, (SDL_Rect *)&item_window);
    
    u32 equipped_item_info_index = get_inventory_info_index(equipped_item_inventory_index);
    u32 selected_item_info_index = get_inventory_info_index(selected_item_inventory_index);
    
    item_info_t *equipped_item_info = &item_info[equipped_item_info_index];
    item_info_t *selected_item_info = &item_info[selected_item_info_index];
    
    v2u name_pos = {item_window.x + 12, item_window.y + 12};
    render_text("%s", name_pos, color_white, 0, fonts[font_dos_vga], selected_item_info->name);
    
    v2u stat_pos = {name_pos.x, name_pos.y + 35};
    u32 stat_offset = 20;
    v4f stat_color = color_yellow;
    
    if(selected_item_info->type != type_consumable)
    {
        if(selected_item_info->stats.min_damage &&
           selected_item_info->stats.max_damage)
        {
            render_text("%u - %u Damage", stat_pos, color_white, 0, fonts[font_dos_vga], selected_item_info->stats.min_damage, selected_item_info->stats.max_damage);
            stat_pos.y += stat_offset;
        }
        
        if(equipped_item_info->stats.strength)
        {
            if(selected_item_info->stats.strength > equipped_item_info->stats.strength)
            {
                stat_color = color_green;
            }
            else if(selected_item_info->stats.strength < equipped_item_info->stats.strength)
            {
                stat_color = color_red;
            }
            else
            {
                stat_color = color_yellow;
            }
            
            render_text("+%u Strength", stat_pos, stat_color, 0, fonts[font_dos_vga], selected_item_info->stats.strength);
            stat_pos.y += stat_offset;
        }
        
        if(equipped_item_info->stats.defence)
        {
            if(selected_item_info->stats.defence > equipped_item_info->stats.defence)
            {
                stat_color = color_green;
            }
            else if(selected_item_info->stats.defence < equipped_item_info->stats.defence)
            {
                stat_color = color_red;
            }
            else
            {
                stat_color = color_yellow;
            }
            
            render_text("+%u Defence", stat_pos, stat_color, 0, fonts[font_dos_vga], selected_item_info->stats.defence);
            stat_pos.y += stat_offset;
        }
        
        if(equipped_item_info->stats.vitality)
        {
            if(selected_item_info->stats.vitality > equipped_item_info->stats.vitality)
            {
                stat_color = color_green;
            }
            else if(selected_item_info->stats.vitality < equipped_item_info->stats.vitality)
            {
                stat_color = color_red;
            }
            else
            {
                stat_color = color_yellow;
            }
            
            render_text("+%u Vitality", stat_pos, stat_color, 0, fonts[font_dos_vga], selected_item_info->stats.vitality);
            stat_pos.y += stat_offset;
        }
        
        v2u equip_pos = {name_pos.x, name_pos.y + 260};
        render_text("[E] Equip", equip_pos, color_white, 0, fonts[font_dos_vga]);
    }
    
    render_text(selected_item_info->description, stat_pos, color_light_brown, 0, fonts[font_alkhemikal]);
    
#if MOONBREATH_SLOW
    v2u unique_id = {name_pos.x, name_pos.y + 240};
    render_text("Unique ID: %u", unique_id, color_orange, 0, fonts[font_dos_vga], inventory.slots[selected_item_inventory_index].unique_id);
#endif
    
    v2u move_pos = {name_pos.x, name_pos.y + 280};
    v2u drop_pos = {name_pos.x, name_pos.y + 300};
    render_text("[M] Move", move_pos, color_white, 0, fonts[font_dos_vga]);
    render_text("[.] Drop", drop_pos, color_white, 0, fonts[font_dos_vga]);
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
    
    for(u32 i = 0; i < array_count(inventory.slots); ++i)
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
        v2u name_pos = {10, game.window_size.h - 152};
        v2u hp_text_pos = {10, game.window_size.h - 128};
        v2u hp_number_pos = {100, game.window_size.h - 128};
        v2u strength_pos = {10, game.window_size.h - 100};
        v2u defence_pos = {10, game.window_size.h - 82};
        v2u level_pos = {10, game.window_size.h - 64};
        v2u turn_pos = {10, game.window_size.h - 26};
        
        render_text(player.name, name_pos, color_white, 0, fonts[font_dos_vga]);
        render_text("HP", hp_text_pos, color_white, 0, fonts[font_dos_vga], player.hp, player.max_hp);
        render_text("%u (%u)", hp_number_pos, color_white, 0, fonts[font_dos_vga], player.hp, player.max_hp);
        render_text("Strength: %u", strength_pos, color_white, 0, fonts[font_dos_vga], player.strength);
        render_text("Defence: %u", defence_pos, color_white, 0, fonts[font_dos_vga], player.defence);
        render_text("Level: %u", level_pos, color_white, 0, fonts[font_dos_vga], player.level);
        render_text("Turn: %u", turn_pos, color_white, 0, fonts[font_dos_vga], game.turn);
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
                inventory_index < array_count(inventory.slots);
                ++inventory_index)
            {
                if(inventory.slots[inventory_index].id)
                {
                    ++new_item_count;
                    u32 inventory_item_info_index = get_inventory_info_index(inventory_index);
                    
                    v2u offset = v2u_from_index(inventory_index, INVENTORY_WIDTH);
                    v4u src = {tile_mul(item_info[inventory_item_info_index].tile.x), tile_mul(item_info[inventory_item_info_index].tile.y), 32, 32};
                    v4u dest = {first_slot.x + tile_mul(offset.x) + (offset.x * padding), first_slot.y + tile_mul(offset.y) + (offset.y * padding), 32, 32};
                    
                    if(inventory.moved_item_src_index != (u32)-1 &&
                       inventory_index == inventory.moved_item_src_index)
                    {
                        // NOTE(rami): Render the item with half opacity if it's being moved
                        SDL_SetTextureAlphaMod(textures.item_tileset.tex, 127);
                        SDL_RenderCopy(game.renderer, textures.item_tileset.tex, (SDL_Rect *)&src, (SDL_Rect *)&dest);
                        SDL_SetTextureAlphaMod(textures.item_tileset.tex, 255);
                    }
                    else
                    {
                        SDL_RenderCopy(game.renderer, textures.item_tileset.tex, (SDL_Rect *)&src, (SDL_Rect *)&dest);
                    }
                    
                    if(inventory.slots[inventory_index].is_equipped)
                    {
                        v2u glyph_pos = {dest.x + 3, dest.y + 2};
                        render_text("E", glyph_pos, color_white, 0, fonts[font_monaco]);
                    }
                    
                    if(inventory_index == index_from_v2u(inventory.current_slot, INVENTORY_WIDTH))
                    {
                        v2u item_win_pos = {inventory_window.x, inventory_window.y};
                        
                        u32_t slot_item_inventory_index = get_inventory_index_of_item_in_inventory_slot(inventory_index);
                        if(slot_item_inventory_index.success)
                        {
                            v4u item_win = render_item_window(item_win_pos, slot_item_inventory_index.value, 1);
                            
                            v2u comparison_item_win_pos = {item_win.x, item_win.y};
                            render_comparison_item_window(comparison_item_win_pos, inventory_index, slot_item_inventory_index.value);
                        }
                        else
                        {
                            render_item_window(item_win_pos, inventory_index, 0);
                        }
                    }
                }
            }
            
            // Render selected slot texture
            u32 selected_x_offset = tile_mul(inventory.current_slot.x) + (inventory.current_slot.x * padding);
            u32 selected_y_offset = tile_mul(inventory.current_slot.y) + (inventory.current_slot.y * padding);
            v4u slot_dest = {first_slot.x + selected_x_offset, first_slot.y + selected_y_offset, textures.inventory_selected_slot.w, textures.inventory_selected_slot.h};
            SDL_RenderCopy(game.renderer, textures.ui, (SDL_Rect *)&textures.inventory_selected_slot, (SDL_Rect *)&slot_dest);
            
            // Render the moving item at the current inventory position
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