internal void
move_entity(entity_t *entity)
{
    set_dungeon_occupied(entity->pos, false);
    entity->pos = entity->new_pos;
    set_dungeon_occupied(entity->pos, true);
}

internal b32
heal_entity(entity_t *entity, u32 value)
{
    b32 result = false;
    
    if(entity->hp < entity->max_hp)
    {
        result = true;
        
        entity->hp += value;
        if(entity->hp > entity->max_hp)
        {
            entity->hp = entity->max_hp;
        }
        
        add_pop_text("%u", entity->pos, text_heal, value);
    }
    
    return(result);
}

internal string_t
get_entity_attack_message(entity_id id)
{
    string_t result = {0};
    
#if 0
    switch(id)
    {
        switch(id)
        {
            case entity_id_player:
            {
            } break;
            
            invalid_default_case;
        }
        
        // TODO(rami): Needs to be redone.
        switch(id)
        {
            case monster_slime:
            {
                strcpy(message, "The Slime bites you for");
            } break;
            
            case monster_skeleton:
            {
                u32 random = random_number(1, 2);
                if(random == 1)
                {
                    strcpy(message, "The Skeleton punches you for");
                }
                else
                {
                    strcpy(message, "The Skeleton kicks you for");
                }
            } break;
            
            case monster_armored_skeleton:
            {
                strcpy(message, "The Skeleton slashes you for");
            } break;
            
            case monster_orc_warrior:
            {
                u32 random = random_number(1, 2);
                if(random == 1)
                {
                    strcpy(message, "The Orc Warrior punches you for");
                }
                else
                {
                    strcpy(message, "The Orc Warrior kicks you for");
                }
            } break;
            
            case monster_cave_bat:
            {
                u32 random = random_number(1, 2);
                if(random == 1)
                {
                    strcpy(message, "The Cave Bat bites you for");
                }
                else
                {
                    strcpy(message, "The Cave Bat slashes you for");
                }
            } break;
            
            case monster_python:
            {
                strcpy(message, "The Python bites you for");
            } break;
            
            case monster_kobold:
            {
                u32 random = random_number(1, 2);
                if(random == 1)
                {
                    strcpy(message, "The Kobold slashes you for");
                }
                else
                {
                    strcpy(message, "The Kobold stabs you for");
                }
            } break;
            
            case monster_ogre:
            {
                strcpy(message, "The Ogre bashes you for");
            } break;
            
            invalid_default_case;
        }
    }
#endif
    
    return(result);
}

internal void
delete_entity(entity_t *entity)
{
    set_dungeon_occupied(entity->pos, false);
    set_dungeon_tile_blood(entity->pos);
    memset(entity, 0, sizeof(entity_t));
}

internal void
attack_entity(entity_t *attacker, entity_t *defender, u32 damage)
{
    defender->hp -= damage;
    if((s32)(defender->hp <= 0))
    {
        // TODO(rami): Don't want the player to die yet.
        if(defender->type != entity_type_player)
        {
            add_log_string("You killed the %s!", defender->name);
            delete_entity(defender);
        }
    }
    else
    {
        // TODO(rami): You Something the Enemy for Number damage.
        string_t attack = get_entity_attack_message(attacker->id);
        add_log_string("%s %u damage.", attack.str, damage);
        add_pop_text("%u", defender->pos, text_normal_attack, damage);
    }
}

internal void
entity_ai_update(entity_t *enemy)
{
    // TODO(rami): No AI right now.
    if(1)
    {
        u32 direction = random_number(dir_up, dir_down_right);
        switch(direction)
        {
            case dir_up:
            {
                --enemy->new_pos.y;
            } break;
            
            case dir_down:
            {
                ++enemy->new_pos.y;
            } break;
            
            case dir_left:
            {
                --enemy->new_pos.x;
                enemy->is_flipped = true;
            } break;
            
            case dir_right:
            {
                ++enemy->new_pos.x;
                enemy->is_flipped = false;
            } break;
            
            case dir_up_left:
            {
                --enemy->new_pos.y;
                --enemy->new_pos.x;
                enemy->is_flipped = true;
            } break;
            
            case dir_up_right:
            {
                --enemy->new_pos.y;
                ++enemy->new_pos.x;
                enemy->is_flipped = false;
            } break;
            
            case dir_down_left:
            {
                ++enemy->new_pos.y;
                --enemy->new_pos.x;
                enemy->is_flipped = true;
            } break;
            
            case dir_down_right:
            {
                ++enemy->new_pos.y;
                ++enemy->new_pos.x;
                enemy->is_flipped = false;
            } break;
            
            invalid_default_case;
        }
    }
}

