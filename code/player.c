internal void
render_player()
{
    v2u player_game_pos = get_game_pos(player.pos);
    
    v4u src = {tile_mul(player.tile.x), tile_mul(player.tile.y), player.w, player.h};
    v4u dest = {player_game_pos.x, player_game_pos.y, player.w, player.h};
    SDL_RenderCopy(game.renderer, textures.sprite_sheet.tex, (SDL_Rect *)&src, (SDL_Rect *)&dest);
    
    // Render Player Items
    for(u32 slot_index = 1;
        slot_index < slot_total;
        ++slot_index)
    {
        for(u32 inventory_index = 0;
            inventory_index < (inventory.w * inventory.h);
            ++inventory_index)
        {
            if(inventory.slots[inventory_index].id)
            {
                u32 item_info_index = item_info_index_from_inventory_index(inventory_index);
                if(item_info[item_info_index].slot == slot_index &&
                   inventory.slots[inventory_index].id &&
                   inventory.slots[inventory_index].is_equipped)
                {
                    v4u src = {tile_mul(item_info[item_info_index].tile.x), tile_mul(item_info[item_info_index].tile.y), 32, 32};
                    v4u dest = {player_game_pos.x, player_game_pos.y, 32, 32};
                    SDL_RenderCopy(game.renderer, textures.wearable_item_tileset.tex, (SDL_Rect *)&src, (SDL_Rect *)&dest);
                    
                    break;
                }
            }
        }
    }
}

