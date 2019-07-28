internal void
add_console_message(char *msg, v4u color, ...)
{
    char msg_final[256];
    
    va_list arg_list;
    va_start(arg_list, color);
    vsnprintf(msg_final, sizeof(msg_final), msg, arg_list);
    va_end(arg_list);
    
    for(u32 i = 0; i < CONSOLE_MESSAGE_COUNT; ++i)
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
    
    for(u32 i = 1; i < CONSOLE_MESSAGE_COUNT; ++i)
    {
        strcpy(console_message[i - 1].msg, console_message[i].msg);
        console_message[i - 1].color = console_message[i].color;
    }
    
    strcpy(console_message[CONSOLE_MESSAGE_COUNT - 1].msg, msg_final);
    console_message[CONSOLE_MESSAGE_COUNT - 1].color = color;
}

internal v4u
render_item_window(v2u pos, u32 item_index)
{
    u32 info_index = inventory.slot[item_index].id - 1;
    
    v4u item_win = {0};
    item_win.w = 250;
    item_win.h = 307;
    item_win.x = pos.x - item_win.w - 4;
    item_win.y = pos.y;
    
    SDL_RenderCopy(game.renderer, texture[tex_inventory_item_win],
                   0, (SDL_Rect *)&item_win);
    
    // Render item info
    v2u start_pos = V2u(item_win.x + 12, item_win.y + 12);
    
    v2u name_pos = start_pos;
    render_text("%s", name_pos, color_white, font[font_classic], item_info[info_index].name);
    
    if(item_info[info_index].category == category_weapon)
    {
        v2u damage_pos = V2u(start_pos.x, start_pos.y + 20);
        v2u description_pos = V2u(start_pos.x, start_pos.y + 40);
        
        render_text("%d Damage", damage_pos, color_white, font[font_classic],
                    item_info[info_index].damage);
        render_text(item_info[info_index].description, description_pos, color_brown,
                    font[font_cursive]);
        
        if(inventory.slot[item_index].equipped)
        {
            v2u unequip_pos = V2u(start_pos.x, start_pos.y + 250);
            render_text("[E] Unequip", unequip_pos, color_white, font[font_classic]);
        }
        else
        {
            v2u equip_pos = V2u(start_pos.x, start_pos.y + 250);
            render_text("[E] Equip", equip_pos, color_white, font[font_classic]);
        }
    }
    else if(item_info[info_index].category == category_armor)
    {
        v2u armor_pos = V2u(start_pos.x, start_pos.y + 20);
        v2u description_pos = V2u(start_pos.x, start_pos.y + 40);
        
        render_text("%d Armor", armor_pos, color_white, font[font_classic],
                    item_info[info_index].armor);
        render_text(item_info[info_index].description, description_pos, color_brown,
                    font[font_cursive]);
        
        if(inventory.slot[item_index].equipped)
        {
            v2u unequip_pos = V2u(start_pos.x, start_pos.y + 250);
            render_text("[E] Unequip", unequip_pos, color_white, font[font_classic]);
        }
        else
        {
            v2u equip_pos = V2u(start_pos.x, start_pos.y + 250);
            render_text("[E] Equip", equip_pos, color_white, font[font_classic]);
        }
    }
    else if(item_info[info_index].category == category_consumable)
    {
        v2u use_pos = V2u(start_pos.x, start_pos.y + 20);
        v2u description_pos = V2u(start_pos.x, start_pos.y + 40);
        v2u consume_pos = V2u(start_pos.x, start_pos.y + 250);
        
        render_text(item_info[info_index].use, use_pos, color_green, font[font_classic]);
        render_text(item_info[info_index].description, description_pos, color_brown,
                    font[font_cursive]);
        render_text("[C]onsume", consume_pos, color_white, font[font_classic]);
    }
    
    v2u drop_pos = V2u(start_pos.x, start_pos.y + 270);
    render_text("[D] Drop", drop_pos, color_white, font[font_classic]);
    
#if MOONBREATH_DEBUG
    v2u debug_pos = V2u(start_pos.x, start_pos.y + 230);
    render_text("ID: %d", debug_pos, color_orange, font[font_classic],
                inventory.slot[item_index].unique_id);
#endif
    
    return(item_win);
}