internal void
update_entities(input_state_t *keyboard)
{
    // Update Player
    b32 update_player = false;
    b32 advance_time = false;
    
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
        else if(is_input_valid(&keyboard[key_drop_item]))
        {
            if(!inventory.is_item_being_moved)
            {
                remove_inventory_item(1);
            }
        }
        else if(is_input_valid(&keyboard[key_equip_item]))
        {
            if(!inventory.is_item_being_moved)
            {
                u32 slot_index = index_from_v2u(inventory.current, inventory.w);
                item_t *item = inventory.slots[slot_index];
                if(item && (item->type == item_type_weapon || item->type == item_type_armor))
                {
                    if(item->is_equipped)
                    {
                        unequip_item(item);
                    }
                    else
                    {
                        u32_t slot_index = get_equipped_item_slot_index(item->slot);
                        if(slot_index.success)
                        {
                            unequip_item(inventory.slots[slot_index.value]);
                        }
                        
                        equip_item(item);
                    }
                }
            }
        }
        else if(is_input_valid(&keyboard[key_consume_item]))
        {
            u32 slot_index = index_from_v2u(inventory.current, inventory.w);
            item_t *item = inventory.slots[slot_index];
            if(item && item->in_inventory && item->type == item_type_consumable)
            {
                if(item->effect == item_effect_healing)
                {
                    if(heal_entity(player, item->effect_amount))
                    {
                        add_log_string("The potion heals you for %d hitpoints.", item->effect_amount);
                        
                        remove_inventory_item(0);
                        remove_game_item(item);
                    }
                    else
                    {
                        add_log_string("You do not feel the need to drink this.");
                    }
                }
            }
        }
        else if(is_input_valid(&keyboard[key_move_item]))
        {
            inventory.moving_item_dest_index = index_from_v2u(inventory.current, inventory.w);
            
            if(inventory.is_item_being_moved)
            {
                if(inventory.moving_item_src_index != inventory.moving_item_dest_index)
                {
                    if(inventory.slots[inventory.moving_item_dest_index])
                    {
                        item_t *temp = inventory.slots[inventory.moving_item_dest_index];
                        
                        inventory.slots[inventory.moving_item_dest_index] = inventory.slots[inventory.moving_item_src_index];
                        inventory.slots[inventory.moving_item_src_index] = temp;
                    }
                    else
                    {
                        inventory.slots[inventory.moving_item_dest_index] = inventory.slots[inventory.moving_item_src_index];
                        inventory.slots[inventory.moving_item_src_index] = 0;
                    }
                    
                    inventory.is_item_being_moved = false;
                }
                
                inventory.is_item_being_moved = false;
                inventory.moving_item_src_index = (u32)-1;
                inventory.moving_item_dest_index = (u32)-1;
            }
            else
            {
                if(inventory.slots[inventory.moving_item_dest_index])
                {
                    inventory.is_item_being_moved = true;
                    inventory.moving_item_src_index = inventory.moving_item_dest_index;
                }
            }
        }
    }
    else
    {
        if(is_input_valid(&keyboard[key_move_up]))
        {
            player->new_pos = V2u(player->pos.x, player->pos.y - 1);
            update_player = true;
        }
        else if(is_input_valid(&keyboard[key_move_down]))
        {
            player->new_pos = V2u(player->pos.x, player->pos.y + 1);
            update_player = true;
        }
        else if(is_input_valid(&keyboard[key_move_left]))
        {
            player->new_pos = V2u(player->pos.x - 1, player->pos.y);
            update_player = true;
        }
        else if(is_input_valid(&keyboard[key_move_right]))
        {
            player->new_pos = V2u(player->pos.x + 1, player->pos.y);
            update_player = true;
        }
        else if(is_input_valid(&keyboard[key_move_up_left]))
        {
            player->new_pos = V2u(player->pos.x - 1, player->pos.y - 1);
            update_player = true;
        }
        else if(is_input_valid(&keyboard[key_move_up_right]))
        {
            player->new_pos = V2u(player->pos.x + 1, player->pos.y - 1);
            update_player = true;
        }
        else if(is_input_valid(&keyboard[key_move_down_left]))
        {
            player->new_pos = V2u(player->pos.x - 1, player->pos.y + 1);
            update_player = true;
        }
        else if(is_input_valid(&keyboard[key_move_down_right]))
        {
            player->new_pos = V2u(player->pos.x + 1, player->pos.y + 1);
            update_player = true;
        }
        else if(is_input_valid(&keyboard[key_pick_up_item]))
        {
            add_inventory_item();
        }
        else if(is_input_valid(&keyboard[key_ascend]))
        {
            if(is_dungeon_tile(player->pos, tile_stone_path_up))
            {
                game.state = state_exit;
            }
            else
            {
                add_log_string("There's nothing here that leads upwards.");
            }
        }
        else if(is_input_valid(&keyboard[key_descend]))
        {
            if(is_dungeon_tile(player->pos, tile_stone_path_down))
            {
                if(dungeon.level < MAX_DUNGEON_LEVEL)
                {
                    add_log_string("You descend further.. Level %u.", dungeon.level + 1);
                    
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
                add_log_string("There's nothing here that leads downwards.");
            }
        }
        else if(is_input_valid(&keyboard[key_wait]))
        {
            game.time += 1.0f;
            advance_time = true;
        }
    }
    
    if(update_player)
    {
#if MOONBREATH_SLOW
        if(debug_player_traversable)
        {
            if(is_inside_dungeon(player->new_pos))
            {
                move_entity(player);
            }
        }
        else
#endif
        
            if(is_inside_dungeon(player->new_pos))
        {
            if(!V2u_equal(player->pos, player->new_pos) &&
               is_dungeon_occupied(player->new_pos))
            {
                for(u32 entity_index = 1;
                    entity_index < array_count(entities);
                    ++entity_index)
                {
                    entity_t *entity = &entities[entity_index];
                    if(V2u_equal(player->new_pos, entity->pos))
                    {
                        u32 player_hit_chance = 15 + (player->dexterity / 2);
                        u32 player_damage = 2;
                        
                        u32_t slot_index = get_equipped_item_slot_index(item_slot_main_hand);
                        if(slot_index.success)
                        {
                            item_t *item = inventory.slots[slot_index.value];
                            
                            player_hit_chance += item->accuracy;
                            player_hit_chance += item->enchantment_level;
                            
                            printf("accuracy: %u\n", item->accuracy);
                            printf("enchantment_level: %u\n", item->enchantment_level);
                        }
                        else
                        {
                            player_hit_chance += 2;
                        }
                        
#if 0
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
                        
                        u32 roll = random_number(0, player_hit_chance);
                        if(roll >= entity->evasion)
                        {
                            printf("%s\n", entity->name);
                            attack_entity(player, entity, player_damage);
                            entity->in_combat = true;
                        }
                        else
                        {
                            add_log_string("The %s dodges your attack!", entity->name);
                        }
                        
                        break;
                    }
                }
                
                advance_time = true;
            }
            else
            {
                if(is_dungeon_tile(player->new_pos, tile_stone_door_closed))
                {
                    add_log_string("You open the door.");
                    set_dungeon_tile(player->new_pos, tile_stone_door_open);
                    
                    advance_time = true;
                }
                else if(is_dungeon_traversable(player->new_pos))
                {
                    move_entity(player);
                    advance_time = true;
                }
            }
            
            if(advance_time)
            {
                game.time += 1.0f;
            }
            
            // NOTE(rami): This is to keep the new_pos locked.
            player->new_pos = player->pos;
        }
    }
    
    update_pathfind_map(pathfind_map, dungeon.w, dungeon.h);
    update_fov();
    
    if(advance_time)
    {
        // Update Enemies
        for(u32 entity_index = 1;
            entity_index < array_count(entities);
            ++entity_index)
        {
            entity_t *enemy = &entities[entity_index];
            if(enemy->type == entity_type_enemy)
            {
                for(u32 action_count = 0;
                    action_count < (1.0f / enemy->action_speed);
                    ++action_count)
                {
                    if(enemy->in_combat)
                    {
                        // NOTE(rami): Turn monster sprite towards target.
                        enemy->is_flipped = (player->pos.x < enemy->pos.x);
                        
                        v2u next_pos = next_pathfind_pos((u32 *)pathfind_map, dungeon.w, enemy);
                        
                        if(V2u_equal(next_pos, player->pos))
                        {
                            attack_entity(enemy, player, enemy->damage);
                        }
                        else
                        {
                            enemy->new_pos = next_pos;
                        }
                    }
                    else
                    {
                        entity_ai_update(enemy);
                    }
                    
                    if(is_dungeon_traversable(enemy->new_pos) &&
                       !is_dungeon_occupied(enemy->new_pos))
                    {
                        move_entity(enemy);
                    }
                    
                    // NOTE(rami): This is to keep the new_pos locked.
                    enemy->new_pos = enemy->pos;
                }
            }
        }
    }
}

