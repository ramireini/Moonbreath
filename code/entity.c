internal b32
player_moved_while_confused(RandomState *random, Entity *player, Direction came_from_direction)
{
    b32 result = false;
    
    if(player->p.effects[EffectType_Confusion].is_enabled)
    {
        u32 chance = random_number(random, 1, 100);
        assert(player->p.effects[EffectType_Confusion].value);
        
        if(chance <= player->p.effects[EffectType_Confusion].value)
        {
            for(;;)
            {
                Direction direction = get_random_direction(random);
                if(direction != came_from_direction)
                {
                    result = true;
                    player->new_pos = get_direction_pos(player->new_pos, direction);
                    
                    break;
                }
            }
        }
    }
    
    return(result);
}

internal void
end_player_status_effect(StatusEffect *effect)
{
    memset(effect, 0, sizeof(StatusEffect));
}

internal void
start_player_status_effect(Entity *player, EffectType index, u32 value, u32 duration)
{
    player->p.effects[index].is_enabled = true;
    player->p.effects[index].duration = duration;
    player->p.effects[index].value = value;
    
    switch(index)
    {
        case EffectType_Resistance: break;
        case EffectType_Flight: break;
        case EffectType_Poison: break;
        case EffectType_Confusion: break;
        
        case EffectType_Might: player->strength += value; break;
        case EffectType_Wisdom: player->intelligence += value; break;
        case EffectType_Agility: player->dexterity += value; break;
        case EffectType_Fortitude: player->defence += value; break;
        case EffectType_Focus: player->evasion += value; break;
        
        case EffectType_Weakness:
        {
            if((player->defence - value) > player->defence)
            {
                player->defence = 0;
            }
            else
            {
                player->defence -= value;
            }
        } break;
        
        case EffectType_Decay:
        {
            player->strength -= value;
            player->intelligence -= value;
            player->dexterity -= value;
        } break;
        
        invalid_default_case;
    }
}

internal b32
entity_will_hit(RandomState *random, u32 hit_chance, u32 evasion)
{
    b32 result = (random_number(random, 1, hit_chance) >= evasion);
    return(result);
}

internal void
move_entity(TileData tiles, v2u new_pos, Entity *entity)
{
    set_tile_occupied(tiles, entity->pos, false);
    entity->pos = entity->new_pos = new_pos;
    set_tile_occupied(tiles, entity->new_pos, true);
}

internal b32
heal_entity(Entity *entity, u32 value)
{
    b32 was_healed = false;
    
    if(entity->hp < entity->max_hp)
    {
        was_healed = true;
        
        entity->hp += value;
        if(entity->hp > entity->max_hp)
        {
            entity->hp = entity->max_hp;
        }
    }
    
    return(was_healed);
}

