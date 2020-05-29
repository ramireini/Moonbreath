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
        
        slot_t slot = equipped_inventory_slot_from_item_equip_slot(item_equip_slot_first_hand, inventory);
        if(slot.item)
        {
            switch(slot.item->id)
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
        add_log_string(log, "%s for %u damage.", attack.str, damage);
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
                entity_t *player,
                entity_t *entities,
                dungeon_t *dungeon,
                item_t *items,
                string_t *log,
                inventory_t *inventory,
                u32 *enemy_levels)
{
    // NOTE(Rami): Update Player
    b32 should_update_player = false;
    player->action_speed = 0.0f;
    
#if MOONBREATH_SLOW
    if(is_input_valid(&keyboard[key_toggle_fov]))
    {
        debug_fov = !debug_fov;
        update_fov(dungeon, player);
    }
    else if(is_input_valid(&keyboard[key_toggle_traversable_check]))
    {
        debug_traversable = !debug_traversable;
        should_update_player = true;
    }
    // NOTE(rami): We need to check this manually
    // so that it works as an expected toggle.
    else if(keyboard[key_toggle_has_been_up_check].is_down &&
            keyboard[key_toggle_has_been_up_check].has_been_up)
    {
        keyboard[key_toggle_has_been_up_check].has_been_up = false;
        debug_has_been_up = !debug_has_been_up;
        should_update_player = true;
    }
    else if(is_input_valid(&keyboard[key_toggle_identify]))
    {
        if(inventory->is_open)
        {
            item_t *item = inventory_item_from_pos(inventory, inventory->current);
            if(item->type == item_type_scroll)
            {
                if(item->is_identified)
                {
                    item->tile = V2u(9, 0);
                }
                else
                {
                    switch(item->id)
                    {
                        case item_scroll_of_identify: item->tile = V2u(9, 1); break;
                        case item_scroll_of_infuse_weapon: item->tile = V2u(9, 2); break;
                        case item_scroll_of_enchant_weapon: item->tile = V2u(9, 3); break;
                        case item_scroll_of_enchant_armor: item->tile = V2u(9, 4); break;
                        case item_scroll_of_magic_mapping: item->tile = V2u(9, 5); break;
                        
                        invalid_default_case;
                    }
                }
            }
            
            item->is_identified = !item->is_identified;
        }
    }
    else
#endif
    
    if(is_input_valid(&keyboard[key_move_up]))
    {
        if(inventory->is_open)
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
        else
        {
            player->new_pos = V2u(player->pos.x, player->pos.y - 1);
            should_update_player = true;
        }
    }
    else if(is_input_valid(&keyboard[key_move_down]))
    {
        if(inventory->is_open)
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
        else
        {
            player->new_pos = V2u(player->pos.x, player->pos.y + 1);
            should_update_player = true;
        }
    }
    else if(is_input_valid(&keyboard[key_move_left]))
    {
        if(inventory->is_open)
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
        else
        {
            player->new_pos = V2u(player->pos.x - 1, player->pos.y);
            should_update_player = true;
        }
    }
    else if(is_input_valid(&keyboard[key_move_right]))
    {
        if(inventory->is_open)
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
        else
        {
            player->new_pos = V2u(player->pos.x + 1, player->pos.y);
            should_update_player = true;
        }
    }
    else if(is_input_valid(&keyboard[key_move_up_left]))
    {
        if(!inventory->is_open)
        {
            player->new_pos = V2u(player->pos.x - 1, player->pos.y - 1);
            should_update_player = true;
        }
    }
    else if(is_input_valid(&keyboard[key_move_up_right]))
    {
        if(!inventory->is_open)
        {
            player->new_pos = V2u(player->pos.x + 1, player->pos.y - 1);
            should_update_player = true;
        }
    }
    else if(is_input_valid(&keyboard[key_move_down_left]))
    {
        if(!inventory->is_open)
        {
            player->new_pos = V2u(player->pos.x - 1, player->pos.y + 1);
            should_update_player = true;
        }
    }
    else if(is_input_valid(&keyboard[key_move_down_right]))
    {
        if(!inventory->is_open)
        {
            player->new_pos = V2u(player->pos.x + 1, player->pos.y + 1);
            should_update_player = true;
        }
    }
    else if(is_input_valid(&keyboard[key_toggle_inventory]))
    {
        // TODO(Rami): Next, think about the cancelling a little bit more.
        // TODO(Rami): Complete this.
        if(inventory->is_item_identifying)
        {
            inventory->has_player_been_asked = true;
            add_log_string(log, "Cancel and waste the item?, [%c] Yes.", game->keybinds[key_yes]);
        }
        else
        {
            inventory->is_open = !inventory->is_open;
            inventory->has_player_been_asked = false;
            inventory->current = V2u(0, 0);
            
            reset_inventory_item_moving(inventory);
            reset_inventory_item_identifying(inventory);
        }
    }
    else if(is_input_valid(&keyboard[key_equip_or_consume_item]))
    {
        if(!inventory->is_item_identifying)
        {
            item_t *item = inventory_item_from_pos(inventory, inventory->current);
            if(item && !inventory->is_item_moving)
            {
                if(is_item_consumable(item->type))
                {
                    switch(item->id)
                    {
                        // NOTE(Rami): Potions
                        case item_potion_of_healing:
                        {
                            if(heal_entity(player, item->c.effect_amount))
                            {
                                add_log_string(log, "##7 The potion heals you for %d hitpoints.", item->c.effect_amount);
                                
                                slot_t slot = inventory_slot_from_pos(inventory, inventory->current);
                                if(slot.item)
                                {
                                    remove_item_from_inventory_and_game(slot, player, log, inventory);
                                }
                            }
                            else
                            {
                                add_log_string(log, "You do not feel the need to drink this.");
                            }
                        } break;
                        
                        // NOTE(Rami): Scrolls
                        case item_scroll_of_identify:
                        {
                            // TODO(Rami): Need to make it so that when you read a
                            // scroll that can be used on something, like identify
                            // or enchant etc. you have to use it on something or
                            // cancel and the scroll gets destroyed.
                            
                            // NOTE(Rami): Try first with both lower and uppercase allowed.
                            // add_log_string(log, "Cancel and waste the item? [Y]es [N]o");
                            
                            if(item->is_identified)
                            {
                                if(!inventory->is_item_identifying)
                                {
                                    add_log_string(log, "You read the Scroll of Identify, choose an item to use it on.");
                                }
                            }
                            else
                            {
                                add_log_string(log, "It's a Scroll of Identify! Choose an item to use it on.");
                                // TODO(Rami): Remember to change the tile like this for every scroll.
                                item->tile = V2u(9, 1);
                                
                                item->is_identified = true;
                            }
                            
                            // TODO(Rami): Now that the inventory is in this mode,
                            // we need to make it work properly.
                            inventory->is_item_identifying = true;
                            inventory->identifying_item_index = inventory_index_from_pos(inventory->current);
                        } break;
                        
                        invalid_default_case;
                    }
                }
                else
                {
                    if(item->is_equipped)
                    {
                        unequip_item(item, player, log);
                    }
                    else
                    {
                        slot_t slot = equipped_inventory_slot_from_item_equip_slot(item->equip_slot, inventory);
                        if(slot.item)
                        {
                            unequip_item(slot.item, player, log);
                        }
                        
                        if(!item->is_identified)
                        {
                            item->is_identified = true;
                        }
                        
                        equip_item(item, player, log);
                    }
                }
            }
        }
    }
    else if(is_input_valid(&keyboard[key_pick_up_or_drop_item]))
    {
        if(inventory->is_open)
        {
            if(!inventory->is_item_moving &&
               !inventory->is_item_identifying)
            {
                slot_t slot = inventory_slot_from_pos(inventory, inventory->current);
                if(slot.item)
                {
                    remove_item_from_inventory(slot, player, log, inventory);
                    
                    // NOTE(Rami): Add drop message.
                    if(slot.item->is_identified)
                    {
                        string_t item_name = full_item_name(slot.item);
                        add_log_string(log, "You drop the %s%s%s.",
                                       item_rarity_color_code(slot.item->rarity),
                                       item_name.str,
                                       end_color());
                    }
                    else
                    {
                        add_log_string(log, "You drop the %s%s%s.",
                                       item_rarity_color_code(slot.item->rarity),
                                       item_id_text(slot.item),
                                       end_color());
                    }
                }
            }
        }
        else
        {
            pick_up_item(items, inventory, player, log);
        }
    }
    else if(is_input_valid(&keyboard[key_identify_item]))
    {
        if(inventory->is_item_identifying)
        {
            item_t *item = inventory_item_from_pos(inventory, inventory->current);
            if(item && !item->is_identified)
            {
                slot_t slot = {inventory->identifying_item_index, inventory->slots[slot.index]};
                remove_item_from_inventory_and_game(slot, player, log, inventory);
                
                item->is_identified = true;
                reset_inventory_item_identifying(inventory);
            }
        }
    }
    else if(is_input_valid(&keyboard[key_move_item]))
    {
        if(inventory->is_open)
        {
            if(!inventory->is_item_identifying)
            {
                inventory->moving_item_dest_index = inventory_index_from_pos(inventory->current);
                
                if(inventory->is_item_moving)
                {
                    if(inventory->moving_item_src_index !=
                       inventory->moving_item_dest_index)
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
                    }
                    
                    reset_inventory_item_moving(inventory);
                }
                else
                {
                    if(inventory->slots[inventory->moving_item_dest_index])
                    {
                        inventory->is_item_moving = true;
                        inventory->moving_item_src_index = inventory->moving_item_dest_index;
                    }
                }
            }
        }
    }
    else if(is_input_valid(&keyboard[key_ascend_or_descend]))
    {
        if(!inventory->is_open)
        {
            if(is_tile_value(dungeon->tiles, player->pos, tile_stone_path_up) ||
               is_tile_value(dungeon->tiles, player->pos, tile_escape))
            {
                game->state = game_state_exit;
            }
            else if(is_tile_value(dungeon->tiles, player->pos, tile_stone_path_down))
            {
                if(dungeon->level < MAX_DUNGEON_LEVEL)
                {
                    create_dungeon(game, dungeon, player, entities, items, enemy_levels);
                    add_log_string(log, "You descend further.. Level %u.", dungeon->level);
                    update_fov(dungeon, player);
                }
                else
                {
                    game->state = game_state_exit;
                }
            }
            else
            {
                add_log_string(log, "You don't see a path here.");
            }
        }
    }
    else if(is_input_valid(&keyboard[key_wait]))
    {
        if(!inventory->is_open)
        {
            should_update_player = true;
            player->action_speed = 1.0f;
        }
    }
    else if(is_input_valid(&keyboard[key_yes]))
    {
        if(inventory->has_player_been_asked &&
           inventory->is_item_identifying)
        {
            slot_t slot = {inventory->identifying_item_index, inventory->slots[slot.index]};
            remove_item_from_inventory_and_game(slot, player, log, inventory);
            reset_inventory_item_identifying(inventory);
            
            add_log_string(log, "The scroll turns illegible and gets destroyed.");
        }
        else
        {
            printf("Else.\n");
        }
    }
    else if(is_input_valid(&keyboard[key_no]))
    {
    }
    
    if(should_update_player)
    {
#if MOONBREATH_SLOW
        if(debug_traversable)
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
                            add_log_string(log, "##2 Your attack misses%s.",
                                           end_color());
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
        
        // NOTE(Rami): Update Enemies
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
                            
                            v2u next_pos = next_pathfind_pos(dungeon, dungeon->pathfind_map, dungeon->w, enemy, player);
                            if(V2u_equal(next_pos, player->pos))
                            {
                                if(does_entity_hit(game, 15, player->evasion))
                                {
                                    attack_entity(game, dungeon, log, inventory, enemy, player, enemy->damage);
                                }
                                else
                                {
                                    add_log_string(log, "##2 You dodge the attack%s.", end_color());
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
render_entities(game_state_t *game, dungeon_t *dungeon, entity_t *player, entity_t *entities, inventory_t *inventory, assets_t *assets)
{
    // Render Player
    v4u src = tile_rect(player->tile);
    v4u dest = game_dest(game, player->pos);
    SDL_RenderCopy(game->renderer, assets->sprite_sheet.tex, (SDL_Rect *)&src, (SDL_Rect *)&dest);
    
    // Render Player Items
    for(u32 item_slot_index = 1;
        item_slot_index < item_equip_slot_total;
        ++item_slot_index)
    {
        for(u32 inventory_slot_index = 0;
            inventory_slot_index < array_count(inventory->slots);
            ++inventory_slot_index)
        {
            item_t *item = inventory->slots[inventory_slot_index];
            if(item &&
               item->is_equipped &&
               (item->equip_slot == item_slot_index))
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
    player->id = entity_player;
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
add_enemy_entity(entity_t *entities, dungeon_t *dungeon, u32 *enemy_levels, entity id, u32 x, u32 y)
{
    assert(id != entity_none && id != entity_player);
    
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
                case entity_rat:
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
                
                case entity_snail:
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
                
                case entity_slime:
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
                
                case entity_giant_slime:
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
                
                case entity_skeleton:
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
                
                case entity_skeleton_warrior:
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
                
                case entity_orc_warrior:
                {
                    strcpy(enemy->name, "Orc Warrior");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(5, 0);
                    
                    enemy->evasion = 4;
                    
                    enemy->action_speed = 1;
                    enemy->e.level = enemy_levels[id];
                    
                    enemy->e.is_red_blooded = true;
                } break;
                
                case entity_cave_bat:
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
                
                case entity_python:
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
                
                case entity_kobold:
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
                
                case entity_ogre:
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
                
                case entity_tormentor:
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
                
                case entity_imp:
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
                
                case entity_giant_demon:
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
                
                case entity_hellhound:
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
                
                case entity_undead_elf_warrior:
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
                
                case entity_assassin:
                {
                    strcpy(enemy->name, "Assassin");
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
                
                case entity_goblin:
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
                
                case entity_goblin_warrior:
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
                
                case entity_viper:
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
                
                case entity_scarlet_kingsnake:
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
                
                case entity_dog:
                {
                    strcpy(enemy->name, "Dog");
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
                
                case entity_wolf:
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
                
                case entity_green_mamba:
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
                
                case entity_floating_eye:
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
                
                case entity_devourer:
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
                
                case entity_ghoul:
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
                
                case entity_cyclops:
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
                
                case entity_dwarwen_warrior:
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
                
                case entity_black_knight:
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
                
                case entity_cursed_black_knight:
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
                
                case entity_treant:
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
                
                case entity_minotaur:
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
                
                case entity_centaur_warrior:
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
                
                case entity_centaur:
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
                
                case entity_frost_shards:
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
                
                case entity_frost_walker:
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
                
                case entity_griffin:
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
                
                case entity_spectre:
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
                
                case entity_flying_skull:
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
                
                case entity_brimstone_imp:
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