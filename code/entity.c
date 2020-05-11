internal b32
does_entity_hit(game_state_t *game, u32 hit_chance, u32 evasion)
{
    b32 result = (random_number(&game->random, 0, hit_chance) >= evasion);
    return(result);
}

internal void
move_entity(dungeon_t *dungeon, entity_t *entity)
{
    set_tile_occupied(dungeon->tiles, entity->pos, false);
    entity->pos = entity->new_pos;
    set_tile_occupied(dungeon->tiles, entity->pos, true);
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
    }
    
    return(result);
}

internal string_t
entity_attack_message(game_state_t *game, entity_t *attacker, entity_t *defender, inventory_t *inventory)
{
    string_t result = {0};
    
    if(attacker->type == entity_type_player)
    {
        char *attack = 0;
        
        u32_bool_t slot_index = equipped_item_slot_index(item_slot_first_hand, inventory);
        if(slot_index.success)
        {
            item_t *item = inventory->slots[slot_index.value];
            switch(item->id)
            {
                case item_dagger:
                case item_sword:
                case item_scimitar:
                case item_katana:
                case item_halberd:
                {
                    u32 roll = random_number(&game->random, 1, 6);
                    switch(roll)
                    {
                        case 1: attack = "stab"; break;
                        case 2: attack = "pierce"; break;
                        case 3: attack = "puncture"; break;
                        case 4: attack = "slash"; break;
                        case 5: attack = "lacerate"; break;
                        case 6: attack = "cleave"; break;
                        
                        invalid_default_case;
                    }
                } break;
                
                case item_club:
                case item_morningstar:
                case item_warhammer:
                {
                    u32 roll = random_number(&game->random, 1, 6);
                    switch(roll)
                    {
                        case 1: attack = "smash"; break;
                        case 2: attack = "bash"; break;
                        case 3: attack = "strike"; break;
                        case 4: attack = "pummel"; break;
                        case 5: attack = "pound"; break;
                        case 6: attack = "crush"; break;
                        
                        invalid_default_case;
                    }
                } break;
                
                case item_hand_axe:
                case item_war_axe:
                case item_battleaxe:
                {
                    u32 roll = random_number(&game->random, 1, 6);
                    switch(roll)
                    {
                        case 1: attack = "hack"; break;
                        case 2: attack = "rend"; break;
                        case 3: attack = "chop"; break;
                        case 4: attack = "slash"; break;
                        case 5: attack = "lacerate"; break;
                        case 6: attack = "cleave"; break;
                        
                        invalid_default_case;
                    }
                } break;
                
                case item_spear:
                case item_trident:
                {
                    u32 roll = random_number(&game->random, 1, 4);
                    switch(roll)
                    {
                        case 1: attack = "stab"; break;
                        case 2: attack = "pierce"; break;
                        case 3: attack = "puncture"; break;
                        case 4: attack = "strike"; break;
                        
                        invalid_default_case;
                    }
                } break;
                
                invalid_default_case;
            }
        }
        else
        {
            attack = "punch";
        }
        
        snprintf(result.str, sizeof(result.str), "You %s the %s", attack, defender->name);
    }
    else if(attacker->type == entity_type_enemy)
    {
        snprintf(result.str, sizeof(result.str), "The %s attacks you", attacker->name);
    }
    
    return(result);
}

internal void
remove_entity(entity_t *entity)
{
    memset(entity, 0, sizeof(entity_t));
}

internal void
kill_enemy_entity(game_state_t *game, dungeon_t *dungeon, entity_t *enemy)
{
    set_tile_occupied(dungeon->tiles, enemy->pos, false);
    
    tile remains = tile_none;
    if(enemy->e.is_red_blooded)
    {
        remains = random_number(&game->random,
                                tile_red_blood_puddle_1,
                                tile_red_blood_splatter_4);
    }
    else if(enemy->e.is_green_blooded)
    {
        remains = random_number(&game->random,
                                tile_green_blood_puddle_1,
                                tile_green_blood_splatter_4);
    }
    
    set_tile_remains_value(dungeon->tiles, enemy->pos, remains);
    remove_entity(enemy);
}

internal void
attack_entity(game_state_t *game, dungeon_t *dungeon, string_t *log,
              inventory_t *inventory, entity_t *attacker, entity_t *defender,
              u32 damage)
{
    defender->hp -= damage;
    if((s32)defender->hp <= 0)
    {
        defender->hp = 0;
        
        if(defender->type == entity_type_player)
        {
            add_log_string(log, "You are dead!", defender->name);
        }
        else
        {
            add_log_string(log, "##4 You kill the %s!", defender->name);
            kill_enemy_entity(game, dungeon, defender);
        }
    }
    else
    {
        string_t attack = entity_attack_message(game, attacker, defender, inventory);
        add_log_string(log, "%s for %u damage", attack.str, damage);
    }
}