internal item_slot_data_t
get_item_equip_slot_data(u32 current)
{
    item_slot_data_t result = {0};
    
    u32 current_item_index = inventory.slot[current].id - 1;
    
    for(u32 i = 0; i < INVENTORY_SLOT_COUNT; ++i)
    {
        u32 i_info_index = inventory.slot[i].id - 1;
        
        if(i != current &&
           i_info_index != -1 &&
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

internal u32
compare_stat(u32 first, u32 second)
{
    u32 result = 0;
    
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

internal void
render_comparison_item_window(v2u pos, u32 selected_item, u32 equipped_item)
{
    u32 equipped_item_info_index = inventory.slot[equipped_item].id - 1;
    u32 selected_item_info_index = inventory.slot[selected_item].id - 1;
    
    v4u item_win = {0};
    item_win.w = 250;
    item_win.h = 307;
    item_win.x = pos.x - item_win.w - 4;
    item_win.y = pos.y;
    
    SDL_RenderCopy(game.renderer, texture[tex_inventory_item_win],
                   0, (SDL_Rect *)&item_win);
    
    // Render item info
    v2u start_pos = V2u(item_win.x + 12, item_win.y + 12);
    
    v2u name_pos = start_pos;
    render_text("%s", name_pos, color_white, font[font_classic], item_info[equipped_item_info_index].name);
    
    if(item_info[equipped_item_info_index].category == category_weapon)
    {
        v2u damage_pos = V2u(start_pos.x, start_pos.y + 20);
        v2u description_pos = V2u(start_pos.x, start_pos.y + 40);
        
        v4u color = color_white;
        u32 result = compare_stat(item_info[selected_item_info_index].damage,
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
        v2u armor_pos = V2u(start_pos.x, start_pos.y + 20);
        v2u description_pos = V2u(start_pos.x, start_pos.y + 40);
        
        v4u color = color_white;
        u32 result = compare_stat(item_info[selected_item_info_index].armor,
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
        v2u use_pos = V2u(start_pos.x, start_pos.y + 20);
        v2u description_pos = V2u(start_pos.x, start_pos.y + 40);
        v2u consume_pos = V2u(start_pos.x, start_pos.y + 250);
        
        render_text(item_info[equipped_item_info_index].use, use_pos, color_green, font[font_classic]);
        render_text(item_info[equipped_item_info_index].description, description_pos, color_brown,
                    font[font_cursive]);
        render_text("[C]onsume", consume_pos, color_white, font[font_classic]);
    }
    
    v2u currently_equipped_pos = V2u(start_pos.x, start_pos.y + 270);
    render_text("Currently Equipped", currently_equipped_pos, color_grey, font[font_classic]);
    
#if MOONBREATH_DEBUG
    v2u debug_pos = V2u(start_pos.x, start_pos.y + 250);
    render_text("ID: %d", debug_pos, color_orange, font[font_classic],
                inventory.slot[equipped_item].unique_id);
#endif
}

internal void
render_inventory()
{
    // Render inventory window
    v4u inventory_win = V4u(game.window_size.w - 324, game.window_size.h - 550,
                            298, 339);
    SDL_RenderCopy(game.renderer, texture[tex_inventory_win],
                   0, (SDL_Rect *)&inventory_win);
    
    // Set the default inventory slot example source and destination
    v4u head_src = V4u(0, 0, 32, 32);
    v4u head_dest = V4u(inventory_win.x + 133, inventory_win.y + 7, 32, 32);
    
    v4u body_src = V4u(32, 0, 32, 32);
    v4u body_dest = V4u(inventory_win.x + 133, inventory_win.y + 79, 32, 32);
    
    v4u legs_src = V4u(64, 0, 32, 32);
    v4u legs_dest = V4u(inventory_win.x + 133, inventory_win.y + 115, 32, 32);
    
    v4u feet_src = V4u(96, 0, 32, 32);
    v4u feet_dest = V4u(inventory_win.x + 133, inventory_win.y + 151, 32, 32);
    
    v4u first_hand_src = V4u(128, 0, 32, 32);
    v4u first_hand_dest = V4u(inventory_win.x + 97, inventory_win.y + 79, 32, 32);
    
    v4u second_hand_src = V4u(160, 0, 32, 32);
    v4u second_hand_dest = V4u(inventory_win.x + 169, inventory_win.y + 79, 32, 32);
    
    v4u amulet_src = V4u(192, 0, 32, 32);
    v4u amulet_dest = V4u(inventory_win.x + 133, inventory_win.y + 43, 32, 32);
    
    v4u first_ring_src = V4u(224, 0, 32, 32);
    v4u first_ring_dest = V4u(inventory_win.x + 97, inventory_win.y + 151, 32, 32);
    
    // If an item is equipped, replace the source with that items source
    for(u32 i = 0; i < INVENTORY_SLOT_COUNT; ++i)
    {
        if(inventory.slot[i].id && inventory.slot[i].equipped)
        {
            u32 info_index = inventory.slot[i].id - 1;
            
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
            }
        }
    }
    
    // Render inventory slot examples
    SDL_RenderCopy(game.renderer, texture[tex_item_tileset],
                   (SDL_Rect *)&head_src, (SDL_Rect *)&head_dest);
    SDL_RenderCopy(game.renderer, texture[tex_item_tileset],
                   (SDL_Rect *)&body_src, (SDL_Rect *)&body_dest);
    SDL_RenderCopy(game.renderer, texture[tex_item_tileset],
                   (SDL_Rect *)&legs_src, (SDL_Rect *)&legs_dest);
    SDL_RenderCopy(game.renderer, texture[tex_item_tileset],
                   (SDL_Rect *)&feet_src, (SDL_Rect *)&feet_dest);
    SDL_RenderCopy(game.renderer, texture[tex_item_tileset],
                   (SDL_Rect *)&first_hand_src, (SDL_Rect *)&first_hand_dest);
    SDL_RenderCopy(game.renderer, texture[tex_item_tileset],
                   (SDL_Rect *)&second_hand_src, (SDL_Rect *)&second_hand_dest);
    SDL_RenderCopy(game.renderer, texture[tex_item_tileset],
                   (SDL_Rect *)&amulet_src, (SDL_Rect *)&amulet_dest);
    SDL_RenderCopy(game.renderer, texture[tex_item_tileset],
                   (SDL_Rect *)&first_ring_src, (SDL_Rect *)&first_ring_dest);
    
    u32 padding = 4;
    v2u first_slot = V2u(inventory_win.x + 7, inventory_win.y + 193);
    
    // Render selected slot texture
    u32 selected_x_offset = tile_mul(inventory.x) + (inventory.x * padding);
    u32 selected_y_offset = tile_mul(inventory.y) + (inventory.y * padding);
    v4u selected = V4u(first_slot.x + selected_x_offset, first_slot.y + selected_y_offset,
                       32, 32);
    SDL_RenderCopy(game.renderer, texture[tex_inventory_selected_item],
                   0, (SDL_Rect *)&selected);
    
    u32 new_item_count = 0;
    
    for(u32 i = 0; i < INVENTORY_SLOT_COUNT; ++i)
    {
        if(inventory.slot[i].id)
        {
            ++new_item_count;
            
            u32 info_index = inventory.slot[i].id - 1;
            
            // Separate i to x, y for the inventory grid
            v2u offset = V2u(i, 0);
            
            if(i >= INVENTORY_WIDTH)
            {
                offset = V2u(i % INVENTORY_WIDTH,
                             i / INVENTORY_WIDTH);
            }
            
            v4u src = V4u(tile_mul(item_info[info_index].tile_x),
                          tile_mul(item_info[info_index].tile_y),
                          32, 32);
            
            v4u dest = V4u(first_slot.x + tile_mul(offset.x) + (offset.x * padding),
                           first_slot.y + tile_mul(offset.y) + (offset.y * padding),
                           32, 32);
            
            // Render item
            SDL_RenderCopy(game.renderer, texture[tex_item_tileset],
                           (SDL_Rect *)&src, (SDL_Rect *)&dest);
            
            // If the item is equipped, render a glyph to indicate that
            if(inventory.slot[i].equipped)
            {
                v2u glyph_pos = V2u(dest.x + 3, dest.y + 2);
                render_text("E", glyph_pos, color_grey, font[font_misc]);
            }
            
            if(i == ((inventory.y * INVENTORY_WIDTH) + inventory.x))
            {
                v4u item_win = render_item_window(V2u(inventory_win.x, inventory_win.y), i);
                
                item_slot_data_t slot = get_item_equip_slot_data(i);
                if(slot.occupied)
                {
                    render_comparison_item_window(V2u(item_win.x, item_win.y),
                                                  i, slot.index);
                }
            }
        }
    }
    
    inventory.item_count = new_item_count;
}

internal void
render_ui()
{
    v4u bottom_rect = V4u(0, game.window_size.h - game.console_size.h,
                          1280, 160);
    SDL_RenderCopy(game.renderer, texture[tex_interface_bottom_win],
                   0, (SDL_Rect *)&bottom_rect);
    
    v4u color = color_red;
    SDL_SetRenderDrawColor(game.renderer, color.r, color.g, color.b, color.a);
    
    // Normalize current health value (range of 0 - 1),
    // multiply by hp bar width to get a value between 0 and hp bar width
    // with the same ratio as the original value
    u32 hp_bar_inside_w = 0;
    if(player.hp > 0)
    {
        hp_bar_inside_w = ((f32)player.hp / (f32)player.max_hp) * 200.0f;
    }
    
    v4u hp_bar_outside = V4u(38, game.window_size.h - 132,
                             204, 24);
    SDL_RenderCopy(game.renderer, texture[tex_health_bar_outside],
                   0, (SDL_Rect *)&hp_bar_outside);
    
    v4u hp_bar_inside_src = V4u(0, 0,
                                hp_bar_inside_w, 20);
    v4u hp_bar_inside_dest = V4u(40, game.window_size.h - 130,
                                 hp_bar_inside_w, 20);
    SDL_RenderCopy(game.renderer, texture[tex_health_bar_inside],
                   (SDL_Rect *)&hp_bar_inside_src, (SDL_Rect *)&hp_bar_inside_dest);
    
    v2u name_pos = V2u(10, game.window_size.h - 152);
    v2u hp_pos = V2u(10, game.window_size.h - 128);
    v2u hp_pos_actual = V2u(100, game.window_size.h - 128);
    v2u damage_pos = V2u(10, game.window_size.h - 100);
    v2u armor_pos = V2u(10, game.window_size.h - 82);
    v2u level_pos = V2u(10, game.window_size.h - 64);
    v2u turn_pos = V2u(10, game.window_size.h - 26);
    
    render_text(player.name, name_pos, color_white, font[font_classic]);
    render_text("HP", hp_pos, color_white, font[font_classic], player.hp, player.max_hp);
    render_text("%d (%d)", hp_pos_actual, color_white, font[font_classic], player.hp, player.max_hp);
    render_text("Damage: %d", damage_pos, color_white, font[font_classic], player.damage);
    render_text("Armor: %d", armor_pos, color_white, font[font_classic], player.armor);
    render_text("Level: %d", level_pos, color_white, font[font_classic], player.level);
    render_text("Turn: %d", turn_pos, color_white, font[font_classic], game.turn);
    
    v2u msg_pos = V2u(396, game.window_size.h - 152);
    u32 msg_offset = 16;
    
    for(u32 i = 0; i < CONSOLE_MESSAGE_COUNT; ++i)
    {
        if(!str_cmp(console_message[i].msg, CONSOLE_MESSAGE_EMPTY))
        {
            render_text(console_message[i].msg, msg_pos, console_message[i].color, font[font_classic]);
            msg_pos.y += msg_offset;
        }
    }
}