internal void
render_entities()
{
    // Render Player
    v2u game_pos = get_game_pos(player->pos);
    
    v4u src = {tile_mul(player->tile.x), tile_mul(player->tile.y), player->w, player->h};
    v4u dest = {game_pos.x, game_pos.y, player->w, player->h};
    SDL_RenderCopy(game.renderer, textures.sprite_sheet.tex, (SDL_Rect *)&src, (SDL_Rect *)&dest);
    
    // Render Player Items
    for(u32 item_slot_index = 1;
        item_slot_index < item_slot_total;
        ++item_slot_index)
    {
        for(u32 inventory_slot_index = 0;
            inventory_slot_index < array_count(inventory.slots);
            ++inventory_slot_index)
        {
            item_t *item = inventory.slots[inventory_slot_index];
            if(item && item->is_equipped && item->slot == item_slot_index)
            {
                v4u src = {tile_mul(item->tile.x), tile_mul(item->tile.y), 32, 32};
                v4u dest = {game_pos.x, game_pos.y, 32, 32};
                SDL_RenderCopy(game.renderer, textures.wearable_item_tileset.tex, (SDL_Rect *)&src, (SDL_Rect *)&dest);
                
                break;
            }
        }
    }
    
    // Render Enemies
    for(u32 entity_index = 1;
        entity_index < array_count(entities);
        ++entity_index)
    {
        entity_t *enemy = &entities[entity_index];
        if(enemy->type == entity_type_enemy)
        {
            if(is_seen(enemy->pos))
            {
                enemy->has_been_seen = true;
                enemy->is_ghost_pos_stored = false;
                
                v2u game_pos = get_game_pos(enemy->pos);
                v4u src = {tile_mul(enemy->tile.x), tile_mul(enemy->tile.y), enemy->w, enemy->h};
                v4u dest = {game_pos.x, game_pos.y, enemy->w, enemy->h};
                
                SDL_RenderCopyEx(game.renderer, textures.sprite_sheet.tex, (SDL_Rect *)&src, (SDL_Rect *)&dest, 0, 0, enemy->is_flipped);
                
                // Render Enemy HP Bar
                if(enemy->in_combat)
                {
                    // HP Bar Outside
                    set_render_color(color_black);
                    v4u hp_bar_outside = {game_pos.x, game_pos.y + 33, 32, 4};
                    SDL_RenderDrawRect(game.renderer, (SDL_Rect *)&hp_bar_outside);
                    
                    // HP Bar Inside
                    set_render_color(color_dark_red);
                    u32 hp_bar_inside_w = get_ratio(enemy->hp, enemy->max_hp, 30);
                    v4u hp_bar_inside = {game_pos.x + 1, game_pos.y + 34, hp_bar_inside_w, 2};
                    SDL_RenderFillRect(game.renderer, (SDL_Rect *)&hp_bar_inside);
                }
            }
            else
            {
                if(is_seen(enemy->ghost_pos))
                {
                    enemy->has_been_seen = false;
                    enemy->ghost_pos = V2u(0, 0);
                }
                else
                {
                    if(enemy->has_been_seen)
                    {
                        if(!enemy->is_ghost_pos_stored)
                        {
                            enemy->ghost_pos = enemy->new_pos;
                            enemy->ghost_is_flipped = enemy->is_flipped;
                            
                            enemy->is_ghost_pos_stored = true;
                        }
                        
                        v2u game_pos = get_game_pos(enemy->ghost_pos);
                        v4u src = {tile_mul(enemy->tile.x), tile_mul(enemy->tile.y), enemy->w, enemy->h};
                        v4u dest = {game_pos.x, game_pos.y, enemy->w, enemy->h};
                        
                        SDL_SetTextureColorMod(textures.sprite_sheet.tex, 64, 64, 64);
                        SDL_RenderCopyEx(game.renderer, textures.sprite_sheet.tex, (SDL_Rect *)&src, (SDL_Rect *)&dest, 0, 0, enemy->ghost_is_flipped);
                        SDL_SetTextureColorMod(textures.sprite_sheet.tex, 255, 255, 255);
                    }
                }
            }
        }
    }
}

