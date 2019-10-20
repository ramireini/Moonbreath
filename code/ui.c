internal void
add_console_text(char *str, v4u color, ...)
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
    v4u item_win = {0};
    item_win.w = textures[tex_inventory_item_win].w;
    item_win.h = textures[tex_inventory_item_win].h;
    item_win.x = pos.x - item_win.w - 6;
    item_win.y = pos.y;
    
    SDL_RenderCopy(game.renderer, textures[tex_inventory_item_win].tex, 0, (SDL_Rect *)&item_win);
    
    item_t *equipped_item_inventory_slot = &inventory.slots[equipped_item_inventory_index];
    u32 equipped_item_info_index = inventory.slots[equipped_item_inventory_index].id - 1;
    item_info_t *equipped_item_info = &item_info[equipped_item_info_index];
    
    v2u name = {item_win.x + 12, item_win.y + 12};
    render_text("%s", name, color_white, fonts[font_classic], equipped_item_info->name);
    
    v2u stat = {name.x, name.y + 30};
    u32 stat_offset = 20;
    
    if(equipped_item_info->type == type_weapon ||
       equipped_item_info->type == type_armor)
    {
        if(equipped_item_info->general.strength)
        {
            render_text("+%u Strength", stat, color_white, fonts[font_classic], equipped_item_info->general.strength);
            stat.y += stat_offset;
        }
        
        if(equipped_item_info->general.defence)
        {
            render_text("+%u Defence", stat, color_white, fonts[font_classic], equipped_item_info->general.defence);
            stat.y += stat_offset;
        }
        
        if(equipped_item_info->general.hp)
        {
            render_text("+%u HP", stat, color_white, fonts[font_classic], equipped_item_info->general.hp);
            stat.y += stat_offset;
        }
        
        if(!comparing_items)
        {
            if(equipped_item_inventory_slot->is_equipped)
            {
                v2u unequip = {name.x, name.y + 230};
                render_text("[E] Unequip", unequip, color_white, fonts[font_classic]);
            }
            else
            {
                v2u equip = {name.x, name.y + 230};
                render_text("[E] Equip", equip, color_white, fonts[font_classic]);
            }
        }
    }
    else if(equipped_item_info->type == type_consumable)
    {
        render_text(equipped_item_info->consumable.effect_text, stat, color_green, fonts[font_classic]);
        stat.y += stat_offset;
        
        if(equipped_item_inventory_slot->is_equipped)
        {
            v2u consume = {name.x, name.y + 230};
            render_text("[C]onsume", consume, color_white, fonts[font_classic]);
        }
    }
    
    render_text(equipped_item_info->description, stat, color_light_brown, fonts[font_cursive]);
    
    if(comparing_items)
    {
#if DEBUG
        v2u id = {name.x, name.y + 250};
        render_text("ID: %u", id, color_orange, fonts[font_classic], equipped_item_inventory_slot->unique_id);
#endif
        
        v2u currently_equipped = {name.x, name.y + 270};
        render_text("Currently Equipped", currently_equipped, color_grey, fonts[font_classic]);
    }
    else
    {
#if DEBUG
        v2u id = {name.x, name.y + 210};
        render_text("ID: %u", id, color_orange, fonts[font_classic], equipped_item_inventory_slot->unique_id);
#endif
        
        v2u move = {name.x, name.y + 250};
        v2u drop = {name.x, name.y + 270};
        render_text("[M] Move", move, color_white, fonts[font_classic]);
        render_text("[D] Drop", drop, color_white, fonts[font_classic]);
    }
    
    return(item_win);
}

internal item_slot_data_t
get_item_equip_slot_data(u32 current)
{
    item_slot_data_t result = {0};
    
    u32 current_item_index = inventory.slots[current].id - 1;
    
    for(u32 i = 0; i < array_count(inventory.slots); ++i)
    {
        u32 info_index = inventory.slots[i].id - 1;
        
        if(i != current &&
           info_index != (u32)-1 &&
           item_info[info_index].slot == item_info[current_item_index].slot &&
           inventory.slots[i].is_equipped)
        {
            result.occupied = true;
            result.index = i;
            break;
        }
    }
    
    return(result);
}

