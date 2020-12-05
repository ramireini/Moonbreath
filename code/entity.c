internal void
log_add_okay(UI *ui)
{
    log_add(ui, "%sOkay.", start_color(Color_Yellow));
}

internal void
log_add_item_action_text(UI *ui, Item *item, ItemActionType type)
{
    char action[16] = {0};
    if(type == ItemActionType_PickUp)
    {
        strcpy(action, "pick up");
    }
    else if(type == ItemActionType_Drop)
    {
        strcpy(action, "drop");
    }
    
    if(item->is_identified)
    {
        String128 item_name = full_item_name(item);
        log_add(ui, "You %s the %s%s%s%s.",
                action,
                item_status_color_new(item),
                item_status_prefix(item->is_cursed),
                item_name.str,
                end_color());
    }
    else
    {
        log_add(ui, "You %s the %s%s%s.",
                action,
                item_status_color_new(item),
                item_id_text(item->id),
                end_color());
    }
}

internal void
update_examine_pos(Examine *examine, Direction move_direction, Dungeon *dungeon)
{
    v2u new_pos = get_direction_pos(examine->pos, move_direction);
    if(is_inside_dungeon(dungeon, new_pos) &&
       has_tile_been_seen(dungeon->tiles, new_pos))
    {
        examine->pos = new_pos;
    }
}

internal b32
is_entity_valid_and_not_player(EntityType type)
{
    b32 result = (type && type != EntityType_Player);
    return(result);
}

internal b32
is_examine_and_inspect_and_inventory_and_log_closed(Game *game,
                                                    Inventory *inventory,
                                                    UI *ui)
{
    b32 result = (!game->examine.is_open &&
                  !game->examine.inspect_type &&
                  !inventory->is_open &&
                  !ui->is_full_log_open);
    
    return(result);
}

internal b32
handle_new_pathfind_items(Tiles tiles, Item *items)
{
    b32 result = false;
    
    for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
    {
        Item *item = &items[index];
        if(is_item_valid_and_not_in_inventory(item) &&
           is_tile_seen(tiles, item->pos) &&
           !item->seen_by_player_pathfind)
        {
            item->seen_by_player_pathfind = true;
            result = true;
        }
    }
    
    return(result);
}

internal void
initialize_player_pathfind(Entity *player, Dungeon *dungeon, Item *items, v2u pathfind_target)
{
    if(!equal_v2u(player->pos, pathfind_target))
    {
        player->p.is_pathfinding = true;
        player->p.pathfind_target = pathfind_target;
        
        player->p.pathfind_map.width = dungeon->width;
        update_pathfind_map(dungeon, &player->p.pathfind_map, player->p.pathfind_target);
        handle_new_pathfind_items(dungeon->tiles, items);
    }
}

internal void
add_player_starting_item(Random *random, Item *items, ItemInfo *item_info, Inventory *inventory, ItemID item_id, u32 x, u32 y)
{
    Item *item = 0;
    
    if(is_item_id_weapon(item_id))
    {
        item = add_weapon_item(random, items, item_id, ItemRarity_Common, x, y);
        item->is_equipped = true;
    }
    else if(is_item_id_potion(item_id))
    {
        item = add_consumable_item(random, items, item_info, item_id, x, y);
        set_as_known_and_identify_existing(item_id, items, item_info);
    }
    
    assert(item);
    
    item->enchantment_level = 0;
    item->is_identified = true;
    item->is_cursed = false;
    
    add_item_to_inventory(item, inventory);
}

internal b32
is_enemy_alerted(u32 turns_in_player_view)
{
    b32 result = (turns_in_player_view == 1);
    return(result);
}

internal b32
is_player_enchanting(UsingItemType type)
{
    b32 result = (type == UsingItemType_EnchantWeapon ||
                  type == UsingItemType_EnchantArmor);
    
    return(result);
}

internal void
player_dodge_log_add(UI *ui)
{
    // TODO(rami): The message could be more descriptive about what you dodged.
    // TODO(rami): Enemy attacks could also be more descriptive.
    log_add(ui, "%sYou dodge the attack.", start_color(Color_LightGray));
}

internal b32
is_underflowed(u32 value)
{
    b32 result = ((s32)value <= 0);
    return(result);
}

