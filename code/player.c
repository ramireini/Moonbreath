internal void
add_player()
{
    player.size = V2u(32, 32);
    strcpy(player.name, "Zerker");
    player.max_hp = 10;
    player.hp = 10;
    player.damage = 1;
    player.speed = 1;
    player.level = 1;
    player.fov = 4;
    
    player.sprite.idle_frame_count = 1;
    player.sprite.current_frame = player.sprite.idle_start_frame;
    player.sprite.idle_frame_duration = 600;
}

internal void
update_player_alignment_points(v2u pos)
{
    if(player.sprite.current_frame.x == 0)
    {
        player.head_ap = V2u(pos.x, pos.y - 7);
        player.body_ap = V2u(pos.x - 1, pos.y + 1);
        player.legs_ap = V2u(pos.x, pos.y + 9);
        player.feet_ap = V2u(pos.x, pos.y + 13);
        player.amulet_ap = pos;
        player.first_hand_ap = V2u(pos.x, pos.y + 6);
        
        if(!player.sprite_flip)
        {
            player.second_hand_ap = V2u(pos.x + 5, pos.y + 6);
        }
        else
        {
            player.second_hand_ap = V2u(pos.x - 5, pos.y + 6);
        }
    }
    else if(player.sprite.current_frame.x == 1)
    {
        player.head_ap = V2u(pos.x, pos.y - 9);
        player.body_ap = V2u(pos.x - 1, pos.y - 1);
        player.legs_ap = V2u(pos.x, pos.y + 7);
        player.feet_ap = V2u(pos.x, pos.y + 13);
        player.amulet_ap = V2u(pos.x, pos.y - 2);
        player.first_hand_ap = V2u(pos.x, pos.y + 4);
        
        if(!player.sprite_flip)
        {
            player.second_hand_ap = V2u(pos.x + 5, pos.y + 4);
        }
        else
        {
            player.second_hand_ap = V2u(pos.x - 5, pos.y + 4);
        }
    }
}

internal v2u
get_player_alignment_point_from_slot(v2u pos, item_slot slot)
{
    v2u result = {0};
    
    switch(slot)
    {
        case slot_head: result = player.head_ap; break;
        case slot_body: result = player.body_ap; break;
        case slot_legs: result = player.legs_ap; break;
        case slot_feet: result = player.feet_ap; break;
        case slot_amulet: result = player.amulet_ap; break;
        case slot_first_hand: result = player.first_hand_ap; break;
        case slot_second_hand: result = player.second_hand_ap; break;
    }
    
    return(result);
}

internal void
render_player_items(v2u pos)
{
    for(u32 i = 1; i < slot_ring; ++i)
    {
        for(u32 k = 0; k < array_count(inventory.slots); ++k)
        {
            u32 item_info_index = inventory.slots[k].id - 1;
            if(item_info_index != -1 &&
               item_info[item_info_index].slot == i &&
               inventory.slots[k].id &&
               inventory.slots[k].equipped)
            {
                v2u item_pos = get_player_alignment_point_from_slot(pos, item_info[item_info_index].slot);
                
                v4u src = V4u(tile_mul(item_info[item_info_index].tile.x), tile_mul(item_info[item_info_index].tile.y), 32, 32);
                v4u dest = V4u(item_pos.x, item_pos.y, 32, 32);
                SDL_RenderCopyEx(game.renderer, textures[tex_wearable_item_tileset].tex, (SDL_Rect *)&src, (SDL_Rect *)&dest, 0, 0, player.sprite_flip);
                
                break;
            }
        }
    }
}

internal void
render_player()
{
    // TODO(rami): Replace second parameter
    update_sprite(&player.sprite, state_idle);
    
    v2u pos = get_game_position(player.pos);
    v4u src = V4u(tile_mul(player.sprite.current_frame.x), tile_mul(player.sprite.current_frame.y), player.size.w, player.size.h);
    v4u dest = V4u(pos.x, pos.y, player.size.w, player.size.h);
    
    update_player_alignment_points(pos);
    
    SDL_RenderCopyEx(game.renderer, textures[tex_sprite_sheet].tex, (SDL_Rect *)&src, (SDL_Rect *)&dest, 0, 0, player.sprite_flip);
    
    if(!is_item_slot_occupied(slot_head))
    {
        v4u hair_src = V4u(0, 0, 32, 32);
        v4u hair_dest = V4u(player.head_ap.x, player.head_ap.y, 32, 32);
        SDL_RenderCopyEx(game.renderer, textures[tex_player_parts].tex, (SDL_Rect *)&hair_src, (SDL_Rect *)&hair_dest, 0, 0, player.sprite_flip);
    }
    
    render_player_items(pos);
}