internal String128
entity_attack_message(GameState *game, Entity *attacker, Entity *defender, Inventory *inventory)
{
    String128 result = {0};
    
    if(attacker->type == EntityType_Player)
    {
        char *attack = 0;
        
        InventorySlot slot = equipped_inventory_slot_from_item_slot(ItemSlot_FirstHand, inventory);
        if(slot.item)
        {
            switch(slot.item->id)
            {
                case ItemID_Dagger:
                case ItemID_Sword:
                {
                    u32 chance = random_number(&game->random, 1, 6);
                    switch(chance)
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
                
                case ItemID_Club:
                case ItemID_Warhammer:
                {
                    u32 chance = random_number(&game->random, 1, 6);
                    switch(chance)
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
                
                case ItemID_Battleaxe:
                {
                    u32 chance = random_number(&game->random, 1, 6);
                    switch(chance)
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
                
                case ItemID_Spear:
                {
                    u32 chance = random_number(&game->random, 1, 4);
                    switch(chance)
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
            u32 chance = random_number(&game->random, 1, 2);
            switch(chance)
            {
                case 1: attack = "punch"; break;
                case 2: attack = "kick"; break;
                
                invalid_default_case;
            }
        }
        
        snprintf(result.str, sizeof(result.str), "You %s the %s", attack, defender->name);
    }
    else if(attacker->type == EntityType_Enemy)
    {
        if(attacker->e.is_ranged)
        {
            snprintf(result.str, sizeof(result.str), "The %s ranged attacks you", attacker->name);
        }
        else
        {
            snprintf(result.str, sizeof(result.str), "The %s attacks you", attacker->name);
        }
    }
    
    return(result);
}

internal void
remove_entity(Entity *entity)
{
    memset(entity, 0, sizeof(Entity));
}

internal void
kill_entity(RandomState *random, TileData tiles, String128 *log, Entity *entity)
{
    if(entity->type == EntityType_Player)
    {
#if !MOONBREATH_SLOW
        // TODO(rami): Message, remains and whatever else.
#endif
        
        entity->hp = 0;
    }
    else if(entity->type == EntityType_Enemy)
    {
        if(entity->remains && can_place_remains_on_pos(tiles, entity->pos))
        {
            TileID remains_id = TileID_None;
            
            switch(entity->remains)
            {
                case EntityRemains_RedBlood:
                {
                    remains_id = random_number(random,
                                               TileID_RedBlood1,
                                               TileID_RedBlood4);
                } break;
                
                case EntityRemains_GreenBlood:
                {
                    remains_id = random_number(random,
                                               TileID_GreenBlood1,
                                               TileID_GreenBlood4);
                } break;
                
                invalid_default_case;
            }
            
            set_tile_remains_value(tiles, entity->pos, remains_id);
        }
        
        log_text(log, "%sThe %s dies!", start_color(Color_LightRed), entity->name);
        set_tile_occupied(tiles, entity->pos, false);
        remove_entity(entity);
    }
}

internal b32
is_entity_dead(Entity *entity)
{
    b32 result = ((s32)entity->hp <= 0);
    return(result);
}

internal void
attack_entity(GameState *game,
              Dungeon *dungeon,
              String128 *log,
              Inventory *inventory,
              Entity *attacker,
              Entity *defender,
              u32 damage)
{
    String128 attack = entity_attack_message(game, attacker, defender, inventory);
    
    if(defender->defence)
    {
        damage -= random_number(&game->random, 0, defender->defence);
    }
    
    if((s32)damage <= 0)
    {
        damage = 1;
    }
    
    log_text(log, "%s for %u damage.", attack.str, damage);
    
    defender->hp -= damage;
    if(is_entity_dead(defender))
    {
        kill_entity(&game->random, dungeon->tiles, log, defender);
    }
    else
    {
        if(defender->remains)
        {
            // TODO(rami): Allow blood on walls?
            TileID remains_id = TileID_None;
            
            switch(defender->remains)
            {
                case EntityRemains_RedBlood:
                {
                    remains_id = random_number(&game->random,
                                               TileID_RedBlood5,
                                               TileID_RedBlood7);
                } break;
                
                case EntityRemains_GreenBlood:
                {
                    remains_id = random_number(&game->random,
                                               TileID_GreenBlood5,
                                               TileID_GreenBlood7);
                } break;
                
                invalid_default_case;
            }
            
            u32 chance = random_number(&game->random, 1, 100);
            if(chance <= 20)
            {
                Direction direction = random_number(&game->random, Direction_None, Direction_DownRight);
                v2u direction_pos = get_direction_pos(defender->pos, direction);
                
                if(can_place_remains_on_pos(dungeon->tiles, direction_pos) &&
                   is_tile_traversable(dungeon->tiles, direction_pos) &&
                   !is_tile_occupied(dungeon->tiles, direction_pos))
                {
                    set_tile_remains_value(dungeon->tiles, direction_pos, remains_id);
                }
            }
        }
    }
}

internal void
update_entities(GameState *game,
                GameInput *input,
                Entity *player,
                Entity *entities,
                Dungeon *dungeon,
                Item *items,
                ConsumableData *consumable_data,
                String128 *log,
                Inventory *inventory,
                u32 *enemy_levels)
{
    // Update Player
    b32 should_update_player = false;
    player->action_time = 0.0f;
    
    if(inventory->is_asking_player)
    {
        if(was_pressed(&input->keyboard[Key_Yes], input->fkey_active))
        {
            log_text(log, "The scroll turns illegible, you discard it.");
            
            inventory->is_asking_player = false;
            complete_inventory_item_use(player, log, inventory);
        }
        else if(was_pressed(&input->keyboard[Key_No], input->fkey_active))
        {
            inventory->is_asking_player = false;
        }
    }
    else
    {
#if MOONBREATH_SLOW
        if(was_pressed(&input->fkeys[1], input->fkey_active))
        {
            input->fkey_active[1] = !input->fkey_active[1];
            update_fov(dungeon, player, input->fkey_active);
        }
        else if(was_pressed(&input->fkeys[2], input->fkey_active))
        {
            should_update_player = true;
            input->fkey_active[2] = !input->fkey_active[2];
        }
        else if(input->fkeys[3].ended_down &&
                input->fkeys[3].has_been_up)
        {
            // Checked manually so works as an expected toggle.
            should_update_player = true;
            input->fkeys[3].has_been_up = false;
            
            input->fkey_active[3] = !input->fkey_active[3];
        }
        else if(was_pressed(&input->fkeys[4], input->fkey_active))
        {
            input->fkey_active[4] = !input->fkey_active[4];
        }
        else if(was_pressed(&input->fkeys[5], input->fkey_active))
        {
            for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
            {
                Item *item = &items[index];
                if(item->id)
                {
                    item->is_identified = !item->is_identified;
                }
            }
        }
        else if(was_pressed(&input->mouse[Button_Right], input->fkey_active))
        {
            for(u32 index = 0; index < MAX_ENTITY_COUNT; ++index)
            {
                Entity *entity = &entities[index];
                if(compare_v2u(entity->pos, input->mouse_tile_pos))
                {
                    printf("Entity Name: %s\n", entity->name);
                    break;
                }
            }
        }
        else
#endif
        
        if(was_pressed(&input->keyboard[Key_Up], input->fkey_active))
        {
            if(inventory->is_open)
            {
                if(inventory->pos.y > 0)
                {
                    --inventory->pos.y;
                }
                else
                {
                    inventory->pos.y = INVENTORY_HEIGHT - 1;
                }
            }
            else
            {
                if(!player_moved_while_confused(&game->random, player, Direction_Up))
                {
                    --player->new_pos.y;
                }
                
                should_update_player = true;
            }
        }
        else if(was_pressed(&input->keyboard[Key_Down], input->fkey_active))
        {
            if(inventory->is_open)
            {
                if((inventory->pos.y + 1) < INVENTORY_HEIGHT)
                {
                    ++inventory->pos.y;
                }
                else
                {
                    inventory->pos.y = 0;
                }
            }
            else
            {
                if(!player_moved_while_confused(&game->random, player, Direction_Down))
                {
                    ++player->new_pos.y;
                }
                
                should_update_player = true;
            }
        }
        else if(was_pressed(&input->keyboard[Key_Left], input->fkey_active))
        {
            if(inventory->is_open)
            {
                if(inventory->pos.x > 0)
                {
                    --inventory->pos.x;
                }
                else
                {
                    inventory->pos.x = INVENTORY_WIDTH - 1;
                }
            }
            else
            {
                if(!player_moved_while_confused(&game->random, player, Direction_Left))
                {
                    --player->new_pos.x;
                }
                
                should_update_player = true;
            }
        }
        else if(was_pressed(&input->keyboard[Key_Right], input->fkey_active))
        {
            if(inventory->is_open)
            {
                if((inventory->pos.x + 1) < INVENTORY_WIDTH)
                {
                    ++inventory->pos.x;
                }
                else
                {
                    inventory->pos.x = 0;
                }
            }
            else
            {
                if(!player_moved_while_confused(&game->random, player, Direction_Right))
                {
                    ++player->new_pos.x;
                }
                
                should_update_player = true;
            }
        }
        else if(was_pressed(&input->keyboard[Key_UpLeft], input->fkey_active))
        {
            if(inventory->is_open)
            {
                if(inventory->pos.y > 0)
                {
                    --inventory->pos.y;
                }
                else
                {
                    inventory->pos.y = INVENTORY_HEIGHT - 1;
                }
                
                if(inventory->pos.x > 0)
                {
                    --inventory->pos.x;
                }
                else
                {
                    inventory->pos.x = INVENTORY_WIDTH - 1;
                }
            }
            else
            {
                if(!player_moved_while_confused(&game->random, player, Direction_UpLeft))
                {
                    --player->new_pos.x;
                    --player->new_pos.y;
                }
                
                should_update_player = true;
            }
        }
        else if(was_pressed(&input->keyboard[Key_UpRight], input->fkey_active))
        {
            if(inventory->is_open)
            {
                if(inventory->pos.y > 0)
                {
                    --inventory->pos.y;
                }
                else
                {
                    inventory->pos.y = INVENTORY_HEIGHT - 1;
                }
                
                if((inventory->pos.x + 1) < INVENTORY_WIDTH)
                {
                    ++inventory->pos.x;
                }
                else
                {
                    inventory->pos.x = 0;
                }
            }
            else
            {
                if(!player_moved_while_confused(&game->random, player, Direction_UpRight))
                {
                    ++player->new_pos.x;
                    --player->new_pos.y;
                }
                
                should_update_player = true;
            }
        }
        else if(was_pressed(&input->keyboard[Key_DownLeft], input->fkey_active))
        {
            if(inventory->is_open)
            {
                if((inventory->pos.y + 1) < INVENTORY_HEIGHT)
                {
                    ++inventory->pos.y;
                }
                else
                {
                    inventory->pos.y = 0;
                }
                
                if(inventory->pos.x > 0)
                {
                    --inventory->pos.x;
                }
                else
                {
                    inventory->pos.x = INVENTORY_WIDTH - 1;
                }
            }
            else
            {
                if(!player_moved_while_confused(&game->random, player, Direction_DownLeft))
                {
                    --player->new_pos.x;
                    ++player->new_pos.y;
                }
                
                should_update_player = true;
            }
        }
        else if(was_pressed(&input->keyboard[Key_DownRight], input->fkey_active))
        {
            if(inventory->is_open)
            {
                if((inventory->pos.y + 1) < INVENTORY_HEIGHT)
                {
                    ++inventory->pos.y;
                }
                else
                {
                    inventory->pos.y = 0;
                }
                
                if((inventory->pos.x + 1) < INVENTORY_WIDTH)
                {
                    ++inventory->pos.x;
                }
                else
                {
                    inventory->pos.x = 0;
                }
            }
            else
            {
                if(!player_moved_while_confused(&game->random, player, Direction_DownRight))
                {
                    ++player->new_pos.x;
                    ++player->new_pos.y;
                }
                
                should_update_player = true;
            }
        }
        else if(was_pressed(&input->keyboard[Key_InventoryToggle], input->fkey_active))
        {
            if(inventory->item_use_type == ItemUseType_Identify ||
               is_player_enchanting(inventory->item_use_type))
            {
                if(!inventory->is_asking_player)
                {
                    ask_for_item_cancel(game, log, inventory);
                }
            }
            else
            {
                inventory->is_open = !inventory->is_open;
                inventory->is_asking_player = false;
                inventory->pos = make_v2u(0, 0);
                
                reset_inventory_item_use(inventory);
            }
        }
        else if(was_pressed(&input->keyboard[Key_InventoryAction], input->fkey_active))
        {
            Item *item = inventory_slot_item(inventory, inventory->pos);
            if(item)
            {
                u32 index = inventory_slot_index(inventory->pos);
                
                if(inventory->item_use_type == ItemUseType_Identify)
                {
                    if(is_item_being_used(ItemUseType_Identify, index, inventory))
                    {
                        if(!inventory->is_asking_player)
                        {
                            ask_for_item_cancel(game, log, inventory);
                        }
                    }
                    else if(!item->is_identified)
                    {
                        item->is_identified = true;
                        complete_inventory_item_use(player, log, inventory);
                    }
                }
                else if(inventory->item_use_type == ItemUseType_EnchantWeapon)
                {
                    if(is_item_being_used(ItemUseType_EnchantWeapon, index, inventory))
                    {
                        if(!inventory->is_asking_player)
                        {
                            ask_for_item_cancel(game, log, inventory);
                        }
                    }
                    else if(item->type == ItemType_Weapon)
                    {
                        u32 chance = random_number(&game->random, 1, 4);
                        switch(chance)
                        {
                            case 1: log_text(log, "The %s glows blue for a moment..", item_id_text(item->id)); break;
                            case 2: log_text(log, "The %s seems sharper than before..", item_id_text(item->id)); break;
                            case 3: log_text(log, "The %s vibrates slightly..", item_id_text(item->id)); break;
                            case 4: log_text(log, "The %s starts shimmering..", item_id_text(item->id)); break;
                            
                            invalid_default_case;
                        }
                        
                        ++item->enchantment_level;
                        complete_inventory_item_use(player, log, inventory);
                    }
                }
                else if(inventory->item_use_type == ItemUseType_EnchantArmor)
                {
                    if(is_item_being_used(ItemUseType_EnchantArmor, index, inventory))
                    {
                        if(!inventory->is_asking_player)
                        {
                            ask_for_item_cancel(game, log, inventory);
                        }
                    }
                    else if(item->type == ItemType_Armor)
                    {
                        u32 chance = random_number(&game->random, 1, 3);
                        switch(chance)
                        {
                            case 1: log_text(log, "The %s glows white for a moment..", item_id_text(item->id)); break;
                            case 2: log_text(log, "The %s looks sturdier than before..", item_id_text(item->id)); break;
                            case 3: log_text(log, "The %s feels warm for a moment..", item_id_text(item->id)); break;
                            case 4: log_text(log, "The %s feels different than before..", item_id_text(item->id)); break;
                            
                            invalid_default_case;
                        }
                        
                        ++item->enchantment_level;
                        complete_inventory_item_use(player, log, inventory);
                    }
                }
                else if(is_item_consumable(item->type))
                {
                    InventorySlot slot = get_slot_from_pos(inventory, inventory->pos);
                    set_consumable_as_known_and_identify_all(item->id, items, consumable_data);
                    
                    switch(item->id)
                    {
                        case ItemID_MightPotion:
                        {
                            if(!inventory->item_use_type)
                            {
                                log_text(log, "You drink the potion.. you feel powerful.");
                                start_player_status_effect(player, EffectType_Might, item->c.value, item->c.duration);
                                remove_item_from_inventory_and_game(slot, player, log, inventory);
                            }
                        } break;
                        
                        case ItemID_WisdomPotion:
                        {
                            if(!is_player_enchanting(inventory->item_use_type))
                            {
                                log_text(log, "You drink the potion.. you feel knowledgeable.");
                                start_player_status_effect(player, EffectType_Wisdom, item->c.value, item->c.duration);
                                remove_item_from_inventory_and_game(slot, player, log, inventory);
                            }
                        } break;
                        
                        case ItemID_AgilityPotion:
                        {
                            if(!is_player_enchanting(inventory->item_use_type))
                            {
                                log_text(log, "You drink the potion.. your body feels nimble.");
                                start_player_status_effect(player, EffectType_Agility, item->c.value, item->c.duration);
                                remove_item_from_inventory_and_game(slot, player, log, inventory);
                            }
                        } break;
                        
                        case ItemID_FortitudePotion:
                        {
                            if(!is_player_enchanting(inventory->item_use_type))
                            {
                                log_text(log, "You drink the potion.. your body feels stronger.");
                                start_player_status_effect(player, EffectType_Fortitude, item->c.value, item->c.duration);
                                remove_item_from_inventory_and_game(slot, player, log, inventory);
                            }
                        } break;
                        
                        case ItemID_ResistancePotion:
                        {
                            if(!is_player_enchanting(inventory->item_use_type))
                            {
                                // TODO(rami): Implement resistances.
                                log_text(log, "You drink the potion.. your body feels resistive.");
                                start_player_status_effect(player, EffectType_Resistance, item->c.value, item->c.duration);
                                remove_item_from_inventory_and_game(slot, player, log, inventory);
                            }
                        } break;
                        
                        case ItemID_HealingPotion:
                        {
                            if(!is_player_enchanting(inventory->item_use_type))
                            {
                                if(heal_entity(player, item->c.value))
                                {
                                    log_text(log, "You drink the potion.. you feel slightly better.");
                                }
                                else
                                {
                                    log_text(log, "You drink the potion.. you feel the same as before.");
                                }
                                
                                remove_item_from_inventory_and_game(slot, player, log, inventory);
                            }
                        } break;
                        
                        case ItemID_FocusPotion:
                        {
                            if(!is_player_enchanting(inventory->item_use_type))
                            {
                                log_text(log, "You drink the potion.. you feel very attentive.");
                                start_player_status_effect(player, EffectType_Focus, item->c.value, item->c.duration);
                                remove_item_from_inventory_and_game(slot, player, log, inventory);
                            }
                        } break;
                        
                        case ItemID_CuringPotion:
                        {
                            if(!is_player_enchanting(inventory->item_use_type))
                            {
                                if(player->p.effects[EffectType_Poison].is_enabled)
                                {
                                    log_text(log, "You drink the potion.. you feel much better.");
                                    end_player_status_effect(&player->p.effects[EffectType_Poison]);
                                }
                                else
                                {
                                    log_text(log, "You drink the potion.. you feel the same as before.");
                                }
                                
                                remove_item_from_inventory_and_game(slot, player, log, inventory);
                            }
                        } break;
                        
                        case ItemID_FlightPotion:
                        {
                            if(!is_player_enchanting(inventory->item_use_type))
                            {
                                // TODO(rami): The only thing we would want to really fly over
                                // right now is just walls, if we have water, lava, whatever in
                                // the future then this becomes more relevant.
                                // Flying and being on the ground would obviously be separate states.
                                
                                log_text(log, "You drink the potion.. you feel much lighter.");
                                start_player_status_effect(player, EffectType_Flight, item->c.value, item->c.duration);
                                remove_item_from_inventory_and_game(slot, player, log, inventory);
                            }
                        } break;
                        
                        case ItemID_DecayPotion:
                        {
                            if(!is_player_enchanting(inventory->item_use_type))
                            {
                                log_text(log, "You drink the potion.. you feel impaired.");
                                start_player_status_effect(player, EffectType_Decay, item->c.value, item->c.duration);
                                remove_item_from_inventory_and_game(slot, player, log, inventory);
                            }
                        } break;
                        
                        case ItemID_WeaknessPotion:
                        {
                            if(!is_player_enchanting(inventory->item_use_type))
                            {
                                log_text(log, "You drink the potion.. you feel weaker.");
                                start_player_status_effect(player, EffectType_Weakness, item->c.value, item->c.duration);
                                remove_item_from_inventory_and_game(slot, player, log, inventory);
                            }
                        } break;
                        
                        case ItemID_WoundingPotion:
                        {
                            if(!is_player_enchanting(inventory->item_use_type))
                            {
                                log_text(log, "You drink the potion.. painful wounds appear on your body.");
                                
                                player->hp -= item->c.value;
                                if(is_entity_dead(player))
                                {
                                    kill_entity(&game->random, dungeon->tiles, log, player);
                                }
                                
                                remove_item_from_inventory_and_game(slot, player, log, inventory);
                            }
                        } break;
                        
                        case ItemID_VenomPotion:
                        {
                            if(!is_player_enchanting(inventory->item_use_type))
                            {
                                log_text(log, "You drink the potion.. you feel very sick.");
                                start_player_status_effect(player, EffectType_Poison, item->c.value, item->c.duration);
                                remove_item_from_inventory_and_game(slot, player, log, inventory);
                            }
                        } break;
                        
                        case ItemID_ConfusionPotion:
                        {
                            if(!is_player_enchanting(inventory->item_use_type))
                            {
                                log_text(log, "You drink the potion.. you feel confused.");
                                start_player_status_effect(player, EffectType_Confusion, item->c.value, item->c.duration);
                                remove_item_from_inventory_and_game(slot, player, log, inventory);
                            }
                        } break;
                        
                        case ItemID_IdentifyScroll:
                        {
                            if(!inventory->item_use_type)
                            {
                                log_text(log, "You read the scroll.. choose an item to identify.");
                                inventory->item_use_type = ItemUseType_Identify;
                                inventory->use_item_src_index = index;
                            }
                        } break;
                        
#if 0
                        case ItemID_InfuseWeaponScroll:
                        {
                            log_text(log, "You read the scroll.. choose an item to infuse.");
                            
                            // TODO(rami): Implement infuse weapon.
                            //inventory->item_use_type = use_type_infuse_weapon;
                            inventory->use_item_src_index = slot_index;
                        } break;
#endif
                        
                        case ItemID_EnchantWeaponScroll:
                        {
                            if(!inventory->item_use_type)
                            {
                                log_text(log, "You read the scroll.. choose a weapon to enchant.");
                                inventory->item_use_type = ItemUseType_EnchantWeapon;
                                inventory->use_item_src_index = index;
                            }
                        } break;
                        
                        case ItemID_EnchantArmorScroll:
                        {
                            if(!inventory->item_use_type)
                            {
                                log_text(log, "You read the scroll.. choose an armor to enchant.");
                                inventory->item_use_type = ItemUseType_EnchantArmor;
                                inventory->use_item_src_index = index;
                            }
                        } break;
                        
                        case ItemID_MagicMappingScroll:
                        {
                            if(!inventory->item_use_type)
                            {
                                log_text(log, "You read the scroll.. your surroundings become clear to you.");
                                remove_item_from_inventory_and_game(slot, player, log, inventory);
                                
                                for(u32 y = 0; y < MAX_DUNGEON_SIZE; ++y)
                                {
                                    for(u32 x = 0; x < MAX_DUNGEON_SIZE; ++x)
                                    {
                                        set_tile_has_been_seen(dungeon->tiles, make_v2u(x, y), true);
                                    }
                                }
                            }
                        } break;
                        
                        case ItemID_TeleportationScroll:
                        {
                            if(!inventory->item_use_type)
                            {
                                log_text(log, "You read the scroll.. you find yourself in a different place.");
                                remove_item_from_inventory_and_game(slot, player, log, inventory);
                                
                                for(;;)
                                {
                                    v2u tele_pos = random_dungeon_pos(game, dungeon);
                                    if(is_tile_traversable_and_not_occupied(dungeon->tiles, tele_pos))
                                    {
                                        move_entity(dungeon->tiles, tele_pos, player);
                                        break;
                                    }
                                }
                                
                                update_fov(dungeon, player, input->fkey_active);
                            }
                        } break;
                        
                        invalid_default_case;
                    }
                }
                else
                {
                    if(!inventory->item_use_type)
                    {
                        if(item->is_equipped)
                        {
                            unequip_item(item, player);
                        }
                        else
                        {
                            InventorySlot slot = equipped_inventory_slot_from_item_slot(item->slot, inventory);
                            if(slot.item)
                            {
                                unequip_item(slot.item, player);
                            }
                            
                            item->is_identified = true;
                            equip_item(item, player);
                        }
                    }
                }
            }
        }
        else if(was_pressed(&input->keyboard[Key_PickupOrDrop], input->fkey_active))
        {
            if(inventory->is_open)
            {
                if(!inventory->item_use_type)
                {
                    InventorySlot slot = get_slot_from_pos(inventory, inventory->pos);
                    if(slot.item)
                    {
                        remove_item_from_inventory(slot, player, log, inventory);
                        
                        if(slot.item->is_identified)
                        {
                            String128 item_name = full_item_name(slot.item);
                            log_text(log, "You drop the %s%s%s.",
                                     item_rarity_color_code(slot.item->rarity),
                                     item_name.str,
                                     end_color());
                        }
                        else
                        {
                            log_text(log, "You drop the %s%s%s.",
                                     item_rarity_color_code(slot.item->rarity),
                                     item_id_text(slot.item->id),
                                     end_color());
                        }
                    }
                }
            }
            else
            {
                Item *item = get_item_on_pos(player->pos, items);
                if(item)
                {
                    if(add_item_to_inventory(item, inventory))
                    {
                        if(item->is_identified)
                        {
                            String128 item_name = full_item_name(item);
                            log_text(log, "You pick up the %s%s%s.",
                                     item_rarity_color_code(item->rarity),
                                     item_name.str,
                                     end_color());
                        }
                        else
                        {
                            log_text(log, "You pick up the %s%s%s.",
                                     item_rarity_color_code(item->rarity),
                                     item_id_text(item->id),
                                     end_color());
                        }
                    }
                    else
                    {
                        log_text(log, "Your inventory is full right now.");
                    }
                }
                else
                {
                    log_text(log, "You find nothing to pick up.");
                }
            }
        }
        else if(was_pressed(&input->keyboard[Key_InventoryMove], input->fkey_active))
        {
            if(inventory->is_open &&
               (!inventory->item_use_type || inventory->item_use_type == ItemUseType_Move))
            {
                if(inventory->item_use_type == ItemUseType_Move)
                {
                    // We are moving the item so the current inventory
                    // pos is assumed to be the destination.
                    inventory->use_item_dest_index = inventory_slot_index(inventory->pos);
                    
                    if(inventory->use_item_src_index != inventory->use_item_dest_index)
                    {
                        if(inventory->slots[inventory->use_item_dest_index])
                        {
                            // Swap the item that we're moving and the item
                            // at the destination of the move.
                            Item *temp = inventory->slots[inventory->use_item_dest_index];
                            
                            inventory->slots[inventory->use_item_dest_index] = inventory->slots[inventory->use_item_src_index];
                            inventory->slots[inventory->use_item_src_index] = temp;
                        }
                        else
                        {
                            // Nothing to swap, so just move the item.
                            inventory->slots[inventory->use_item_dest_index] = inventory->slots[inventory->use_item_src_index];
                            inventory->slots[inventory->use_item_src_index] = 0;
                        }
                    }
                    
                    reset_inventory_item_use(inventory);
                }
                else
                {
                    // If there is an item then start moving it.
                    u32 index = inventory_slot_index(inventory->pos);
                    if(inventory->slots[index])
                    {
                        inventory->item_use_type = ItemUseType_Move;
                        inventory->use_item_src_index = index;
                    }
                }
            }
        }
        else if(was_pressed(&input->keyboard[Key_AscendOrDescend], input->fkey_active))
        {
            if(!inventory->is_open)
            {
                if(is_tile_id(dungeon->tiles, player->pos, TileID_StonePathUp) ||
                   is_tile_id(dungeon->tiles, player->pos, TileID_ExitDungeon))
                {
                    game->mode = GameMode_Quit;
                }
                else if(is_tile_id(dungeon->tiles, player->pos, TileID_StonePathDown))
                {
                    if(dungeon->level < MAX_DUNGEON_LEVEL)
                    {
                        create_dungeon(game, dungeon, player, log, entities, items, consumable_data, enemy_levels);
                        log_text(log, "You descend further.. Level %u.", dungeon->level);
                        update_fov(dungeon, player, input->fkey_active);
                    }
                    else
                    {
                        game->mode = GameMode_Quit;
                    }
                }
                else
                {
                    log_text(log, "You don't see a path here.");
                }
            }
        }
        else if(was_pressed(&input->keyboard[Key_Wait], input->fkey_active))
        {
            if(!inventory->is_open)
            {
                should_update_player = true;
                player->action_time = 1.0f;
            }
        }
        
        if(should_update_player)
        {
#if MOONBREATH_SLOW
            if(input->fkey_active[2])
            {
                if(is_inside_dungeon(dungeon, player->new_pos))
                {
                    move_entity(dungeon->tiles, player->new_pos, player);
                    update_fov(dungeon, player, input->fkey_active);
                }
            }
            else
#endif
            
            // TODO(rami): We have this occupied thing so we
            // know if there's someone on our new position but
            // do we actually need it. We could check manually.
            
            if(!compare_v2u(player->pos, player->new_pos) &&
               is_tile_occupied(dungeon->tiles, player->new_pos))
            {
                for(u32 index = 1; index < MAX_ENTITY_COUNT; ++index)
                {
                    Entity *enemy = &entities[index];
                    if(compare_v2u(player->new_pos, enemy->pos))
                    {
                        u32 player_hit_chance = 15 + (player->dexterity / 2);
                        player_hit_chance += player->p.accuracy;
                        
#if MOONBREATH_SLOW
                        if(input->fkey_active[4])
                        {
                            // Player Hit Test
                            printf("\nHit Chance: %u\n", player_hit_chance);
                            printf("Target Evasion: %u\n", enemy->evasion);
                            
                            u32 hit_count = 0;
                            u32 miss_count = 0;
                            u32 loop_count = 100;
                            
                            for(u32 index = 0; index < loop_count; ++index)
                            {
                                if(entity_will_hit(&game->random, player_hit_chance, enemy->evasion))
                                {
                                    ++hit_count;
                                }
                                else
                                {
                                    ++miss_count;
                                }
                            }
                            
                            printf("Hit Count: %u (%.0f%%)\n", hit_count, ((f32)hit_count / (f32)loop_count) * 100.0f);
                            printf("Miss Count: %u (%.0f%%)\n\n", miss_count, ((f32)miss_count / (f32)loop_count) * 100.0f);
                        }
                        else
#endif
                        {
                            if(entity_will_hit(&game->random, player_hit_chance, enemy->evasion))
                            {
                                // Apply strength bonus to damage.
                                u32 modified_player_damage = player->damage;
                                if(player->strength < 10)
                                {
                                    modified_player_damage -= (10 - player->strength);
                                }
                                else
                                {
                                    modified_player_damage += (player->strength - 10);
                                }
                                
                                //printf("modified_player_damage: %u\n", modified_player_damage);
                                attack_entity(game, dungeon, log, inventory, player, enemy, modified_player_damage);
                            }
                            else
                            {
                                log_text(log, "%sYou miss the %s.", start_color(Color_LightGray), enemy->name);
                            }
                            
                            enemy->e.in_combat = true;
                        }
                        
                        player->action_time = player->p.attack_speed;
                        break;
                    }
                }
            }
            else
            {
                if(is_tile_id(dungeon->tiles, player->new_pos, TileID_StoneDoorClosed))
                {
                    set_tile_id(dungeon->tiles, player->new_pos, TileID_StoneDoorOpen);
                    player->action_time = 1.0f;
                }
                else if(is_tile_traversable(dungeon->tiles, player->new_pos))
                {
                    move_entity(dungeon->tiles, player->new_pos, player);
                    player->action_time = 1.0f;
                }
            }
            
            // Changing the new position must be based on the current position.
            player->new_pos = player->pos;
            game->time += player->action_time;
        }
    }
    
    if(player->action_time)
    {
        for(u32 index = 0; index < EffectType_Count; ++index)
        {
            StatusEffect *effect = &player->p.effects[index];
            if(effect->is_enabled)
            {
                --effect->duration;
                
                if(effect->duration)
                {
                    if(index == EffectType_Poison)
                    {
                        player->hp -= effect->value;
                        if(player->hp > player->max_hp)
                        {
                            player->hp = 0;
                        }
                    }
                }
                else
                {
                    switch(index)
                    {
                        case EffectType_Might:
                        {
                            log_text(log, "You don't feel as powerful anymore..");
                            player->strength -= effect->value;
                        } break;
                        
                        case EffectType_Wisdom:
                        {
                            log_text(log, "You don't feel as knowledgeable anymore..");
                            player->intelligence -= effect->value;
                        } break;
                        
                        case EffectType_Agility:
                        {
                            log_text(log, "Your body feels less nimble..");
                            player->dexterity -= effect->value;
                        } break;
                        
                        case EffectType_Fortitude:
                        {
                            log_text(log, "You don't feel as strong anymore..");
                            player->defence -= effect->value;
                        } break;
                        
                        case EffectType_Resistance:
                        {
                            log_text(log, "You don't feel as resistive anymore..");
                        } break;
                        
                        case EffectType_Focus:
                        {
                            log_text(log, "You don't feel as attentive anymore..");
                            player->evasion -= effect->value;
                        } break;
                        
                        case EffectType_Flight:
                        {
                            log_text(log, "You don't feel light anymore..");
                        } break;
                        
                        case EffectType_Decay:
                        {
                            log_text(log, "You don't feel impaired anymore..");
                            player->strength += effect->value;
                            player->intelligence += effect->value;
                            player->dexterity += effect->value;
                        } break;
                        
                        case EffectType_Weakness:
                        {
                            log_text(log, "You don't feel weak anymore..");
                            player->defence += effect->value;
                        } break;
                        
                        case EffectType_Poison:
                        {
                            log_text(log, "You don't feel sick anymore..");
                        } break;
                        
                        case EffectType_Confusion:
                        {
                            log_text(log, "You don't feel confused anymore..");
                        } break;
                        
                        invalid_default_case;
                    }
                    
                    end_player_status_effect(effect);
                }
            }
        }
        
        update_fov(dungeon, player, input->fkey_active);
        update_pathfind_map(dungeon, player->pos);
        
        // Update Enemies
        for(u32 index = 1; index < MAX_ENTITY_COUNT; ++index)
        {
            Entity *enemy = &entities[index];
            if(enemy->type == EntityType_Enemy)
            {
                enemy->e.time_waited += player->action_time;
                u32 enemy_action_count = (u32)(enemy->e.time_waited / enemy->action_time);
                
#if 0
                printf("player->action_speed: %.1f\n", player->action_speed);
                printf("wait_timer: %.1f\n", enemy->e.time_waited);
                printf("enemy_action_count: %u\n\n", enemy_action_count);
#endif
                
                while(enemy_action_count--)
                {
                    enemy->e.time_waited = 0.0f;
                    
                    
#if MOONBREATH_SLOW
                    if(enemy->e.in_combat || (!input->fkey_active[1] && tile_is_seen(dungeon->tiles, enemy->pos)))
#else
                    if(enemy->e.in_combat || tile_is_seen(dungeon->tiles, enemy->pos))
#endif
                    {
                        enemy->e.in_combat = true;
                        
                        if(player->pos.x < enemy->pos.x)
                        {
                            enemy->e.is_flipped = true;
                        }
                        else
                        {
                            enemy->e.is_flipped = false;
                        }
                        
                        v2u next_pos = next_pathfind_pos(dungeon, player->pos, enemy->pos);
                        
                        if(compare_v2u(next_pos, player->pos) ||
                           (enemy->e.is_ranged && tile_is_seen(dungeon->tiles, enemy->pos)))
                        {
                            u32 enemy_hit_chance = 40;
                            assert(player->evasion < enemy_hit_chance);
                            
                            if(entity_will_hit(&game->random, enemy_hit_chance, player->evasion))
                            {
                                attack_entity(game, dungeon, log, inventory, enemy, player, enemy->damage);
                            }
                            else
                            {
                                log_text(log, "%sYou dodge the attack.", start_color(Color_LightGray));
                            }
                        }
                        else
                        {
                            // TODO(rami): Maybe ranged characters should back off sometimes,
                            // previous_pathfind_pos().
                            
                            enemy->new_pos = next_pos;
                        }
                    }
                    else
                    {
                        enemy->new_pos = enemy->pos;
                        
                        Direction direction = get_random_direction(&game->random);
                        enemy->new_pos = get_direction_pos(enemy->new_pos, direction);
                        
                        switch(direction)
                        {
                            case Direction_Left:
                            case Direction_UpLeft:
                            case Direction_DownLeft: enemy->e.is_flipped = true; break;
                            
                            case Direction_Right:
                            case Direction_UpRight:
                            case Direction_DownRight: enemy->e.is_flipped = false; break;
                        }
                    }
                    
                    // TODO(rami): I saw two enemy ghosts on the same tile at
                    // some point. This situation should not happen.
                    
                    // Calling move_entity() will set the pos of the entity to new_pos.
                    // Before that happens we save the pos into pos_save_for_ghost
                    // because the code that renders the enemy ghosts needs it.
                    enemy->e.pos_save_for_ghost = enemy->pos;
                    
                    if(is_tile_traversable_and_not_occupied(dungeon->tiles, enemy->new_pos))
                    {
                        move_entity(dungeon->tiles, enemy->new_pos, enemy);
                    }
                }
            }
        }
    }
}

internal void
render_entities(GameState *game,
                Dungeon *dungeon,
                Entity *player,
                Entity *entities,
                Inventory *inventory,
                Assets *assets)
{
    // Render Player
    v4u src = tile_rect(player->tile);
    v4u dest = game_dest(game, player->pos);
    SDL_RenderCopy(game->renderer, assets->sprite_sheet.tex, (SDL_Rect *)&src, (SDL_Rect *)&dest);
    
    // Render Player Items
    for(u32 index = 1; index < ItemSlot_Count; ++index)
    {
        for(u32 inventory_slot_index = 0; inventory_slot_index < array_count(inventory->slots); ++inventory_slot_index)
        {
            Item *item = inventory->slots[inventory_slot_index];
            if(item && item->is_equipped && (item->slot == index))
            {
                v4u src = tile_rect(item->tile);
                SDL_RenderCopy(game->renderer, assets->wearable_item_tileset.tex, (SDL_Rect *)&src, (SDL_Rect *)&dest);
                break;
            }
        }
    }
    
    // Render Enemies
    for(u32 index = 1; index < MAX_ENTITY_COUNT; ++index)
    {
        Entity *enemy = &entities[index];
        if(enemy->type == EntityType_Enemy)
        {
            if(tile_is_seen(dungeon->tiles, enemy->pos))
            {
                enemy->e.has_been_seen = true;
                enemy->e.is_ghost_enabled = false;
                
                v4u src = tile_rect(enemy->tile);
                v4u dest = game_dest(game, enemy->pos);
                SDL_RenderCopyEx(game->renderer, assets->sprite_sheet.tex, (SDL_Rect *)&src, (SDL_Rect *)&dest, 0, 0, enemy->e.is_flipped);
                
                // Render Enemy HP Bar
                if(enemy->e.in_combat &&
                   enemy->hp != enemy->max_hp)
                {
                    // HP Bar Outside
                    set_render_color(game, Color_Black);
                    v4u hp_bar_outside = {dest.x, dest.y + 33, 32, 4};
                    SDL_RenderDrawRect(game->renderer, (SDL_Rect *)&hp_bar_outside);
                    
                    // HP Bar Inside
                    set_render_color(game, Color_DarkRed);
                    u32 hp_bar_inside_w = ratio(enemy->hp, enemy->max_hp, 30);
                    v4u hp_bar_inside = {dest.x + 1, dest.y + 34, hp_bar_inside_w, 2};
                    SDL_RenderFillRect(game->renderer, (SDL_Rect *)&hp_bar_inside);
                }
            }
            else
            {
                if(enemy->e.has_been_seen)
                {
                    if(enemy->e.is_ghost_enabled)
                    {
                        if(tile_is_seen(dungeon->tiles, enemy->e.ghost_pos))
                        {
                            enemy->e.has_been_seen = false;
                            enemy->e.is_ghost_enabled = false;
                        }
                        else
                        {
                            v4u src = tile_rect(enemy->tile);
                            v4u dest = game_dest(game, enemy->e.ghost_pos);
                            render_texture_half_color(game->renderer, assets->sprite_sheet.tex, src, dest);
                        }
                    }
                    else
                    {
                        // If enemy pos is seen then enemy ghost is placed on new enemy pos.
                        // This means that the enemy moved.
                        
                        // else enemy pos is not seen so enemy ghost is placed on enemy pos.
                        // This means that the player moved.
                        if(tile_is_seen(dungeon->tiles, enemy->e.pos_save_for_ghost))
                        {
                            enemy->e.ghost_pos = enemy->new_pos;
                        }
                        else
                        {
                            enemy->e.ghost_pos = enemy->e.pos_save_for_ghost;
                        }
                        
                        enemy->e.is_ghost_enabled = true;
                        enemy->e.is_ghost_flipped = enemy->e.is_flipped;
                    }
                }
            }
        }
    }
}

internal void
add_player_entity(GameState *game, Entity *player, Item *items, Inventory *inventory)
{
    player->type = EntityType_Player;
    
    strcpy(player->name, "Name");
    
#if 1
    player->max_hp = player->hp = 50;
#else
    player->max_hp = player->hp = U32_MAX;
#endif
    
    player->w = player->h = 32;
    player->remains = EntityRemains_RedBlood;
    
    player->strength = 10;
    player->intelligence = 10;
    player->dexterity = 10;
    
    player->damage = 1;
    player->evasion = 10;
    player->p.accuracy = 2;
    player->p.attack_speed = 1.0f;
    player->p.fov = 6;
    player->p.weight_to_evasion_ratio = 3;
    
#if 1
    { // Give the player their starting item.
        add_weapon_item(game, items, ItemID_Sword, ItemRarity_Common, player->pos.x, player->pos.y);
        
        Item *item = get_item_on_pos(player->pos, items);
        item->enchantment_level = 0;
        item->is_identified = true;
        item->is_cursed = false;
        
        add_item_to_inventory(item, inventory);
        equip_item(item, player);
    }
#endif
}

internal void
add_enemy_entity(Entity *entities,
                 TileData tiles,
                 u32 *enemy_levels,
                 EntityID id,
                 u32 x, u32 y)
{
    for(u32 index = 1; index < MAX_ENTITY_COUNT; ++index)
    {
        Entity *enemy = &entities[index];
        if(!enemy->type)
        {
            enemy->id = id;
            enemy->new_pos = enemy->pos = make_v2u(x, y);
            enemy->w = enemy->h = 32;
            enemy->type = EntityType_Enemy;
            set_tile_occupied(tiles, enemy->pos, true);
            enemy->e.level = enemy_levels[id];
            
            switch(id)
            {
                case EntityID_Dummy:
                {
                    // This is a dummy entity for testing purposes.
                    strcpy(enemy->name, "Dummy");
                    enemy->max_hp = enemy->hp = U32_MAX;
                    
                    enemy->evasion = 10;
                    enemy->action_time = 1.0f;
                    
                    enemy->e.is_ranged = true;
                } break;
                
                case EntityID_SkeletonWarrior:
                {
                    strcpy(enemy->name, "Skeleton Warrior");
                    enemy->max_hp = enemy->hp = 18;
                    enemy->tile = make_v2u(0, 2);
                    
                    enemy->damage = 5;
                    enemy->evasion = 5;
                    enemy->action_time = 1.0f;
                } break;
                
                case EntityID_SkeletonArcher:
                {
                    strcpy(enemy->name, "Skeleton Archer");
                    enemy->max_hp = enemy->hp = 18;
                    enemy->tile = make_v2u(0, 2);
                    
                    enemy->damage = 5;
                    enemy->evasion = 5;
                    enemy->action_time = 1.0f;
                    
                    enemy->e.is_ranged = true;
                } break;
                
                case EntityID_SkeletonMage:
                {
                    strcpy(enemy->name, "Skeleton Mage");
                    enemy->max_hp = enemy->hp = 18;
                    enemy->tile = make_v2u(0, 2);
                    
                    enemy->damage = 5;
                    enemy->evasion = 5;
                    enemy->action_time = 1.0f;
                    
                    enemy->e.is_ranged = true;
                } break;
                
                case EntityID_Bat:
                {
                    strcpy(enemy->name, "Bat");
                    enemy->max_hp = enemy->hp = 14;
                    enemy->tile = make_v2u(0, 2);
                    
                    enemy->damage = 1;
                    enemy->evasion = 15;
                    enemy->action_time = 0.3f;
                } break;
                
                case EntityID_Rat:
                {
                    strcpy(enemy->name, "Rat");
                    enemy->max_hp = enemy->hp = 10;
                    enemy->tile = make_v2u(0, 2);
                    
                    enemy->damage = 1;
                    enemy->evasion = 13;
                    enemy->action_time = 0.5f;
                } break;
                
                case EntityID_KoboldWarrior:
                {
                    strcpy(enemy->name, "Kobold Warrior");
                    enemy->max_hp = enemy->hp = 24;
                    enemy->tile = make_v2u(0, 2);
                    
                    enemy->damage = 4;
                    enemy->evasion = 8;
                    enemy->action_time = 1.0f;
                } break;
                
                case EntityID_KoboldShaman:
                {
                    strcpy(enemy->name, "Kobold Shaman");
                    enemy->max_hp = enemy->hp = 24;
                    enemy->tile = make_v2u(0, 2);
                    
                    enemy->damage = 4;
                    enemy->evasion = 8;
                    enemy->action_time = 1.0f;
                    
                    enemy->e.is_ranged = true;
                } break;
                
                case EntityID_Snail:
                {
                    strcpy(enemy->name, "Snail");
                    enemy->max_hp = enemy->hp = 32;
                    enemy->tile = make_v2u(0, 2);
                    
                    enemy->damage = 5;
                    enemy->evasion = 1;
                    enemy->action_time = 1.5f;
                } break;
                
                case EntityID_Slime:
                {
                    strcpy(enemy->name, "Slime");
                    enemy->max_hp = enemy->hp = 20;
                    enemy->tile = make_v2u(0, 2);
                    
                    enemy->damage = 3;
                    enemy->evasion = 3;
                    enemy->action_time = 1.0f;
                } break;
                
                case EntityID_Dog:
                {
                    strcpy(enemy->name, "Dog");
                    enemy->max_hp = enemy->hp = 16;
                    enemy->tile = make_v2u(0, 2);
                    
                    enemy->damage = 3;
                    enemy->evasion = 10;
                    enemy->action_time = 0.5f;
                } break;
                
                case EntityID_OrcWarrior:
                {
                    strcpy(enemy->name, "Orc Warrior");
                    enemy->max_hp = enemy->hp = 0;
                    enemy->tile = make_v2u(0, 2);
                    
                    enemy->damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                } break;
                
                case EntityID_OrcArcher:
                {
                    strcpy(enemy->name, "Orc Archer");
                    enemy->max_hp = enemy->hp = 0;
                    enemy->tile = make_v2u(0, 2);
                    
                    enemy->damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    
                    enemy->e.is_ranged = true;
                } break;
                
                case EntityID_OrcShaman:
                {
                    strcpy(enemy->name, "Orc Shaman");
                    enemy->max_hp = enemy->hp = 0;
                    enemy->tile = make_v2u(0, 2);
                    
                    enemy->damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    
                    enemy->e.is_ranged = true;
                } break;
                
                case EntityID_Python:
                {
                    strcpy(enemy->name, "Python");
                    enemy->max_hp = enemy->hp = 0;
                    enemy->tile = make_v2u(0, 2);
                    
                    enemy->damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                } break;
                
                case EntityID_Shade:
                {
                    strcpy(enemy->name, "Shade");
                    enemy->max_hp = enemy->hp = 0;
                    enemy->tile = make_v2u(0, 2);
                    
                    enemy->damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                } break;
                
                case EntityID_ElfKnight:
                {
                    strcpy(enemy->name, "Elf Knight");
                    enemy->max_hp = enemy->hp = 0;
                    enemy->tile = make_v2u(0, 2);
                    
                    enemy->damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                } break;
                
                case EntityID_ElfArbalest:
                {
                    strcpy(enemy->name, "Elf Arbalest");
                    enemy->max_hp = enemy->hp = 0;
                    enemy->tile = make_v2u(0, 2);
                    
                    enemy->damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    
                    enemy->e.is_ranged = true;
                } break;
                
                case EntityID_ElfMage:
                {
                    strcpy(enemy->name, "Elf Mage");
                    enemy->max_hp = enemy->hp = 0;
                    enemy->tile = make_v2u(0, 2);
                    
                    enemy->damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    
                    enemy->e.is_ranged = true;
                } break;
                
                case EntityID_GiantSlime:
                {
                    strcpy(enemy->name, "Giant Slime");
                    enemy->max_hp = enemy->hp = 0;
                    enemy->tile = make_v2u(0, 2);
                    
                    enemy->damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                } break;
                
                case EntityID_Spectre:
                {
                    strcpy(enemy->name, "Spectre");
                    enemy->max_hp = enemy->hp = 0;
                    enemy->tile = make_v2u(0, 2);
                    
                    enemy->damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                } break;
                
                case EntityID_OrcAssassin:
                {
                    strcpy(enemy->name, "Orc Assassin");
                    enemy->max_hp = enemy->hp = 0;
                    enemy->tile = make_v2u(0, 2);
                    
                    enemy->damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                } break;
                
                case EntityID_OrcSorcerer:
                {
                    strcpy(enemy->name, "Orc Sorcerer");
                    enemy->max_hp = enemy->hp = 0;
                    enemy->tile = make_v2u(0, 2);
                    
                    enemy->damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    
                    enemy->e.is_ranged = true;
                } break;
                
                case EntityID_Minotaur:
                {
                    strcpy(enemy->name, "Minotaur");
                    enemy->max_hp = enemy->hp = 0;
                    enemy->tile = make_v2u(0, 2);
                    
                    enemy->damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                } break;
                
                case EntityID_Treant:
                {
                    strcpy(enemy->name, "Treant");
                    enemy->max_hp = enemy->hp = 0;
                    enemy->tile = make_v2u(0, 2);
                    
                    enemy->damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                } break;
                
                case EntityID_Viper:
                {
                    strcpy(enemy->name, "Viper");
                    enemy->max_hp = enemy->hp = 0;
                    enemy->tile = make_v2u(0, 2);
                    
                    enemy->damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                } break;
                
                case EntityID_CentaurWarrior:
                {
                    strcpy(enemy->name, "Centaur Warrior");
                    enemy->max_hp = enemy->hp = 0;
                    enemy->tile = make_v2u(0, 2);
                    
                    enemy->damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                } break;
                
                case EntityID_CentaurSpearman:
                {
                    strcpy(enemy->name, "Centaur Spearman");
                    enemy->max_hp = enemy->hp = 0;
                    enemy->tile = make_v2u(0, 2);
                    
                    enemy->damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                } break;
                
                case EntityID_CentaurArcher:
                {
                    strcpy(enemy->name, "Centaur Archer");
                    enemy->max_hp = enemy->hp = 0;
                    enemy->tile = make_v2u(0, 2);
                    
                    enemy->damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    
                    enemy->e.is_ranged = true;
                } break;
                
                case EntityID_CursedSkull:
                {
                    strcpy(enemy->name, "Cursed Skull");
                    enemy->max_hp = enemy->hp = 0;
                    enemy->tile = make_v2u(0, 2);
                    
                    enemy->damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                } break;
                
                case EntityID_Wolf:
                {
                    strcpy(enemy->name, "Wolf");
                    enemy->max_hp = enemy->hp = 0;
                    enemy->tile = make_v2u(0, 2);
                    
                    enemy->damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                } break;
                
                case EntityID_OgreWarrior:
                {
                    strcpy(enemy->name, "Ogre Warrior");
                    enemy->max_hp = enemy->hp = 0;
                    enemy->tile = make_v2u(0, 2);
                    
                    enemy->damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                } break;
                
                case EntityID_OgreArcher:
                {
                    strcpy(enemy->name, "Ogre Archer");
                    enemy->max_hp = enemy->hp = 0;
                    enemy->tile = make_v2u(0, 2);
                    
                    enemy->damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    
                    enemy->e.is_ranged = true;
                } break;
                
                case EntityID_OgreMage:
                {
                    strcpy(enemy->name, "Ogre Mage");
                    enemy->max_hp = enemy->hp = 0;
                    enemy->tile = make_v2u(0, 2);
                    
                    enemy->damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    
                    enemy->e.is_ranged = true;
                } break;
                
                case EntityID_DwarwenWarrior:
                {
                    strcpy(enemy->name, "Dwarwen Warrior");
                    enemy->max_hp = enemy->hp = 0;
                    enemy->tile = make_v2u(0, 2);
                    
                    enemy->damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                } break;
                
                case EntityID_DwarwenSorcerer:
                {
                    strcpy(enemy->name, "Dwarwen Sorcerer");
                    enemy->max_hp = enemy->hp = 0;
                    enemy->tile = make_v2u(0, 2);
                    
                    enemy->damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    
                    enemy->e.is_ranged = true;
                } break;
                
                case EntityID_DwarwenPriest:
                {
                    strcpy(enemy->name, "Dwarwen Priest");
                    enemy->max_hp = enemy->hp = 0;
                    enemy->tile = make_v2u(0, 2);
                    
                    enemy->damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    
                    enemy->e.is_ranged = true;
                } break;
                
                case EntityID_ScarletSnake:
                {
                    strcpy(enemy->name, "Scarlet Snake");
                    enemy->max_hp = enemy->hp = 0;
                    enemy->tile = make_v2u(0, 2);
                    
                    enemy->damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                } break;
                
                case EntityID_Lich:
                {
                    strcpy(enemy->name, "Lich");
                    enemy->max_hp = enemy->hp = 0;
                    enemy->tile = make_v2u(0, 2);
                    
                    enemy->damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    
                    enemy->e.is_ranged = true;
                } break;
                
                case EntityID_AbyssalFiend:
                {
                    strcpy(enemy->name, "Abyssal Fiend");
                    enemy->max_hp = enemy->hp = 0;
                    enemy->tile = make_v2u(0, 2);
                    
                    enemy->damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    
                    enemy->e.is_ranged = true;
                } break;
                
                case EntityID_BloodTroll:
                {
                    strcpy(enemy->name, "Blood Troll");
                    enemy->max_hp = enemy->hp = 0;
                    enemy->tile = make_v2u(0, 2);
                    
                    enemy->damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                } break;
                
                case EntityID_IronGolem:
                {
                    strcpy(enemy->name, "Iron Golem");
                    enemy->max_hp = enemy->hp = 0;
                    enemy->tile = make_v2u(0, 2);
                    
                    enemy->damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                } break;
                
                case EntityID_Griffin:
                {
                    strcpy(enemy->name, "Griffin");
                    enemy->max_hp = enemy->hp = 0;
                    enemy->tile = make_v2u(0, 2);
                    
                    enemy->damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                } break;
                
                case EntityID_Imp:
                {
                    strcpy(enemy->name, "Imp");
                    enemy->max_hp = enemy->hp = 0;
                    enemy->tile = make_v2u(0, 2);
                    
                    enemy->damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                } break;
                
                case EntityID_BlackKnight:
                {
                    strcpy(enemy->name, "Black Knight");
                    enemy->max_hp = enemy->hp = 0;
                    enemy->tile = make_v2u(0, 2);
                    
                    enemy->damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                } break;
                
                case EntityID_GiantDemon:
                {
                    strcpy(enemy->name, "Giant Demon");
                    enemy->max_hp = enemy->hp = 0;
                    enemy->tile = make_v2u(0, 2);
                    
                    enemy->damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                } break;
                
                case EntityID_Hellhound:
                {
                    strcpy(enemy->name, "Hellhound");
                    enemy->max_hp = enemy->hp = 0;
                    enemy->tile = make_v2u(0, 2);
                    
                    enemy->damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                } break;
                
                case EntityID_AbyssalHexmaster:
                {
                    strcpy(enemy->name, "Abyssal Hexmaster");
                    enemy->max_hp = enemy->hp = 0;
                    enemy->tile = make_v2u(0, 2);
                    
                    enemy->damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    
                    enemy->e.is_ranged = true;
                } break;
                
                case EntityID_Mahjarrat:
                {
                    strcpy(enemy->name, "Mahjarrat");
                    enemy->max_hp = enemy->hp = 0;
                    enemy->tile = make_v2u(0, 2);
                    
                    enemy->damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    
                    enemy->e.is_ranged = true;
                } break;
                
                invalid_default_case;
            }
            
            return;
        }
    }
    
    assert(false);
}