internal v2u
get_entity_tile_pos(EntityID id)
{
    v2u result = {0};
    
    switch(id)
    {
        case EntityID_Player: result = make_v2u(1, 12); break;
        case EntityID_Dummy: result = make_v2u(1, 12); break;
        
        case EntityID_SkeletonWarrior: result = make_v2u(1, 14); break;
        case EntityID_SkeletonArcher: result = make_v2u(1, 14); break;
        case EntityID_SkeletonMage: result = make_v2u(1, 14); break;
        case EntityID_Bat: result = make_v2u(1, 14); break;
        case EntityID_Rat: result = make_v2u(1, 14); break;
        
        case EntityID_KoboldWarrior: result = make_v2u(1, 14); break;
        case EntityID_KoboldShaman: result = make_v2u(1, 14); break;
        case EntityID_Snail: result = make_v2u(1, 14); break;
        case EntityID_Slime: result = make_v2u(1, 14); break;
        case EntityID_Dog: result = make_v2u(1, 14); break;
        
        case EntityID_OrcWarrior: result = make_v2u(1, 14); break;
        case EntityID_OrcArcher: result = make_v2u(1, 14); break;
        case EntityID_OrcShaman: result = make_v2u(1, 14); break;
        case EntityID_Python: result = make_v2u(1, 14); break;
        case EntityID_Shade: result = make_v2u(1, 14); break;
        
        case EntityID_ElfKnight: result = make_v2u(1, 14); break;
        case EntityID_ElfArbalest: result = make_v2u(1, 14); break;
        case EntityID_ElfMage: result = make_v2u(1, 14); break;
        case EntityID_GiantSlime: result = make_v2u(1, 14); break;
        case EntityID_Spectre: result = make_v2u(1, 14); break;
        
        case EntityID_OrcAssassin: result = make_v2u(1, 14); break;
        case EntityID_OrcSorcerer: result = make_v2u(1, 14); break;
        case EntityID_Minotaur: result = make_v2u(1, 14); break;
        case EntityID_Treant: result = make_v2u(1, 14); break;
        case EntityID_Viper: result = make_v2u(1, 14); break;
        
        case EntityID_CentaurWarrior: result = make_v2u(1, 14); break;
        case EntityID_CentaurSpearman: result = make_v2u(1, 14); break;
        case EntityID_CentaurArcher: result = make_v2u(1, 14); break;
        case EntityID_CursedSkull: result = make_v2u(1, 14); break;
        case EntityID_Wolf: result = make_v2u(1, 14); break;
        
        case EntityID_OgreWarrior: result = make_v2u(1, 14); break;
        case EntityID_OgreArcher: result = make_v2u(1, 14); break;
        case EntityID_OgreMage: result = make_v2u(1, 14); break;
        case EntityID_Cyclops: result = make_v2u(1, 14); break;
        case EntityID_ShadowWalker: result = make_v2u(1, 14); break;
        
        case EntityID_DwarwenWarrior: result = make_v2u(1, 14); break;
        case EntityID_DwarwenSorcerer: result = make_v2u(1, 14); break;
        case EntityID_DwarwenPriest: result = make_v2u(1, 14); break;
        case EntityID_ScarletSnake: result = make_v2u(1, 14); break;
        case EntityID_Lich: result = make_v2u(1, 14); break;
        
        case EntityID_AbyssalFiend: result = make_v2u(1, 14); break;
        case EntityID_BloodTroll: result = make_v2u(1, 14); break;
        case EntityID_IronGolem: result = make_v2u(1, 14); break;
        case EntityID_Griffin: result = make_v2u(1, 14); break;
        case EntityID_Imp: result = make_v2u(1, 14); break;
        
        case EntityID_BlackKnight: result = make_v2u(1, 14); break;
        case EntityID_GiantDemon: result = make_v2u(1, 14); break;
        case EntityID_Hellhound: result = make_v2u(1, 14); break;
        case EntityID_AbyssalHexmaster: result = make_v2u(1, 14); break;
        case EntityID_Mahjarrat: result = make_v2u(1, 14); break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal void
add_enemy_spell(Entity *enemy, SpellID id, u32 value, u32 chance)
{
    assert(enemy->type == EntityType_Enemy &&
           enemy->e.is_spellcaster);
    
    for(u32 index = 0; index < MAX_ENTITY_SPELL_COUNT; ++index)
    {
        if(!enemy->e.spells[index].id)
        {
            ++enemy->e.spell_count;
            enemy->e.spells[index].id = id;
            enemy->e.spells[index].value = value;
            enemy->e.spells[index].chance = chance;
            
            return;
        }
    }
    
    assert(false);
}

internal void
update_player_new_pos(Random *random, Entity *player, UI *ui, Direction move_direction)
{
    b32 did_player_stumble = false;
    
    StatusEffect *statuses = player->p.statuses;
    if(statuses[StatusEffectType_Confusion].is_enabled)
    {
        assert(statuses[StatusEffectType_Confusion].value);
        
        if(random_number(random, 1, 100) <= statuses[StatusEffectType_Confusion].value)
        {
            Direction confused_move_direction = get_random_direction(random);
            if(confused_move_direction != move_direction)
            {
                did_player_stumble = true;
                log_add(ui, "%sYou stumble around..", start_color(Color_LightGray));
                player->new_pos = get_direction_pos(player->new_pos, confused_move_direction);
            }
        }
    }
    
    if(!did_player_stumble)
    {
        player->new_pos = get_direction_pos(player->pos, move_direction);
    }
}

internal void
start_player_status_effect(Entity *player, StatusEffectType type, u32 value, u32 duration)
{
    player->p.statuses[type].is_enabled = true;
    player->p.statuses[type].duration = duration;
    player->p.statuses[type].value = value;
    
    switch(type)
    {
        case StatusEffectType_Might: player->p.strength += value; break;
        case StatusEffectType_Wisdom: player->p.intelligence += value; break;
        case StatusEffectType_Agility: player->p.dexterity += value; break;
        case StatusEffectType_Elusion: break;
        
        case StatusEffectType_Decay:
        {
            player->p.strength -= value;
            player->p.intelligence -= value;
            player->p.dexterity -= value;
        } break;
        
        case StatusEffectType_Confusion: break;
        case StatusEffectType_Poison: break;
        
        invalid_default_case;
    }
}

internal b32
entity_will_hit(Random *random, u32 hit_chance, u32 evasion)
{
    b32 result = (random_number(random, 1, hit_chance) >= evasion);
    return(result);
}

internal void
move_entity(Tiles tiles, Entity *entity, v2u new_pos)
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
entity_attack_message(Random *random, Entity *attacker, Entity *defender, Inventory *inventory)
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
                    switch(random_number(random, 1, 6))
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
                    switch(random_number(random, 1, 6))
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
                    switch(random_number(random, 1, 6))
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
                    switch(random_number(random, 1, 4))
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
            switch(random_number(random, 1, 2))
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
        // TODO(rami): Better messages for enemy attacks like with the player.
        
        if(attacker->e.is_ranger)
        {
            snprintf(result.str, sizeof(result.str), "The %s fires an arrow at you", attacker->name);
        }
        else if(attacker->e.is_spellcaster)
        {
            if(attacker->e.spells[attacker->e.spell_index].id == SpellID_DarkBolt)
            {
                snprintf(result.str, sizeof(result.str), "The %s casts a Dark Bolt at you for", attacker->name);
            }
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
kill_entity(Random *random, Tiles tiles, UI *ui, Entity *entity)
{
    if(entity->type == EntityType_Player)
    {
        // TODO(rami): Log, remains and whatever else.
        // TODO(rami): Perhaps a more, dramatic and descriptive death.
        log_add(ui, "Oh no, you are dead!");
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
        
        log_add(ui, "%sThe %s dies!", start_color(Color_LightRed), entity->name);
        set_tile_occupied(tiles, entity->pos, false);
        remove_entity(entity);
    }
}

internal void
update_player_status_effects(Game *game,
                             Dungeon *dungeon,
                             Entity *player,
                             UI *ui)
{
    for(u32 index = 0; index < StatusEffectType_Count; ++index)
    {
        StatusEffect *status = &player->p.statuses[index];
        if(status->is_enabled)
        {
            --status->duration;
            
            if(status->duration)
            {
                if(index == StatusEffectType_Poison)
                {
                    log_add(ui, "%sThe poison wrecks you for %u damage.", start_color(Color_DarkGreen), status->value);
                    
                    player->hp -= status->value;
                    if(is_underflowed(player->hp))
                    {
                        kill_entity(&game->random, dungeon->tiles, ui, player);
                    }
                }
            }
            else
            {
                // End Player Status Effect
                switch(index)
                {
                    case StatusEffectType_Might:
                    {
                        log_add(ui, "%sYou don't feel as mighty anymore..", start_color(Color_LightGray));
                        player->p.strength -= status->value;
                    } break;
                    
                    case StatusEffectType_Wisdom:
                    {
                        log_add(ui, "%sYou don't feel as wise anymore..", start_color(Color_LightGray));
                        player->p.intelligence -= status->value;
                    } break;
                    
                    case StatusEffectType_Agility:
                    {
                        log_add(ui, "%sYou don't feel as agile anymore..", start_color(Color_LightGray));
                        player->p.dexterity -= status->value;
                    } break;
                    
                    case StatusEffectType_Elusion:
                    {
                        log_add(ui, "%sYou don't feel as elusive anymore..", start_color(Color_LightGray));
                    } break;
                    
                    case StatusEffectType_Decay:
                    {
                        log_add(ui, "%sYou don't feel as weak anymore..", start_color(Color_LightGray));
                        player->p.strength += status->value;
                        player->p.intelligence += status->value;
                        player->p.dexterity += status->value;
                    } break;
                    
                    case StatusEffectType_Confusion:
                    {
                        log_add(ui, "%sYou don't feel confused anymore..", start_color(Color_LightGray));
                    } break;
                    
                    case StatusEffectType_Poison:
                    {
                        log_add(ui, "%sYou don't feel sick anymore..", start_color(Color_LightGray));
                    } break;
                    
                    invalid_default_case;
                }
                
                memset(status, 0, sizeof(StatusEffect));
            }
        }
    }
}

internal void
attack_entity(Random *random,
              Dungeon *dungeon,
              UI *ui,
              Inventory *inventory,
              Entity *attacker,
              Entity *defender,
              u32 damage)
{
    String128 attack = entity_attack_message(random, attacker, defender, inventory);
    
    if(defender->defence)
    {
        damage -= random_number(random, 0, defender->defence);
    }
    
    if(is_underflowed(damage))
    {
        log_add(ui, "%sYour armor blocks the attack.", start_color(Color_LightGray));
    }
    else
    {
        log_add(ui, "%s for %u damage.", attack.str, damage);
        
        if(defender->id != EntityID_Dummy)
        {
            defender->hp -= damage;
            if(is_underflowed(defender->hp))
            {
                kill_entity(random, dungeon->tiles, ui, defender);
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
                            remains_id = random_number(random,
                                                       TileID_RedBlood5,
                                                       TileID_RedBlood7);
                        } break;
                        
                        case EntityRemains_GreenBlood:
                        {
                            remains_id = random_number(random,
                                                       TileID_GreenBlood5,
                                                       TileID_GreenBlood7);
                        } break;
                        
                        invalid_default_case;
                    }
                    
                    if(random_number(random, 1, 100) <= 30)
                    {
                        Direction direction = random_number(random, Direction_None, Direction_DownRight);
                        v2u direction_pos = get_direction_pos(defender->pos, direction);
                        
                        if(can_place_remains_on_pos(dungeon->tiles, direction_pos) &&
                           is_tile_traversable_and_not_occupied(dungeon->tiles, direction_pos))
                        {
                            set_tile_remains_value(dungeon->tiles, direction_pos, remains_id);
                        }
                    }
                }
            }
        }
    }
}

internal char
get_pressed_alphabet_letter(u32 *alphabet, Input *input)
{
    char result = 0;
    
    for(u32 index = 0; index < AlphabetKey_Count; ++index)
    {
        if(was_pressed(&input->alphabet_keys[index]))
        {
            result = alphabet[index];
            
            // Turn to uppercase.
            if(input->is_shift_down)
            {
                result -= 32;
            }
            
            break;
        }
    }
    
    return(result);
}