internal void
add_player_entity()
{
    // TODO(rami): Note all these zeros.
    entity_t *player = &entities[0];
    player->id = entity_id_player;
    strcpy(player->name, "Name");
    
    player->max_hp = 10;
    player->hp = 5;
    
    player->pos = V2u(0, 0);
    player->new_pos = player->pos;
    
    player->w = player->h = 32;
    player->tile = V2u(0, 0);
    
    player->strength = 10;
    player->intelligence = 10;
    player->dexterity = 10;
    player->defence = 0;
    player->evasion = 10;
    
    player->speed = 1;
    player->level = 0;
    player->gold = 0;
    player->fov = 8;
    player->type = entity_type_player;
}

internal void
add_enemy_entity(entity_id id, u32 x, u32 y)
{
    assert(id != entity_id_none && id != entity_id_player);
    
    for(u32 entity_index = 1;
        entity_index < array_count(entities);
        ++entity_index)
    {
        entity_t *enemy = &entities[entity_index];
        if(!enemy->type)
        {
            enemy->id = id;
            enemy->type = entity_type_enemy;
            // TODO(rami): These are here here for now!
            enemy->action_speed = 1.0f;
            enemy->damage = 2;
            set_dungeon_occupied(enemy->pos, true);
            
            // TODO(rami): Note all these zeros.
            switch(id)
            {
                case entity_id_baby_slime:
                {
                    strcpy(enemy->name, "Baby Slime");
                    
                    enemy->max_hp = enemy->hp = 4;
                    
                    enemy->pos = V2u(x, y);
                    enemy->new_pos = enemy->pos;
                    
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(1, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->speed = 1;
                    enemy->level = 1;
                    enemy->type = entity_type_enemy;
                } break;
                
                case entity_id_slime:
                {
                    strcpy(enemy->name, "Slime");
                    
                    enemy->max_hp = enemy->hp = 4;
                    
                    enemy->pos = V2u(x, y);
                    enemy->new_pos = enemy->pos;
                    
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(2, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->speed = 1;
                    enemy->level = 1;
                    enemy->type = entity_type_enemy;
                } break;
                
                case entity_id_skeleton:
                {
                    strcpy(enemy->name, "Skeleton");
                    
                    enemy->max_hp = enemy->hp = 4;
                    
                    enemy->pos = V2u(x, y);
                    enemy->new_pos = enemy->pos;
                    
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(3, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->speed = 1;
                    enemy->level = 1;
                    enemy->type = entity_type_enemy;
                } break;
                
                case entity_id_skeleton_warrior:
                {
                    strcpy(enemy->name, "Skeleton Warrior");
                    
                    enemy->max_hp = enemy->hp = 4;
                    
                    enemy->pos = V2u(x, y);
                    enemy->new_pos = enemy->pos;
                    
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(4, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->speed = 1;
                    enemy->level = 1;
                    enemy->type = entity_type_enemy;
                } break;
                
                case entity_id_orc_warrior:
                {
                    strcpy(enemy->name, "Orc Warrior");
                    
                    enemy->max_hp = enemy->hp = 4;
                    
                    enemy->pos = V2u(x, y);
                    enemy->new_pos = enemy->pos;
                    
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(5, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->speed = 1;
                    enemy->level = 1;
                    enemy->type = entity_type_enemy;
                } break;
                
                case entity_id_cave_bat:
                {
                    strcpy(enemy->name, "Cave Bat");
                    
                    enemy->max_hp = enemy->hp = 4;
                    
                    enemy->pos = V2u(x, y);
                    enemy->new_pos = enemy->pos;
                    
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(6, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->speed = 1;
                    enemy->level = 1;
                    enemy->type = entity_type_enemy;
                } break;
                
                case entity_id_python:
                {
                    strcpy(enemy->name, "Python");
                    
                    enemy->max_hp = enemy->hp = 4;
                    
                    enemy->pos = V2u(x, y);
                    enemy->new_pos = enemy->pos;
                    
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(7, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->speed = 1;
                    enemy->level = 1;
                    enemy->type = entity_type_enemy;
                } break;
                
                case entity_id_kobold:
                {
                    strcpy(enemy->name, "Kobold");
                    
                    enemy->max_hp = enemy->hp = 4;
                    
                    enemy->pos = V2u(x, y);
                    enemy->new_pos = enemy->pos;
                    
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(8, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->speed = 1;
                    enemy->level = 1;
                    enemy->type = entity_type_enemy;
                } break;
                
                case entity_id_ogre:
                {
                    strcpy(enemy->name, "Ogre");
                    
                    enemy->max_hp = enemy->hp = 4;
                    
                    enemy->pos = V2u(x, y);
                    enemy->new_pos = enemy->pos;
                    
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(9, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->speed = 1;
                    enemy->level = 1;
                    enemy->type = entity_type_enemy;
                } break;
                
                case entity_id_tormentor:
                {
                    strcpy(enemy->name, "Tormentor");
                    
                    enemy->max_hp = enemy->hp = 4;
                    
                    enemy->pos = V2u(x, y);
                    enemy->new_pos = enemy->pos;
                    
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(10, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->speed = 1;
                    enemy->level = 1;
                    enemy->type = entity_type_enemy;
                } break;
                
                case entity_id_imp:
                {
                    strcpy(enemy->name, "Imp");
                    
                    enemy->max_hp = enemy->hp = 4;
                    
                    enemy->pos = V2u(x, y);
                    enemy->new_pos = enemy->pos;
                    
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(11, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->speed = 1;
                    enemy->level = 1;
                    enemy->type = entity_type_enemy;
                } break;
                
                case entity_id_giant_demon:
                {
                    strcpy(enemy->name, "Giant Demon");
                    
                    enemy->max_hp = enemy->hp = 4;
                    
                    enemy->pos = V2u(x, y);
                    enemy->new_pos = enemy->pos;
                    
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(12, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->speed = 1;
                    enemy->level = 1;
                    enemy->type = entity_type_enemy;
                } break;
                
                case entity_id_hellhound:
                {
                    strcpy(enemy->name, "Hellhound");
                    
                    enemy->max_hp = enemy->hp = 4;
                    
                    enemy->pos = V2u(x, y);
                    enemy->new_pos = enemy->pos;
                    
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(13, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->speed = 1;
                    enemy->level = 1;
                    enemy->type = entity_type_enemy;
                } break;
                
                case entity_id_undead_elf_warrior:
                {
                    strcpy(enemy->name, "Undead Elf Warrior");
                    
                    enemy->max_hp = enemy->hp = 4;
                    
                    enemy->pos = V2u(x, y);
                    enemy->new_pos = enemy->pos;
                    
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(14, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->speed = 1;
                    enemy->level = 1;
                    enemy->type = entity_type_enemy;
                } break;
                
                case entity_id_shadow_thief:
                {
                    strcpy(enemy->name, "Shadow Thief");
                    
                    enemy->max_hp = enemy->hp = 4;
                    
                    enemy->pos = V2u(x, y);
                    enemy->new_pos = enemy->pos;
                    
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(15, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->speed = 1;
                    enemy->level = 1;
                    enemy->type = entity_type_enemy;
                } break;
                
                case entity_id_goblin:
                {
                    strcpy(enemy->name, "Goblin");
                    
                    enemy->max_hp = enemy->hp = 4;
                    
                    enemy->pos = V2u(x, y);
                    enemy->new_pos = enemy->pos;
                    
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(16, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->speed = 1;
                    enemy->level = 1;
                    enemy->type = entity_type_enemy;
                } break;
                
                case entity_id_goblin_warrior:
                {
                    strcpy(enemy->name, "Goblin Warrior");
                    
                    enemy->max_hp = enemy->hp = 4;
                    
                    enemy->pos = V2u(x, y);
                    enemy->new_pos = enemy->pos;
                    
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(17, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->speed = 1;
                    enemy->level = 1;
                    enemy->type = entity_type_enemy;
                } break;
                
                case entity_id_viper:
                {
                    strcpy(enemy->name, "Viper");
                    
                    enemy->max_hp = enemy->hp = 4;
                    
                    enemy->pos = V2u(x, y);
                    enemy->new_pos = enemy->pos;
                    
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(18, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->speed = 1;
                    enemy->level = 1;
                    enemy->type = entity_type_enemy;
                } break;
                
                case entity_id_scarlet_kingsnake:
                {
                    strcpy(enemy->name, "Scarlet Kingsnake");
                    
                    enemy->max_hp = enemy->hp = 4;
                    
                    enemy->pos = V2u(x, y);
                    enemy->new_pos = enemy->pos;
                    
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(19, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->speed = 1;
                    enemy->level = 1;
                    enemy->type = entity_type_enemy;
                } break;
                
                case entity_id_stray_dog:
                {
                    strcpy(enemy->name, "Stray Dog");
                    
                    enemy->max_hp = enemy->hp = 4;
                    
                    enemy->pos = V2u(x, y);
                    enemy->new_pos = enemy->pos;
                    
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(20, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->speed = 1;
                    enemy->level = 1;
                    enemy->type = entity_type_enemy;
                } break;
                
                case entity_id_wolf:
                {
                    strcpy(enemy->name, "Wolf");
                    
                    enemy->max_hp = enemy->hp = 4;
                    
                    enemy->pos = V2u(x, y);
                    enemy->new_pos = enemy->pos;
                    
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(21, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->speed = 1;
                    enemy->level = 1;
                    enemy->type = entity_type_enemy;
                } break;
                
                case entity_id_green_mamba:
                {
                    strcpy(enemy->name, "Green Mamba");
                    
                    enemy->max_hp = enemy->hp = 4;
                    
                    enemy->pos = V2u(x, y);
                    enemy->new_pos = enemy->pos;
                    
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(22, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->speed = 1;
                    enemy->level = 1;
                    enemy->type = entity_type_enemy;
                } break;
                
                case entity_id_floating_eye:
                {
                    strcpy(enemy->name, "Floating Eye");
                    
                    enemy->max_hp = enemy->hp = 4;
                    
                    enemy->pos = V2u(x, y);
                    enemy->new_pos = enemy->pos;
                    
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(23, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->speed = 1;
                    enemy->level = 1;
                    enemy->type = entity_type_enemy;
                } break;
                
                case entity_id_devourer:
                {
                    strcpy(enemy->name, "Devourer");
                    
                    enemy->max_hp = enemy->hp = 4;
                    
                    enemy->pos = V2u(x, y);
                    enemy->new_pos = enemy->pos;
                    
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(24, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->speed = 1;
                    enemy->level = 1;
                    enemy->type = entity_type_enemy;
                } break;
                
                case entity_id_ghoul:
                {
                    strcpy(enemy->name, "Ghoul");
                    
                    enemy->max_hp = enemy->hp = 4;
                    
                    enemy->pos = V2u(x, y);
                    enemy->new_pos = enemy->pos;
                    
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(25, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->speed = 1;
                    enemy->level = 1;
                    enemy->type = entity_type_enemy;
                } break;
                
                case entity_id_cyclops:
                {
                    strcpy(enemy->name, "Cyclops");
                    
                    enemy->max_hp = enemy->hp = 4;
                    
                    enemy->pos = V2u(x, y);
                    enemy->new_pos = enemy->pos;
                    
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(26, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->speed = 1;
                    enemy->level = 1;
                    enemy->type = entity_type_enemy;
                } break;
                
                case entity_id_dwarwen_warrior:
                {
                    strcpy(enemy->name, "Dwarwen Warrior");
                    
                    enemy->max_hp = enemy->hp = 4;
                    
                    enemy->pos = V2u(x, y);
                    enemy->new_pos = enemy->pos;
                    
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(27, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->speed = 1;
                    enemy->level = 1;
                    enemy->type = entity_type_enemy;
                } break;
                
                case entity_id_black_knight:
                {
                    strcpy(enemy->name, "Black Knight");
                    
                    enemy->max_hp = enemy->hp = 4;
                    
                    enemy->pos = V2u(x, y);
                    enemy->new_pos = enemy->pos;
                    
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(28, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->speed = 1;
                    enemy->level = 1;
                    enemy->type = entity_type_enemy;
                } break;
                
                case entity_id_cursed_black_knight:
                {
                    strcpy(enemy->name, "Cursed Black Knight");
                    
                    enemy->max_hp = enemy->hp = 4;
                    
                    enemy->pos = V2u(x, y);
                    enemy->new_pos = enemy->pos;
                    
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(29, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->speed = 1;
                    enemy->level = 1;
                    enemy->type = entity_type_enemy;
                } break;
                
                case entity_id_treant:
                {
                    strcpy(enemy->name, "Treant");
                    
                    enemy->max_hp = enemy->hp = 4;
                    
                    enemy->pos = V2u(x, y);
                    enemy->new_pos = enemy->pos;
                    
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(30, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->speed = 1;
                    enemy->level = 1;
                    enemy->type = entity_type_enemy;
                } break;
                
                case entity_id_minotaur:
                {
                    strcpy(enemy->name, "Minotaur");
                    
                    enemy->max_hp = enemy->hp = 4;
                    
                    enemy->pos = V2u(x, y);
                    enemy->new_pos = enemy->pos;
                    
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(31, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->speed = 1;
                    enemy->level = 1;
                    enemy->type = entity_type_enemy;
                } break;
                
                case entity_id_centaur_warrior:
                {
                    strcpy(enemy->name, "Centaur Warrior");
                    
                    enemy->max_hp = enemy->hp = 4;
                    
                    enemy->pos = V2u(x, y);
                    enemy->new_pos = enemy->pos;
                    
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(32, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->speed = 1;
                    enemy->level = 1;
                    enemy->type = entity_type_enemy;
                } break;
                
                case entity_id_centaur:
                {
                    strcpy(enemy->name, "Centaur");
                    
                    enemy->max_hp = enemy->hp = 4;
                    
                    enemy->pos = V2u(x, y);
                    enemy->new_pos = enemy->pos;
                    
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(33, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->speed = 1;
                    enemy->level = 1;
                    enemy->type = entity_type_enemy;
                } break;
                
                case entity_id_frost_shards:
                {
                    strcpy(enemy->name, "Frost Shards");
                    
                    enemy->max_hp = enemy->hp = 4;
                    
                    enemy->pos = V2u(x, y);
                    enemy->new_pos = enemy->pos;
                    
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(34, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->speed = 1;
                    enemy->level = 1;
                    enemy->type = entity_type_enemy;
                } break;
                
                case entity_id_frost_walker:
                {
                    strcpy(enemy->name, "Frost Walker");
                    
                    enemy->max_hp = enemy->hp = 4;
                    
                    enemy->pos = V2u(x, y);
                    enemy->new_pos = enemy->pos;
                    
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(35, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->speed = 1;
                    enemy->level = 1;
                    enemy->type = entity_type_enemy;
                } break;
                
                case entity_id_griffin:
                {
                    strcpy(enemy->name, "Griffin");
                    
                    enemy->max_hp = enemy->hp = 4;
                    
                    enemy->pos = V2u(x, y);
                    enemy->new_pos = enemy->pos;
                    
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(36, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->speed = 1;
                    enemy->level = 1;
                    enemy->type = entity_type_enemy;
                } break;
                
                case entity_id_spectre:
                {
                    strcpy(enemy->name, "Spectre");
                    
                    enemy->max_hp = enemy->hp = 4;
                    
                    enemy->pos = V2u(x, y);
                    enemy->new_pos = enemy->pos;
                    
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(37, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->speed = 1;
                    enemy->level = 1;
                    enemy->type = entity_type_enemy;
                } break;
                
                case entity_id_flying_skull:
                {
                    strcpy(enemy->name, "Flying Skull");
                    
                    enemy->max_hp = enemy->hp = 4;
                    
                    enemy->pos = V2u(x, y);
                    enemy->new_pos = enemy->pos;
                    
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(38, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->speed = 1;
                    enemy->level = 1;
                    enemy->type = entity_type_enemy;
                } break;
                
                case entity_id_brimstone_imp:
                {
                    strcpy(enemy->name, "Brimstone Imp");
                    
                    enemy->max_hp = enemy->hp = 4;
                    
                    enemy->pos = V2u(x, y);
                    enemy->new_pos = enemy->pos;
                    
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(39, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->speed = 1;
                    enemy->level = 1;
                    enemy->type = entity_type_enemy;
                } break;
                
                invalid_default_case;
            }
            
            return;
        }
    }
    
    assert(false);
}