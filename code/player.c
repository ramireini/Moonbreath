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
        for(u32 slot_index = 0;
            slot_index < (inventory.w * inventory.h);
            ++slot_index)
        {
            if(inventory.slots[slot_index])
            {
                item_info_t *info = item_info_from_slot_index(slot_index);
                if(info->slot == slot_index &&
                   inventory.slots[slot_index] &&
                   inventory.slots[slot_index]->is_equipped)
                {
                    v4u src = {tile_mul(info->tile.x), tile_mul(info->tile.y), 32, 32};
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
    for(u32 monster_index = 0;
        monster_index < array_count(monsters);
        ++monster_index)
    {
        monster_t *monster = &monsters[monster_index];
        if(monster->id)
        {
            u32 monster_info_index = monster_info_index_from_monster_id(monster->id);
            monster_info_t *monster_info = &monster_information[monster_info_index];
            
            if(V2u_equal(player.new_pos, monster->pos))
            {
                char attack[64] = {0};
                get_player_attack_message(attack);
                
                u32 player_hit_chance = 15 + (player.dexterity / 2);
                u32 player_damage = 2;
                
                u32_t slot_index = get_equipped_item_slot_index(slot_main_hand);
                if(slot_index.success)
                {
                    item_info_t *info = item_info_from_slot_index(slot_index.value);
                    
                    player_hit_chance += info->accuracy;
                    player_hit_chance += inventory.slots[slot_index.value]->enchantment_level;
                    
                    printf("accuracy: %u\n", info->accuracy);
                    printf("enchantment_level: %u\n", inventory.slots[slot_index.value]->enchantment_level);
                }
                else
                {
                    player_hit_chance += 2;
                }
                
#if 1
                u32 roll = random_number(0, player_hit_chance);
                if(roll >= monster_info->evasion)
                {
                    add_log_message("You %s the %s for %u damage.", color_white, attack, monster_info->name, player_damage);
                    add_pop_text("%u", monster->pos, text_normal_attack, player_damage);
                    
                    monster->hp -= player_damage;
                    if((s32)monster->hp <= 0)
                    {
                        add_log_message("You killed the %s!", color_light_red, monster_info->name);
                        remove_monster(monster);
                    }
                }
                else
                {
                    add_log_message("The %s dodges your attack!", color_white, monster_info->name);
                }
                
                monster->in_combat = true;
                assert(player_damage);
#else
                // TODO(rami): Hit Test
                u32 hit_count = 0;
                u32 miss_count = 0;
                for(u32 i = 0; i < 100; ++i)
                {
                    printf("player_hit_chance: %u\n", player_hit_chance);
                    printf("monster evasion: %u\n", monster_info->evasion);
                    
                    u32 roll = random_number(0, player_hit_chance);
                    if(roll >= monster_info->evasion)
                    {
                        ++hit_count;
                    }
                    else
                    {
                        ++miss_count;
                    }
                }
                
                printf("hit_count: %u\n", hit_count);
                printf("miss_count: %u\n\n", miss_count);
#endif
                
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

internal b32
update_player(input_state_t *keyboard)
{
    b32 result = false;
    b32 update_player = false;
    
#if MOONBREATH_SLOW
    if(is_input_valid(&keyboard[key_debug_fov]))
    {
        debug_fov = !debug_fov;
        update_player = true;
    }
    else if(is_input_valid(&keyboard[key_debug_player_traversable_check]))
    {
        debug_player_traversable = !debug_player_traversable;
        update_player = true;
    }
    // NOTE(rami): We need to check this manually
    // so that it works as an expected toggle.
    else if(keyboard[key_debug_has_been_up_check].is_down &&
            keyboard[key_debug_has_been_up_check].has_been_up)
    {
        keyboard[key_debug_has_been_up_check].has_been_up = false;
        debug_has_been_up = !debug_has_been_up;
        update_player = true;
    }
    else
#endif
    
        if(is_input_valid(&keyboard[key_inventory]))
    {
        inventory.is_open = !inventory.is_open;
        inventory.current = V2u(0, 0);
        
        inventory.is_item_being_moved = false;
        inventory.moving_item_src_index = (u32)-1;
        inventory.moving_item_dest_index = (u32)-1;
    }
    else if(inventory.is_open)
    {
        if(is_input_valid(&keyboard[key_move_up]))
        {
            if(inventory.current.y > 0)
            {
                --inventory.current.y;
            }
            else
            {
                inventory.current.y = inventory.h - 1;
            }
        }
        else if(is_input_valid(&keyboard[key_move_down]))
        {
            if((inventory.current.y + 1) < inventory.h)
            {
                ++inventory.current.y;
            }
            else
            {
                inventory.current.y = 0;
            }
        }
        else if(is_input_valid(&keyboard[key_move_left]))
        {
            if(inventory.current.x > 0)
            {
                --inventory.current.x;
            }
            else
            {
                inventory.current.x = inventory.w - 1;
            }
        }
        else if(is_input_valid(&keyboard[key_move_right]))
        {
            if((inventory.current.x + 1) < inventory.w)
            {
                ++inventory.current.x;
            }
            else
            {
                inventory.current.x = 0;
            }
        }
        else if(is_input_valid(&keyboard[key_drop]))
        {
            if(!inventory.is_item_being_moved)
            {
                remove_inventory_item(1);
            }
        }
        else if(is_input_valid(&keyboard[key_equip]))
        {
            if(!inventory.is_item_being_moved)
            {
                u32 slot_index = index_from_v2u(inventory.current, inventory.w);
                item_t *item = inventory.slots[slot_index];
                item_info_t *info = item_info_from_slot_index(slot_index);
                
                if(item)
                {
                    if(item->is_equipped)
                    {
                        unequip_item(item, info);
                    }
                    else
                    {
                        u32_t slot = get_equipped_item_slot_index(info->slot);
                        if(slot.success)
                        {
                            unequip_item(item, info);
                        }
                        
                        equip_item(item, info);
                    }
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
    }
    else
    {
        if(is_input_valid(&keyboard[key_move_up]))
        {
            player.new_pos = V2u(player.pos.x, player.pos.y - 1);
            update_player = true;
        }
        else if(is_input_valid(&keyboard[key_move_down]))
        {
            player.new_pos = V2u(player.pos.x, player.pos.y + 1);
            update_player = true;
        }
        else if(is_input_valid(&keyboard[key_move_left]))
        {
            player.new_pos = V2u(player.pos.x - 1, player.pos.y);
            update_player = true;
        }
        else if(is_input_valid(&keyboard[key_move_right]))
        {
            player.new_pos = V2u(player.pos.x + 1, player.pos.y);
            update_player = true;
        }
        else if(is_input_valid(&keyboard[key_move_up_left]))
        {
            player.new_pos = V2u(player.pos.x - 1, player.pos.y - 1);
            update_player = true;
        }
        else if(is_input_valid(&keyboard[key_move_up_right]))
        {
            player.new_pos = V2u(player.pos.x + 1, player.pos.y - 1);
            update_player = true;
        }
        else if(is_input_valid(&keyboard[key_move_down_left]))
        {
            player.new_pos = V2u(player.pos.x - 1, player.pos.y + 1);
            update_player = true;
        }
        else if(is_input_valid(&keyboard[key_move_down_right]))
        {
            player.new_pos = V2u(player.pos.x + 1, player.pos.y + 1);
            update_player = true;
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
                    add_log_message("You descend further.. Level %u.", color_cyan, dungeon.level + 1);
                    
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
        else if(is_input_valid(&keyboard[key_wait]))
        {
            game.time += 1.0f;
            result = true;
        }
    }
    
    if(update_player)
    {
#if MOONBREATH_SLOW
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
        
            if(is_inside_dungeon(player.new_pos))
        {
            b32 advance_time = false;
            
            if(!V2u_equal(player.pos, player.new_pos) &&
               is_dungeon_occupied(player.new_pos))
            {
                // TODO(rami): If we have other entity types than monsters,
                // we'll have to know who we're trying to interact with here.
                player_attack_monster();
                advance_time = true;
            }
            else
            {
                if(is_dungeon_tile(player.new_pos, tile_stone_door_closed))
                {
                    add_log_message("You open the door.", color_white);
                    set_dungeon_tile(player.new_pos, tile_stone_door_open);
                    
                    advance_time = true;
                }
                else if(is_dungeon_traversable(player.new_pos))
                {
                    set_dungeon_occupied(player.pos, false);
                    player.pos = player.new_pos;
                    set_dungeon_occupied(player.pos, true);
                    
                    advance_time = true;
                }
            }
            
            if(advance_time)
            {
                result = true;
                game.time += 1.0f;
            }
            
            // NOTE(rami): This is to keep the new_pos locked.
            player.new_pos = player.pos;
        }
    }
    
    return(result);
}