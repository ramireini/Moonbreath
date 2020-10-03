internal b32
is_enemy_alerted(u32 turns_in_player_view)
{
    b32 result = (turns_in_player_view == 1);
    return(result);
}

internal b32
is_player_enchanting(ItemUseType type)
{
    b32 result = (type == ItemUseType_EnchantWeapon ||
                  type == ItemUseType_EnchantArmor);
    
    return(result);
}

internal void
player_dodge_log_add(String128 *log)
{
    // TODO(rami): The message could be more descriptive about what you dodged.
    log_add(log, "%sYou dodge the attack.", start_color(Color_LightGray));
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

internal v4u
get_player_fov_rect(u32 dungeon_w, u32 dungeon_h, v2u pos, u32 fov)
{
    v4u result =
    {
        pos.x - fov,
        pos.y - fov,
        fov * 2,
        fov * 2
    };
    
    // Clamp the values if needed.
    u32 width_pos = result.x + result.w;
    if((s32)result.x < 0)
    {
        result.w = result.w + (s32)result.x;
        result.x = 0;
    }
    else if(width_pos >= dungeon_w)
    {
        u32 difference = (width_pos - dungeon_w) + 1;
        result.w -= difference;
    }
    
    u32 height_pos = result.y + result.h;
    if((s32)result.y < 0)
    {
        result.h = result.h + (s32)result.y;
        result.y = 0;
    }
    else if(height_pos >= dungeon_h)
    {
        u32 difference = (height_pos - dungeon_h) + 1;
        result.h -= difference;
    }
    
    return(result);
}

internal b32
player_moved_while_confused(RandomState *random, Entity *player, Direction origin_direction)
{
    b32 result = false;
    
    StatusEffect *statuses = player->p.statuses;
    if(statuses[StatusEffectType_Confusion].is_enabled)
    {
        assert(statuses[StatusEffectType_Confusion].value);
        if(random_number(random, 1, 100) <= statuses[StatusEffectType_Confusion].value)
        {
            for(;;)
            {
                Direction direction = get_random_direction(random);
                if(direction != origin_direction)
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
start_player_status_effect(Entity *player, StatusEffectType index, u32 value, u32 duration)
{
    player->p.statuses[index].is_enabled = true;
    player->p.statuses[index].duration = duration;
    player->p.statuses[index].value = value;
    
    switch(index)
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
entity_will_hit(RandomState *random, u32 hit_chance, u32 evasion)
{
    b32 result = (random_number(random, 1, hit_chance) >= evasion);
    return(result);
}

internal void
move_entity(DungeonTiles tiles, Entity *entity, v2u new_pos)
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
entity_attack_message(RandomState *random, Entity *attacker, Entity *defender, Inventory *inventory)
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
kill_entity(RandomState *random, DungeonTiles tiles, String128 *log, Entity *entity)
{
    if(entity->type == EntityType_Player)
    {
        // TODO(rami): Log, remains and whatever else.
        // TODO(rami): Perhaps a more, dramatic and descriptive death.
        log_add(log, "Oh no, you're dead!");
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
        
        log_add(log, "%sThe %s dies!", start_color(Color_LightRed), entity->name);
        set_tile_occupied(tiles, entity->pos, false);
        remove_entity(entity);
    }
}

internal void
update_player_status_effects(GameState *game,
                             Dungeon *dungeon,
                             Entity *player,
                             String128 *log)
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
                    log_add(log, "%sThe poison wrecks you for %u damage.", start_color(Color_DarkGreen), status->value);
                    
                    player->hp -= status->value;
                    if(is_underflowed(player->hp))
                    {
                        kill_entity(&game->random, dungeon->tiles, log, player);
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
                        log_add(log, "%sYou don't feel as mighty anymore..", start_color(Color_LightGray));
                        player->p.strength -= status->value;
                    } break;
                    
                    case StatusEffectType_Wisdom:
                    {
                        log_add(log, "%sYou don't feel as wise anymore..", start_color(Color_LightGray));
                        player->p.intelligence -= status->value;
                    } break;
                    
                    case StatusEffectType_Agility:
                    {
                        log_add(log, "%sYou don't feel as agile anymore..", start_color(Color_LightGray));
                        player->p.dexterity -= status->value;
                    } break;
                    
                    case StatusEffectType_Elusion:
                    {
                        log_add(log, "%sYou don't feel as elusive anymore..", start_color(Color_LightGray));
                    } break;
                    
                    case StatusEffectType_Decay:
                    {
                        log_add(log, "%sYou don't feel as weak anymore..", start_color(Color_LightGray));
                        player->p.strength += status->value;
                        player->p.intelligence += status->value;
                        player->p.dexterity += status->value;
                    } break;
                    
                    case StatusEffectType_Confusion:
                    {
                        log_add(log, "%sYou don't feel disoriented anymore..", start_color(Color_LightGray));
                    } break;
                    
                    case StatusEffectType_Poison:
                    {
                        log_add(log, "%sYou don't feel sick anymore..", start_color(Color_LightGray));
                    } break;
                    
                    invalid_default_case;
                }
                
                memset(status, 0, sizeof(StatusEffect));
            }
        }
    }
}

internal void
attack_entity(RandomState *random,
              Dungeon *dungeon,
              String128 *log,
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
        log_add(log, "%sYour armor blocks the attack.", start_color(Color_LightGray));
    }
    else
    {
        log_add(log, "%s for %u damage.", attack.str, damage);
        
        if(defender->id != EntityID_Dummy)
        {
            defender->hp -= damage;
            if(is_underflowed(defender->hp))
            {
                kill_entity(random, dungeon->tiles, log, defender);
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
                           is_tile_traversable(dungeon->tiles, direction_pos) &&
                           !is_tile_occupied(dungeon->tiles, direction_pos))
                        {
                            set_tile_remains_value(dungeon->tiles, direction_pos, remains_id);
                        }
                    }
                }
            }
        }
    }
}

