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
    
    player.sprite.current_frame = player.sprite.start_frame;
    player.sprite.frame_count = 2;
    player.sprite.frame_duration = 500;
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
    for(i32 i = 1; i < slot_ring; ++i)
    {
        for(i32 k = 0; k < INVENTORY_SLOT_COUNT; ++k)
        {
            u32 item_info_index = inventory.slot[k].id;
            if(item_info[item_info_index].slot == i && inventory.slot[k].id &&
               inventory.slot[k].equipped)
            {
                SDL_Rect src = {tile_mul(item_info[item_info_index].tile_x),
                    tile_mul(item_info[item_info_index].tile_y),
                    32, 32};
                
                v2u item_pos = get_player_alignment_point_from_slot(pos, item_info[item_info_index].slot);
                SDL_Rect dest = {item_pos.x, item_pos.y, 32, 32};
                
                SDL_RenderCopyEx(game.renderer, texture[tex_wearable_item_tileset], &src, &dest,
                                 0, 0, player.sprite_flip);
                
                break;
            }
        }
    }
}

internal void
render_player()
{
    update_sprite(&player.sprite);
    
    SDL_Rect src = {tile_mul(player.sprite.current_frame.x),
        tile_mul(player.sprite.current_frame.y),
        player.size.w, player.size.h};
    
    v2u pos = get_game_position(player.pos);
    SDL_Rect dest = {pos.x, pos.y, player.size.w, player.size.h};
    
    update_player_alignment_points(pos);
    
    // TODO(rami):
    //if(is_lit(player.pos))
    {
        //v4i color = get_color_from_light_value(player.pos);
        //SDL_SetTextureColorMod(texture[tex_sprite_sheet], color.r, color.g, color.b);
        SDL_SetTextureColorMod(texture[tex_sprite_sheet], 255, 255, 255);
        SDL_RenderCopyEx(game.renderer, texture[tex_sprite_sheet], &src, &dest, 0, 0, player.sprite_flip);
        
        if(!is_item_slot_occupied(slot_head))
        {
            //SDL_SetTextureColorMod(texture[tex_player_parts], color.r, color.g, color.b);
            
            SDL_Rect hair_src = {0, 0, 32, 32};
            SDL_Rect hair_dest = {player.head_ap.x, player.head_ap.y, 32, 32};
            
            SDL_RenderCopyEx(game.renderer, texture[tex_player_parts], &hair_src, &hair_dest, 0, 0, player.sprite_flip);
        }
        
        render_player_items(pos);
    }
}

