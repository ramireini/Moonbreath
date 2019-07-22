internal void
add_console_message(char *msg, v4i color, ...)
{
    char msg_final[256];
    
    va_list arg_list;
    va_start(arg_list, color);
    vsnprintf(msg_final, sizeof(msg_final), msg, arg_list);
    va_end(arg_list);
    
    for(i32 i = 0; i < CONSOLE_MESSAGE_COUNT; ++i)
    {
        if(str_cmp(console_message[i].msg, CONSOLE_MESSAGE_EMPTY))
        {
            strcpy(console_message[i].msg, msg_final);
            console_message[i].color = color;
            return;
        }
    }
    
    strcpy(console_message[0].msg, CONSOLE_MESSAGE_EMPTY);
    console_message[0].color = color_black;
    
    for(i32 i = 1; i < CONSOLE_MESSAGE_COUNT; ++i)
    {
        strcpy(console_message[i - 1].msg, console_message[i].msg);
        console_message[i - 1].color = console_message[i].color;
    }
    
    strcpy(console_message[CONSOLE_MESSAGE_COUNT - 1].msg, msg_final);
    console_message[CONSOLE_MESSAGE_COUNT - 1].color = color;
}

internal SDL_Rect
render_item_window(v2i pos, i32 item_index)
{
    i32 info_index = inventory.slot[item_index].id - 1;
    
    SDL_Rect item_win = {0};
    item_win.w = 250;
    item_win.h = 307;
    item_win.y = pos.y;
    item_win.x = pos.x - item_win.w - 4;
    
    SDL_RenderCopy(game.renderer, texture[tex_inventory_item_win], 0, &item_win);
    
    // Render item info
    v2i start_pos = V2i(item_win.x + 12, item_win.y + 12);
    
    v2i name_pos = start_pos;
    render_text("%s", name_pos, color_white, font[font_classic], item_info[info_index].name);
    
    if(item_info[info_index].category == category_weapon)
    {
        v2i damage_pos = V2i(start_pos.x, start_pos.y + 20);
        v2i description_pos = V2i(start_pos.x, start_pos.y + 40);
        
        render_text("%d Damage", damage_pos, color_white, font[font_classic],
                    item_info[info_index].damage);
        render_text(item_info[info_index].description, description_pos, color_brown,
                    font[font_cursive]);
        
        if(inventory.slot[item_index].equipped)
        {
            v2i unequip_pos = V2i(start_pos.x, start_pos.y + 250);
            render_text("[E] Unequip", unequip_pos, color_white, font[font_classic]);
        }
        else
        {
            v2i equip_pos = V2i(start_pos.x, start_pos.y + 250);
            render_text("[E] Equip", equip_pos, color_white, font[font_classic]);
        }
    }
    else if(item_info[info_index].category == category_armor)
    {
        v2i armor_pos = V2i(start_pos.x, start_pos.y + 20);
        v2i description_pos = V2i(start_pos.x, start_pos.y + 40);
        
        render_text("%d Armor", armor_pos, color_white, font[font_classic],
                    item_info[info_index].armor);
        render_text(item_info[info_index].description, description_pos, color_brown,
                    font[font_cursive]);
        
        if(inventory.slot[item_index].equipped)
        {
            v2i unequip_pos = V2i(start_pos.x, start_pos.y + 250);
            render_text("[E] Unequip", unequip_pos, color_white, font[font_classic]);
        }
        else
        {
            v2i equip_pos = V2i(start_pos.x, start_pos.y + 250);
            render_text("[E] Equip", equip_pos, color_white, font[font_classic]);
        }
    }
    else if(item_info[info_index].category == category_consumable)
    {
        v2i use_pos = V2i(start_pos.x, start_pos.y + 20);
        v2i description_pos = V2i(start_pos.x, start_pos.y + 40);
        v2i consume_pos = V2i(start_pos.x, start_pos.y + 250);
        
        render_text(item_info[info_index].use, use_pos, color_green, font[font_classic]);
        render_text(item_info[info_index].description, description_pos, color_brown,
                    font[font_cursive]);
        render_text("[C]onsume", consume_pos, color_white, font[font_classic]);
    }
    
    v2i drop_pos = V2i(start_pos.x, start_pos.y + 270);
    render_text("[D]rop", drop_pos, color_white, font[font_classic]);
    
#if MOONBREATH_DEBUG
    v2i debug_pos = V2i(start_pos.x, start_pos.y + 230);
    render_text("ID: %d", debug_pos, color_orange, font[font_classic],
                inventory.slot[item_index].unique_id);
#endif
    
    
    return(item_win);
}