internal player_input_result
update_player_input(GameState *game,
                    GameInput *input,
                    Entity *player,
                    Entity *entities,
                    Dungeon *dungeon,
                    Item *items,
                    ItemInfo *item_info,
                    String128 *log,
                    Inventory *inventory,
                    u32 *entity_levels)
{
    player_input_result result = {0};
    player->new_pos = player->pos;
    
    if(inventory->is_asking_player)
    {
        if(was_pressed(&input->keyboard[Key_Yes], input->fkey_active))
        {
            log_add(log, "%sThe scroll turns illegible, you discard it.", start_color(Color_LightGray));
            
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
            result.should_update = true;
            input->fkey_active[2] = !input->fkey_active[2];
        }
        else if(input->fkeys[3].ended_down &&
                input->fkeys[3].has_been_up)
        {
            // This is checked for manually above so it works as expected.
            result.should_update = true;
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
            b32 was_entity = false;
            for(u32 index = 0; index < MAX_ENTITY_COUNT; ++index)
            {
                Entity *entity = &entities[index];
                if(entity->type != EntityType_Player &&
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
                        
                        break;
                    }
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
                    player->new_pos = get_direction_pos(player->pos, Direction_Up);
                }
                
                result.should_update = true;
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
                    player->new_pos = get_direction_pos(player->pos, Direction_Down);
                }
                
                result.should_update = true;
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
                    player->new_pos = get_direction_pos(player->pos, Direction_Left);
                }
                
                result.should_update = true;
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
                    player->new_pos = get_direction_pos(player->pos, Direction_Right);
                }
                
                result.should_update = true;
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
                    player->new_pos = get_direction_pos(player->pos, Direction_UpLeft);
                }
                
                result.should_update = true;
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
                    player->new_pos = get_direction_pos(player->pos, Direction_UpRight);
                }
                
                result.should_update = true;
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
                    player->new_pos = get_direction_pos(player->pos, Direction_DownLeft);
                }
                
                result.should_update = true;
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
                    player->new_pos = get_direction_pos(player->pos, Direction_DownRight);
                }
                
                result.should_update = true;
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
                        switch(random_number(&game->random, 1, 4))
                        {
                            case 1: log_add(log, "%sThe %s glows blue for a moment..", start_color(Color_LightBlue), item_id_text(item->id)); break;
                            case 2: log_add(log, "%sThe %s seems sharper than before..", start_color(Color_LightBlue), item_id_text(item->id)); break;
                            case 3: log_add(log, "%sThe %s vibrates slightly..", start_color(Color_LightBlue), item_id_text(item->id)); break;
                            case 4: log_add(log, "%sThe %s starts shimmering..", start_color(Color_LightBlue), item_id_text(item->id)); break;
                            
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
                        switch(random_number(&game->random, 1, 3))
                        {
                            case 1: log_add(log, "%sThe %s glows white for a moment..", start_color(Color_LightBlue), item_id_text(item->id)); break;
                            case 2: log_add(log, "%sThe %s looks sturdier than before..", start_color(Color_LightBlue), item_id_text(item->id)); break;
                            case 3: log_add(log, "%sThe %s feels warm for a moment..", start_color(Color_LightBlue), item_id_text(item->id)); break;
                            case 4: log_add(log, "%sThe %s feels different than before..", start_color(Color_LightBlue), item_id_text(item->id)); break;
                            
                            invalid_default_case;
                        }
                        
                        ++item->enchantment_level;
                        
                        if(item->is_equipped)
                        {
                            ++player->defence;
                        }
                        
                        complete_inventory_item_use(player, log, inventory);
                    }
                }
                else if(is_item_consumable(item->type))
                {
                    InventorySlot slot = get_slot_from_pos(inventory, inventory->pos);
                    set_consumable_as_known_and_identify_all(item->id, items, item_info);
                    
                    switch(item->id)
                    {
                        case ItemID_MightPotion:
                        {
                            if(!inventory->item_use_type)
                            {
                                log_add(log, "You drink the potion.. you feel more mighty.");
                                start_player_status_effect(player, StatusEffectType_Might, item->c.value, item->c.duration);
                                remove_item_from_inventory_and_game(slot, player, log, inventory);
                            }
                        } break;
                        
                        case ItemID_WisdomPotion:
                        {
                            if(!inventory->item_use_type)
                            {
                                log_add(log, "You drink the potion.. you feel more wise.");
                                start_player_status_effect(player, StatusEffectType_Wisdom, item->c.value, item->c.duration);
                                remove_item_from_inventory_and_game(slot, player, log, inventory);
                            }
                        } break;
                        
                        case ItemID_AgilityPotion:
                        {
                            if(!inventory->item_use_type)
                            {
                                log_add(log, "You drink the potion.. you feel more dexterous.");
                                start_player_status_effect(player, StatusEffectType_Agility, item->c.value, item->c.duration);
                                remove_item_from_inventory_and_game(slot, player, log, inventory);
                            }
                        } break;
                        
                        case ItemID_ElusionPotion:
                        {
                            if(!inventory->item_use_type)
                            {
                                log_add(log, "You drink the potion.. you feel more evasive.");
                                start_player_status_effect(player, StatusEffectType_Elusion, item->c.value, item->c.duration);
                                remove_item_from_inventory_and_game(slot, player, log, inventory);
                            }
                        } break;
                        
                        case ItemID_HealingPotion:
                        {
                            if(!inventory->item_use_type)
                            {
                                if(player->hp == player->max_hp)
                                {
                                    log_add(log, "You don't feel the need to drink that.");
                                }
                                else
                                {
                                    log_add(log, "You drink the potion.. you feel healthier.");
                                    heal_entity(player, item->c.value);
                                    remove_item_from_inventory_and_game(slot, player, log, inventory);
                                }
                            }
                        } break;
                        
                        case ItemID_DecayPotion:
                        {
                            if(!inventory->item_use_type)
                            {
                                log_add(log, "You drink the potion.. you feel much weaker.");
                                start_player_status_effect(player, StatusEffectType_Decay, item->c.value, item->c.duration);
                                remove_item_from_inventory_and_game(slot, player, log, inventory);
                            }
                        } break;
                        
                        case ItemID_ConfusionPotion:
                        {
                            if(!inventory->item_use_type)
                            {
                                log_add(log, "You drink the potion.. you feel disoriented.");
                                start_player_status_effect(player, StatusEffectType_Confusion, item->c.value, item->c.duration);
                                remove_item_from_inventory_and_game(slot, player, log, inventory);
                            }
                        } break;
                        
                        case ItemID_IdentifyScroll:
                        {
                            if(!inventory->item_use_type)
                            {
                                log_add(log, "You read the scroll.. choose an item to identify.");
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
                                log_add(log, "You read the scroll.. choose a weapon to enchant.");
                                inventory->item_use_type = ItemUseType_EnchantWeapon;
                                inventory->use_item_src_index = index;
                            }
                        } break;
                        
                        case ItemID_EnchantArmorScroll:
                        {
                            if(!inventory->item_use_type)
                            {
                                log_add(log, "You read the scroll.. choose an armor to enchant.");
                                inventory->item_use_type = ItemUseType_EnchantArmor;
                                inventory->use_item_src_index = index;
                            }
                        } break;
                        
                        case ItemID_MagicMappingScroll:
                        {
                            if(!inventory->item_use_type)
                            {
                                log_add(log, "You read the scroll.. your surroundings become clear to you.");
                                remove_item_from_inventory_and_game(slot, player, log, inventory);
                                
                                for(u32 y = 0; y < dungeon->h; ++y)
                                {
                                    for(u32 x = 0; x < dungeon->w; ++x)
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
                                log_add(log, "You read the scroll.. you find yourself in a different place.");
                                remove_item_from_inventory_and_game(slot, player, log, inventory);
                                
                                for(;;)
                                {
                                    v2u tele_pos = random_dungeon_pos(&game->random, dungeon);
                                    if(is_tile_traversable_and_not_occupied(dungeon->tiles, tele_pos))
                                    {
                                        move_entity(dungeon->tiles, player, tele_pos);
                                        break;
                                    }
                                }
                                
                                update_fov(dungeon, player, input->fkey_active);
                            }
                        } break;
                        
                        case ItemID_Ration:
                        {
                            if(!inventory->item_use_type)
                            {
                                if(player->hp == player->max_hp)
                                {
                                    log_add(log, "You don't feel the need to eat that.");
                                }
                                else
                                {
                                    log_add(log, "%sYou eat the ration and gain %u health.", start_color(Color_LightGreen), item->c.value);
                                    heal_entity(player, item->c.value);
                                    remove_item_from_inventory_and_game(slot, player, log, inventory);
                                }
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
                            item->is_equipped = false;
                        }
                        else
                        {
                            InventorySlot slot = equipped_inventory_slot_from_item_slot(item->slot, inventory);
                            if(slot.item)
                            {
                                slot.item->is_equipped = false;
                            }
                            
                            item->is_identified = true;
                            item->is_equipped = true;
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
                            log_add(log, "You drop the %s%s%s.",
                                    item_rarity_color_code(slot.item->rarity),
                                    item_name.str,
                                    end_color());
                        }
                        else
                        {
                            log_add(log, "You drop the %s%s%s.",
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
                            log_add(log, "You pick up the %s%s%s.",
                                    item_rarity_color_code(item->rarity),
                                    item_name.str,
                                    end_color());
                        }
                        else
                        {
                            log_add(log, "You pick up the %s%s%s.",
                                    item_rarity_color_code(item->rarity),
                                    item_id_text(item->id),
                                    end_color());
                        }
                    }
                    else
                    {
                        log_add(log, "Your inventory is too full.");
                    }
                }
                else
                {
                    log_add(log, "You find nothing to pick up.");
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
                        create_dungeon(&game->random, dungeon, player, log, entities, items, inventory, item_info, entity_levels);
                        log_add(log, "You descend further.. Level %u.", dungeon->level);
                        update_fov(dungeon, player, input->fkey_active);
                    }
                    else
                    {
                        game->mode = GameMode_Quit;
                    }
                }
                else
                {
                    log_add(log, "You don't see a path here.");
                }
            }
        }
        else if(was_pressed(&input->keyboard[Key_Wait], input->fkey_active))
        {
            if(!inventory->is_open)
            {
                result.should_update = true;
                result.new_action_time = 1.0f;
            }
        }
        
        return(result);
    }
}