internal b32
was_pressed(InputState *state)
{
    if(state->ended_down)
    {
#if MOONBREATH_SLOW
        if(fkey_active[3])
        {
            return(true);
        }
#endif
        
        if(state->has_been_up)
        {
            state->has_been_up = false;
            return(true);
        }
    }
    
    return(false);
}

internal player_input_result
update_player_input(Game *game,
                    Input *input,
                    Entity *player,
                    Entity *entities,
                    Dungeon *dungeon,
                    Item *items,
                    ItemInfo *item_info,
                    UI *ui,
                    Assets *assets,
                    Inventory *inventory,
                    u32 *entity_levels)
{
    player_input_result result = {0};
    player->new_pos = player->pos;
    
    if(player->p.is_pathfinding)
    {
        b32 found_something_new = handle_new_pathfind_items(dungeon->tiles, items);
        
        for(u32 index = 0; index < EntityID_Count; ++index)
        {
            Entity *entity = &entities[index];
            if(entity->type == EntityType_Enemy &&
               is_tile_seen(dungeon->tiles, entity->pos))
            {
                found_something_new = true;
                break;
            }
        }
        
        if(found_something_new)
        {
            player->p.is_pathfinding = false;
        }
        else
        {
            player->new_pos = next_pathfind_pos(&player->p.pathfind_map, dungeon->tiles,
                                                player->pos, player->p.pathfind_target);
            
#if 0
            printf("Auto Explore: Destination %u, %u\n", player->p.pathfind_target.x, player->p.pathfind_target.y);
            printf("Auto Explore: New Pos %u, %u\n\n", player->new_pos.x, player->new_pos.y);
#endif
            
            if(equal_v2u(player->new_pos, player->p.pathfind_target))
            {
                //printf("Auto Explore: Destination Reached\n");
                player->p.is_pathfinding = false;
            }
            
            result.should_update = true;
        }
    }
    else if(inventory->is_asking_player)
    {
        if(was_pressed(&input->GameKey_Yes))
        {
            Item *item = inventory->slots[inventory->inspect_index];
            
            if(item->id == ItemID_HealingPotion)
            {
                log_add(ui, "You drink the potion.");
                heal_entity(player, item->c.value);
            }
            else if(item->id == ItemID_Ration)
            {
                log_add(ui, "You eat the ration.");
                heal_entity(player, item->c.value);
            }
            else if(inventory->using_item_type)
            {
                log_add(ui, "%sThe scroll turns illegible, you discard it.", start_color(Color_LightGray));
                inventory->using_item_type = UsingItemType_None;
            }
            
            remove_item_from_inventory_and_game(item, inventory);
            inventory->is_inspecting = false;
            inventory->is_asking_player = false;
        }
        else if(was_pressed(&input->GameKey_No))
        {
            log_add_okay(ui);
            inventory->is_asking_player = false;
        }
    }
    else
    {
#if MOONBREATH_SLOW
        if(was_pressed(&input->fkeys[1]))
        {
            fkey_active[1] = !fkey_active[1];
            update_fov(dungeon, player);
        }
        else if(was_pressed(&input->fkeys[2]))
        {
            result.should_update = true;
            fkey_active[2] = !fkey_active[2];
        }
        else if(input->fkeys[3].ended_down &&
                input->fkeys[3].has_been_up)
        {
            // This is checked for manually above so it works as expected.
            result.should_update = true;
            input->fkeys[3].has_been_up = false;
            
            fkey_active[3] = !fkey_active[3];
        }
        else if(was_pressed(&input->fkeys[4]))
        {
            fkey_active[4] = !fkey_active[4];
        }
        else if(was_pressed(&input->fkeys[5]))
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
        else if(was_pressed(&input->fkeys[6]))
        {
            for(u32 index = 0; index < dungeon->rooms.count; ++index)
            {
                Rooms *rooms = &dungeon->rooms;
                if(is_inside_room(rooms->array[index], player->pos))
                {
                    printf("Room Index: %u\n", index);
                    printf("room.x: %u\n", rooms->array[index].x);
                    printf("room.y: %u\n", rooms->array[index].y);
                    printf("room.w: %u\n", rooms->array[index].w);
                    printf("room.h: %u\n", rooms->array[index].h);
                    printf("enemy_count: %u\n", rooms->enemy_count[index]);
                    printf("item_count: %u\n\n", rooms->item_count[index]);
                    
                    break;
                }
            }
        }
        else if(was_pressed(&input->Button_Right))
        {
            b32 was_entity = false;
            for(u32 index = 0; index < MAX_ENTITY_COUNT; ++index)
            {
                Entity *entity = &entities[index];
                if(is_entity_valid_and_not_player(entity->type) &&
                   equal_v2u(entity->pos, input->mouse_tile_pos))
                {
                    was_entity = true;
                    printf("Entity Name: %s\n", entity->name);
                    
                    break;
                }
            }
            
            if(!was_entity)
            {
                for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
                {
                    Item *item = &items[index];
                    if(equal_v2u(item->pos, input->mouse_tile_pos))
                    {
                        if(is_item_consumable(item->type))
                        {
                            printf("Item Name: %s\n", item->name);
                        }
                        else
                        {
                            printf("Item Name: %c%d %s\n",
                                   sign(item->enchantment_level),
                                   absolute(item->enchantment_level),
                                   item->name);
                        }
                        
                        if(is_item_consumable(item->type))
                        {
                            printf("Stack Count: %u\n", item->c.stack_count);
                        }
                        
                        break;
                    }
                }
            }
        }
        else
#endif
        
        {
            b32 was_direction_pressed = false;
            for(Direction direction = Direction_Up; direction <= Direction_DownRight; ++direction)
            {
                u32 index = direction - 1;
                if(was_pressed(&input->game_keys[index]) || game->examine.is_key_held[index])
                {
                    if(game->examine.is_open)
                    {
                        update_examine_pos(&game->examine, direction, dungeon);
                    }
                    else if(is_examine_and_inspect_and_inventory_and_log_closed(game, inventory, ui))
                    {
                        update_player_new_pos(&game->random, player, ui, direction);
                        result.should_update = true;
                    }
                    
                    was_direction_pressed = true;
                    break;
                }
            }
            
            if(!was_direction_pressed)
            {
                if(was_pressed(&input->GameKey_OpenInventory))
                {
                    if(is_examine_and_inspect_and_inventory_and_log_closed(game, inventory, ui))
                    {
                        inventory->is_open = true;
                        inventory->is_asking_player = false;
                        set_view_at_start(&inventory->view);
                    }
                }
                else if(was_pressed(&input->GameKey_Back))
                {
                    Examine *examine = &game->examine;
                    if(examine->is_open ||
                       examine->inspect_type)
                    {
                        examine->is_open = false;
                        
                        examine->inspect_type = InspectType_None;
                        examine->item = 0;
                        examine->entity = 0;
                        examine->tile_id = TileID_None;
                    }
                    else if(inventory->is_adjusting_letter)
                    {
                        log_add_okay(ui);
                        inventory->is_adjusting_letter = false;
                    }
                    else if(inventory->is_inspecting)
                    {
                        inventory->is_inspecting = false;
                    }
                    else if(inventory->using_item_type)
                    {
                        ask_for_item_cancel(game, ui, inventory);
                    }
                    else if(inventory->is_open)
                    {
                        inventory->is_open = false;
                        inventory->is_ready_for_pressed_letter = false;
                    }
                }
                else if(was_pressed(&input->GameKey_Pickup))
                {
                    if(is_examine_and_inspect_and_inventory_and_log_closed(game, inventory, ui))
                    {
                        Item *item = get_item_on_pos(player->pos, items);
                        if(item)
                        {
                            AddedItemResult item_result = add_item_to_inventory(item, inventory);
                            if(item_result.was_added)
                            {
                                log_add_item_action_text(ui, item, ItemActionType_PickUp);
                                
                                if(item_result.was_added_to_stack)
                                {
                                    remove_item_from_game(item);
                                }
                            }
                            else
                            {
                                log_add(ui, "Your inventory is full.");
                            }
                        }
                        else
                        {
                            log_add(ui, "You find nothing to pick up.");
                        }
                    }
                }
                else if(was_pressed(&input->GameKey_AscendDescend))
                {
                    if(is_examine_and_inspect_and_inventory_and_log_closed(game, inventory, ui))
                    {
                        if(is_tile_id(dungeon->tiles, player->pos, TileID_StoneStaircaseUp) ||
                           is_tile_id(dungeon->tiles, player->pos, TileID_ExitDungeon))
                        {
                            game->mode = GameMode_Quit;
                        }
                        else if(is_tile_id(dungeon->tiles, player->pos, TileID_StoneStaircaseDown))
                        {
                            if(dungeon->level < MAX_DUNGEON_LEVEL)
                            {
                                create_dungeon(&game->random, dungeon, player, ui, entities, items, inventory, item_info, entity_levels);
                                log_add(ui, "You descend further.. Level %u.", dungeon->level);
                                update_fov(dungeon, player);
                            }
                            else
                            {
                                game->mode = GameMode_Quit;
                            }
                        }
                        else
                        {
                            log_add(ui, "You don't see a passage here.");
                        }
                    }
                }
                else if(was_pressed(&input->GameKey_AutoExplore))
                {
                    if(is_examine_and_inspect_and_inventory_and_log_closed(game, inventory, ui))
                    {
                        assert(!player->p.is_pathfinding);
                        
                        b32 is_pathfind_target_valid = false;
                        v2u pathfind_pos = {0};
                        
                        while(is_dungeon_explorable(dungeon))
                        {
                            pathfind_pos = random_dungeon_pos(&game->random, dungeon);
                            if(is_tile_traversable_and_has_not_been_seen(dungeon->tiles, pathfind_pos))
                            {
                                is_pathfind_target_valid = true;
                                break;
                            }
                        }
                        
                        if(is_pathfind_target_valid)
                        {
                            initialize_player_pathfind(player, dungeon, items, pathfind_pos);
                        }
                        else
                        {
                            log_add(ui, "Nothing more to explore.");
                            player->p.is_pathfinding = false;
                        }
                    }
                }
                else if(was_pressed(&input->GameKey_Examine))
                {
                    Examine *examine = &game->examine;
                    
                    if(is_examine_and_inspect_and_inventory_and_log_closed(game, inventory, ui))
                    {
                        examine->is_open = true;
                        examine->pos = player->pos;
                        
                        examine->start_down_passages_from_first = true;
                        examine->down_passage_index = 0;
                        
                        examine->start_up_passages_from_first = true;
                        examine->up_passage_index = 0;
                    }
                }
                else if(was_pressed(&input->GameKey_Log))
                {
                    if(is_examine_and_inspect_and_inventory_and_log_closed(game, inventory, ui))
                    {
                        ui->is_full_log_open = true;
                        ui->is_full_log_view_set_at_end = false;
                        
                        set_view_at_start(&ui->full_log_view);
                    }
                    else if(ui->is_full_log_open)
                    {
                        ui->is_full_log_open = false;
                    }
                }
                else if(was_pressed(&input->GameKey_Wait))
                {
                    if(is_examine_and_inspect_and_inventory_and_log_closed(game, inventory, ui))
                    {
                        result.should_update = true;
                        result.new_action_time = 1.0f;
                    }
                }
                else if(input->scroll)
                {
                    if(inventory->is_open &&
                       is_inside_rect(inventory->rect, input->mouse_pos))
                    {
                        update_view_scrollbar(&inventory->view, input->scroll);
                    }
                    else if(ui->is_full_log_open &&
                            is_inside_rect(ui->full_log_rect, input->mouse_pos))
                    {
                        update_view_scrollbar(&ui->full_log_view, input->scroll);
                    }
                    
                    input->scroll = MouseScroll_None;
                }
                else if(inventory->is_open)
                {
                    if(inventory->is_inspecting)
                    {
                        Item *item = inventory->slots[inventory->inspect_index];
                        
                        if(inventory->is_adjusting_letter)
                        {
                            char pressed_letter = get_pressed_alphabet_letter(game->alphabet, input);
                            if(pressed_letter)
                            {
                                Item *inventory_item = get_inventory_item_with_letter(inventory, pressed_letter);
                                if(inventory_item)
                                {
                                    inventory_item->inventory_letter = get_free_item_letter(inventory);
                                }
                                
                                item->inventory_letter = pressed_letter;
                                String128 letter_string = get_item_letter_string(item->inventory_letter);
                                log_add(ui, "%sAdjusted to %s%s.", start_color(Color_Yellow), letter_string.str, item->name);
                                inventory->is_adjusting_letter = false;
                            }
                        }
                        else
                        {
                            if(was_pressed(&input->alphabet_keys[AlphabetKey_A]))
                            {
                                log_add(ui, "%sAdjust to which letter? (%c to quit).", start_color(Color_Yellow), game->keybinds[GameKey_Back]);
                                inventory->is_adjusting_letter = true;
                            }
                            else if(was_pressed(&input->alphabet_keys[AlphabetKey_E]))
                            {
                                if(is_item_equipment(item->type))
                                {
                                    item->is_identified = true;
                                    item->is_equipped = true;
                                }
                            }
                            else if(was_pressed(&input->alphabet_keys[AlphabetKey_U]))
                            {
                                if(is_item_equipment(item->type))
                                {
                                    if(is_item_equipped_and_cursed(item))
                                    {
                                        log_add_cursed_unequip(ui, item);
                                    }
                                    else
                                    {
                                        item->is_equipped = false;
                                    }
                                }
                            }
                            else if(was_pressed(&input->alphabet_keys[AlphabetKey_R]))
                            {
                                if(item->type == ItemType_Scroll)
                                {
                                    set_as_known_and_identify_existing(item->id, items, item_info);
                                    
                                    switch(item->id)
                                    {
                                        case ItemID_IdentifyScroll:
                                        {
                                            log_add(ui, "You read the scroll.. choose an item to identify.");
                                            inventory->using_item_type = UsingItemType_Identify;
                                        } break;
                                        
                                        case ItemID_EnchantWeaponScroll:
                                        {
                                            log_add(ui, "You read the scroll.. choose a weapon to enchant.");
                                            inventory->using_item_type = UsingItemType_EnchantWeapon;
                                        } break;
                                        
                                        case ItemID_EnchantArmorScroll:
                                        {
                                            log_add(ui, "You read the scroll.. choose an armor to enchant.");
                                            inventory->using_item_type = UsingItemType_EnchantArmor;
                                        } break;
                                        
                                        case ItemID_MagicMappingScroll:
                                        {
                                            log_add(ui, "You read the scroll.. your surroundings become clear to you.");
                                            
                                            for(u32 y = 0; y < dungeon->height; ++y)
                                            {
                                                for(u32 x = 0; x < dungeon->width; ++x)
                                                {
                                                    set_tile_has_been_seen(dungeon->tiles, make_v2u(x, y), true);
                                                }
                                            }
                                            
                                            remove_item_from_inventory_and_game(item, inventory);
                                        } break;
                                        
                                        case ItemID_TeleportationScroll:
                                        {
                                            log_add(ui, "You read the scroll.. you find yourself in a different place.");
                                            
                                            for(;;)
                                            {
                                                v2u pos = random_dungeon_pos(&game->random, dungeon);
                                                if(is_tile_traversable_and_not_occupied(dungeon->tiles, pos))
                                                {
                                                    move_entity(dungeon->tiles, player, pos);
                                                    break;
                                                }
                                            }
                                            
                                            update_fov(dungeon, player);
                                            remove_item_from_inventory_and_game(item, inventory);
                                        } break;
                                        
                                        invalid_default_case;
                                    }
                                    
                                    inventory->is_inspecting = false;
                                }
                            }
                            else if(was_pressed(&input->alphabet_keys[AlphabetKey_C]))
                            {
                                if(item->type == ItemType_Potion ||
                                   item->type == ItemType_Ration)
                                {
                                    if(item->type == ItemType_Potion)
                                    {
                                        set_as_known_and_identify_existing(item->id, items, item_info);
                                        
                                        switch(item->id)
                                        {
                                            case ItemID_MightPotion:
                                            {
                                                log_add(ui, "You drink the potion.. you feel more mighty.");
                                                start_player_status_effect(player, StatusEffectType_Might, item->c.value, item->c.duration);
                                            } break;
                                            
                                            case ItemID_WisdomPotion:
                                            {
                                                log_add(ui, "You drink the potion.. you feel more wise.");
                                                start_player_status_effect(player, StatusEffectType_Wisdom, item->c.value, item->c.duration);
                                            } break;
                                            
                                            case ItemID_AgilityPotion:
                                            {
                                                log_add(ui, "You drink the potion.. you feel more dexterous.");
                                                start_player_status_effect(player, StatusEffectType_Agility, item->c.value, item->c.duration);
                                            } break;
                                            
                                            case ItemID_ElusionPotion:
                                            {
                                                log_add(ui, "You drink the potion.. you feel more evasive.");
                                                start_player_status_effect(player, StatusEffectType_Elusion, item->c.value, item->c.duration);
                                            } break;
                                            
                                            case ItemID_HealingPotion:
                                            {
                                                if(player->hp == player->max_hp)
                                                {
                                                    ask_for_confirm(game, ui, inventory);
                                                }
                                                else
                                                {
                                                    log_add(ui, "%sYou drink the potion.. it restores %uHP.", start_color(Color_LightGreen), item->c.value);
                                                    heal_entity(player, item->c.value);
                                                }
                                            } break;
                                            
                                            case ItemID_DecayPotion:
                                            {
                                                log_add(ui, "You drink the potion.. you feel much weaker.");
                                                start_player_status_effect(player, StatusEffectType_Decay, item->c.value, item->c.duration);
                                            } break;
                                            
                                            case ItemID_ConfusionPotion:
                                            {
                                                log_add(ui, "You drink the potion.. you feel confused.");
                                                start_player_status_effect(player, StatusEffectType_Confusion, item->c.value, item->c.duration);
                                            } break;
                                            
                                            invalid_default_case;
                                        }
                                    }
                                    else if(item->type == ItemType_Ration)
                                    {
                                        if(player->hp == player->max_hp)
                                        {
                                            ask_for_confirm(game, ui, inventory);
                                        }
                                        else
                                        {
                                            log_add(ui, "%sYou eat the ration.. it restores %uHP.", start_color(Color_LightGreen), item->c.value);
                                            heal_entity(player, item->c.value);
                                        }
                                    }
                                    
                                    if(!inventory->is_asking_player)
                                    {
                                        remove_item_from_inventory_and_game(item, inventory);
                                        inventory->is_inspecting = false;
                                    }
                                }
                            }
                            else if(was_pressed(&input->alphabet_keys[AlphabetKey_D]))
                            {
                                Item *item = inventory->slots[inventory->inspect_index];
                                if(is_item_equipped_and_cursed(item))
                                {
                                    log_add_cursed_unequip(ui, item);
                                }
                                else
                                {
                                    log_add_item_action_text(ui, item, ItemActionType_Drop);
                                    remove_item_from_inventory(item, inventory, player->pos);
                                    
                                    set_view_at_start(&inventory->view);
                                    inventory->is_inspecting = false;
                                }
                            }
                            else if(was_pressed(&input->alphabet_keys[AlphabetKey_M]))
                            {
                                // TODO(rami): Mark
                                printf("Mark\n");
                            }
                        }
                    }
                    else
                    {
                        char pressed_letter = get_pressed_alphabet_letter(game->alphabet, input);
                        if(pressed_letter)
                        {
                            if(inventory->using_item_type)
                            {
                                for(u32 index = 0; index < INVENTORY_SLOT_COUNT; ++index)
                                {
                                    Item *item = inventory->slots[index];
                                    if(item &&
                                       item->inventory_letter == pressed_letter &&
                                       item_fits_inventory_using_item_type(inventory, item))
                                    {
                                        assert(inventory->using_item_type);
                                        
                                        if(inventory->using_item_type == UsingItemType_Identify)
                                        {
                                            item->is_identified = true;
                                        }
                                        else if(inventory->using_item_type == UsingItemType_EnchantWeapon)
                                        {
                                            switch(random_number(&game->random, 1, 4))
                                            {
                                                case 1: log_add(ui, "%sThe %s glows blue for a moment..", start_color(Color_LightBlue), item_id_text(item->id)); break;
                                                case 2: log_add(ui, "%sThe %s seems sharper than before..", start_color(Color_LightBlue), item_id_text(item->id)); break;
                                                case 3: log_add(ui, "%sThe %s vibrates slightly..", start_color(Color_LightBlue), item_id_text(item->id)); break;
                                                case 4: log_add(ui, "%sThe %s starts shimmering..", start_color(Color_LightBlue), item_id_text(item->id)); break;
                                                
                                                invalid_default_case;
                                            }
                                            
                                            ++item->enchantment_level;
                                        }
                                        else if(inventory->using_item_type == UsingItemType_EnchantArmor)
                                        {
                                            switch(random_number(&game->random, 1, 4))
                                            {
                                                case 1: log_add(ui, "%sThe %s glows white for a moment..", start_color(Color_LightBlue), item_id_text(item->id)); break;
                                                case 2: log_add(ui, "%sThe %s looks sturdier than before..", start_color(Color_LightBlue), item_id_text(item->id)); break;
                                                case 3: log_add(ui, "%sThe %s feels warm for a moment..", start_color(Color_LightBlue), item_id_text(item->id)); break;
                                                case 4: log_add(ui, "%sThe %s feels different than before..", start_color(Color_LightBlue), item_id_text(item->id)); break;
                                                
                                                invalid_default_case;
                                            }
                                            
                                            ++item->enchantment_level;
                                        }
                                        
                                        Item *use_item = inventory->slots[inventory->inspect_index];
                                        remove_item_from_inventory_and_game(use_item, inventory);
                                        inventory->using_item_type = UsingItemType_None;
                                    }
                                }
                            }
                            else
                            {
                                if(pressed_letter == game->keybinds[GameKey_OpenInventory] &&
                                   !inventory->is_ready_for_pressed_letter)
                                {
                                    inventory->is_ready_for_pressed_letter = true;
                                }
                                else
                                {
                                    for(u32 index = 0; index < INVENTORY_SLOT_COUNT; ++index)
                                    {
                                        Item *item = inventory->slots[index];
                                        if(item &&
                                           item->inventory_letter == pressed_letter)
                                        {
                                            inventory->is_inspecting = true;
                                            inventory->inspect_index = index;
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    return(result);
}

internal void
update_entities(Game *game,
                Input *input,
                Entity *player,
                Entity *entities,
                Dungeon *dungeon,
                Item *items,
                ItemInfo *item_info,
                UI *ui,
                Assets *assets,
                Inventory *inventory,
                u32 *entity_levels)
{
    player_input_result input_result = update_player_input(game, input, player, entities, dungeon, items, item_info, ui, assets, inventory, entity_levels);
    
    for(u32 index = 0; index < EntityID_Count; ++index)
    {
        Entity *entity = &entities[index];
        if(entity->type == EntityType_Player)
        {
            player->defence = 0;
            player->p.weight = 0;
            
            for(u32 slot_index = 1; slot_index < ItemSlot_Count; ++slot_index)
            {
                for(u32 inventory_index = 0; inventory_index < INVENTORY_SLOT_COUNT; ++inventory_index)
                {
                    Item *item = inventory->slots[inventory_index];
                    if(item && item->is_equipped &&
                       (item->slot == slot_index) &&
                       item->type == ItemType_Armor)
                    {
                        player->defence += (item->a.defence + item->enchantment_level);
                        player->p.weight += item->a.weight;
                    }
                }
            }
            
            player->evasion = 10 - (player->p.weight / player->p.weight_to_evasion_ratio);
            if(player->p.statuses[StatusEffectType_Elusion].is_enabled)
            {
                player->evasion += 2;
            }
            
#if 0
            printf("Player Defence: %u\n", player->defence);
            printf("Player Weight: %u\n", player->p.weight);
            printf("Player Evasion: %u\n\n", player->evasion);
#endif
            
            if(input_result.should_update)
            {
#if MOONBREATH_SLOW
                if(fkey_active[2])
                {
                    if(is_inside_dungeon(dungeon, player->new_pos))
                    {
                        move_entity(dungeon->tiles, player, player->new_pos);
                        update_fov(dungeon, player);
                    }
                }
                else
#endif
                
                if(!equal_v2u(player->pos, player->new_pos) &&
                   is_tile_occupied(dungeon->tiles, player->new_pos))
                {
                    for(u32 index = 0; index < MAX_ENTITY_COUNT; ++index)
                    {
                        Entity *entity = &entities[index];
                        if(entity->type == EntityType_Enemy)
                        {
                            if(equal_v2u(player->new_pos, entity->pos))
                            {
                                u32 player_damage = 2;
                                u32 player_accuracy = 1;
                                f32 player_attack_speed = 1.0f;
                                
                                InventorySlot slot = equipped_inventory_slot_from_item_slot(ItemSlot_FirstHand, inventory);
                                if(slot.item)
                                {
                                    player_damage = slot.item->w.damage + slot.item->enchantment_level;
                                    player_accuracy = slot.item->w.accuracy + slot.item->enchantment_level;
                                    player_attack_speed = slot.item->w.speed;
                                }
                                
#if 0
                                printf("player_damage: %u\n", player_damage);
                                printf("player_accuracy: %u\n", player_accuracy);
                                printf("player_attack_speed: %.01f\n", player_attack_speed);
#endif
                                
                                u32 player_hit_chance = 15 + (player->p.dexterity / 2);
                                player_hit_chance += player_accuracy;
                                
#if MOONBREATH_SLOW
                                if(fkey_active[4])
                                {
                                    // Player Hit Test
                                    printf("\nHit Chance: %u\n", player_hit_chance);
                                    printf("Target Evasion: %u\n", entity->evasion);
                                    
                                    u32 hit_count = 0;
                                    u32 miss_count = 0;
                                    u32 loop_count = 100;
                                    
                                    for(u32 index = 0; index < loop_count; ++index)
                                    {
                                        if(entity_will_hit(&game->random, player_hit_chance, entity->evasion))
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
                                    if(entity_will_hit(&game->random, player_hit_chance, entity->evasion))
                                    {
                                        // Apply strength bonus to damage.
                                        u32 modified_player_damage = player_damage;
                                        if(player->p.strength < 10)
                                        {
                                            modified_player_damage -= (10 - player->p.strength);
                                        }
                                        else
                                        {
                                            modified_player_damage += (player->p.strength - 10);
                                        }
                                        
                                        //printf("modified_player_damage: %u\n", modified_player_damage);
                                        attack_entity(&game->random, dungeon, ui, inventory, player, entity, modified_player_damage);
                                    }
                                    else
                                    {
                                        log_add(ui, "%sYou miss the %s.", start_color(Color_LightGray), entity->name);
                                    }
                                    
                                    entity->e.in_combat = true;
                                }
                                
                                input_result.new_action_time = player_attack_speed;
                                break;
                            }
                        }
                    }
                }
                else
                {
                    if(is_tile_id(dungeon->tiles, player->new_pos, TileID_StoneDoorClosed))
                    {
                        set_tile_id(dungeon->tiles, player->new_pos, TileID_StoneDoorOpen);
                        input_result.new_action_time = 1.0f;
                    }
                    else if(is_tile_traversable(dungeon->tiles, player->new_pos))
                    {
                        move_entity(dungeon->tiles, player, player->new_pos);
                        input_result.new_action_time = 1.0f;
                    }
                }
            }
            
            if(input_result.new_action_time)
            {
                game->time += input_result.new_action_time;
                player->action_time = input_result.new_action_time;
                
                update_player_status_effects(game, dungeon, player, ui);
                update_fov(dungeon, player);
                update_pathfind_map(dungeon, &dungeon->pathfind_map, player->pos);
            }
        }
        else if(entity->type == EntityType_Enemy)
        {
            if(input_result.new_action_time)
            {
                Entity *enemy = &entities[index];
                if(enemy->id && enemy->id != EntityID_Dummy)
                {
                    enemy->e.action_wait_timer += input_result.new_action_time;
                    u32 enemy_action_count = (u32)(enemy->e.action_wait_timer / enemy->action_time);
                    
#if 0
                    printf("input_result.new_action_time: %.1f\n", input_result.new_action_time);
                    printf("wait_timer: %.1f\n", enemy->e.time_waited);
                    printf("enemy_action_count: %u\n\n", enemy_action_count);
#endif
                    
                    while(enemy_action_count--)
                    {
                        enemy->e.action_wait_timer = 0.0f;
                        
#if MOONBREATH_SLOW
                        if(is_tile_seen(dungeon->tiles, enemy->pos) && !fkey_active[1])
#else
                        if(is_tile_seen(dungeon->tiles, enemy->pos))
#endif
                        {
                            enemy->e.in_combat = true;
                            enemy->e.is_pathfinding = false;
                            
                            if(player->pos.x < enemy->pos.x)
                            {
                                enemy->e.is_flipped = true;
                            }
                            else
                            {
                                enemy->e.is_flipped = false;
                            }
                            
                            ++enemy->e.turns_in_player_view;
                            if(is_enemy_alerted(enemy->e.turns_in_player_view))
                            {
                                break;
                            }
                            
                            v2u pathfind_pos = next_pathfind_pos(&dungeon->pathfind_map, dungeon->tiles, enemy->pos, player->pos);
                            u32 enemy_hit_chance = 30;
                            assert(player->evasion < enemy_hit_chance);
                            
                            if(enemy->e.is_spellcaster)
                            {
                                b32 spell_was_used = false;
                                while(!spell_was_used)
                                {
                                    u32 break_value = 100;
                                    u32 counter = 0;
                                    
                                    for(;;)
                                    {
                                        u32 index = random_number(&game->random, 0, enemy->e.spell_count);
                                        counter += enemy->e.spells[index].chance;
                                        
                                        if(counter >= break_value)
                                        {
                                            enemy->e.spell_index = index;
                                            break;
                                        }
                                    }
                                    
                                    EntitySpell *spell = &enemy->e.spells[enemy->e.spell_index];
                                    if(spell->id == SpellID_LesserHeal)
                                    {
                                        for(u32 index = 1; index < MAX_ENTITY_COUNT; ++index)
                                        {
                                            Entity *target = &entities[index];
                                            if(target->id)
                                            {
                                                if(!equal_v2u(enemy->pos, target->pos))
                                                {
                                                    v4u player_fov_rect = get_dimension_rect(dungeon, player->pos, player->p.fov);
                                                    if(is_inside_rect(player_fov_rect, target->pos) &&
                                                       target->e.in_combat &&
                                                       target->hp < target->max_hp)
                                                    {
                                                        log_add(ui, "The %s heals %s for %u HP.", enemy->name, target->name, spell->value);
                                                        heal_entity(target, spell->value);
                                                        spell_was_used = true;
                                                        
                                                        break;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if(spell->id == SpellID_DarkBolt)
                                    {
                                        if(entity_will_hit(&game->random, enemy_hit_chance, player->evasion))
                                        {
                                            attack_entity(&game->random, dungeon, ui, inventory, enemy, player, spell->value);
                                        }
                                        else
                                        {
                                            player_dodge_log_add(ui);
                                        }
                                        
                                        spell_was_used = true;
                                    }
                                }
                            }
                            else if(enemy->e.is_ranger || equal_v2u(pathfind_pos, player->pos))
                            {
                                if(entity_will_hit(&game->random, enemy_hit_chance, player->evasion))
                                {
                                    attack_entity(&game->random, dungeon, ui, inventory, enemy, player, enemy->e.damage);
                                    
                                    if(enemy->e.poison_chance &&
                                       !player->p.statuses[StatusEffectType_Poison].is_enabled &&
                                       enemy->e.poison_chance <= random_number(&game->random, 1, 100))
                                    {
                                        log_add(ui, "%sYou start feeling sick..", start_color(Color_LightGray));
                                        start_player_status_effect(player, StatusEffectType_Poison, enemy->e.poison_damage, enemy->e.poison_duration);
                                    }
                                }
                                else
                                {
                                    player_dodge_log_add(ui);
                                }
                            }
                            else
                            {
                                // TODO(rami): Maybe ranged characters should back off, previous_pathfind_pos()?
                                enemy->new_pos = pathfind_pos;
                            }
                        }
                        else
                        {
                            if(enemy->e.in_combat && !is_enemy_alerted(enemy->e.turns_in_player_view))
                            {
                                if(!enemy->e.is_pathfinding)
                                {
                                    enemy->e.is_pathfinding = true;
                                    enemy->e.pathfind_map = dungeon->pathfind_map;
                                    enemy->e.pathfind_target = player->pos;
                                    
                                    //printf("Enemy Pathfind: Target %u, %u\n", enemy->e.pathfind_pos.x, enemy->e.pathfind_pos.y);
                                }
                                
                                enemy->new_pos = next_pathfind_pos(&enemy->e.pathfind_map, dungeon->tiles, enemy->pos, enemy->e.pathfind_target);
                                //printf("Enemy Pathfind: New Pos %u, %u\n", enemy->new_pos.x, enemy->new_pos.y);
                                
                                if(equal_v2u(enemy->new_pos, enemy->e.pathfind_target))
                                {
                                    //printf("Enemy Pathfind: Target Reached\n");
                                    
                                    enemy->e.in_combat = false;
                                    enemy->e.is_pathfinding = false;
                                    enemy->e.turns_in_player_view = 0;
                                }
                            }
                            else
                            {
                                enemy->e.in_combat = false;
                                enemy->e.turns_in_player_view = 0;
                                
                                Direction direction = get_random_direction(&game->random);
                                //direction = Direction_Left;
                                enemy->new_pos = get_direction_pos(enemy->pos, direction);
                                
                                switch(direction)
                                {
                                    case Direction_Left:
                                    case Direction_UpLeft:
                                    case Direction_DownLeft: enemy->e.is_flipped = true; break;
                                    
                                    case Direction_Right:
                                    case Direction_UpRight:
                                    case Direction_DownRight: enemy->e.is_flipped = false; break;
                                }
                                
                                if(is_tile_traversable(dungeon->tiles, enemy->new_pos))
                                {
#if MOONBREATH_SLOW
                                    if(is_tile_seen(dungeon->tiles, enemy->new_pos) && !fkey_active[1])
#else
                                    if(is_tile_seen(dungeon->tiles, enemy->new_pos))
#endif
                                    {
                                        if(!enemy_action_count)
                                        {
                                            ++enemy->e.turns_in_player_view;
                                        }
                                    }
                                }
                            }
                        }
                        
                        // Calling move_entity() will set the pos of the entity to new_pos.
                        // Before that happens we save the pos into pos_save_for_ghost
                        // because the code that renders the enemy ghosts needs it.
                        if(is_tile_seen(dungeon->tiles, enemy->new_pos))
                        {
                            enemy->e.pos_save_for_ghost = enemy->new_pos;
                        }
                        else
                        {
                            enemy->e.pos_save_for_ghost = enemy->pos;
                        }
                        
                        if(is_tile_traversable_and_not_occupied(dungeon->tiles, enemy->new_pos))
                        {
                            move_entity(dungeon->tiles, enemy, enemy->new_pos);
                        }
                    }
                    
#if 0
                    printf("\nturns_in_player_view: %u\n", enemy->e.turns_in_player_view);
                    printf("is_pathfinding: %u\n", enemy->e.is_pathfinding);
                    printf("in_combat: %u\n\n", enemy->e.in_combat);
#endif
                }
            }
        }
    }
}

internal void
render_entities(Game *game,
                Dungeon *dungeon,
                Entity *entities,
                Inventory *inventory,
                Assets *assets)
{
    for(u32 index = 0; index < EntityID_Count; ++index)
    {
        Entity *entity = &entities[index];
        if(entity->type == EntityType_Player)
        {
            Entity *player = entity;
            v4u src = get_tile_rect(player->tile_pos);
            v4u dest = get_game_dest(game, player->pos);
            SDL_RenderCopy(game->renderer, assets->tileset.tex, (SDL_Rect *)&src, (SDL_Rect *)&dest);
            
#if 0
            // TODO(rami): Render player items when the time is right.
            // Render Player Items
            for(u32 slot_index = 1; slot_index < ItemSlot_Count; ++slot_index)
            {
                for(u32 inventory_index = 0; inventory_index < INVENTORY_SLOT_COUNT; ++inventory_index)
                {
                    Item *item = inventory->slots[inventory_index];
                    if(item && item->is_equipped && (item->slot == slot_index))
                    {
                        v4u src = get_tile_rect(item->tile_pos);
                        SDL_RenderCopy(game->renderer, assets->tileset.tex, (SDL_Rect *)&src, (SDL_Rect *)&dest);
                        
                        break;
                    }
                }
            }
#endif
            
        }
        else if(entity->type == EntityType_Enemy)
        {
            Entity *enemy = entity;
            if(is_tile_seen(dungeon->tiles, enemy->pos))
            {
                enemy->e.has_been_seen = true;
                enemy->e.is_ghost_enabled = false;
                
                v4u src = get_tile_rect(enemy->tile_pos);
                v4u dest = get_game_dest(game, enemy->pos);
                SDL_RenderCopyEx(game->renderer, assets->tileset.tex, (SDL_Rect *)&src, (SDL_Rect *)&dest, 0, 0, enemy->e.is_flipped);
                
                if(enemy->e.turns_in_player_view == 1)
                {
                    v4u status_src = get_tile_rect(make_v2u(7, 15));
                    SDL_RenderCopy(game->renderer, assets->tileset.tex, (SDL_Rect *)&status_src, (SDL_Rect *)&dest);
                }
                else if(enemy->e.in_combat)
                {
                    // HP Bar Outside
                    v4u hp_bar_outside = {dest.x, dest.y + 33, 32, 4};
                    render_draw_rect(game, hp_bar_outside, Color_Black);
                    
                    // HP Bar Inside
                    u32 hp_bar_inside_w = get_ratio(enemy->hp, enemy->max_hp, 30);
                    v4u hp_bar_inside = {dest.x + 1, dest.y + 34, hp_bar_inside_w, 2};
                    render_fill_rect(game, hp_bar_inside, Color_DarkRed);
                }
            }
            else
            {
                if(enemy->e.has_been_seen)
                {
                    if(enemy->e.is_ghost_enabled)
                    {
                        if(is_tile_seen(dungeon->tiles, enemy->e.ghost_pos))
                        {
                            enemy->e.has_been_seen = false;
                            enemy->e.is_ghost_enabled = false;
                        }
                        else
                        {
                            v4u src = get_tile_rect(enemy->tile_pos);
                            v4u dest = get_game_dest(game, enemy->e.ghost_pos);
                            render_texture_half_color(game->renderer, assets->tileset.tex, src, dest, enemy->e.is_ghost_flipped);
                        }
                    }
                    else
                    {
                        // If enemy pos is seen then enemy ghost is placed on new enemy pos.
                        // This means that the enemy moved.
                        if(is_tile_seen(dungeon->tiles, enemy->e.pos_save_for_ghost))
                        {
                            enemy->e.ghost_pos = enemy->new_pos;
                        }
                        // else enemy pos is not seen so enemy ghost is placed on enemy pos.
                        // This means that the player moved.
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
add_player_entity(Random *random, Entity *player, Item *items, Inventory *inventory)
{
    player->id = EntityID_Player;
    strcpy(player->name, "Name");
    
    player->max_hp = 80;
    
#if 0
    player->hp = 1;
#elif 1
    player->hp = player->max_hp;
#else
    player->hp = 1000000;
#endif
    
    player->w = player->h = 32;
    player->tile_pos = get_entity_tile_pos(player->id);
    player->remains = EntityRemains_RedBlood;
    player->type = EntityType_Player;
    
    player->p.strength = 10;
    player->p.intelligence = 10;
    player->p.dexterity = 10;
    
    player->evasion = 10;
    player->p.fov = 6;
    player->p.weight_to_evasion_ratio = 3;
}

internal void
add_enemy_entity(Entity *entities,
                 Tiles tiles,
                 u32 *entity_levels,
                 EntityID id,
                 u32 x, u32 y)
{
    for(u32 index = 0; index < MAX_ENTITY_COUNT; ++index)
    {
        Entity *enemy = &entities[index];
        if(!enemy->type)
        {
            enemy->id = id;
            enemy->new_pos = enemy->pos = make_v2u(x, y);
            enemy->w = enemy->h = 32;
            enemy->tile_pos = get_entity_tile_pos(enemy->id);
            enemy->type = EntityType_Enemy;
            set_tile_occupied(tiles, enemy->pos, true);
            enemy->e.level = entity_levels[id];
            
#if 0
            printf("id: %u\n", id);
            printf("EntityID_Cyclops: %u\n", EntityID_Cyclops);
            printf("EntityID_Count: %u\n\n", EntityID_Count);
#endif
            
            switch(id)
            {
                case EntityID_Dummy:
                {
                    // This is a dummy entity for testing purposes.
                    strcpy(enemy->name, "Dummy");
                    enemy->max_hp = enemy->hp = U32_MAX;
                    
                    enemy->evasion = 10;
                    enemy->action_time = 1.0f;
                } break;
                
                case EntityID_SkeletonWarrior:
                {
                    strcpy(enemy->name, "Skeleton Warrior");
                    enemy->max_hp = enemy->hp = 18;
                    
                    enemy->e.damage = 5;
                    enemy->evasion = 5;
                    enemy->action_time = 1.0f;
                } break;
                
                case EntityID_SkeletonArcher:
                {
                    strcpy(enemy->name, "Skeleton Archer");
                    enemy->max_hp = enemy->hp = 18;
                    
                    enemy->e.damage = 4;
                    enemy->evasion = 5;
                    enemy->action_time = 1.0f;
                    
                    enemy->e.is_ranger = true;
                } break;
                
                case EntityID_SkeletonMage:
                {
                    strcpy(enemy->name, "Skeleton Mage");
                    enemy->max_hp = enemy->hp = 18;
                    
                    enemy->evasion = 5;
                    enemy->action_time = 1.0f;
                    
                    enemy->e.is_spellcaster = true;
                    add_enemy_spell(enemy, SpellID_DarkBolt, 3, 100);
                } break;
                
                case EntityID_Bat:
                {
                    strcpy(enemy->name, "Bat");
                    enemy->max_hp = enemy->hp = 14;
                    
                    enemy->e.damage = 1;
                    enemy->evasion = 14;
                    enemy->action_time = 0.3f;
                    
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_Rat:
                {
                    strcpy(enemy->name, "Rat");
                    enemy->max_hp = enemy->hp = 10;
                    
                    enemy->e.damage = 1;
                    enemy->evasion = 13;
                    enemy->action_time = 0.5f;
                    
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_KoboldWarrior:
                {
                    strcpy(enemy->name, "Kobold Warrior");
                    enemy->max_hp = enemy->hp = 24;
                    
                    enemy->e.damage = 4;
                    enemy->evasion = 8;
                    enemy->action_time = 1.0f;
                    
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_KoboldShaman:
                {
                    strcpy(enemy->name, "Kobold Shaman");
                    enemy->max_hp = enemy->hp = 24;
                    
                    enemy->e.damage = 3;
                    enemy->evasion = 8;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                    
                    enemy->e.is_spellcaster = true;
                    add_enemy_spell(enemy, SpellID_LesserHeal, 5, 30);
                    add_enemy_spell(enemy, SpellID_DarkBolt, 3, 70);
                } break;
                
                case EntityID_Snail:
                {
                    strcpy(enemy->name, "Snail");
                    enemy->max_hp = enemy->hp = 32;
                    
                    enemy->e.damage = 6;
                    enemy->evasion = 1;
                    enemy->action_time = 1.5f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_Slime:
                {
                    strcpy(enemy->name, "Slime");
                    enemy->max_hp = enemy->hp = 20;
                    
                    enemy->e.damage = 3;
                    enemy->evasion = 3;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_GreenBlood;
                } break;
                
                case EntityID_Dog:
                {
                    strcpy(enemy->name, "Dog");
                    enemy->max_hp = enemy->hp = 16;
                    
                    enemy->e.damage = 3;
                    enemy->evasion = 8;
                    enemy->action_time = 0.5f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_OrcWarrior:
                {
                    strcpy(enemy->name, "Orc Warrior");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_OrcArcher:
                {
                    strcpy(enemy->name, "Orc Archer");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                    
                    enemy->e.is_ranger = true;
                } break;
                
                case EntityID_OrcShaman:
                {
                    strcpy(enemy->name, "Orc Shaman");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                    
                    enemy->e.is_spellcaster = true;
                } break;
                
                case EntityID_Python:
                {
                    strcpy(enemy->name, "Python");
                    enemy->max_hp = enemy->hp = 1;
                    
                    enemy->e.damage = 4;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                    enemy->e.poison_chance = 33;
                    enemy->e.poison_damage = 2;
                    enemy->e.poison_duration = 10;
                } break;
                
                case EntityID_Shade:
                {
                    strcpy(enemy->name, "Shade");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    
                    enemy->e.is_spellcaster = true;
                } break;
                
                case EntityID_ElfKnight:
                {
                    strcpy(enemy->name, "Elf Knight");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_ElfArbalest:
                {
                    strcpy(enemy->name, "Elf Arbalest");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                    
                    enemy->e.is_ranger = true;
                } break;
                
                case EntityID_ElfMage:
                {
                    strcpy(enemy->name, "Elf Mage");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                    
                    enemy->e.is_spellcaster = true;
                } break;
                
                case EntityID_GiantSlime:
                {
                    strcpy(enemy->name, "Giant Slime");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_GreenBlood;
                } break;
                
                case EntityID_Spectre:
                {
                    strcpy(enemy->name, "Spectre");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    
                    enemy->e.is_spellcaster = true;
                } break;
                
                case EntityID_OrcAssassin:
                {
                    strcpy(enemy->name, "Orc Assassin");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_OrcSorcerer:
                {
                    strcpy(enemy->name, "Orc Sorcerer");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                    
                    enemy->e.is_spellcaster = true;
                } break;
                
                case EntityID_Minotaur:
                {
                    strcpy(enemy->name, "Minotaur");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_Treant:
                {
                    strcpy(enemy->name, "Treant");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                } break;
                
                case EntityID_Viper:
                {
                    strcpy(enemy->name, "Viper");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_CentaurWarrior:
                {
                    strcpy(enemy->name, "Centaur Warrior");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_CentaurSpearman:
                {
                    strcpy(enemy->name, "Centaur Spearman");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_CentaurArcher:
                {
                    strcpy(enemy->name, "Centaur Archer");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                    
                    enemy->e.is_ranger = true;
                } break;
                
                case EntityID_CursedSkull:
                {
                    strcpy(enemy->name, "Cursed Skull");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    
                    enemy->e.is_spellcaster = true;
                } break;
                
                case EntityID_Wolf:
                {
                    strcpy(enemy->name, "Wolf");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_OgreWarrior:
                {
                    strcpy(enemy->name, "Ogre Warrior");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_OgreArcher:
                {
                    strcpy(enemy->name, "Ogre Archer");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                    
                    enemy->e.is_ranger = true;
                } break;
                
                case EntityID_OgreMage:
                {
                    strcpy(enemy->name, "Ogre Mage");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                    
                    enemy->e.is_spellcaster = true;
                } break;
                
                case EntityID_Cyclops:
                {
                    strcpy(enemy->name, "Cyclops");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_ShadowWalker:
                {
                    strcpy(enemy->name, "Shadow Walker");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_DwarwenWarrior:
                {
                    strcpy(enemy->name, "Dwarwen Warrior");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_DwarwenSorcerer:
                {
                    strcpy(enemy->name, "Dwarwen Sorcerer");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                    
                    enemy->e.is_spellcaster = true;
                } break;
                
                case EntityID_DwarwenPriest:
                {
                    strcpy(enemy->name, "Dwarwen Priest");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                    
                    enemy->e.is_spellcaster = true;
                } break;
                
                case EntityID_ScarletSnake:
                {
                    strcpy(enemy->name, "Scarlet Snake");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_Lich:
                {
                    strcpy(enemy->name, "Lich");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    
                    enemy->e.is_spellcaster = true;
                } break;
                
                case EntityID_AbyssalFiend:
                {
                    strcpy(enemy->name, "Abyssal Fiend");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                    
                    enemy->e.is_ranger = true;
                } break;
                
                case EntityID_BloodTroll:
                {
                    strcpy(enemy->name, "Blood Troll");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_IronGolem:
                {
                    strcpy(enemy->name, "Iron Golem");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                } break;
                
                case EntityID_Griffin:
                {
                    strcpy(enemy->name, "Griffin");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_Imp:
                {
                    strcpy(enemy->name, "Imp");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_BlackKnight:
                {
                    strcpy(enemy->name, "Black Knight");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                } break;
                
                case EntityID_GiantDemon:
                {
                    strcpy(enemy->name, "Giant Demon");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_Hellhound:
                {
                    strcpy(enemy->name, "Hellhound");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_AbyssalHexmaster:
                {
                    strcpy(enemy->name, "Abyssal Hexmaster");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                    
                    enemy->e.is_spellcaster = true;
                } break;
                
                case EntityID_Mahjarrat:
                {
                    strcpy(enemy->name, "Mahjarrat");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    
                    enemy->e.is_spellcaster = true;
                } break;
                
                invalid_default_case;
            }
            
            return;
        }
    }
    
    assert(false);
}