internal void
render_comparison_item_window(v2u pos, u32 equipped_item_inventory_index, u32 selected_item_inventory_index)
{
#if 0
    // TODO(rami):
    printf("equipped_item_inventory_index: %u\n", equipped_item_inventory_index);
    printf("selected_item_inventory_index: %u\n\n", selected_item_inventory_index);
#endif
    
    v4u item_win = {0};
    item_win.w = textures[tex_inventory_item_win].w;
    item_win.h = textures[tex_inventory_item_win].h;
    item_win.x = pos.x - item_win.w - 6;
    item_win.y = pos.y;
    
    SDL_RenderCopy(game.renderer, textures[tex_inventory_item_win].tex, 0, (SDL_Rect *)&item_win);
    
#if 0
    // TODO(rami):
    printf("equipped_item_id: %u\n", inventory.slots[equipped_item_inventory_index].id);
    u32 equipped_item_info_index = inventory.slots[equipped_item_inventory_index].id - 1;
    printf("equipped_item_name: %s\n", item_info[equipped_item_info_index].name);
    
    printf("selected_item_id: %u\n", inventory.slots[selected_item_inventory_index].id);
    u32 selected_item_info_index = inventory.slots[selected_item_inventory_index].id - 1;
    printf("selected_item_name: %s\n", item_info[selected_item_info_index].name);
#endif
    
    // TODO(rami):
    return;
    
#if 0
    u32 equipped_info_index = inventory.slots[equipped_item_index].id - 1;
    u32 selected_info_index = inventory.slots[selected_item_index].id - 1;
    item_info_t *equipped_info = &item_info[equipped_item_index];
    item_info_t *selected_info = &item_info[selected_item_index];
    
    v2u name = {item_win.x + 12, item_win.y + 12};
    render_text("%s", name, color_white, fonts[font_classic], equipped_info->name);
    
    if(equipped_info->type == type_weapon)
    {
        v2u strength_pos = {name.x, name.y + 20};
        v2u description_pos = {name.x, name.y + 40};
        
        v4u color = color_yellow;
        if(selected_info->general.strength > equipped_info->general.strength)
        {
            color = color_red;
        }
        else if(selected_info->general.strength < equipped_info->general.strength)
        {
            color = color_green;
        }
        
        render_text("%u Strength", strength_pos, color, fonts[font_classic], equipped_info->general.strength);
        render_text(equipped_info->description, description_pos, color_light_brown, fonts[font_cursive]);
    }
#if 0
    else if(equipped_info->type == type_armor)
    {
        v2u defence_pos = {name.x, name.y + 20};
        v2u description_pos = {name.x, name.y + 40};
        
        v4u color = color_yellow;
        if(selected_info->general.defence > equipped_info->general.defence)
        {
            color = color_red;
        }
        else if(selected_info->general.defence < equipped_info->general.defence)
        {
            color = color_green;
        }
        
        render_text("%u Defence", defence_pos, color, fonts[font_classic], item_info[equipped_info_index].general.defence);
        render_text(item_info[equipped_info_index].description, description_pos, color_light_brown, fonts[font_cursive]);
    }
    else if(equipped_info->type == type_consumable)
    {
        v2u effect_text_pos = {name.x, name.y + 20};
        v2u description_pos = {name.x, name.y + 40};
        v2u consume_pos = {name.x, name.y + 250};
        
        render_text(equipped_info->consumable.effect_text, effect_text_pos, color_green, fonts[font_classic]);
        render_text(equipped_info->description, description_pos, color_light_brown, fonts[font_cursive]);
        render_text("[C]onsume", consume_pos, color_white, fonts[font_classic]);
    }
#endif
    
#if DEBUG
    v2u debug = {name.x, name.y + 250};
    render_text("ID: %u", debug, color_orange, fonts[font_classic], inventory.slots[equipped_item_index].unique_id);
#endif
    
    v2u currently_equipped = {name.x, name.y + 270};
    render_text("Currently Equipped", currently_equipped, color_grey, fonts[font_classic]);
#endif
}

internal u32
get_inventory_pos_index()
{
    u32 result = (inventory.current_slot.y * INVENTORY_WIDTH) + inventory.current_slot.x;
    return(result);
}