internal item_slot_data_t
get_item_equip_slot_data(i32 current)
{
    item_slot_data_t result = {0};
    
    i32 current_item_index = inventory.slot[current].id - 1;
    
    for(i32 i = 0; i < INVENTORY_SLOT_COUNT; ++i)
    {
        i32 i_info_index = inventory.slot[i].id - 1;
        
        if(i != current &&
           item_info[i_info_index].slot == item_info[current_item_index].slot &&
           inventory.slot[i].equipped)
        {
            result.occupied = 1;
            result.index = i;
            break;
        }
    }
    
    return(result);
}

internal i32
compare_stat(i32 first, i32 second)
{
    i32 result = 0;
    
    if(first > second)
    {
        result = 1;
    }
    else if(second > first)
    {
        result = 2;
    }
    else if(first == second)
    {
        result = 3;
    }
    
    return(result);
}

// NOTE(rami): Do the actual comparison stuff
internal void
render_comparison_item_window(v2i pos, i32 selected_item, i32 equipped_item)
{
    i32 equipped_item_info_index = inventory.slot[equipped_item].id - 1;
    i32 selected_item_info_index = inventory.slot[selected_item].id - 1;
    
    SDL_Rect item_win = {0};
    item_win.w = 250;
    item_win.h = 307;
    item_win.y = pos.y;
    item_win.x = pos.x - item_win.w - 4;
    
    SDL_RenderCopy(game.renderer, texture[tex_inventory_item_win], 0, &item_win);
    
    // Render item info
    v2i start_pos = V2i(item_win.x + 12, item_win.y + 12);
    
    v2i name_pos = start_pos;
    render_text("%s", name_pos, color_white, font[font_classic], item_info[equipped_item_info_index].name);
    
    if(item_info[equipped_item_info_index].category == category_weapon)
    {
        v2i damage_pos = V2i(start_pos.x, start_pos.y + 20);
        v2i description_pos = V2i(start_pos.x, start_pos.y + 40);
        
        v4i color = color_white;
        i32 result = compare_stat(item_info[selected_item_info_index].damage,
                                  item_info[equipped_item_info_index].damage);
        if(result == 1)
        {
            color = color_red;
        }
        else if(result == 2)
        {
            color = color_green;
        }
        else if(result == 3)
        {
            color = color_yellow;
        }
        
        render_text("%d Damage", damage_pos, color, font[font_classic],
                    item_info[equipped_item_info_index].damage);
        render_text(item_info[equipped_item_info_index].description, description_pos, color_brown,
                    font[font_cursive]);
    }
    else if(item_info[equipped_item_info_index].category == category_armor)
    {
        v2i armor_pos = V2i(start_pos.x, start_pos.y + 20);
        v2i description_pos = V2i(start_pos.x, start_pos.y + 40);
        
        v4i color = color_white;
        i32 result = compare_stat(item_info[selected_item_info_index].armor,
                                  item_info[equipped_item_info_index].armor);
        if(result == 1)
        {
            color = color_red;
        }
        else if(result == 2)
        {
            color = color_green;
        }
        else if(result == 3)
        {
            color = color_yellow;
        }
        
        render_text("%d Armor", armor_pos, color, font[font_classic],
                    item_info[equipped_item_info_index].armor);
        render_text(item_info[equipped_item_info_index].description, description_pos, color_brown,
                    font[font_cursive]);
    }
    else if(item_info[equipped_item_info_index].category == category_consumable)
    {
        v2i use_pos = V2i(start_pos.x, start_pos.y + 20);
        v2i description_pos = V2i(start_pos.x, start_pos.y + 40);
        v2i consume_pos = V2i(start_pos.x, start_pos.y + 250);
        
        render_text(item_info[equipped_item_info_index].use, use_pos, color_green, font[font_classic]);
        render_text(item_info[equipped_item_info_index].description, description_pos, color_brown,
                    font[font_cursive]);
        render_text("[C]onsume", consume_pos, color_white, font[font_classic]);
    }
    
    v2i currently_equipped_pos = V2i(start_pos.x, start_pos.y + 270);
    render_text("Currently Equipped", currently_equipped_pos, color_grey, font[font_classic]);
    
#if MOONBREATH_DEBUG
    v2i debug_pos = V2i(start_pos.x, start_pos.y + 250);
    render_text("ID: %d", debug_pos, color_orange, font[font_classic],
                inventory.slot[equipped_item].unique_id);
#endif
}