internal void
player_keypress(SDL_Scancode key)
{
    // TODO(rami): Switches
    if(key == SDL_SCANCODE_Q)
    {
        game.state = state_quit;
    }
    // TODO(rami): Debug
    else if(key == SDL_SCANCODE_P)
    {
        printf("player x: %u\n", player.pos.x);
        printf("player y: %u\n\n", player.pos.y);
        
        printf("player x mul: %u\n", tile_mul(player.pos.x));
        printf("player y mul: %u\n\n", tile_mul(player.pos.y));
    }
    else if(key == SDL_SCANCODE_I)
    {
        inventory.open = !inventory.open;
        // TODO(rami): Do we want inventory position to persist?
        inventory.pos = V2u(0, 0);
        
        inventory.item_is_moving = false;
        inventory.moved_item_src_index = -1;
        inventory.moved_item_dest_index = -1;
    }
    else if(inventory.open)
    {
        if(key == SDL_SCANCODE_K)
        {
            if(inventory.pos.y > 0)
            {
                --inventory.pos.y;
            }
            else
            {
                inventory.pos.y = INVENTORY_HEIGHT - 1;
            }
        }
        else if(key == SDL_SCANCODE_J)
        {
            if((inventory.pos.y + 1) < INVENTORY_HEIGHT)
            {
                ++inventory.pos.y;
            }
            else
            {
                inventory.pos.y = 0;
            }
        }
        else if(key == SDL_SCANCODE_H)
        {
            if(inventory.pos.x > 0)
            {
                --inventory.pos.x;
            }
            else
            {
                inventory.pos.x = INVENTORY_WIDTH - 1;
            }
        }
        else if(key == SDL_SCANCODE_L)
        {
            if((inventory.pos.x + 1) < INVENTORY_WIDTH)
            {
                ++inventory.pos.x;
            }
            else
            {
                inventory.pos.x = 0;
            }
        }
        else if(key == SDL_SCANCODE_D)
        {
            drop_item(1);
        }
        else if(key == SDL_SCANCODE_E)
        {
            toggle_equipped_item();
        }
        else if(key == SDL_SCANCODE_C)
        {
            consume_item();
        }
        else if(key == SDL_SCANCODE_M)
        {
            if(inventory.item_is_moving)
            {
                inventory.moved_item_dest_index = get_index_from_pos(inventory.pos, INVENTORY_WIDTH);
                if(inventory.moved_item_src_index != inventory.moved_item_dest_index)
                {
                    move_item(inventory.moved_item_src_index,
                              inventory.moved_item_dest_index);
                }
                
                inventory.item_is_moving = false;
                inventory.moved_item_src_index = -1;
                inventory.moved_item_dest_index = -1;
            }
            else
            {
                u32 index = get_index_from_pos(inventory.pos,
                                               INVENTORY_WIDTH);
                if(inventory.slots[index].id)
                {
                    inventory.item_is_moving = true;
                    inventory.moved_item_src_index = index;
                }
            }
        }
    }
    else if(!inventory.open)
    {
        if(key == SDL_SCANCODE_K)
        {
            player.new_pos = V2u(player.pos.x, player.pos.y - 1);
        }
        else if(key == SDL_SCANCODE_J)
        {
            player.new_pos = V2u(player.pos.x, player.pos.y + 1);
        }
        else if(key == SDL_SCANCODE_H)
        {
            player.new_pos = V2u(player.pos.x - 1, player.pos.y);
            player.sprite_flip = true;
        }
        else if(key == SDL_SCANCODE_L)
        {
            player.new_pos = V2u(player.pos.x + 1, player.pos.y);
            player.sprite_flip = false;
        }
        else if(key == SDL_SCANCODE_COMMA)
        {
            pick_up_item();
        }
        else if(key == SDL_SCANCODE_D)
        {
            if(is_tile(player.pos, tile_path_down))
            {
                level.current_level += 1;
                printf("%u\n", level.current_level);
                add_console_message("You descend further.. Level %u", color_orange, level.current_level);
                add_console_message("-----------------------------------------", color_orange);
                generate_level();
            }
        }
        else if(key == SDL_SCANCODE_U)
        {
            if(is_tile(player.pos, tile_path_up))
            {
                game.state = state_quit;
            }
        }
    }
    
    if(!inventory.open)
    {
        game.turn_changed = true;
    }
}

internal void
get_player_attack_message(char *message)
{
    u32 i = rand_num(1, 4);
    if(i == 1)
    {
        strcpy(message, "bash");
    }
    else if(i == 2)
    {
        strcpy(message, "slash");
    }
    else if(i == 3)
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
        
        add_pop_up_text("%u", player.pos, (player.size.w / 2) / 2, -8, text_heal, amount);
    }
    
    return(was_healed);
}

internal void
player_attack_monster()
{
    for(u32 i = 0; i < array_count(monsters); ++i)
    {
        if(monsters[i].type)
        {
            if(V2u_equal(player.new_pos, monsters[i].pos))
            {
                if(monsters[i].state)
                {
                    char attack[64] = {0};
                    get_player_attack_message(attack);
                    add_console_message("You %s the %s for %u damage", color_white, attack, monsters[i].name, player.damage);
                    add_pop_up_text("%u", monsters[i].pos, (monsters[i].size.w / 2) / 2, -8, text_normal_attack, player.damage);
                    
                    monsters[i].hp -= player.damage;
                    if((i32)monsters[i].hp <= 0)
                    {
                        add_console_message("You killed the %s!", color_red, monsters[i].name);
                        set_monster_sprite_state(i, state_died);
                    }
                    else
                    {
                        monsters[i].in_combat = true;
                    }
                }
                
                return;
            }
        }
    }
}

internal void
update_player()
{
    if(is_inside_level(player.new_pos))
    {
        // TODO(rami): Force move
#if 1
        set_occupied(player.pos, false);
        player.pos = player.new_pos;
        set_occupied(player.pos, true);
        return;
#endif
        
        if(is_traversable(player.new_pos))
        {
            if(is_occupied(player.new_pos))
            {
                // TODO(rami): If we have other entity types than monsters,
                // we'll have to know who we're trying to interact with here.
                player_attack_monster();
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
            if(is_tile(player.new_pos, tile_wall_stone))
            {
                add_console_message("A wall stops you", color_white);
            }
            else if(is_tile(player.new_pos, tile_door_closed))
            {
                add_console_message("You push the door open", color_white);
                set_tile(player.new_pos, tile_door_open);
            }
            else if(is_tile(player.new_pos, tile_path_up))
            {
                add_console_message("A path to the surface, [A]scend to flee the mountain", color_white);
            }
            else if(is_tile(player.new_pos, tile_path_down))
            {
                add_console_message("A path that leads further downwards.. [D]escend?", color_white);
            }
        }
        
        ++game.turn;
    }
}