internal void
set_and_render_inventory_slot_items(v4u inventory_win)
{
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
    
    v4u ring_src = V4u(224, 0, 32, 32);
    v4u ring_dest = V4u(inventory_win.x + 97, inventory_win.y + 151, 32, 32);
    
    for(u32 i = 0; i < array_count(inventory.slots); ++i)
    {
        if(inventory.slots[i].id && inventory.slots[i].is_equipped)
        {
            u32 info_index = inventory.slots[i].id - 1;
            
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
            }
        }
    }
    
    SDL_RenderCopy(game.renderer, textures[tex_item_tileset].tex, (SDL_Rect *)&head_src, (SDL_Rect *)&head_dest);
    SDL_RenderCopy(game.renderer, textures[tex_item_tileset].tex, (SDL_Rect *)&body_src, (SDL_Rect *)&body_dest);
    SDL_RenderCopy(game.renderer, textures[tex_item_tileset].tex, (SDL_Rect *)&legs_src, (SDL_Rect *)&legs_dest);
    SDL_RenderCopy(game.renderer, textures[tex_item_tileset].tex, (SDL_Rect *)&feet_src, (SDL_Rect *)&feet_dest);
    SDL_RenderCopy(game.renderer, textures[tex_item_tileset].tex, (SDL_Rect *)&first_hand_src, (SDL_Rect *)&first_hand_dest);
    SDL_RenderCopy(game.renderer, textures[tex_item_tileset].tex, (SDL_Rect *)&second_hand_src, (SDL_Rect *)&second_hand_dest);
    SDL_RenderCopy(game.renderer, textures[tex_item_tileset].tex, (SDL_Rect *)&amulet_src, (SDL_Rect *)&amulet_dest);
    SDL_RenderCopy(game.renderer, textures[tex_item_tileset].tex, (SDL_Rect *)&ring_src, (SDL_Rect *)&ring_dest);
}

internal void
render_inventory()
{
    v4u inventory_win = V4u(game.window_size.w - 324, game.window_size.h - 550, textures[tex_inventory_win].w, textures[tex_inventory_win].h);
    SDL_RenderCopy(game.renderer, textures[tex_inventory_win].tex, 0, (SDL_Rect *)&inventory_win);
    
    set_and_render_inventory_slot_items(inventory_win);
    
    u32 padding = 4;
    v2u first_slot = {inventory_win.x + 7, inventory_win.y + 193};
    u32 new_item_count = 0;
    
    for(u32 inventory_index = 0;
        inventory_index < array_count(inventory.slots);
        ++inventory_index)
    {
        if(inventory.slots[inventory_index].id)
        {
            ++new_item_count;
            u32 info_index = inventory.slots[inventory_index].id - 1;
            
            v2u offset = get_pos_from_index(inventory_index, INVENTORY_WIDTH);
            v4u src = V4u(tile_mul(item_info[info_index].tile.x), tile_mul(item_info[info_index].tile.y), 32, 32);
            v4u dest = V4u(first_slot.x + tile_mul(offset.x) + (offset.x * padding), first_slot.y + tile_mul(offset.y) + (offset.y * padding), 32, 32);
            
            if(inventory.moved_item_src_index != -1 &&
               (s32)inventory_index == inventory.moved_item_src_index)
            {
                // NOTE(rami): Render the item with half opacity if it's being moved
                SDL_SetTextureAlphaMod(textures[tex_item_tileset].tex, 128);
                SDL_RenderCopy(game.renderer, textures[tex_item_tileset].tex, (SDL_Rect *)&src, (SDL_Rect *)&dest);
                SDL_SetTextureAlphaMod(textures[tex_item_tileset].tex, 255);
            }
            else
            {
                SDL_RenderCopy(game.renderer, textures[tex_item_tileset].tex, (SDL_Rect *)&src, (SDL_Rect *)&dest);
            }
            
            if(inventory.slots[inventory_index].is_equipped)
            {
                v2u glyph_pos = {dest.x + 3, dest.y + 2};
                render_text("E", glyph_pos, color_grey, fonts[font_misc]);
            }
            
            if(inventory_index == get_inventory_pos_index())
            {
                // TODO(rami): Check if the slot is being equipped
                // if it is, render equipped item on the left of inventory
                // and render the currently viewed item on the left of the
                // equipped item with the comparison colors etc.
                
                // if it's not equipped then just render the currently viewed
                // item on the left of the inventory.
                
                // NOTE(rami): We got most of the stuff working,
                // the only thing to do now is the actually render all the stuff
                // in the comparison window, which means the stats and whether
                // they are better or not, as well as the equip, move, drop
                // messages.
                
#if 1
                v2u item_win_pos = {inventory_win.x, inventory_win.y};
                
                item_slot_data_t slot = get_item_equip_slot_data(inventory_index);
                if(slot.occupied)
                {
#if 0
                    // TODO(rami):
                    printf("index of the item equipped: %u\n", slot.index);
                    printf("index of the item selected: %u\n\n", inventory_index);
#endif
                    
                    v4u item_win = render_item_window(item_win_pos, slot.index, 1);
                    
                    v2u comparison_item_win_pos = {item_win.x, item_win.y};
                    render_comparison_item_window(comparison_item_win_pos, slot.index, inventory_index);
                }
                else
                {
                    render_item_window(item_win_pos, inventory_index, 0);
                }
#endif
                
                
                
#if 0
                v2u item_win_pos = {inventory_win.x, inventory_win.y};
                v4u item_win = render_item_window(item_win_pos, inventory_index);
                
                item_slot_data_t slot = get_item_equip_slot_data(inventory_index);
                if(slot.occupied)
                {
                    v2u comparison_item_win_pos = {item_win.x, item_win.y};
                    render_comparison_item_window(comparison_item_win_pos, inventory_index, slot.index);
                }
#endif
            }
        }
    }
    
    // Render selected slot texture
    u32 selected_x_offset = tile_mul(inventory.current_slot.x) + (inventory.current_slot.x * padding);
    u32 selected_y_offset = tile_mul(inventory.current_slot.y) + (inventory.current_slot.y * padding);
    v4u slot_dest = V4u(first_slot.x + selected_x_offset, first_slot.y + selected_y_offset, textures[tex_inventory_selected_item].w, textures[tex_inventory_selected_item].h);
    SDL_RenderCopy(game.renderer, textures[tex_inventory_selected_item].tex, 0, (SDL_Rect *)&slot_dest);
    
    // Render the moving item at the current inventory position
    if(inventory.item_is_being_moved)
    {
        u32 info_index = inventory.slots[inventory.moved_item_src_index].id - 1;
        v4u slot_src = V4u(tile_mul(item_info[info_index].tile.x), tile_mul(item_info[info_index].tile.y), 32, 32);
        SDL_RenderCopy(game.renderer, textures[tex_item_tileset].tex, (SDL_Rect *)&slot_src, (SDL_Rect *)&slot_dest);
    }
    
    inventory.item_count = new_item_count;
}