internal void
render_inventory()
{
    // Render inventory window
    SDL_Rect inventory_win = {game.window_size.w - 324, game.window_size.h - 550, 298, 339};
    SDL_RenderCopy(game.renderer, texture[tex_inventory_win], 0, &inventory_win);
    
    // Render equipped items on equip slots
    SDL_Rect head_src = {0, 0, 32, 32};
    SDL_Rect head_dest = {inventory_win.x + 133, inventory_win.y + 7, 32, 32};
    
    SDL_Rect body_src = {32, 0, 32, 32};
    SDL_Rect body_dest = {inventory_win.x + 133, inventory_win.y + 79, 32, 32};
    
    SDL_Rect legs_src = {64, 0, 32, 32};
    SDL_Rect legs_dest = {inventory_win.x + 133, inventory_win.y + 115, 32, 32};
    
    SDL_Rect feet_src = {96, 0, 32, 32};
    SDL_Rect feet_dest = {inventory_win.x + 133, inventory_win.y + 151, 32, 32};
    
    SDL_Rect first_hand_src = {128, 0, 32, 32};
    SDL_Rect first_hand_dest = {inventory_win.x + 97, inventory_win.y + 79, 32, 32};
    
    SDL_Rect second_hand_src = {160, 0, 32, 32};
    SDL_Rect second_hand_dest = {inventory_win.x + 169, inventory_win.y + 79, 32, 32};
    
    SDL_Rect amulet_src = {192, 0, 32, 32};
    SDL_Rect amulet_dest = {inventory_win.x + 133, inventory_win.y + 43, 32, 32};
    
    SDL_Rect first_ring_src = {224, 0, 32, 32};
    SDL_Rect first_ring_dest = {inventory_win.x + 97, inventory_win.y + 151, 32, 32};
    
    // NOTE(rami): Need to look into cases such as:
    // you are wearing two rings and try to equip a third one,
    // this should replace the first ring always,
    // unless if the player holds some special key which indicates
    // that he wants to switch the second ring to the new third one.
    
    for(i32 i = 0; i < INVENTORY_SLOT_COUNT; ++i)
    {
        if(inventory.slot[i].id && inventory.slot[i].equipped)
        {
            i32 info_index = inventory.slot[i].id - 1;
            
            switch(item_info[info_index].slot)
            {
                case slot_head:
                {
                    head_src.x = tile_mul(item_info[info_index].tile_x);
                    head_src.y = tile_mul(item_info[info_index].tile_y);
                } break;
                
                case slot_body:
                {
                    body_src.x = tile_mul(item_info[info_index].tile_x);
                    body_src.y = tile_mul(item_info[info_index].tile_y);
                } break;
                
                case slot_legs:
                {
                    legs_src.x = tile_mul(item_info[info_index].tile_x);
                    legs_src.y = tile_mul(item_info[info_index].tile_y);
                } break;
                
                case slot_feet:
                {
                    feet_src.x = tile_mul(item_info[info_index].tile_x);
                    feet_src.y = tile_mul(item_info[info_index].tile_y);
                } break;
                
                case slot_first_hand:
                {
                    first_hand_src.x = tile_mul(item_info[info_index].tile_x);
                    first_hand_src.y = tile_mul(item_info[info_index].tile_y);
                } break;
                
                case slot_second_hand:
                {
                    second_hand_src.x = tile_mul(item_info[info_index].tile_x);
                    second_hand_src.y = tile_mul(item_info[info_index].tile_y);
                } break;
                
                case slot_amulet:
                {
                    amulet_src.x = tile_mul(item_info[info_index].tile_x);
                    amulet_src.y = tile_mul(item_info[info_index].tile_y);
                } break;
                
                case slot_ring:
                {
                    first_ring_src.x = tile_mul(item_info[info_index].tile_x);
                    first_ring_src.y = tile_mul(item_info[info_index].tile_y);
                } break;
                
                default: break;
            }
        }
    }
    
    SDL_SetTextureColorMod(texture[tex_item_tileset], 255, 255, 255);
    SDL_RenderCopy(game.renderer, texture[tex_item_tileset], &head_src, &head_dest);
    SDL_RenderCopy(game.renderer, texture[tex_item_tileset], &body_src, &body_dest);
    SDL_RenderCopy(game.renderer, texture[tex_item_tileset], &legs_src, &legs_dest);
    SDL_RenderCopy(game.renderer, texture[tex_item_tileset], &feet_src, &feet_dest);
    SDL_RenderCopy(game.renderer, texture[tex_item_tileset], &first_hand_src, &first_hand_dest);
    SDL_RenderCopy(game.renderer, texture[tex_item_tileset], &second_hand_src, &second_hand_dest);
    SDL_RenderCopy(game.renderer, texture[tex_item_tileset], &amulet_src, &amulet_dest);
    SDL_RenderCopy(game.renderer, texture[tex_item_tileset], &first_ring_src, &first_ring_dest);
    
    i32 padding = 4;
    i32 first_slot_x = inventory_win.x + 7;
    i32 first_slot_y = inventory_win.y + 193;
    
    // Render selected slot texture
    i32 selected_x_offset = tile_mul(inventory.x) + (inventory.x * padding);
    i32 selected_y_offset = tile_mul(inventory.y) + (inventory.y * padding);
    SDL_Rect selected = {first_slot_x + selected_x_offset, first_slot_y + selected_y_offset, 32, 32};
    SDL_RenderCopy(game.renderer, texture[tex_inventory_selected_item], 0, &selected);
    
    i32 new_item_count = 0;
    
    for(i32 i = 0; i < INVENTORY_SLOT_COUNT; ++i)
    {
        if(inventory.slot[i].id)
        {
            ++new_item_count;
            
            i32 info_index = inventory.slot[i].id - 1;
            
            // Separate i to x, y for the inventory grid
            i32 x_offset = i;
            i32 y_offset = 0;
            
            if(i >= INVENTORY_WIDTH)
            {
                x_offset = i % INVENTORY_WIDTH;
                y_offset = i / INVENTORY_WIDTH;
            }
            
            // Render item
            SDL_Rect src = {tile_mul(item_info[info_index].tile_x),
                tile_mul(item_info[info_index].tile_y),
                32, 32};
            
            SDL_Rect dest = {first_slot_x + tile_mul(x_offset) + (x_offset * padding),
                first_slot_y + tile_mul(y_offset) + (y_offset * padding),
                32, 32};
            
            SDL_RenderCopy(game.renderer, texture[tex_item_tileset], &src, &dest);
            
            if(i == ((inventory.y * INVENTORY_WIDTH) + inventory.x))
            {
                SDL_Rect item_win = render_item_window(V2i(inventory_win.x, inventory_win.y), i);
                
                item_slot_data_t data = get_item_equip_slot_data(i);
                if(data.occupied)
                {
                    render_comparison_item_window(V2i(item_win.x, item_win.y), i, data.index);
                }
            }
        }
    }
    
    inventory.item_count = new_item_count;
}