internal void
update_entities(GameState *game,
                GameInput *input,
                Entity *player,
                Entity *entities,
                Dungeon *dungeon,
                Item *items,
                ItemInfo *item_info,
                String128 *log,
                Inventory *inventory,
                u32 *entity_levels)
{
    player_input_result input_result = update_player_input(game, input, player, entities, dungeon, items, item_info, log, inventory, entity_levels);
    
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
                if(input->fkey_active[2])
                {
                    if(is_inside_dungeon(dungeon, player->new_pos))
                    {
                        move_entity(dungeon->tiles, player, player->new_pos);
                        update_fov(dungeon, player, input->fkey_active);
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
                                if(input->fkey_active[4])
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
                                        attack_entity(&game->random, dungeon, log, inventory, player, entity, modified_player_damage);
                                    }
                                    else
                                    {
                                        log_add(log, "%sYou miss the %s.", start_color(Color_LightGray), entity->name);
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
                
                update_player_status_effects(game, dungeon, player, log);
                update_fov(dungeon, player, input->fkey_active);
                update_pathfind_map(dungeon, player->pos);
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
                        if(tile_is_seen(dungeon->tiles, enemy->pos) && !input->fkey_active[1])
#else
                        if(tile_is_seen(dungeon->tiles, enemy->pos))
#endif
                        {
                            enemy->e.in_combat = true;
                            enemy->e.is_pathfind_set = false;
                            
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
                            
                            v2u next_pos = next_pathfind_pos(&dungeon->pathfind, dungeon->tiles, player->pos, enemy->pos);
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
                                                    v4u player_fov_rect = get_player_fov_rect(dungeon->w, dungeon->h, player->pos, player->p.fov);
                                                    if(is_inside_rect(player_fov_rect, target->pos) &&
                                                       target->e.in_combat &&
                                                       target->hp < target->max_hp)
                                                    {
                                                        log_add(log, "The %s heals %s for %u HP.", enemy->name, target->name, spell->value);
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
                                            attack_entity(&game->random, dungeon, log, inventory, enemy, player, spell->value);
                                        }
                                        else
                                        {
                                            player_dodge_log_add(log);
                                        }
                                        
                                        spell_was_used = true;
                                    }
                                }
                            }
                            else if(enemy->e.is_ranger || equal_v2u(next_pos, player->pos))
                            {
                                if(entity_will_hit(&game->random, enemy_hit_chance, player->evasion))
                                {
                                    attack_entity(&game->random, dungeon, log, inventory, enemy, player, enemy->e.damage);
                                    
                                    if(enemy->e.poison_chance &&
                                       !player->p.statuses[StatusEffectType_Poison].is_enabled &&
                                       enemy->e.poison_chance <= random_number(&game->random, 1, 100))
                                    {
                                        log_add(log, "%sYou start feeling sick..", start_color(Color_LightGray));
                                        start_player_status_effect(player, StatusEffectType_Poison, enemy->e.poison_damage, enemy->e.poison_duration);
                                    }
                                }
                                else
                                {
                                    player_dodge_log_add(log);
                                }
                            }
                            else
                            {
                                // TODO(rami): Maybe ranged characters should back off, previous_pathfind_pos()?
                                enemy->new_pos = next_pos;
                            }
                        }
                        else
                        {
                            if(enemy->e.in_combat && !is_enemy_alerted(enemy->e.turns_in_player_view))
                            {
                                if(!enemy->e.is_pathfind_set)
                                {
                                    enemy->e.is_pathfind_set = true;
                                    enemy->e.pathfind = dungeon->pathfind;
                                    enemy->e.pathfind_pos = player->pos;
                                    
                                    //printf("Pathfind Set: %u, %u\n", enemy->e.pathfind_pos.x, enemy->e.pathfind_pos.y);
                                }
                                
                                enemy->new_pos = next_pathfind_pos(&enemy->e.pathfind, dungeon->tiles, enemy->e.pathfind_pos, enemy->pos);
                                //printf("Pathfind New Pos: %u, %u\n", enemy->new_pos.x, enemy->new_pos.y);
                                
                                if(equal_v2u(enemy->new_pos, enemy->e.pathfind_pos))
                                {
                                    //printf("Pathfind: At Destination\n");
                                    
                                    enemy->e.in_combat = false;
                                    enemy->e.is_pathfind_set = false;
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
                                    if(tile_is_seen(dungeon->tiles, enemy->new_pos) && !input->fkey_active[1])
#else
                                    if(tile_is_seen(dungeon->tiles, enemy->new_pos))
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
                        if(is_tile_traversable(dungeon->tiles, enemy->new_pos))
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
                    printf("is_pathfind_set: %u\n", enemy->e.is_pathfind_set);
                    printf("in_combat: %u\n\n", enemy->e.in_combat);
#endif
                }
            }
        }
    }
}

internal void
render_entities(GameState *game,
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
            if(tile_is_seen(dungeon->tiles, enemy->pos))
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
                            v4u src = get_tile_rect(enemy->tile_pos);
                            v4u dest = get_game_dest(game, enemy->e.ghost_pos);
                            render_texture_half_color(game->renderer, assets->tileset.tex, src, dest, enemy->e.is_ghost_flipped);
                        }
                    }
                    else
                    {
                        // If enemy pos is seen then enemy ghost is placed on new enemy pos.
                        // This means that the enemy moved.
                        if(tile_is_seen(dungeon->tiles, enemy->e.pos_save_for_ghost))
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
add_player_entity(RandomState *random, Entity *player, Item *items, Inventory *inventory)
{
    player->id = EntityID_Player;
    strcpy(player->name, "Name");
    
#if 0
    player->hp = 1;
#elif 1
    player->hp = 80;
#else
    player->hp = 1000000;
#endif
    
    player->max_hp = 80;
    
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
                 DungeonTiles tiles,
                 u32 *entity_levels,
                 EntityID id,
                 u32 x, u32 y)
{
    for(u32 index = ENEMY_START_ID; index < ENEMY_ENTITY_COUNT; ++index)
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
                    
                    enemy->e.damage = 5;
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