internal void
render_health_bar()
{
    v4u hp_bar_outside = V4u(38, game.window_size.h - 132, textures[tex_health_bar_outside].w, textures[tex_health_bar_outside].h);
    SDL_RenderCopy(game.renderer, textures[tex_health_bar_outside].tex, 0, (SDL_Rect *)&hp_bar_outside);
    
    // NOTE(rami): Normalize current health value (range of 0 - 1),
    // multiply by hp bar width to get a value between 0 and hp bar width
    // with the same ratio as the original value
    u32 hp_bar_inside_w = 0;
    if(player.hp > 0)
    {
        hp_bar_inside_w = ((f32)player.hp / (f32)player.max_hp) * 200.0f;
    }
    
    v4u hp_bar_inside_src = V4u(0, 0, hp_bar_inside_w, textures[tex_health_bar_inside].h);
    v4u hp_bar_inside_dest = V4u(40, game.window_size.h - 130, hp_bar_inside_w, 20);
    SDL_RenderCopy(game.renderer, textures[tex_health_bar_inside].tex, (SDL_Rect *)&hp_bar_inside_src, (SDL_Rect *)&hp_bar_inside_dest);
}

internal void
render_player_stats()
{
    v2u name_pos = {10, game.window_size.h - 152};
    v2u hp_pos = {10, game.window_size.h - 128};
    v2u hp_pos_actual = {100, game.window_size.h - 128};
    v2u strength_pos = {10, game.window_size.h - 100};
    v2u defence_pos = {10, game.window_size.h - 82};
    v2u level_pos = {10, game.window_size.h - 64};
    v2u turn_pos = {10, game.window_size.h - 26};
    
    render_text(player.name, name_pos, color_white, fonts[font_classic]);
    render_text("HP", hp_pos, color_white, fonts[font_classic], player.hp, player.max_hp);
    render_text("%u (%u)", hp_pos_actual, color_white, fonts[font_classic], player.hp, player.max_hp);
    render_text("Strength: %u", strength_pos, color_white, fonts[font_classic], player.strength);
    render_text("Defence: %u", defence_pos, color_white, fonts[font_classic], player.defence);
    render_text("Level: %u", level_pos, color_white, fonts[font_classic], player.level);
    render_text("Turn: %u", turn_pos, color_white, fonts[font_classic], game.turn);
}

internal void
render_console_text()
{
    v2u msg_pos = {396, game.window_size.h - 152};
    u32 msg_offset = 18;
    
    for(u32 i = 0; i < array_count(console_texts); ++i)
    {
        if(console_texts[i].str[0])
        {
            render_text(console_texts[i].str, msg_pos, console_texts[i].color, fonts[font_classic]);
            msg_pos.y += msg_offset;
        }
    }
}

internal void
render_ui()
{
    v4u bottom_rect = V4u(0, game.window_size.h - game.console_size.h, textures[tex_interface_bottom_win].w, textures[tex_interface_bottom_win].h);
    SDL_RenderCopy(game.renderer, textures[tex_interface_bottom_win].tex, 0, (SDL_Rect *)&bottom_rect);
    
    render_health_bar();
    render_player_stats();
    render_console_text();
}