internal void
entity_ai_update(game_state_t *game, entity_t *enemy)
{
    enemy->new_pos = enemy->pos;
    
    u32 direction = random_number(&game->random, dir_up, dir_down_right);
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
            enemy->e.is_flipped = true;
        } break;
        
        case dir_right:
        {
            ++enemy->new_pos.x;
            enemy->e.is_flipped = false;
        } break;
        
        case dir_up_left:
        {
            --enemy->new_pos.y;
            --enemy->new_pos.x;
            enemy->e.is_flipped = true;
        } break;
        
        case dir_up_right:
        {
            --enemy->new_pos.y;
            ++enemy->new_pos.x;
            enemy->e.is_flipped = false;
        } break;
        
        case dir_down_left:
        {
            ++enemy->new_pos.y;
            --enemy->new_pos.x;
            enemy->e.is_flipped = true;
        } break;
        
        case dir_down_right:
        {
            ++enemy->new_pos.y;
            ++enemy->new_pos.x;
            enemy->e.is_flipped = false;
        } break;
        
        invalid_default_case;
    }
}

internal void
update_entities(game_state_t *game,
                input_state_t *keyboard,
                entity_t *entities,
                dungeon_t *dungeon,
                item_t *items,
                string_t *log,
                inventory_t *inventory,
                u32 *enemy_levels)
{
    // Update Player
    b32 should_update_player = false;
    entity_t *player = &entities[0];
    player->action_speed = 0.0f;
    
#if MOONBREATH_SLOW
    if(is_input_valid(&keyboard[key_debug_fov]))
    {
        debug_fov = !debug_fov;
        update_fov(dungeon, player);
    }
    else if(is_input_valid(&keyboard[key_debug_player_traversable_check]))
    {
        debug_player_traversable = !debug_player_traversable;
        should_update_player = true;
    }
    // NOTE(rami): We need to check this manually
    // so that it works as an expected toggle.
    else if(keyboard[key_debug_has_been_up_check].is_down &&
            keyboard[key_debug_has_been_up_check].has_been_up)
    {
        keyboard[key_debug_has_been_up_check].has_been_up = false;
        debug_has_been_up = !debug_has_been_up;
        should_update_player = true;
    }
    else
#endif
    
    if(is_input_valid(&keyboard[key_inventory]))
    {
        inventory->is_open = !inventory->is_open;
        inventory->current = V2u(0, 0);
        
        inventory->is_item_being_moved = false;
        inventory->moving_item_src_index = (u32)-1;
        inventory->moving_item_dest_index = (u32)-1;
    }
    else if(inventory->is_open)
    {
        if(is_input_valid(&keyboard[key_move_up]))
        {
            if(inventory->current.y > 0)
            {
                --inventory->current.y;
            }
            else
            {
                inventory->current.y = INVENTORY_HEIGHT - 1;
            }
        }
        else if(is_input_valid(&keyboard[key_move_down]))
        {
            if((inventory->current.y + 1) < INVENTORY_HEIGHT)
            {
                ++inventory->current.y;
            }
            else
            {
                inventory->current.y = 0;
            }
        }
        else if(is_input_valid(&keyboard[key_move_left]))
        {
            if(inventory->current.x > 0)
            {
                --inventory->current.x;
            }
            else
            {
                inventory->current.x = INVENTORY_WIDTH - 1;
            }
        }
        else if(is_input_valid(&keyboard[key_move_right]))
        {
            if((inventory->current.x + 1) < INVENTORY_WIDTH)
            {
                ++inventory->current.x;
            }
            else
            {
                inventory->current.x = 0;
            }
        }
        else if(is_input_valid(&keyboard[key_drop_item]))
        {
            if(!inventory->is_item_being_moved)
            {
                remove_inventory_item(1, player, log, inventory);
            }
        }
        else if(is_input_valid(&keyboard[key_equip_item]))
        {
            if(!inventory->is_item_being_moved)
            {
                u32 slot_index = index_from_v2u(inventory->current, INVENTORY_WIDTH);
                item_t *item = inventory->slots[slot_index];
                if(item && (item->type == item_type_weapon || item->type == item_type_armor))
                {
                    if(item->is_equipped)
                    {
                        unequip_item(item, player, log);
                    }
                    else
                    {
                        u32_bool_t slot_index = equipped_item_slot_index(item->slot, inventory);
                        if(slot_index.success)
                        {
                            unequip_item(inventory->slots[slot_index.value], player, log);
                        }
                        
                        equip_item(item, player, log);
                    }
                }
            }
        }
        else if(is_input_valid(&keyboard[key_consume_item]))
        {
            u32 slot_index = index_from_v2u(inventory->current, INVENTORY_WIDTH);
            item_t *item = inventory->slots[slot_index];
            if(item && item->in_inventory)
            {
                if(item->type == item_type_potion || item->type == item_type_scroll)
                {
                    if(item->c.effect == item_effect_healing)
                    {
                        if(heal_entity(player, item->c.effect_amount))
                        {
                            add_log_string(log, "##7 The potion heals you for %d hitpoints", item->c.effect_amount);
                            
                            remove_inventory_item(0, player, log, inventory);
                            remove_game_item(item);
                        }
                        else
                        {
                            add_log_string(log, "You do not feel the need to drink this");
                        }
                    }
                }
            }
        }
        else if(is_input_valid(&keyboard[key_move_item]))
        {
            inventory->moving_item_dest_index = index_from_v2u(inventory->current, INVENTORY_WIDTH);
            
            if(inventory->is_item_being_moved)
            {
                if(inventory->moving_item_src_index != inventory->moving_item_dest_index)
                {
                    if(inventory->slots[inventory->moving_item_dest_index])
                    {
                        item_t *temp = inventory->slots[inventory->moving_item_dest_index];
                        
                        inventory->slots[inventory->moving_item_dest_index] = inventory->slots[inventory->moving_item_src_index];
                        inventory->slots[inventory->moving_item_src_index] = temp;
                    }
                    else
                    {
                        inventory->slots[inventory->moving_item_dest_index] = inventory->slots[inventory->moving_item_src_index];
                        inventory->slots[inventory->moving_item_src_index] = 0;
                    }
                    
                    inventory->is_item_being_moved = false;
                }
                
                inventory->is_item_being_moved = false;
                inventory->moving_item_src_index = (u32)-1;
                inventory->moving_item_dest_index = (u32)-1;
            }
            else
            {
                if(inventory->slots[inventory->moving_item_dest_index])
                {
                    inventory->is_item_being_moved = true;
                    inventory->moving_item_src_index = inventory->moving_item_dest_index;
                }
            }
        }
    }
    else
    {
        if(is_input_valid(&keyboard[key_move_up]))
        {
            player->new_pos = V2u(player->pos.x, player->pos.y - 1);
            should_update_player = true;
        }
        else if(is_input_valid(&keyboard[key_move_down]))
        {
            player->new_pos = V2u(player->pos.x, player->pos.y + 1);
            should_update_player = true;
        }
        else if(is_input_valid(&keyboard[key_move_left]))
        {
            player->new_pos = V2u(player->pos.x - 1, player->pos.y);
            should_update_player = true;
        }
        else if(is_input_valid(&keyboard[key_move_right]))
        {
            player->new_pos = V2u(player->pos.x + 1, player->pos.y);
            should_update_player = true;
        }
        else if(is_input_valid(&keyboard[key_move_up_left]))
        {
            player->new_pos = V2u(player->pos.x - 1, player->pos.y - 1);
            should_update_player = true;
        }
        else if(is_input_valid(&keyboard[key_move_up_right]))
        {
            player->new_pos = V2u(player->pos.x + 1, player->pos.y - 1);
            should_update_player = true;
        }
        else if(is_input_valid(&keyboard[key_move_down_left]))
        {
            player->new_pos = V2u(player->pos.x - 1, player->pos.y + 1);
            should_update_player = true;
        }
        else if(is_input_valid(&keyboard[key_move_down_right]))
        {
            player->new_pos = V2u(player->pos.x + 1, player->pos.y + 1);
            should_update_player = true;
        }
        else if(is_input_valid(&keyboard[key_pick_up_item]))
        {
            add_inventory_item(items, inventory, player, log);
        }
        else if(is_input_valid(&keyboard[key_ascend]))
        {
            if(is_tile_value(dungeon->tiles, player->pos, tile_stone_path_up))
            {
                game->state = game_state_exit;
            }
            else
            {
                add_log_string(log, "There's nothing here that leads upwards");
            }
        }
        else if(is_input_valid(&keyboard[key_descend]))
        {
            if(is_tile_value(dungeon->tiles, player->pos, tile_stone_path_down))
            {
                if(dungeon->level < MAX_DUNGEON_LEVEL)
                {
                    add_log_string(log, "You descend further.. Level %u", dungeon->level + 1);
                    create_dungeon(game, dungeon, entities, items, enemy_levels);
                }
                else
                {
                    game->state = game_state_exit;
                }
            }
            else
            {
                add_log_string(log, "There's nothing here that leads downwards");
            }
        }
        else if(is_input_valid(&keyboard[key_wait]))
        {
            should_update_player = true;
            player->action_speed = 1.0f;
        }
    }
    
    if(should_update_player)
    {
#if MOONBREATH_SLOW
        if(debug_player_traversable)
        {
            if(is_pos_in_dungeon(dungeon, player->new_pos))
            {
                move_entity(dungeon, player);
            }
        }
        else
#endif
        if(is_pos_in_dungeon(dungeon, player->new_pos))
        {
            if(!V2u_equal(player->pos, player->new_pos) &&
               is_tile_occupied(dungeon->tiles, player->new_pos))
            {
                for(u32 entity_index = 1;
                    entity_index < MAX_ENTITIES;
                    ++entity_index)
                {
                    entity_t *enemy = &entities[entity_index];
                    if(V2u_equal(player->new_pos, enemy->pos))
                    {
                        u32 player_hit_chance = 15 + (player->dexterity / 2);
                        player_hit_chance += player->p.accuracy;
                        
                        if(does_entity_hit(game, player_hit_chance, enemy->evasion))
                        {
                            attack_entity(game, dungeon, log, inventory, player, enemy, player->damage);
                        }
                        else
                        {
                            add_log_string(log, "##2 Your attack misses");
                        }
                        
                        enemy->e.in_combat = true;
                        
#if 0
                        // NOTE(rami): Hit Test
                        printf("player_hit_chance: %u\n", player_hit_chance);
                        printf("entity evasion: %u\n", entity->evasion);
                        
                        u32 hit_count = 0;
                        u32 miss_count = 0;
                        for(u32 i = 0; i < 100; ++i)
                        {
                            u32 roll = random_number(0, player_hit_chance);
                            if(will_entity_hit(player_hit_chance, entity->evasion))
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
                        player->action_speed = player->p.attack_speed;
                        break;
                    }
                }
            }
            else
            {
                if(is_tile_value(dungeon->tiles, player->new_pos, tile_stone_door_closed))
                {
                    add_log_string(log, "You push the door open..");
                    set_tile_value(dungeon->tiles, player->new_pos, tile_stone_door_open);
                    player->action_speed = 1.0f;
                }
                else if(is_tile_traversable(dungeon->tiles, player->new_pos))
                {
                    move_entity(dungeon, player);
                    player->action_speed = 1.0f;
                }
            }
            
            // NOTE(Rami): Changing the new position must be based on the current position.
            player->new_pos = player->pos;
            game->time += player->action_speed;
        }
    }
    
    if(player->action_speed)
    {
        update_pathfind_map(dungeon, player);
        update_fov(dungeon, player);
        
        // Update Enemies
        for(u32 entity_index = 1;
            entity_index < MAX_ENTITIES;
            ++entity_index)
        {
            entity_t *enemy = &entities[entity_index];
            if(enemy->type == entity_type_enemy)
            {
                enemy->e.wait_timer += player->action_speed;
                u32 action_count = (u32)(enemy->e.wait_timer / enemy->action_speed);
#if 0
                printf("player->action_speed: %.1f\n", player->action_speed);
                printf("wait_timer: %.1f\n", enemy->e.wait_timer);
                printf("action_count: %u\n\n", action_count);
#endif
                
                if(action_count)
                {
                    enemy->e.wait_timer = 0.0f;
                    
                    while(action_count--)
                    {
                        if(enemy->e.in_combat)
                        {
                            // NOTE(rami): Turn enemy towards target.
                            enemy->e.is_flipped = (player->pos.x < enemy->pos.x);
                            
                            v2u next_pos = next_pathfind_pos(dungeon, dungeon->pathfind_map, dungeon->width, enemy, player);
                            if(V2u_equal(next_pos, player->pos))
                            {
                                if(does_entity_hit(game, 15, player->evasion))
                                {
                                    attack_entity(game, dungeon, log, inventory, enemy, player, enemy->damage);
                                }
                                else
                                {
                                    add_log_string(log, "##2 You dodge the attack", enemy->name);
                                }
                            }
                            else
                            {
                                enemy->new_pos = next_pos;
                            }
                        }
                        else
                        {
                            // NOTE(Rami): I guess right now this could be, if the
                            // player sees the enemy, then the enemy sees the player.
                            
                            // If the enemy is aggressive and seen, it goes into
                            // combat.
                            
                            // If the enemy is in combat and not seen, it goes out
                            // of combat
                            
#if MOONBREATH_SLOW
                            if(!debug_fov && is_seen(dungeon, enemy->pos))
#else
                            if(is_seen(dungeon, enemy->pos))
#endif
                            {
                                //enemy->e.in_combat = true;
                                entity_ai_update(game, enemy);
                            }
                            else
                            {
                                entity_ai_update(game, enemy);
                            }
                        }
                        
                        // NOTE(Rami): Calling move_entity() will set the pos of
                        // the entity to new pos. Before that happens we want to
                        // save the pos into enemy_pos_for_ghost. The reason we
                        // save it is because the code that renders the enemy
                        // ghosts needs it.
                        enemy->e.enemy_pos_for_ghost = enemy->pos;
                        
                        if(is_tile_traversable(dungeon->tiles, enemy->new_pos) &&
                           !is_tile_occupied(dungeon->tiles, enemy->new_pos))
                        {
                            move_entity(dungeon, enemy);
                        }
                    }
                }
            }
        }
    }
}

internal void
render_entities(game_state_t *game, dungeon_t *dungeon, entity_t *entities, inventory_t *inventory, assets_t *assets)
{
    // Render Player
    entity_t *player = &entities[0];
    
    v4u src = tile_rect(player->tile);
    v4u dest = game_dest(game, player->pos);
    SDL_RenderCopy(game->renderer, assets->sprite_sheet.tex, (SDL_Rect *)&src, (SDL_Rect *)&dest);
    
    // Render Player Items
    for(u32 item_slot_index = 1;
        item_slot_index < item_slot_total;
        ++item_slot_index)
    {
        for(u32 inventory_slot_index = 0;
            inventory_slot_index < array_count(inventory->slots);
            ++inventory_slot_index)
        {
            item_t *item = inventory->slots[inventory_slot_index];
            if(item && item->is_equipped && item->slot == item_slot_index)
            {
                v4u src = tile_rect(item->tile);
                SDL_RenderCopy(game->renderer, assets->wearable_item_tileset.tex, (SDL_Rect *)&src, (SDL_Rect *)&dest);
                
                break;
            }
        }
    }
    
    // Render Enemies
    for(u32 entity_index = 1;
        entity_index < MAX_ENTITIES;
        ++entity_index)
    {
        entity_t *enemy = &entities[entity_index];
        if(enemy->type == entity_type_enemy)
        {
            // TODO(Rami): There might be a bug with ghosts. If an enemy has a ghost,
            // and you see it's location then could it be that another one gets created
            // where the enemy currently is, when that shouldn't happen until we actually
            // see the enemy again.
            
            if(is_seen(dungeon, enemy->pos))
            {
                enemy->e.has_been_seen = true;
                enemy->e.is_ghost_saved = false;
                
                v4u src = tile_rect(enemy->tile);
                v4u dest = game_dest(game, enemy->pos);
                SDL_RenderCopyEx(game->renderer, assets->sprite_sheet.tex, (SDL_Rect *)&src, (SDL_Rect *)&dest, 0, 0, enemy->e.is_flipped);
                
                // Render Enemy HP Bar
                if(enemy->e.in_combat &&
                   enemy->hp != enemy->max_hp)
                {
                    // HP Bar Outside
                    set_render_color(game, color_black);
                    v4u hp_bar_outside = {dest.x, dest.y + 33, 32, 4};
                    SDL_RenderDrawRect(game->renderer, (SDL_Rect *)&hp_bar_outside);
                    
                    // HP Bar Inside
                    set_render_color(game, color_dark_red);
                    u32 hp_bar_inside_w = ratio(enemy->hp, enemy->max_hp, 30);
                    v4u hp_bar_inside = {dest.x + 1, dest.y + 34, hp_bar_inside_w, 2};
                    SDL_RenderFillRect(game->renderer, (SDL_Rect *)&hp_bar_inside);
                }
            }
            else
            {
                if(enemy->e.has_been_seen)
                {
                    if(enemy->e.is_ghost_saved)
                    {
                        if(is_seen(dungeon, enemy->e.ghost_pos))
                        {
                            enemy->e.has_been_seen = false;
                            enemy->e.is_ghost_saved = false;
                        }
                        else
                        {
                            v4u src = tile_rect(enemy->tile);
                            v4u dest = game_dest(game, enemy->e.ghost_pos);
                            
                            SDL_SetTextureColorMod(assets->sprite_sheet.tex, 127, 127, 127);
                            SDL_RenderCopyEx(game->renderer, assets->sprite_sheet.tex, (SDL_Rect *)&src, (SDL_Rect *)&dest, 0, 0, enemy->e.ghost_is_flipped);
                            SDL_SetTextureColorMod(assets->sprite_sheet.tex, 255, 255, 255);
                        }
                    }
                    else
                    {
                        // NOTE(Rami): If you move into a pos where you can't
                        // see the enemy then the ghost should be placed on that last
                        // seen pos.
                        
                        // If the enemy moves into a pos where you can't see it
                        // then the ghost should be placed on the new pos of the
                        // enemy.
                        if(is_seen(dungeon, enemy->e.enemy_pos_for_ghost))
                        {
                            enemy->e.ghost_pos = enemy->new_pos;
                        }
                        else
                        {
                            enemy->e.ghost_pos = enemy->e.enemy_pos_for_ghost;
                        }
                        
                        enemy->e.ghost_is_flipped = enemy->e.is_flipped;
                        enemy->e.is_ghost_saved = true;
                    }
                }
            }
        }
    }
}

internal void
add_player_entity(entity_t *player)
{
    player->id = entity_id_player;
    player->type = entity_type_player;
    
    strcpy(player->name, "Name");
    player->max_hp = 10;
    player->hp = 5;
    player->w = player->h = 32;
    
    player->strength = 10;
    player->intelligence = 10;
    player->dexterity = 10;
    
    player->damage = 1;
    player->p.accuracy = 2;
    player->p.attack_speed = 1.0f;
    player->evasion = 10;
    
    player->p.fov = 6;
}

internal void
add_enemy_entity(entity_t *entities, dungeon_t *dungeon, u32 *enemy_levels, entity_id id, u32 x, u32 y)
{
    assert(id != entity_id_none && id != entity_id_player);
    
    for(u32 entity_index = 1;
        entity_index < MAX_ENTITIES;
        ++entity_index)
    {
        entity_t *enemy = &entities[entity_index];
        if(!enemy->type)
        {
            enemy->id = id;
            enemy->type = entity_type_enemy;
            // TODO(rami): These need to be defined per enemy.
            enemy->action_speed = 1.0f;
            enemy->damage = 2;
            
            switch(id)
            {
                case entity_id_rat:
                {
                    strcpy(enemy->name, "Rat");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(1, 1);
                    
                    enemy->evasion = 4;
                    
                    enemy->action_speed = 1;
                    enemy->e.level = enemy_levels[id];
                    
                    enemy->e.is_red_blooded = true;
                } break;
                
                case entity_id_snail:
                {
                    strcpy(enemy->name, "Snail");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(0, 1);
                    
                    enemy->evasion = 4;
                    
                    enemy->action_speed = 1;
                    enemy->e.level = enemy_levels[id];
                    
                    enemy->e.is_red_blooded = true;
                } break;
                
                case entity_id_slime:
                {
                    strcpy(enemy->name, "Slime");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(1, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->action_speed = 1;
                    enemy->e.level = enemy_levels[id];
                    
                    enemy->e.is_green_blooded = true;
                } break;
                
                case entity_id_giant_slime:
                {
                    strcpy(enemy->name, "Giant Slime");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(2, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->action_speed = 1;
                    enemy->e.level = enemy_levels[id];
                    
                    enemy->e.is_green_blooded = true;
                } break;
                
                case entity_id_skeleton:
                {
                    strcpy(enemy->name, "Skeleton");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(3, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->action_speed = 1;
                    enemy->e.level = enemy_levels[id];
                    
                    enemy->e.is_made_of_bone = true;
                } break;
                
                case entity_id_skeleton_warrior:
                {
                    strcpy(enemy->name, "Skeleton Warrior");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(4, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->action_speed = 1;
                    enemy->e.level = enemy_levels[id];
                    
                    enemy->e.is_made_of_bone = true;
                } break;
                
                case entity_id_orc:
                {
                    strcpy(enemy->name, "Orc");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(5, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->action_speed = 1;
                    enemy->e.level = enemy_levels[id];
                    
                    enemy->e.is_red_blooded = true;
                } break;
                
                case entity_id_cave_bat:
                {
                    strcpy(enemy->name, "Cave Bat");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(6, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->action_speed = 1;
                    enemy->e.level = enemy_levels[id];
                    
                    enemy->e.is_red_blooded = true;
                } break;
                
                case entity_id_python:
                {
                    strcpy(enemy->name, "Python");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(7, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->action_speed = 1;
                    enemy->e.level = enemy_levels[id];
                    enemy->type = entity_type_enemy;
                    
                    enemy->e.is_red_blooded = true;
                } break;
                
                case entity_id_kobold:
                {
                    strcpy(enemy->name, "Kobold");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(8, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->action_speed = 1;
                    enemy->e.level = enemy_levels[id];
                    enemy->type = entity_type_enemy;
                    
                    enemy->e.is_red_blooded = true;
                } break;
                
                case entity_id_ogre:
                {
                    strcpy(enemy->name, "Ogre");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(9, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->action_speed = 1;
                    enemy->e.level = enemy_levels[id];
                    enemy->type = entity_type_enemy;
                    
                    enemy->e.is_red_blooded = true;
                } break;
                
                case entity_id_tormentor:
                {
                    strcpy(enemy->name, "Tormentor");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(10, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->action_speed = 1;
                    enemy->e.level = enemy_levels[id];
                    enemy->type = entity_type_enemy;
                    
                    enemy->e.is_made_of_bone = true;
                } break;
                
                case entity_id_imp:
                {
                    strcpy(enemy->name, "Imp");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(11, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->action_speed = 1;
                    enemy->e.level = enemy_levels[id];
                    enemy->type = entity_type_enemy;
                    
                    enemy->e.is_red_blooded = true;
                } break;
                
                case entity_id_giant_demon:
                {
                    strcpy(enemy->name, "Giant Demon");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(12, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->action_speed = 1;
                    enemy->e.level = enemy_levels[id];
                    enemy->type = entity_type_enemy;
                    
                    enemy->e.is_red_blooded = true;
                } break;
                
                case entity_id_hellhound:
                {
                    strcpy(enemy->name, "Hellhound");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(13, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->action_speed = 1;
                    enemy->e.level = enemy_levels[id];
                    enemy->type = entity_type_enemy;
                    
                    enemy->e.is_red_blooded = true;
                } break;
                
                case entity_id_undead_elf_warrior:
                {
                    strcpy(enemy->name, "Undead Elf Warrior");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(14, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->action_speed = 1;
                    enemy->e.level = enemy_levels[id];
                    enemy->type = entity_type_enemy;
                    
                    enemy->e.is_made_of_bone = true;
                } break;
                
                case entity_id_shadow_thief:
                {
                    strcpy(enemy->name, "Shadow Thief");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(15, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->action_speed = 1;
                    enemy->e.level = enemy_levels[id];
                    enemy->type = entity_type_enemy;
                    
                    enemy->e.is_red_blooded = true;
                } break;
                
                case entity_id_goblin:
                {
                    strcpy(enemy->name, "Goblin");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(16, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->action_speed = 1;
                    enemy->e.level = enemy_levels[id];
                    enemy->type = entity_type_enemy;
                    
                    enemy->e.is_red_blooded = true;
                } break;
                
                case entity_id_goblin_warrior:
                {
                    strcpy(enemy->name, "Goblin Warrior");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(17, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->action_speed = 1;
                    enemy->e.level = enemy_levels[id];
                    enemy->type = entity_type_enemy;
                    
                    enemy->e.is_red_blooded = true;
                } break;
                
                case entity_id_viper:
                {
                    strcpy(enemy->name, "Viper");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(18, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->action_speed = 1;
                    enemy->e.level = enemy_levels[id];
                    enemy->type = entity_type_enemy;
                    
                    enemy->e.is_red_blooded = true;
                } break;
                
                case entity_id_scarlet_kingsnake:
                {
                    strcpy(enemy->name, "Scarlet Kingsnake");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(19, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->action_speed = 1;
                    enemy->e.level = enemy_levels[id];
                    enemy->type = entity_type_enemy;
                    
                    enemy->e.is_red_blooded = true;
                } break;
                
                case entity_id_stray_dog:
                {
                    strcpy(enemy->name, "Stray Dog");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(20, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->action_speed = 1;
                    enemy->e.level = enemy_levels[id];
                    enemy->type = entity_type_enemy;
                    
                    enemy->e.is_red_blooded = true;
                } break;
                
                case entity_id_wolf:
                {
                    strcpy(enemy->name, "Wolf");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(21, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->action_speed = 1;
                    enemy->e.level = enemy_levels[id];
                    enemy->type = entity_type_enemy;
                    
                    enemy->e.is_red_blooded = true;
                } break;
                
                case entity_id_green_mamba:
                {
                    strcpy(enemy->name, "Green Mamba");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(22, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->action_speed = 1;
                    enemy->e.level = enemy_levels[id];
                    enemy->type = entity_type_enemy;
                    
                    enemy->e.is_red_blooded = true;
                } break;
                
                case entity_id_floating_eye:
                {
                    strcpy(enemy->name, "Floating Eye");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(23, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->action_speed = 1;
                    enemy->e.level = enemy_levels[id];
                    enemy->type = entity_type_enemy;
                    
                    enemy->e.is_red_blooded = true;
                } break;
                
                case entity_id_devourer:
                {
                    strcpy(enemy->name, "Devourer");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(24, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->action_speed = 1;
                    enemy->e.level = enemy_levels[id];
                    enemy->type = entity_type_enemy;
                    
                    enemy->e.is_red_blooded = true;
                } break;
                
                case entity_id_ghoul:
                {
                    strcpy(enemy->name, "Ghoul");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(25, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->action_speed = 1;
                    enemy->e.level = enemy_levels[id];
                    enemy->type = entity_type_enemy;
                    
                    enemy->e.is_made_of_bone = true;
                } break;
                
                case entity_id_cyclops:
                {
                    strcpy(enemy->name, "Cyclops");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(26, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->action_speed = 1;
                    enemy->e.level = enemy_levels[id];
                    enemy->type = entity_type_enemy;
                    
                    enemy->e.is_red_blooded = true;
                } break;
                
                case entity_id_dwarwen_warrior:
                {
                    strcpy(enemy->name, "Dwarwen Warrior");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(27, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->action_speed = 1;
                    enemy->e.level = enemy_levels[id];
                    enemy->type = entity_type_enemy;
                    
                    enemy->e.is_red_blooded = true;
                } break;
                
                case entity_id_black_knight:
                {
                    strcpy(enemy->name, "Black Knight");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(28, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->action_speed = 1;
                    enemy->e.level = enemy_levels[id];
                    enemy->type = entity_type_enemy;
                } break;
                
                case entity_id_cursed_black_knight:
                {
                    strcpy(enemy->name, "Cursed Black Knight");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(29, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->action_speed = 1;
                    enemy->e.level = enemy_levels[id];
                    enemy->type = entity_type_enemy;
                } break;
                
                case entity_id_treant:
                {
                    strcpy(enemy->name, "Treant");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(30, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->action_speed = 1;
                    enemy->e.level = enemy_levels[id];
                    enemy->type = entity_type_enemy;
                } break;
                
                case entity_id_minotaur:
                {
                    strcpy(enemy->name, "Minotaur");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(31, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->action_speed = 1;
                    enemy->e.level = enemy_levels[id];
                    enemy->type = entity_type_enemy;
                    
                    enemy->e.is_red_blooded = true;
                } break;
                
                case entity_id_centaur_warrior:
                {
                    strcpy(enemy->name, "Centaur Warrior");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(32, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->action_speed = 1;
                    enemy->e.level = enemy_levels[id];
                    enemy->type = entity_type_enemy;
                    
                    enemy->e.is_red_blooded = true;
                } break;
                
                case entity_id_centaur:
                {
                    strcpy(enemy->name, "Centaur");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(33, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->action_speed = 1;
                    enemy->e.level = enemy_levels[id];
                    enemy->type = entity_type_enemy;
                    
                    enemy->e.is_red_blooded = true;
                } break;
                
                case entity_id_frost_shards:
                {
                    strcpy(enemy->name, "Frost Shards");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(34, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->action_speed = 1;
                    enemy->e.level = enemy_levels[id];
                    enemy->type = entity_type_enemy;
                } break;
                
                case entity_id_frost_walker:
                {
                    strcpy(enemy->name, "Frost Walker");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(35, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->action_speed = 1;
                    enemy->e.level = enemy_levels[id];
                    enemy->type = entity_type_enemy;
                } break;
                
                case entity_id_griffin:
                {
                    strcpy(enemy->name, "Griffin");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(36, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->action_speed = 1;
                    enemy->e.level = enemy_levels[id];
                    enemy->type = entity_type_enemy;
                    
                    enemy->e.is_red_blooded = true;
                } break;
                
                case entity_id_spectre:
                {
                    strcpy(enemy->name, "Spectre");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(37, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->action_speed = 1;
                    enemy->e.level = enemy_levels[id];
                    enemy->type = entity_type_enemy;
                } break;
                
                case entity_id_flying_skull:
                {
                    strcpy(enemy->name, "Flying Skull");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(38, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->action_speed = 1;
                    enemy->e.level = enemy_levels[id];
                    enemy->type = entity_type_enemy;
                } break;
                
                case entity_id_brimstone_imp:
                {
                    strcpy(enemy->name, "Brimstone Imp");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(39, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->action_speed = 1;
                    enemy->e.level = enemy_levels[id];
                    enemy->type = entity_type_enemy;
                    
                    enemy->e.is_red_blooded = true;
                } break;
                
                invalid_default_case;
            }
            
            set_tile_occupied(dungeon->tiles, enemy->pos, true);
            return;
        }
    }
    
    assert(false);
}