internal void
player_keypress(SDL_Scancode key)
{
    if(key == SDL_SCANCODE_Q)
    {
        game.state = state_quit;
    }
    // TODO(rami): 
    else if(key == SDL_SCANCODE_P)
    {
        printf("player x: %d\n", player.pos.x);
        printf("player y: %d\n\n", player.pos.y);
        
        printf("player x mul: %d\n", tile_mul(player.pos.x));
        printf("player y mul: %d\n\n", tile_mul(player.pos.y));
    }
    else if(key == SDL_SCANCODE_I)
    {
        inventory.x = 0;
        inventory.y = 0;
        inventory.open = !inventory.open;
    }
    // NOTE(rami):
    else if(key == SDL_SCANCODE_F)
    {
        monster[0].in_combat = !monster[0].in_combat;
    }
    else if(inventory.open)
    {
        if(key == SDL_SCANCODE_K)
        {
            if((inventory.y - 1) > -1)
            {
                --inventory.y;
            }
            else
            {
                inventory.y = INVENTORY_HEIGHT - 1;
            }
        }
        else if(key == SDL_SCANCODE_J)
        {
            if((inventory.y + 1) < INVENTORY_HEIGHT)
            {
                ++inventory.y;
            }
            else
            {
                inventory.y = 0;
            }
        }
        else if(key == SDL_SCANCODE_H)
        {
            if((inventory.x - 1) > -1)
            {
                --inventory.x;
            }
            else
            {
                inventory.x = INVENTORY_WIDTH - 1;
            }
        }
        else if(key == SDL_SCANCODE_L)
        {
            if((inventory.x + 1) < INVENTORY_WIDTH)
            {
                ++inventory.x;
            }
            else
            {
                inventory.x = 0;
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
            player.sprite_flip = 1;
        }
        else if(key == SDL_SCANCODE_L)
        {
            player.new_pos = V2u(player.pos.x + 1, player.pos.y);
            player.sprite_flip = 0;
        }
        else if(key == SDL_SCANCODE_Y)
        {
            player.new_pos = V2u(player.pos.x - 1, player.pos.y - 1);
            player.sprite_flip = 1;
        }
        else if(key == SDL_SCANCODE_U)
        {
            player.new_pos = V2u(player.pos.x + 1, player.pos.y - 1);
            player.sprite_flip = 0;
        }
        else if(key == SDL_SCANCODE_B)
        {
            player.new_pos = V2u(player.pos.x - 1, player.pos.y + 1);
            player.sprite_flip = 1;
        }
        else if(key == SDL_SCANCODE_N)
        {
            player.new_pos = V2u(player.pos.x + 1, player.pos.y + 1);
            player.sprite_flip = 0;
        }
        else if(key == SDL_SCANCODE_COMMA)
        {
            pick_up_item();
        }
        else if(key == SDL_SCANCODE_D)
        {
            if(is_tile(player.pos, tile_path_down))
            {
                generate_level();
            }
        }
        else if(key == SDL_SCANCODE_A)
        {
            if(is_tile(player.pos, tile_path_up))
            {
                game.state = state_quit;
            }
        }
    }
    
    if(!inventory.open)
    {
        game.turn_changed = 1;
    }
}

internal void
player_attack_monster(u32 i)
{
    monster[i].hp -= player.damage;
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

internal u32
heal_player(u32 amount)
{
    u32 result = 0;
    
    if(player.hp < player.max_hp)
    {
        result = 1;
        
        player.hp += amount;
        if(player.hp > player.max_hp)
        {
            player.hp = player.max_hp;
        }
        
        add_pop_up_text("%d", player.pos,
                        (player.size.w / 2) / 2, -8,
                        text_normal_attack, amount);
    }
    
    
    return(result);
}

internal u32
is_player_colliding_with_monster()
{
    u32 result = 0;
    
    for(u32 i = 0; i < MONSTER_COUNT; ++i)
    {
        if(monster[i].type)
        {
            if(V2u_equal(player.new_pos, monster[i].pos))
            {
                result = 1;
                
                char name[32] = {0};
                get_monster_name(monster[i].type, name);
                
                char attack[64] = {0};
                get_player_attack_message(attack);
                
                add_console_message("You %s the %s for %d damage", color_white, attack, name,
                                    player.damage);
                
                add_pop_up_text("%d", monster[i].pos,
                                (monster[i].size.w / 2) / 2, -8,
                                text_normal_attack, player.damage);
                
                player_attack_monster(i);
                monster[i].in_combat = 1;
                
                if(!is_monster_alive(i))
                {
                    add_console_message("You killed the %s!", color_orange, name);
                    remove_monster(i);
                }
                
                break;
            }
        }
    }
    
    return(result);
}

internal void
update_player()
{
    if(is_traversable(player.new_pos))
    {
        if(!is_player_colliding_with_monster())
        {
            player.pos = player.new_pos;
        }
    }
    else
    {
        if(level.tiles[(player.new_pos.y * LEVEL_TILE_WIDTH) + player.new_pos.x] == tile_wall_stone)
        {
            add_console_message("A wall stops you", color_white);
        }
        else if(level.tiles[(player.new_pos.y * LEVEL_TILE_WIDTH) + player.new_pos.x] == tile_door_closed)
        {
            add_console_message("You push the door open", color_white);
            level.tiles[(player.new_pos.y * LEVEL_TILE_WIDTH) + player.new_pos.x] = tile_door_open;
        }
        else if(level.tiles[(player.new_pos.y * LEVEL_TILE_WIDTH) + player.new_pos.x] == tile_path_up)
        {
            add_console_message("A path to the surface, [A]scend to flee the mountain", color_white);
        }
        else if(level.tiles[(player.new_pos.y * LEVEL_TILE_WIDTH) + player.new_pos.x] == tile_path_down)
        {
            add_console_message("A path that leads further downwards.. [D]escend?", color_white);
        }
    }
    
    // NOTE(rami): Force move
#if MOONBREATH_DEBUG
    player.pos = player.new_pos;
    return;
#endif
    
    player.new_pos = player.pos;
    ++game.turn;
}