internal void
get_player_attack_message(char *message)
{
    u32 random = random_number(1, 4);
    if(random == 1)
    {
        strcpy(message, "bash");
    }
    else if(random == 2)
    {
        strcpy(message, "slash");
    }
    else if(random == 3)
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
                // that finds the currently equipped weapon.
                
                for(u32 inventory_index = 0;
                    inventory_index < (inventory.w * inventory.h);
                    ++inventory_index)
                {
                    u32 item_info_index = item_info_index_from_inventory_index(inventory_index);
                    item_info_t *info = &item_info[item_info_index];
                    
                    if(inventory.slots[inventory_index].id &&
                       inventory.slots[inventory_index].is_equipped &&
                       info->slot == slot_first_hand)
                    {
                        player_damage = random_number(info->stats.min_damage, info->stats.max_damage);
                        break;
                    }
                }
                
                assert(player_damage, "Player damage was zero.");
                
                add_log_message("You %s the %s for %u damage.", color_white, attack, monster->name, player_damage);
                add_pop_text("%u", monster->pos, text_normal_attack, player_damage);
                
                monster->hp -= player_damage;
                if((s32)monster->hp <= 0)
                {
                    add_log_message("You killed the %s!", color_red, monster->name);
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
is_input_valid(input_state_t *state)
{
    b32 result = false;
    
#if MOONBREATH_SLOW
    if(debug_has_been_up)
    {
        if(state->is_down)
        {
            result = true;
        }
    }
    else
#endif
    {
        if(state->is_down && state->has_been_up)
        {
            state->has_been_up = false;
            result = true;
        }
    }
    
    return(result);
}

// TODO(rami): We can probably make this faster and easier to read
// by using switches and having #if preprocessor statements getting
// rid of the debug stuff in is_player_input_valid() and process_events() plus
// wherever else they might be.
internal b32
process_player_input(input_state_t *keyboard)
{
    b32 result = true;
    
#if MOONBREATH_SLOW
    if(is_input_valid(&keyboard[key_debug_fov]))
    {
        debug_fov = !debug_fov;
    }
    else if(is_input_valid(&keyboard[key_debug_player_traversable_check]))
    {
        debug_player_traversable = !debug_player_traversable;
    }
    // NOTE(rami): We need to check this manually
    // so that it works as an expected toggle.
    else if(keyboard[key_debug_has_been_up_check].is_down &&
            keyboard[key_debug_has_been_up_check].has_been_up)
    {
        keyboard[key_debug_has_been_up_check].has_been_up = false;
        debug_has_been_up = !debug_has_been_up;
    }
    else
#endif
    
        if(is_input_valid(&keyboard[key_inventory]))
    {
        inventory.is_open = !inventory.is_open;
        inventory.current_slot = V2u(0, 0);
        
        inventory.item_is_being_moved = false;
        inventory.moved_item_src_index = (u32)-1;
        inventory.moved_item_dest_index = (u32)-1;
    }
    else if(inventory.is_open)
    {
        if(is_input_valid(&keyboard[key_move_up]))
        {
            if(inventory.current_slot.y > 0)
            {
                --inventory.current_slot.y;
            }
            else
            {
                inventory.current_slot.y = inventory.h - 1;
            }
        }
        else if(is_input_valid(&keyboard[key_move_down]))
        {
            if((inventory.current_slot.y + 1) < inventory.h)
            {
                ++inventory.current_slot.y;
            }
            else
            {
                inventory.current_slot.y = 0;
            }
        }
        else if(is_input_valid(&keyboard[key_move_left]))
        {
            if(inventory.current_slot.x > 0)
            {
                --inventory.current_slot.x;
            }
            else
            {
                inventory.current_slot.x = inventory.w - 1;
            }
        }
        else if(is_input_valid(&keyboard[key_move_right]))
        {
            if((inventory.current_slot.x + 1) < inventory.w)
            {
                ++inventory.current_slot.x;
            }
            else
            {
                inventory.current_slot.x = 0;
            }
        }
        else if(is_input_valid(&keyboard[key_drop]))
        {
            remove_inventory_item(1);
        }
        else if(is_input_valid(&keyboard[key_equip]))
        {
            u32 inventory_index = index_from_v2u(inventory.current_slot, inventory.w);
            if(inventory.slots[inventory_index].is_equipped)
            {
                unequip_item(inventory_index);
            }
            else
            {
                equip_slot_t slot = get_item_equip_slot_status(inventory_index);
                if(slot.has_an_item)
                {
                    unequip_item(slot.equipped_item_inventory_index);
                    equip_item(inventory_index);
                }
                else
                {
                    equip_item(inventory_index);
                }
            }
        }
        else if(is_input_valid(&keyboard[key_consume]))
        {
            consume_item();
        }
        else if(is_input_valid(&keyboard[key_move]))
        {
            move_item();
        }
        else
        {
            result = false;
        }
    }
    else
    {
        if(is_input_valid(&keyboard[key_move_up]))
        {
            player.new_pos = V2u(player.pos.x, player.pos.y - 1);
        }
        else if(is_input_valid(&keyboard[key_move_down]))
        {
            player.new_pos = V2u(player.pos.x, player.pos.y + 1);
        }
        else if(is_input_valid(&keyboard[key_move_left]))
        {
            player.new_pos = V2u(player.pos.x - 1, player.pos.y);
        }
        else if(is_input_valid(&keyboard[key_move_right]))
        {
            player.new_pos = V2u(player.pos.x + 1, player.pos.y);
        }
        else if(is_input_valid(&keyboard[key_move_up_left]))
        {
            player.new_pos = V2u(player.pos.x - 1, player.pos.y - 1);
        }
        else if(is_input_valid(&keyboard[key_move_up_right]))
        {
            player.new_pos = V2u(player.pos.x + 1, player.pos.y - 1);
        }
        else if(is_input_valid(&keyboard[key_move_down_left]))
        {
            player.new_pos = V2u(player.pos.x - 1, player.pos.y + 1);
        }
        else if(is_input_valid(&keyboard[key_move_down_right]))
        {
            player.new_pos = V2u(player.pos.x + 1, player.pos.y + 1);
        }
        else if(is_input_valid(&keyboard[key_pick_up]))
        {
            add_inventory_item();
        }
        else if(is_input_valid(&keyboard[key_ascend]))
        {
            if(is_dungeon_tile(player.pos, tile_stone_path_up))
            {
                game.state = state_exit;
            }
            else
            {
                add_log_message("There's nothing here that leads upwards.", color_white);
            }
        }
        else if(is_input_valid(&keyboard[key_descend]))
        {
            if(is_dungeon_tile(player.pos, tile_stone_path_down))
            {
                if(dungeon.level < MAX_DUNGEON_LEVEL)
                {
                    add_log_message("You descend further.. Level %u.", color_orange, dungeon.level + 1);
                    
                    ++dungeon.level;
                    generate_dungeon();
                }
                else
                {
                    game.state = state_exit;
                }
            }
            else
            {
                add_log_message("There's nothing here that leads downwards.", color_white);
            }
        }
        else
        {
            result = false;
        }
    }
    
    if(!inventory.is_open && result)
    {
        ++game.turn;
    }
    else
    {
        result = false;
    }
    
    return(result);
}

internal void
update_player(input_state_t *keyboard)
{
    //#if MOONBREATH_SLOW
#if 1
    if(debug_player_traversable)
    {
        if(is_inside_dungeon(player.new_pos))
        {
            set_dungeon_occupied(player.pos, false);
            player.pos = player.new_pos;
            set_dungeon_occupied(player.pos, true);
        }
    }
    else
#endif
    
        if(is_dungeon_traversable(player.new_pos))
    {
        if(!V2u_equal(player.pos, player.new_pos) &&
           is_dungeon_occupied(player.new_pos))
        {
            // TODO(rami): If we have other entity types than monsters,
            // we'll have to know who we're trying to interact with here.
            player_attack_monster();
        }
        else
        {
            set_dungeon_occupied(player.pos, false);
            player.pos = player.new_pos;
            set_dungeon_occupied(player.pos, true);
        }
    }
    else
    {
        if(is_dungeon_tile(player.new_pos, tile_stone_door_closed))
        {
            add_log_message("You open the door.", color_white);
            set_dungeon_tile(player.new_pos, tile_stone_door_open);
        }
    }
    
    // NOTE(rami): This is to keep the new_pos locked.
    player.new_pos = player.pos;
}