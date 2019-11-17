internal void
initialize_player()
{
    player.w = 32;
    player.h = 32;
    strcpy(player.name, "Name");
    player.max_hp = 10;
    player.hp = 8;
    player.move_speed = 1;
    player.fov = 6;
}

internal v2u
get_wearable_item_pos_from_slot(item_slot slot)
{
    v2u result = {0};
    
    switch(slot)
    {
        case slot_head: result = player.head_wear_pos; break;
        case slot_body: result = player.body_wear_pos; break;
        case slot_legs: result = player.legs_wear_pos; break;
        case slot_feet: result = player.feet_wear_pos; break;
        case slot_amulet: result = player.amulet_wear_pos; break;
        case slot_ring: result = player.ring_wear_pos; break;
        case slot_first_hand: result = player.first_hand_wear_pos; break;
        case slot_second_hand: result = player.second_hand_wear_pos; break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal void
update_wearable_item_positions(v2u pos)
{
    // TODO(rami): Once all of these have been filled out, delete this line.
    
    if(player.tile_flip)
    {
        player.head_wear_pos = V2u(pos.x, pos.y);
        player.body_wear_pos = V2u(pos.x, pos.y);
        player.legs_wear_pos = V2u(pos.x, pos.y);
        player.feet_wear_pos = V2u(pos.x, pos.y + 13);
        player.amulet_wear_pos = V2u(pos.x, pos.y);
        player.ring_wear_pos = V2u(pos.x + 6, pos.y + 2);
        player.first_hand_wear_pos = V2u(pos.x - 2, pos.y + 2);
        player.second_hand_wear_pos = V2u(pos.x, pos.y);
    }
    else
    {
        player.head_wear_pos = V2u(pos.x, pos.y);
        player.body_wear_pos = V2u(pos.x, pos.y);
        player.legs_wear_pos = V2u(pos.x, pos.y);
        player.feet_wear_pos = V2u(pos.x, pos.y + 13);
        player.amulet_wear_pos = V2u(pos.x, pos.y);
        player.ring_wear_pos = V2u(pos.x - 6, pos.y + 2);
        player.first_hand_wear_pos = V2u(pos.x + 2, pos.y + 2);
        player.second_hand_wear_pos = V2u(pos.x, pos.y);
    }
}

internal void
render_player()
{
    v2u player_game_pos = get_game_pos(player.pos);
    update_wearable_item_positions(player_game_pos);
    
    v4u src = {tile_mul(player.tile.x), tile_mul(player.tile.y), player.w, player.h};
    v4u dest = {player_game_pos.x, player_game_pos.y, player.w, player.h};
    SDL_RenderCopyEx(game.renderer, textures[tex_sprite_sheet].tex, (SDL_Rect *)&src, (SDL_Rect *)&dest, 0, 0, player.tile_flip);
    
    // TODO(rami): We want to probably have this functionality in the future,
    // if not then delete.
#if 0
    if(!is_item_slot_occupied(slot_head))
    {
        v4u hair_src = {0, 0, 32, 32};
        v4u hair_dest = {player.head_wear_pos.x, player.head_wear_pos.y, 32, 32};
        SDL_RenderCopyEx(game.renderer, textures[tex_player_parts].tex, (SDL_Rect *)&hair_src, (SDL_Rect *)&hair_dest, 0, 0, player.tile_flip);
    }
#endif
    
    { // Render Player Items
        for(u32 slot_index = 1;
            slot_index < slot_total;
            ++slot_index)
        {
            for(u32 inventory_index = 0;
                inventory_index < array_count(inventory.slots);
                ++inventory_index)
            {
                if(inventory.slots[inventory_index].id)
                {
                    u32 item_info_index = get_inventory_info_index(inventory_index);
                    if(item_info[item_info_index].slot == slot_index &&
                       inventory.slots[inventory_index].id &&
                       inventory.slots[inventory_index].is_equipped)
                    {
                        v2u item_pos = get_wearable_item_pos_from_slot(item_info[item_info_index].slot);
                        
                        v4u src = {tile_mul(item_info[item_info_index].tile.x), tile_mul(item_info[item_info_index].tile.y), 32, 32};
                        v4u dest = {item_pos.x, item_pos.y, 32, 32};
                        SDL_RenderCopyEx(game.renderer, textures[tex_wearable_item_tileset].tex, (SDL_Rect *)&src, (SDL_Rect *)&dest, 0, 0, player.tile_flip);
                        
                        break;
                    }
                }
            }
        }
    }
}


#if 0
// TODO(rami): Remove function.
internal void
player_keypress(SDL_Scancode key)
{
    if(key == SDL_SCANCODE_Q)
    {
        game.state = state_quit;
    }
    
#if 1
    else if(key == SDL_SCANCODE_1)
    {
        toggle_fov = !toggle_fov;
    }
    else if(key == SDL_SCANCODE_2)
    {
        for(u32 i = 0; i < array_count(monsters); ++i)
        {
            monster_t *monster = &monsters[i];
            if(monster->type)
            {
                monster->in_combat = !monster->in_combat;
            }
        }
    }
#endif
    
    else if(key == SDL_SCANCODE_I)
    {
        inventory.is_open = !inventory.is_open;
        inventory.current_slot = V2u(0, 0);
        
        inventory.item_is_being_moved = false;
        inventory.moved_item_src_index = (u32)-1;
        inventory.moved_item_dest_index = (u32)-1;
    }
    else if(inventory.is_open)
    {
        switch(key)
        {
            case SDL_SCANCODE_K:
            {
                if(inventory.current_slot.y > 0)
                {
                    --inventory.current_slot.y;
                }
                else
                {
                    inventory.current_slot.y = INVENTORY_HEIGHT - 1;
                }
            } break;
            
            case SDL_SCANCODE_J:
            {
                if((inventory.current_slot.y + 1) < INVENTORY_HEIGHT)
                {
                    ++inventory.current_slot.y;
                }
                else
                {
                    inventory.current_slot.y = 0;
                }
            } break;
            
            case SDL_SCANCODE_H:
            {
                if(inventory.current_slot.x > 0)
                {
                    --inventory.current_slot.x;
                }
                else
                {
                    inventory.current_slot.x = INVENTORY_WIDTH - 1;
                }
            } break;
            
            case SDL_SCANCODE_L:
            {
                if((inventory.current_slot.x + 1) < INVENTORY_WIDTH)
                {
                    ++inventory.current_slot.x;
                }
                else
                {
                    inventory.current_slot.x = 0;
                }
            } break;
            
            case SDL_SCANCODE_D:
            {
                remove_inventory_item(1);
            } break;
            
            case SDL_SCANCODE_E:
            {
                toggle_equipped_item();
            } break;
            
            case SDL_SCANCODE_C:
            {
                consume_item();
            } break;
            
            case SDL_SCANCODE_M:
            {
                if(inventory.item_is_being_moved)
                {
                    inventory.moved_item_dest_index = get_inventory_pos_index();
                    if(inventory.moved_item_src_index != inventory.moved_item_dest_index)
                    {
                        move_item_in_inventory(inventory.moved_item_src_index, inventory.moved_item_dest_index);
                    }
                    
                    inventory.item_is_being_moved = false;
                    inventory.moved_item_src_index = (u32)-1;
                    inventory.moved_item_dest_index = (u32)-1;
                }
                else
                {
                    u32 index = get_inventory_pos_index();
                    if(inventory.slots[index].id)
                    {
                        inventory.item_is_being_moved = true;
                        inventory.moved_item_src_index = index;
                    }
                }
            } break;
        }
    }
    else if(!inventory.is_open)
    {
        switch(key)
        {
            case SDL_SCANCODE_K:
            {
                player.new_pos = V2u(player.pos.x, player.pos.y - 1);
            } break;
            
            case SDL_SCANCODE_J:
            {
                player.new_pos = V2u(player.pos.x, player.pos.y + 1);
            } break;
            
            case SDL_SCANCODE_H:
            {
                player.new_pos = V2u(player.pos.x - 1, player.pos.y);
                player.tile_flip = true;
            } break;
            
            case SDL_SCANCODE_L:
            {
                player.new_pos = V2u(player.pos.x + 1, player.pos.y);
                player.tile_flip = false;
            } break;
            
            case SDL_SCANCODE_COMMA:
            {
                add_inventory_item();
            } break;
            
            case SDL_SCANCODE_D:
            {
                if(is_tile(player.pos, tile_stone_path_down))
                {
                    ++dungeon.level;
                    add_console_text("You descend further.. Level %u.", color_orange, dungeon.level);
                    add_console_text("-----------------------------------------", color_orange);
                    
                    generate_dungeon();
                }
            } break;
            
            case SDL_SCANCODE_U:
            {
                if(is_tile(player.pos, tile_stone_path_up))
                {
                    game.state = state_quit;
                }
            } break;
        }
    }
    
    if(!inventory.is_open)
    {
        game.turn_has_changed = true;
    }
}
#endif

internal void
get_player_attack_message(char *message)
{
    u32 value = rand_num(1, 4);
    if(value == 1)
    {
        strcpy(message, "bash");
    }
    else if(value == 2)
    {
        strcpy(message, "slash");
    }
    else if(value == 3)
    {
        strcpy(message, "pierce");
    }
    else
    {
        strcpy(message, "smash");
    }
}

internal b32
heal_player(u32 amount)
{
    b32 was_healed = false;
    
    if(player.hp < player.max_hp)
    {
        was_healed = true;
        
        player.hp += amount;
        if(player.hp > player.max_hp)
        {
            player.hp = player.max_hp;
        }
        
        add_pop_text("%u", player.pos, text_heal, amount);
    }
    
    return(was_healed);
}

internal void
player_attack_monster()
{
    for(u32 i = 0; i < array_count(monsters); ++i)
    {
        monster_t *monster = &monsters[i];
        if(monster->type)
        {
            if(V2u_equal(player.new_pos, monster->pos))
            {
                char attack[64] = {0};
                get_player_attack_message(attack);
                
                // TODO(rami): This is where player Strength etc.
                // would affect the damage of the worn weapon.
                
                // NOTE(rami): If player is wearing nothing to attack with
                // his default damage will be one.
                u32 player_damage = 1;
                
                // TODO(rami): We might want to pull this into a function
                // that finds the currently equipped weapon (or parameter)
                // slot item.
                
                for(u32 i = 0; i < array_count(inventory.slots); ++i)
                {
                    u32 item_info_index = get_inventory_info_index(i);
                    item_info_t *info = &item_info[item_info_index];
                    
                    if(inventory.slots[i].id &&
                       inventory.slots[i].is_equipped &&
                       info->slot == slot_first_hand)
                    {
                        player_damage = rand_num(info->stats.min_damage, info->stats.max_damage);
                        break;
                    }
                }
                
                assert(player_damage, "Player damage should not be zero.");
                
                add_console_text("You %s the %s for %u damage.", color_white, attack, monster->name, player_damage);
                add_pop_text("%u", monster->pos, text_normal_attack, player_damage);
                
                monster->hp -= player_damage;
                if((s32)monster->hp <= 0)
                {
                    add_console_text("You killed the %s!", color_red, monster->name);
                    remove_monster(monster);
                }
                else
                {
                    monster->in_combat = true;
                }
                
                return;
            }
        }
    }
}

internal b32
process_player_input(input_state_t *keyboard)
{
    b32 input_was_valid = true;
    
    if(keyboard[key_move_up].is_down &&
       keyboard[key_move_up].was_up)
    {
        keyboard[key_move_up].was_up = false;
        player.new_pos = V2u(player.pos.x, player.pos.y - 1);
    }
    else if(keyboard[key_move_down].is_down &&
            keyboard[key_move_down].was_up)
    {
        keyboard[key_move_down].was_up = false;
        player.new_pos = V2u(player.pos.x, player.pos.y + 1);
    }
    else if(keyboard[key_move_left].is_down &&
            keyboard[key_move_left].was_up)
    {
        keyboard[key_move_left].was_up = false;
        player.new_pos = V2u(player.pos.x - 1, player.pos.y);
        player.tile_flip = true;
    }
    else if(keyboard[key_move_right].is_down &&
            keyboard[key_move_right].was_up)
    {
        keyboard[key_move_right].was_up = false;
        player.new_pos = V2u(player.pos.x + 1, player.pos.y);
        player.tile_flip = false;
    }
    else
    {
        input_was_valid = false;
    }
    
    return(input_was_valid);
}

internal void
update_player(input_state_t *keyboard)
{
#if 0
    if(is_inside_dungeon(player.new_pos))
    {
        set_occupied(player.pos, false);
        player.pos = player.new_pos;
        set_occupied(player.pos, true);
    }
    
    return;
#endif
    
    if(is_traversable(player.new_pos))
    {
        if(!V2u_equal(player.pos, player.new_pos) &&
           is_occupied(player.new_pos))
        {
            // TODO(rami): If we have other entity types than monsters,
            // we'll have to know who we're trying to interact with here.
            player_attack_monster();
            player.new_pos = player.pos;
        }
        else
        {
            set_occupied(player.pos, false);
            player.pos = player.new_pos;
            set_occupied(player.pos, true);
        }
    }
    else
    {
        if(is_tile(player.new_pos, tile_stone_door_closed))
        {
            add_console_text("You open the door.", color_white);
            set_tile(player.new_pos, tile_stone_door_open);
        }
    }
    
    game.turn_has_changed = true;
    ++game.turn;
}