internal void
render_ui()
{
    SDL_Rect bottom_rect = {0, game.window_size.h - game.console_size.h, 1280, 160};
    SDL_RenderCopy(game.renderer, texture[tex_interface_bottom_win], 0, &bottom_rect);
    
    v4i color = color_red;
    SDL_SetRenderDrawColor(game.renderer, color.r, color.g, color.b, color.a);
    
    // Normalize current health value (range of 0 - 1),
    // multiply by hp bar width to get a value between 0 and hp bar width
    // with the same ratio as the original value
    i32 hp_bar_inside_w = 0;
    if(player.hp > 0)
    {
        hp_bar_inside_w = ((r32)player.hp / (r32)player.max_hp) * 200.0f;
    }
    
    SDL_Rect hp_bar_outside = {38, game.window_size.h - 132, 204, 24};
    SDL_RenderCopy(game.renderer, texture[tex_health_bar_outside], 0, &hp_bar_outside);
    
    SDL_Rect hp_bar_inside_src = {0, 0, hp_bar_inside_w, 20};
    SDL_Rect hp_bar_inside_dest = {40, game.window_size.h - 130, hp_bar_inside_w, 20};
    SDL_RenderCopy(game.renderer, texture[tex_health_bar_inside], &hp_bar_inside_src, &hp_bar_inside_dest);
    
    v2i name_pos = V2i(10, game.window_size.h - 152);
    v2i hp_pos = V2i(10, game.window_size.h - 128);
    v2i hp_pos_actual = V2i(100, game.window_size.h - 128);
    v2i damage_pos = V2i(10, game.window_size.h - 100);
    v2i armor_pos = V2i(10, game.window_size.h - 82);
    v2i level_pos = V2i(10, game.window_size.h - 64);
    v2i turn_pos = V2i(10, game.window_size.h - 26);
    
    render_text(player.name, name_pos, color_white, font[font_classic]);
    render_text("HP", hp_pos, color_white, font[font_classic], player.hp, player.max_hp);
    render_text("%d (%d)", hp_pos_actual, color_white, font[font_classic], player.hp, player.max_hp);
    render_text("Damage: %d", damage_pos, color_white, font[font_classic], player.damage);
    render_text("Armor: %d", armor_pos, color_white, font[font_classic], player.armor);
    render_text("Level: %d", level_pos, color_white, font[font_classic], player.level);
    render_text("Turn: %d", turn_pos, color_white, font[font_classic], game.turn);
    
    v2i msg_pos = V2i(396, game.window_size.h - 152);
    i32 msg_offset = 16;
    
    for(i32 i = 0; i < CONSOLE_MESSAGE_COUNT; ++i)
    {
        if(!str_cmp(console_message[i].msg, CONSOLE_MESSAGE_EMPTY))
        {
            render_text(console_message[i].msg, msg_pos, console_message[i].color, font[font_classic]);
            msg_pos.y += msg_offset;
        }
    }
}