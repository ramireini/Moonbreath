internal void
add_item_mark_character(char c, Item *item, UI *ui)
{
    if(item->mark_length < (MAX_MARK_SIZE - 1))
    {
        item->mark[item->mark_length++] = c;
        
        ++ui->mark_view.entry_count;
        if(view_needs_scrolling(ui->mark_view))
        {
            set_view_at_end(&ui->mark_view);
        }
    }
}

internal b32
is_entity_under_status_effect(Entity *entity, StatusEffectType index)
{
    b32 result = entity->statuses[index].is_enabled;
    return(result);
}

internal b32
is_entity_under_any_status_effect(Entity *entity)
{
    for(u32 status_index = 0; status_index < StatusEffectType_Count; ++status_index)
    {
        if(is_entity_under_status_effect(entity, status_index))
        {
            return(true);
        }
    }
    
    return(false);
}

internal void
get_confused_move_pos(Random *random, Dungeon *dungeon, UI *ui, Entity *entity)
{
    if(is_tile_traversable(dungeon->tiles, entity->new_pos))
    {
        StatusEffect *status = &entity->statuses[StatusEffectType_Confusion];
        assert(status->chance);
        
        if(random_chance_number(random) <= status->chance)
        {
            Direction confused_direction = get_random_direction(random);
            v2u confused_pos = get_direction_pos(entity->pos, confused_direction);
            
            if(confused_direction != entity->new_direction &&
               is_tile_traversable_and_not_occupied(dungeon->tiles, confused_pos))
            {
                if(entity->type == EntityType_Player)
                {
                    log_add(ui, "%sYou stumble slightly.", start_color(Color_LightGray));
                }
                else if(entity->type == EntityType_Enemy)
                {
                    log_add(ui, "%sThe %s stumbles slightly.", start_color(Color_LightGray), entity->name);
                }
                
                entity->new_pos = confused_pos;
                
#if 0
                printf("confused_direction: %u\n", confused_direction);
                printf("entity->new_pos: %u, %u\n", entity->new_pos.x, entity->new_pos.y);
#endif
            }
        }
    }
}

internal b32
in_spell_range(u32 range, v2u origin, v2u target)
{
    b32 result = (!range || tile_dist_cardinal_and_ordinal(origin, target) <= range);
    return(result);
}

internal b32
is_inside_rect_and_in_spell_range(v4u rect, u32 range, v2u origin, v2u target)
{
    b32 result = (is_inside_rect(rect, target) &&
                  in_spell_range(range, origin, target));
    
    return(result);
}

internal char *
get_status_effect_name(StatusEffectType type)
{
    char *result = 0;
    
    switch(type)
    {
        case StatusEffectType_Bolster: result = "Bolster"; break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal char *
get_spell_name(SpellID id)
{
    char *result = 0;
    
    switch(id)
    {
        case SpellID_DarkBolt: result = "Dark Bolt"; break;
        case SpellID_LesserHeal: result = "Lesser Heal"; break;
        case SpellID_Bolster: result = "Bolster"; break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal char *
get_spell_description(SpellID id)
{
    char *result = 0;
    
    switch(id)
    {
        case SpellID_DarkBolt: result = "Hurls a bolt of dark energy at the target."; break;
        case SpellID_LesserHeal: result = "Heals the target by a slight amount."; break;
        case SpellID_Bolster: result = "Increases the defence of the target."; break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal void
log_add_okay(UI *ui)
{
    log_add(ui, "%sOkay.", start_color(Color_Yellow));
}

internal void
log_add_item_action_text(UI *ui, Item *item, ItemActionType type)
{
    char action_text[8] = {0};
    if(type == ItemActionType_PickUp)
    {
        strcpy(action_text, "pick up");
    }
    else if(type == ItemActionType_Drop)
    {
        strcpy(action_text, "drop");
    }
    
    String128 item_name = get_full_item_name(item);
    log_add(ui, "You %s the %s%s%s%s",
            action_text,
            item_status_color(item),
            item_status_prefix(item),
            item_name.str,
            end_color());
}

internal void
update_examine_pos(Examine *examine, Direction move_direction, Dungeon *dungeon)
{
    v2u new_pos = get_direction_pos(examine->pos, move_direction);
    
#if MOONBREATH_SLOW
    if(fkey_active[2])
    {
        if(is_inside_dungeon(dungeon, new_pos))
        {
            examine->pos = new_pos;
        }
    }
    else
#endif
    {
        if(is_inside_dungeon(dungeon, new_pos) &&
           has_tile_been_seen(dungeon->tiles, new_pos))
        {
            examine->pos = new_pos;
        }
    }
}

internal b32
is_entity_valid_and_not_player(EntityType type)
{
    b32 result = (type && type != EntityType_Player);
    return(result);
}

internal b32
is_examine_and_inspect_and_inventory_and_log_closed(Game *game, Inventory *inventory, UI *ui)
{
    b32 result = (!is_set(game->examine.flags, ExamineFlags_Open) &&
                  !game->examine.type &&
                  !is_set(inventory->flags, InventoryFlags_Open) &&
                  !ui->is_full_log_open);
    
    return(result);
}

internal void
start_entity_pathfind(Entity *entity, Dungeon *dungeon, Item *items, v2u pathfind_target)
{
    if(!equal_v2u(entity->pos, pathfind_target))
    {
        set(entity->flags, EntityFlags_Pathfinding);
        
        entity->pathfind.width = dungeon->width;
        entity->pathfind_target = pathfind_target;
        update_pathfind_map(dungeon, &entity->pathfind, entity->pathfind_target);
        
        if(entity->type == EntityType_Player)
        {
            handle_new_pathfind_items(dungeon->tiles, items);
        }
    }
}

internal void
add_player_starting_item(Random *random, Item *items, ItemInfo *item_info, Inventory *inventory, ItemID item_id, u32 x, u32 y)
{
    Item *item = 0;
    
    if(is_item_id_weapon(item_id))
    {
        item = add_weapon_item(random, items, item_id, ItemRarity_Common, x, y, false);
        set(item->flags, ItemFlags_Equipped);
    }
    else if(is_item_id_potion(item_id))
    {
        item = add_consumable_item(random, items, item_info, item_id, x, y);
        set_as_known_and_identify_existing(item_id, items, item_info);
    }
    
    assert(item);
    
    item->enchantment_level = 0;
    set(item->flags, ItemFlags_Identified);
    unset(item->flags, ItemFlags_Cursed);
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

internal b32
is_zero_or_underflow(u32 value)
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
add_enemy_spell(Entity *enemy, SpellID id)
{
    assert(enemy->type == EntityType_Enemy &&
           is_set(enemy->flags, EntityFlags_MagicAttacks));
    
    for(u32 index = 0; index < MAX_ENTITY_SPELL_COUNT; ++index)
    {
        Spell *spell = &enemy->e.spells[index];
        if(!spell->id)
        {
            spell->id = id;
            StatusEffect *effect = &spell->effect;
            
            switch(id)
            {
                case SpellID_DarkBolt:
                {
                    spell->type = SpellType_Offensive;
                    spell->damage_type = DamageType_Darkness;
                    effect->value = 5;
                    effect->chance = 40;
                } break;
                
                case SpellID_LesserHeal:
                {
                    spell->type = SpellType_Healing;
                    effect->value = 2;
                    effect->chance = 30;
                } break;
                
                case SpellID_Bolster:
                {
                    spell->type = SpellType_Buff;
                    effect->type = StatusEffectType_Bolster;
                    effect->value = 2;
                    effect->chance = 20;
                    effect->duration = 6;
                } break;
                
                invalid_default_case;
            }
            
            ++enemy->e.spell_count;
            return;
        }
    }
    
    assert(false);
}

internal void
start_entity_status_effect(Entity *entity, StatusEffect status)
{
    StatusEffect *entity_status = &entity->statuses[status.type];
    entity_status->is_enabled = true;
    entity_status->value = status.value;
    entity_status->chance = status.chance;
    entity_status->duration = status.duration;
    
    switch(status.type)
    {
        case StatusEffectType_Might: entity->strength += status.value; break;
        case StatusEffectType_Wisdom: entity->intelligence += status.value; break;
        case StatusEffectType_Agility: entity->dexterity += status.value; break;
        case StatusEffectType_Elusion: break;
        
        case StatusEffectType_Decay:
        {
            entity->strength -= status.value;
            entity->intelligence -= status.value;
            entity->dexterity -= status.value;
        } break;
        
        case StatusEffectType_Confusion: break;
        case StatusEffectType_Poison: break;
        
        case StatusEffectType_Bolster: entity->defence += status.value; break;
        
        invalid_default_case;
    }
}

internal void
update_entity_status_effects(Game *game, Dungeon *dungeon, UI *ui, Entity *entity)
{
    for(u32 index = 0; index < StatusEffectType_Count; ++index)
    {
        StatusEffect *status = &entity->statuses[index];
        if(status->is_enabled)
        {
            if(status->duration)
            {
                switch(index)
                {
                    case StatusEffectType_Might: break;
                    case StatusEffectType_Wisdom: break;
                    case StatusEffectType_Agility: break;
                    case StatusEffectType_Confusion: break;
                    case StatusEffectType_Elusion: break;
                    case StatusEffectType_Decay: break;
                    
                    case StatusEffectType_Poison:
                    {
                        if(entity->type == EntityType_Player)
                        {
                            log_add(ui, "%sThe poison wrecks you for %u damage.", start_color(Color_DarkGreen), status->value);
                        }
                        
                        entity->hp -= status->value;
                        if(is_zero_or_underflow(entity->hp))
                        {
                            kill_entity(&game->random, dungeon->tiles, ui, entity);
                        }
                    } break;
                    
                    case StatusEffectType_Bolster: break;
                    
                    invalid_default_case;
                }
            }
            
            --status->duration;
            if(!status->duration)
            {
                switch(index)
                {
                    case StatusEffectType_Might:
                    {
                        if(entity->type == EntityType_Player)
                        {
                            log_add(ui, "%sYou don't feel as mighty anymore.", start_color(Color_LightGray));
                        }
                        
                        entity->strength -= status->value;
                    } break;
                    
                    case StatusEffectType_Wisdom:
                    {
                        if(entity->type == EntityType_Player)
                        {
                            log_add(ui, "%sYou don't feel as wise anymore.", start_color(Color_LightGray));
                        }
                        
                        entity->intelligence -= status->value;
                    } break;
                    
                    case StatusEffectType_Agility:
                    {
                        if(entity->type == EntityType_Player)
                        {
                            log_add(ui, "%sYou don't feel as agile anymore.", start_color(Color_LightGray));
                        }
                        
                        entity->dexterity -= status->value;
                    } break;
                    
                    case StatusEffectType_Elusion:
                    {
                        if(entity->type == EntityType_Player)
                        {
                            log_add(ui, "%sYou don't feel as elusive anymore.", start_color(Color_LightGray));
                        }
                    } break;
                    
                    case StatusEffectType_Decay:
                    {
                        if(entity->type == EntityType_Player)
                        {
                            log_add(ui, "%sYou don't feel as weak anymore.", start_color(Color_LightGray));
                        }
                        
                        entity->strength += status->value;
                        entity->intelligence += status->value;
                        entity->dexterity += status->value;
                    } break;
                    
                    case StatusEffectType_Confusion:
                    {
                        if(entity->type == EntityType_Player)
                        {
                            log_add(ui, "%sYou don't feel confused anymore.", start_color(Color_LightGray));
                        }
                    } break;
                    
                    case StatusEffectType_Poison:
                    {
                        if(entity->type == EntityType_Player)
                        {
                            log_add(ui, "%sYou don't feel sick anymore.", start_color(Color_LightGray));
                        }
                    } break;
                    
                    case StatusEffectType_Bolster: entity->defence -= status->value; break;
                    
                    invalid_default_case;
                }
                
                zero_struct(*status);
            }
        }
    }
}

internal b32
will_entity_hit(Random *random, Entity *attacker, Entity *defender)
{
    b32 result = false;
    
    u32 attacker_hit_chance = attacker->hit_chance;
    if(is_set(defender->flags, EntityFlags_Invisible))
    {
        attacker_hit_chance /= 2;
    }
    
    u32 defender_evasion = defender->evasion;
    if(is_set(attacker->flags, EntityFlags_Invisible))
    {
        defender_evasion /= 3;
    }
    
    if(random_number(random, 1, attacker_hit_chance) >= defender_evasion)
    {
        result = true;
    }
    
    return(result);
}

internal void
move_entity(Entity *entity, Tiles tiles, v2u new_pos)
{
    set_tile_occupied(tiles, entity->pos, false);
    entity->pos = entity->new_pos = new_pos;
    set_tile_occupied(tiles, entity->new_pos, true);
    
    switch(entity->new_direction)
    {
        case Direction_Left:
        case Direction_UpLeft:
        case Direction_DownLeft: set(entity->flags, EntityFlags_Flipped); break;
        
        case Direction_Right:
        case Direction_UpRight:
        case Direction_DownRight: unset(entity->flags, EntityFlags_Flipped); break;
    }
}

internal b32
heal_entity(Entity *entity, u32 value)
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

internal String128
get_entity_attack_text(Random *random, Inventory *inventory,
                       Entity *attacker, Entity *defender,
                       u32 value)
{
    String128 result = {0};
    
    if(attacker->type == EntityType_Player)
    {
        char *attack_text = 0;
        
        Item *item = get_equipped_item_from_slot(ItemSlot_FirstHand, inventory);
        if(item)
        {
            switch(item->id)
            {
                case ItemID_Dagger:
                case ItemID_Sword:
                {
                    switch(random_number(random, 1, 6))
                    {
                        case 1: attack_text = "stab"; break;
                        case 2: attack_text = "pierce"; break;
                        case 3: attack_text = "puncture"; break;
                        case 4: attack_text = "slash"; break;
                        case 5: attack_text = "lacerate"; break;
                        case 6: attack_text = "cleave"; break;
                        
                        invalid_default_case;
                    }
                } break;
                
                case ItemID_Club:
                case ItemID_Warhammer:
                {
                    switch(random_number(random, 1, 6))
                    {
                        case 1: attack_text = "smash"; break;
                        case 2: attack_text = "bash"; break;
                        case 3: attack_text = "strike"; break;
                        case 4: attack_text = "pummel"; break;
                        case 5: attack_text = "pound"; break;
                        case 6: attack_text = "crush"; break;
                        
                        invalid_default_case;
                    }
                } break;
                
                case ItemID_Battleaxe:
                {
                    switch(random_number(random, 1, 6))
                    {
                        case 1: attack_text = "hack"; break;
                        case 2: attack_text = "rend"; break;
                        case 3: attack_text = "chop"; break;
                        case 4: attack_text = "slash"; break;
                        case 5: attack_text = "lacerate"; break;
                        case 6: attack_text = "cleave"; break;
                        
                        invalid_default_case;
                    }
                } break;
                
                case ItemID_Spear:
                {
                    switch(random_number(random, 1, 4))
                    {
                        case 1: attack_text = "stab"; break;
                        case 2: attack_text = "pierce"; break;
                        case 3: attack_text = "puncture"; break;
                        case 4: attack_text = "strike"; break;
                        
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
                case 1: attack_text = "punch"; break;
                case 2: attack_text = "kick"; break;
                
                invalid_default_case;
            }
        }
        
        if(is_set(defender->flags, EntityFlags_Invisible))
        {
            snprintf(result.str, sizeof(result.str), "You attack the something, dealing %u damage.", value);
        }
        else
        {
            snprintf(result.str, sizeof(result.str), "You %s the %s, dealing %u damage.", attack_text, defender->name, value);
        }
    }
    else if(attacker->type == EntityType_Enemy)
    {
        if(is_set(attacker->flags, EntityFlags_MagicAttacks))
        {
            Spell *spell = &attacker->e.spells[attacker->e.spell_index];
            
            if(defender->type == EntityType_Player)
            {
                snprintf(result.str, sizeof(result.str), "The %s casts %s at you, dealing %u damage.", attacker->name, get_spell_name(spell->id), value);
            }
            else
            {
                snprintf(result.str, sizeof(result.str), "The %s casts %s at the %s, healing it for %u health.", attacker->name, get_spell_name(spell->id), defender->name, value);
            }
        }
        else if(is_set(attacker->flags, EntityFlags_RangedAttacks))
        {
            snprintf(result.str, sizeof(result.str), "The %s fires an arrow at you, dealing %u damage.", attacker->name, value);
        }
        else if(is_set(attacker->flags, EntityFlags_Invisible))
        {
            snprintf(result.str, sizeof(result.str), "Something attacks you, dealing %u damage.", value);
        }
        else
        {
            snprintf(result.str, sizeof(result.str), "The %s attacks you, dealing %u damage.", attacker->name, value);
        }
    }
    
    return(result);
}

internal void
remove_entity(Entity *entity)
{
    zero_struct(*entity);
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
attack_entity(Random *random,
              Dungeon *dungeon,
              Inventory *inventory,
              UI *ui,
              Entity *attacker,
              Entity *defender,
              u32 damage)
{
#if MOONBREATH_SLOW
    if(defender->id == EntityID_Dummy) return;
#endif
    
    
#if MOONBREATH_SLOW
    // Player Hit Test
    if(attacker->type == EntityType_Player && fkey_active[4])
    {
        printf("attacker->hit_chance: %u\n", attacker->hit_chance);
        printf("defender->evasion: %u\n", defender->evasion);
        
        u32 hit_count = 0;
        u32 miss_count = 0;
        u32 loop_count = 1000;
        
        for(u32 loop_index = 0; loop_index < loop_count; ++loop_index)
        {
            if(will_entity_hit(random, attacker, defender))
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
        
        return;
    }
#endif
    
    if(will_entity_hit(random, attacker, defender))
    {
        if(defender->defence)
        {
            damage -= random_number(random, 0, defender->defence);
        }
        
        if(!is_zero_or_underflow(damage))
        {
            String128 attack_text = get_entity_attack_text(random, inventory, attacker, defender, damage);
            log_add(ui, attack_text.str);
            
            defender->hp -= damage;
            if(!is_zero_or_underflow(defender->hp))
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
                    
                    if(random_chance_number(random) <= 30)
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
                
                if(attacker->type == EntityType_Enemy &&
                   defender->type == EntityType_Player)
                {
                    if(!is_entity_under_status_effect(defender, StatusEffectType_Poison) &&
                       attacker->e.poison.is_enabled &&
                       attacker->e.poison.chance <= random_chance_number(random))
                    {
                        log_add(ui, "%sYou start feeling sick.", start_color(Color_LightGray));
                        start_entity_status_effect(defender, attacker->e.poison);
                    }
                }
            }
            else
            {
                kill_entity(random, dungeon->tiles, ui, defender);
            }
        }
        else
        {
            if(defender->type == EntityType_Player)
            {
                log_add(ui, "%sYour armor blocks the attack.", start_color(Color_LightGray));
            }
            else if(defender->type == EntityType_Enemy)
            {
                log_add(ui, "%sThe %s's armor blocks your attack.", start_color(Color_LightGray), defender->name);
            }
        }
    }
    else
    {
        if(attacker->type == EntityType_Player)
        {
            log_add(ui, "%sYou miss the %s.", start_color(Color_LightGray), defender->name);
        }
        else if(attacker->type == EntityType_Enemy)
        {
            log_add(ui, "%sYou dodge the attack.", start_color(Color_LightGray));
        }
    }
}

internal char
get_pressed_alphabet_letter(InputState *keyboard)
{
    char result = 0;
    char letter = 'a';
    
    for(u32 index = KeyboardKey_A; index <= KeyboardKey_Z; ++index)
    {
        if(was_pressed(&keyboard[index]))
        {
            result = letter;
            
            // Turn to uppercase.
            if(keyboard[KeyboardKey_Shift].ended_down)
            {
                result -= 32;
            }
            
            break;
        }
        
        ++letter;
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
    player->new_direction = Direction_None;
    set(player->flags, EntityFlags_LogMultipleItemMessage);
    
    if(is_set(player->flags, EntityFlags_Pathfinding))
    {
        b32 found_something = handle_new_pathfind_items(dungeon->tiles, items);
        
        if(!found_something)
        {
            for(u32 index = 0; index < EntityID_Count; ++index)
            {
                Entity *entity = &entities[index];
                if(entity->type == EntityType_Enemy &&
                   is_tile_seen(dungeon->tiles, entity->pos))
                {
                    found_something = true;
                    break;
                }
            }
        }
        
        if(found_something)
        {
            unset(player->flags, EntityFlags_Pathfinding);
        }
        else
        {
            player->new_pos = next_pathfind_pos(&player->pathfind, dungeon->tiles, player->pos, player->pathfind_target);
            
#if 0
            printf("Auto Explore: Destination %u, %u\n", player->p.pathfind_target.x, player->p.pathfind_target.y);
            printf("Auto Explore: New Pos %u, %u\n\n", player->new_pos.x, player->new_pos.y);
#endif
            
            if(equal_v2u(player->new_pos, player->pathfind_target))
            {
                //printf("Auto Explore: Destination Reached\n");
                unset(player->flags, EntityFlags_Pathfinding);
            }
            
            result.should_update = true;
        }
    }
    else if(is_set(inventory->flags, InventoryFlags_AskingPlayer))
    {
        if(was_pressed(&input->GameKey_Yes))
        {
            Item *item = inventory->slots[inventory->inspect_index];
            if(inventory->using_item_type)
            {
                log_add(ui, "%sThe scroll turns illegible, you discard it.", start_color(Color_LightGray));
                inventory->using_item_type = UsingItemType_None;
            }
            
            remove_item_from_inventory_and_game(&game->random, items, item_info, item, inventory);
            unset(inventory->flags, InventoryFlags_AskingPlayer | InventoryFlags_ReadyForKeypress);
        }
        else if(was_pressed(&input->GameKey_No))
        {
            log_add_okay(ui);
            unset(inventory->flags, InventoryFlags_AskingPlayer | InventoryFlags_ReadyForKeypress);
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
                if(item->id && item->type != ItemID_Ration)
                {
                    if(is_set(item->flags, ItemFlags_Identified))
                    {
                        unset(item->flags, ItemFlags_Identified);
                    }
                    else
                    {
                        set(item->flags, ItemFlags_Identified);
                    }
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
        else if(input->MouseButton_Left.ended_down)
        {
            for(u32 index = 0; index < MAX_ENTITY_COUNT; ++index)
            {
                Entity *entity = &entities[index];
                if(is_entity_valid_and_not_player(entity->type) &&
                   equal_v2u(entity->pos, input->mouse_tile_pos))
                {
                    remove_entity(entity);
                    break;
                }
            }
        }
        else if(was_pressed(&input->MouseButton_Right))
        {
            b32 found_entity = false;
            for(u32 index = 0; index < MAX_ENTITY_COUNT; ++index)
            {
                Entity *entity = &entities[index];
                if(is_entity_valid_and_not_player(entity->type) &&
                   equal_v2u(entity->pos, input->mouse_tile_pos))
                {
                    found_entity = true;
                    
                    printf("Entity Name: %s\n", entity->name);
                    printf("Entity Defence: %u\n", entity->defence);
                    
                    break;
                }
            }
            
            if(!found_entity)
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
                if(was_pressed(&input->game_keys[index]) || game->examine.is_key_pressed[index])
                {
                    if(is_set(game->examine.flags, ExamineFlags_Open) && !game->examine.type)
                    {
                        update_examine_pos(&game->examine, direction, dungeon);
                    }
                    else if(is_examine_and_inspect_and_inventory_and_log_closed(game, inventory, ui))
                    {
                        player->new_pos = get_direction_pos(player->pos, direction);
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
                        set_view_at_start(&inventory->view);
                        
                        set(inventory->flags, InventoryFlags_Open);
                        unset(inventory->flags, InventoryFlags_AskingPlayer);
                    }
                }
                else if(was_pressed(&input->GameKey_Back))
                {
                    Examine *examine = &game->examine;
                    if(is_set(game->examine.flags, ExamineFlags_Open) || examine->type)
                    {
                        if(examine->type == ExamineType_EntitySpell)
                        {
                            examine->type = ExamineType_Entity;
                        }
                        else
                        {
                            unset(game->examine.flags, ExamineFlags_Open | ExamineFlags_ReadyForKeypress);
                            
                            examine->type = ExamineType_None;
                            examine->item = 0;
                            examine->entity = 0;
                            examine->spell = 0;
                            examine->tile_id = TileID_None;
                        }
                    }
                    else if(is_set(inventory->flags, InventoryFlags_Adjusting))
                    {
                        unset(inventory->flags, InventoryFlags_Adjusting);
                        log_add_okay(ui);
                    }
                    else if(is_set(inventory->flags, InventoryFlags_Inspecting))
                    {
                        unset(inventory->flags, InventoryFlags_Inspecting);
                    }
                    else if(inventory->using_item_type)
                    {
                        ask_for_item_cancel(game, ui, inventory);
                    }
                    else if(is_set(inventory->flags, InventoryFlags_Open))
                    {
                        unset(inventory->flags, InventoryFlags_Open | InventoryFlags_ReadyForKeypress);
                    }
                    else if(ui->is_full_log_open)
                    {
                        ui->is_full_log_open = false;
                    }
                }
                else if(was_pressed(&input->GameKey_Pickup))
                {
                    if(is_examine_and_inspect_and_inventory_and_log_closed(game, inventory, ui))
                    {
                        Item *item = get_item_on_pos(player->pos, items);
                        if(item)
                        {
                            AddedItemResult add_result = add_item_to_inventory(item, inventory);
                            if(add_result.added_to_inventory)
                            {
                                log_add_item_action_text(ui, item, ItemActionType_PickUp);
                                
                                if(add_result.added_to_consumable_stack)
                                {
                                    remove_item_from_game(item);
                                }
                                
                                unset(player->flags, EntityFlags_LogMultipleItemMessage);
                                result.action_count = 1.0f;
                            }
                            else
                            {
                                log_add(ui, "Your inventory is full.");
                            }
                        }
                        else
                        {
                            log_add(ui, "You see nothing to pick up here.");
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
                        assert(!is_set(player->flags, EntityFlags_Pathfinding));
                        
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
                            start_entity_pathfind(player, dungeon, items, pathfind_pos);
                        }
                        else
                        {
                            log_add(ui, "Nothing more to explore.");
                        }
                    }
                }
                else if(was_pressed(&input->GameKey_Examine))
                {
                    Examine *examine = &game->examine;
                    
                    if(is_examine_and_inspect_and_inventory_and_log_closed(game, inventory, ui))
                    {
                        set(game->examine.flags, ExamineFlags_Open);
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
                }
                else if(was_pressed(&input->GameKey_Wait))
                {
                    if(is_examine_and_inspect_and_inventory_and_log_closed(game, inventory, ui))
                    {
                        unset(player->flags, EntityFlags_LogMultipleItemMessage);
                        
                        result.should_update = true;
                        result.action_count = 1.0f;
                    }
                }
                else if(input->MouseButton_ScrollUp.ended_down ||
                        input->MouseButton_ScrollDown.ended_down ||
                        input->KeyboardKey_PageUp.ended_down ||
                        input->KeyboardKey_PageDown.ended_down)
                {
                    if(is_set(inventory->flags, InventoryFlags_Open) &&
                       is_inside_rect(inventory->rect, input->mouse_pos))
                    {
                        update_view_scrolling(&inventory->view, input);
                    }
                    else if(ui->is_full_log_open &&
                            is_inside_rect(ui->full_log_rect, input->mouse_pos))
                    {
                        update_view_scrolling(&ui->full_log_view, input);
                    }
                }
                else if(is_set(inventory->flags, InventoryFlags_Open))
                {
                    Item *inspect_item = inventory->slots[inventory->inspect_index];
                    
                    if(is_set(inventory->flags, InventoryFlags_Inspecting))
                    {
                        if(is_set(inventory->flags, InventoryFlags_Marking))
                        {
                            ui->mark_view.entry_count = inspect_item->mark_length;
                            
                            if(was_pressed(&input->KeyboardKey_Enter))
                            {
                                unset(inventory->flags, InventoryFlags_Marking);
                            }
                            else if(was_pressed(&input->KeyboardKey_Backspace) ||
                                    input->KeyboardKey_Backspace.repeating)
                            {
                                if(inspect_item->mark_length)
                                {
                                    inspect_item->mark[inspect_item->mark_length - 1] = 0;
                                    --inspect_item->mark_length;
                                    
                                    move_view_towards_start(&ui->mark_view);
                                }
                            }
                            else if(was_pressed(&input->KeyboardKey_Space))
                            {
                                add_item_mark_character(' ', inspect_item, ui);
                            }
                            else
                            {
                                char pressed_letter = get_pressed_alphabet_letter(input->keyboard);
                                if(pressed_letter)
                                {
                                    add_item_mark_character(pressed_letter, inspect_item, ui);
                                }
                            }
                            
                            //printf("Mark: %s\n", inspect_item->mark);
                        }
                        else
                        {
                            if(is_set(inventory->flags, InventoryFlags_Adjusting))
                            {
                                char pressed_letter = get_pressed_alphabet_letter(input->keyboard);
                                if(pressed_letter)
                                {
                                    // If there is an item that already has the letter we want, then
                                    // get that item a free item letter.
                                    Item *inventory_item = get_inventory_item_with_letter(inventory, pressed_letter);
                                    if(inventory_item)
                                    {
                                        inventory_item->inventory_letter = get_free_item_letter(inventory);
                                    }
                                    
                                    inspect_item->inventory_letter = pressed_letter;
                                    unset(inventory->flags, InventoryFlags_Adjusting);
                                    
                                    String128 letter_string = get_item_letter_string(inspect_item->inventory_letter);
                                    String128 item_name = get_full_item_name(inspect_item);
                                    log_add(ui, "%s%s%s", letter_string.str, item_status_prefix(inspect_item), item_name.str);
                                }
                            }
                            else
                            {
                                if(was_pressed(&input->KeyboardKey_A))
                                {
                                    set(inventory->flags, InventoryFlags_Adjusting);
                                    log_add(ui, "%sAdjust to which letter? (%c to quit).", start_color(Color_Yellow), game->keybinds[GameKey_Back]);
                                }
                                else if(was_pressed(&input->KeyboardKey_E))
                                {
                                    if(is_item_equipment(inspect_item->type) &&
                                       !is_set(inspect_item->flags, ItemFlags_Equipped))
                                    {
                                        b32 can_equip_new_item = true;
                                        
                                        Item *equipped_item = get_equipped_item_from_slot(inspect_item->slot, inventory);
                                        if(equipped_item)
                                        {
                                            if(unequip_item(ui, equipped_item))
                                            {
                                                unset(inspect_item->flags, ItemFlags_Equipped);
                                                result.action_count = 1.0f;
                                            }
                                            else
                                            {
                                                can_equip_new_item = false;
                                            }
                                        }
                                        
                                        if(can_equip_new_item)
                                        {
                                            if(is_set(inspect_item->flags, ItemFlags_Cursed))
                                            {
                                                log_add(ui, "%sThe %s feels like it's stuck to your hand.", start_color(Color_LightRed), item_id_text(inspect_item->id));
                                            }
                                            
                                            set(inspect_item->flags, ItemFlags_Identified | ItemFlags_Equipped);
                                            result.action_count += 1.0f;
                                        }
                                    }
                                }
                                else if(was_pressed(&input->KeyboardKey_U))
                                {
                                    if(unequip_item(ui, inspect_item))
                                    {
                                        result.action_count = 1.0f;
                                    }
                                }
                                else if(was_pressed(&input->KeyboardKey_R))
                                {
                                    if(inspect_item->type == ItemType_Scroll)
                                    {
                                        set_as_known_and_identify_existing(inspect_item->id, items, item_info);
                                        inventory->view_update_item_type = inspect_item->type;
                                        result.action_count = 1.0f;
                                        
                                        switch(inspect_item->id)
                                        {
                                            case ItemID_IdentifyScroll:
                                            {
                                                log_add(ui, "You read the scroll, choose an item to identify.");
                                                inventory->using_item_type = UsingItemType_Identify;
                                            } break;
                                            
                                            case ItemID_EnchantWeaponScroll:
                                            {
                                                log_add(ui, "You read the scroll, choose a weapon to enchant.");
                                                inventory->using_item_type = UsingItemType_EnchantWeapon;
                                            } break;
                                            
                                            case ItemID_EnchantArmorScroll:
                                            {
                                                log_add(ui, "You read the scroll, choose an armor to enchant.");
                                                inventory->using_item_type = UsingItemType_EnchantArmor;
                                            } break;
                                            
                                            case ItemID_MagicMappingScroll:
                                            {
                                                log_add(ui, "You read the scroll, your surroundings become clear to you.");
                                                
                                                for(u32 y = 0; y < dungeon->height; ++y)
                                                {
                                                    for(u32 x = 0; x < dungeon->width; ++x)
                                                    {
                                                        set_tile_has_been_seen(dungeon->tiles, make_v2u(x, y), true);
                                                    }
                                                }
                                                
                                                remove_item_from_inventory_and_game(&game->random, items, item_info, inspect_item, inventory);
                                            } break;
                                            
                                            case ItemID_TeleportationScroll:
                                            {
                                                log_add(ui, "You read the scroll, you find yourself in a different place.");
                                                
                                                for(;;)
                                                {
                                                    v2u pos = random_dungeon_pos(&game->random, dungeon);
                                                    if(is_tile_traversable_and_not_occupied(dungeon->tiles, pos))
                                                    {
                                                        move_entity(player, dungeon->tiles, pos);
                                                        break;
                                                    }
                                                }
                                                
                                                update_fov(dungeon, player);
                                                remove_item_from_inventory_and_game(&game->random, items, item_info, inspect_item, inventory);
                                            } break;
                                            
                                            case ItemID_UncurseScroll:
                                            {
                                                log_add(ui, "You read the scroll, choose an item to uncurse.");
                                                inventory->using_item_type = UsingItemType_Uncurse;
                                            } break;
                                            
                                            invalid_default_case;
                                        }
                                        
                                        unset(inventory->flags, InventoryFlags_Inspecting);
                                    }
                                }
                                else if(was_pressed(&input->KeyboardKey_C))
                                {
                                    if(inspect_item->type == ItemType_Potion ||
                                       inspect_item->type == ItemType_Ration)
                                    {
                                        if(inspect_item->type == ItemType_Potion)
                                        {
                                            set_as_known_and_identify_existing(inspect_item->id, items, item_info);
                                            
                                            switch(inspect_item->id)
                                            {
                                                case ItemID_MightPotion:
                                                {
                                                    log_add(ui, "You drink the %s, you feel more mighty.", inspect_item->name);
                                                    start_entity_status_effect(player, inspect_item->c.status_effect);
                                                } break;
                                                
                                                case ItemID_WisdomPotion:
                                                {
                                                    log_add(ui, "You drink the %s, you feel more wise.", inspect_item->name);
                                                    start_entity_status_effect(player, inspect_item->c.status_effect);
                                                } break;
                                                
                                                case ItemID_AgilityPotion:
                                                {
                                                    log_add(ui, "You drink the %s, you feel more dexterous.", inspect_item->name);
                                                    start_entity_status_effect(player, inspect_item->c.status_effect);
                                                } break;
                                                
                                                case ItemID_ElusionPotion:
                                                {
                                                    log_add(ui, "You drink the %s, you feel more evasive.", inspect_item->name);
                                                    start_entity_status_effect(player, inspect_item->c.status_effect);
                                                } break;
                                                
                                                case ItemID_HealingPotion:
                                                {
                                                    if(player->hp == player->max_hp)
                                                    {
                                                        log_add(ui, "You drink the %s, you feel no different.", inspect_item->name);
                                                    }
                                                    else
                                                    {
                                                        log_add(ui, "You drink the %s, it heals you for %u health.", start_color(Color_LightGreen), inspect_item->name, inspect_item->c.heal_value);
                                                        heal_entity(player, inspect_item->c.heal_value);
                                                    }
                                                } break;
                                                
                                                case ItemID_DecayPotion:
                                                {
                                                    log_add(ui, "You drink the %s, you feel much weaker.", inspect_item->name);
                                                    start_entity_status_effect(player, inspect_item->c.status_effect);
                                                } break;
                                                
                                                case ItemID_ConfusionPotion:
                                                {
                                                    log_add(ui, "You drink the %s, you feel confused.", inspect_item->name);
                                                    start_entity_status_effect(player, inspect_item->c.status_effect);
                                                } break;
                                                
                                                invalid_default_case;
                                            }
                                        }
                                        else if(inspect_item->type == ItemType_Ration)
                                        {
                                            if(player->hp == player->max_hp)
                                            {
                                                log_add(ui, "%sYou eat the ration.", start_color(Color_LightGray));
                                            }
                                            else
                                            {
                                                log_add(ui, "%sYou eat the ration, it heals you for %u health.", start_color(Color_LightGreen), inspect_item->c.heal_value);
                                                heal_entity(player, inspect_item->c.heal_value);
                                            }
                                        }
                                        
                                        inventory->view_update_item_type = inspect_item->type;
                                        remove_item_from_inventory_and_game(&game->random, items, item_info, inspect_item, inventory);
                                        result.action_count = 1.0f;
                                    }
                                }
                                else if(was_pressed(&input->KeyboardKey_D))
                                {
                                    if(is_set(inspect_item->flags, ItemFlags_Equipped | ItemFlags_Cursed))
                                    {
                                        log_add_cursed_unequip(ui, inspect_item);
                                    }
                                    else
                                    {
                                        log_add_item_action_text(ui, inspect_item, ItemActionType_Drop);
                                        unset(player->flags, EntityFlags_LogMultipleItemMessage);
                                        
                                        inventory->view_update_item_type = inspect_item->type;
                                        unset(inventory->flags, InventoryFlags_Inspecting);
                                        
                                        if(is_set(inspect_item->flags, ItemFlags_Equipped))
                                        {
                                            result.action_count = 2.0f;
                                        }
                                        else
                                        {
                                            result.action_count = 1.0f;
                                        }
                                        
                                        remove_item_from_inventory(&game->random, items, item_info, inspect_item, inventory, player->pos);
                                    }
                                }
                                else if(was_pressed(&input->KeyboardKey_M))
                                {
                                    set(inventory->flags, InventoryFlags_Marking);
                                    set_view_at_start(&ui->mark_view);
                                }
                            }
                        }
                    }
                    else
                    {
                        char pressed_letter = get_pressed_alphabet_letter(input->keyboard);
                        if(pressed_letter)
                        {
                            if(inventory->using_item_type)
                            {
                                if(!is_set(inventory->flags, InventoryFlags_ReadyForKeypress))
                                {
                                    set(inventory->flags, InventoryFlags_ReadyForKeypress);
                                }
                                else
                                {
                                    for(u32 index = 0; index < MAX_INVENTORY_SLOT_COUNT; ++index)
                                    {
                                        Item *item = inventory->slots[index];
                                        if(item &&
                                           item->inventory_letter == pressed_letter &&
                                           item_fits_using_item_type(inventory->using_item_type, item))
                                        {
                                            assert(inventory->using_item_type);
                                            
                                            if(inventory->using_item_type == UsingItemType_Identify)
                                            {
                                                set(item->flags, ItemFlags_Identified);
                                                
                                                String128 item_name = get_full_item_name(item);
                                                log_add(ui, "You identify the %s.", item_name.str);
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
                                            else if(inventory->using_item_type == UsingItemType_Uncurse)
                                            {
                                                unset(item->flags, ItemFlags_Cursed);
                                                log_add(ui, "The %s seems slightly different now..", item_id_text(item->id));
                                            }
                                            
                                            remove_item_from_inventory_and_game(&game->random, items, item_info, inspect_item, inventory);
                                            inventory->using_item_type = UsingItemType_None;
                                            set_view_at_start(&inventory->view);
                                        }
                                    }
                                }
                            }
                            else
                            {
                                if(pressed_letter == game->keybinds[GameKey_OpenInventory] ||
                                   (is_set(inventory->flags, InventoryFlags_Open) &&
                                    !is_set(inventory->flags, InventoryFlags_ReadyForKeypress)))
                                {
                                    set(inventory->flags, InventoryFlags_ReadyForKeypress);
                                }
                                else
                                {
                                    for(u32 index = 0; index < MAX_INVENTORY_SLOT_COUNT; ++index)
                                    {
                                        Item *item = inventory->slots[index];
                                        if(item &&
                                           item->inventory_letter == pressed_letter)
                                        {
                                            set(inventory->flags, InventoryFlags_Inspecting);
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
    
    for(u32 entity_index = 0; entity_index < EntityID_Count; ++entity_index)
    {
        Entity *entity = &entities[entity_index];
        if(entity->type == EntityType_Player)
        {
            // Update player
            
            player->defence = 0;
            player->p.weight = 0;
            
            for(u32 slot_index = ItemSlot_None + 1; slot_index < ItemSlot_Count; ++slot_index)
            {
                for(u32 inventory_index = 0; inventory_index < MAX_INVENTORY_SLOT_COUNT; ++inventory_index)
                {
                    Item *item = inventory->slots[inventory_index];
                    if(item &&
                       is_set(item->flags, ItemFlags_Equipped) &&
                       (item->slot == slot_index) &&
                       item->type == ItemType_Armor)
                    {
                        player->defence += (item->a.defence + item->enchantment_level);
                        player->p.weight += item->a.weight;
                    }
                }
            }
            
            player->evasion = 10 - (player->p.weight / player->p.weight_to_evasion_ratio);
            if(player->statuses[StatusEffectType_Elusion].is_enabled)
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
                        move_entity(player, dungeon->tiles, player->new_pos);
                        update_fov(dungeon, player);
                    }
                }
                else
#endif
                
                player->new_direction = get_direction_moved_from(player->pos, player->new_pos);
                
                if(!equal_v2u(player->pos, player->new_pos) &&
                   is_tile_occupied(dungeon->tiles, player->new_pos))
                {
                    for(u32 target_index = 0; target_index < MAX_ENTITY_COUNT; ++target_index)
                    {
                        Entity *target = &entities[target_index];
                        if(target->type == EntityType_Enemy &&
                           equal_v2u(player->new_pos, target->pos))
                        {
                            u32 player_damage = 2;
                            u32 player_accuracy = 1;
                            f32 player_attack_speed = 1.0f;
                            
                            Item *item = get_equipped_item_from_slot(ItemSlot_FirstHand, inventory);
                            if(item)
                            {
                                player_damage = item->w.damage + item->enchantment_level;
                                player_accuracy = item->w.accuracy + item->enchantment_level;
                                player_attack_speed = item->w.speed;
                            }
                            
#if 0
                            printf("player_damage: %u\n", player_damage);
                            printf("player_accuracy: %u\n", player_accuracy);
                            printf("player_attack_speed: %.01f\n", player_attack_speed);
#endif
                            
                            player->hit_chance = 15 + (player->dexterity / 2);
                            player->hit_chance += player_accuracy;
                            
                            // Apply strength bonus to damage.
                            u32 modified_player_damage = player_damage;
                            if(player->strength < 10)
                            {
                                modified_player_damage -= (10 - player->strength);
                            }
                            else
                            {
                                modified_player_damage += (player->strength - 10);
                            }
                            
                            //printf("modified_player_damage: %u\n", modified_player_damage);
                            
                            set(target->flags, EntityFlags_Combat);
                            attack_entity(&game->random, dungeon, inventory, ui, player, target, modified_player_damage);
                            
                            input_result.action_count = player_attack_speed;
                        }
                    }
                }
                else
                {
                    if(is_entity_under_status_effect(player, StatusEffectType_Confusion))
                    {
                        get_confused_move_pos(&game->random, dungeon, ui, player);
                    }
                    
                    if(is_tile_id(dungeon->tiles, player->new_pos, TileID_StoneDoorClosed))
                    {
                        set_tile_id(dungeon->tiles, player->new_pos, TileID_StoneDoorOpen);
                        input_result.action_count = 1.0f;
                    }
                    else if(is_tile_traversable(dungeon->tiles, player->new_pos))
                    {
                        move_entity(player, dungeon->tiles, player->new_pos);
                        input_result.action_count = 1.0f;
                    }
                }
            }
            
            if(input_result.action_count)
            {
                game->time += input_result.action_count;
                player->action_count = input_result.action_count;
                
                update_fov(dungeon, player);
                update_pathfind_map(dungeon, &dungeon->pathfind, player->pos);
                
                for(u32 status_index = 0; status_index < input_result.action_count; ++status_index)
                {
                    update_entity_status_effects(game, dungeon, ui, player);
                }
                
                // Inform the player if there are multiple items on your position.
                if(is_set(player->flags, EntityFlags_LogMultipleItemMessage))
                {
                    u32 player_pos_item_count = 0;
                    for(u32 item_index = 0; item_index < MAX_ITEM_COUNT; ++item_index)
                    {
                        Item *item = &items[item_index];
                        if(is_item_valid_and_not_in_inventory(item) &&
                           equal_v2u(item->pos, player->pos))
                        {
                            ++player_pos_item_count;
                        }
                    }
                    
                    if(player_pos_item_count > 1)
                    {
                        log_add(ui, "There are multiple items here.");
                    }
                    
                    //printf("player_pos_item_count: %u\n", player_pos_item_count);
                }
            }
        }
        else if(entity->type == EntityType_Enemy)
        {
            if(input_result.action_count)
            {
                Entity *enemy = &entities[entity_index];
                if(enemy->id)
                {
                    
#if MOONBREATH_SLOW
                    if(enemy->id == EntityID_Dummy) continue;
#endif
                    
                    enemy->e.action_count_timer += enemy->action_count;
                    u32 enemy_action_count = enemy->e.action_count_timer / input_result.action_count;
                    
#if 0
                    printf("input_result.action_count: %.1f\n", input_result.action_count);
                    printf("action_count_timer: %.1f\n", enemy->e.action_count_timer);
                    printf("enemy_action_count: %u\n\n", enemy_action_count);
#endif
                    
                    while(enemy_action_count--)
                    {
                        enemy->e.action_count_timer = 0.0f;
                        
#if MOONBREATH_SLOW
                        if(is_tile_seen(dungeon->tiles, enemy->pos) && !fkey_active[1])
#else
                        if(is_tile_seen(dungeon->tiles, enemy->pos))
#endif
                        {
                            set(enemy->flags, EntityFlags_Combat);
                            unset(enemy->flags, EntityFlags_Pathfinding);
                            
                            if(player->pos.x < enemy->pos.x)
                            {
                                enemy->new_direction = Direction_Left;
                            }
                            else
                            {
                                enemy->new_direction = Direction_Right;
                            }
                            
                            ++enemy->e.turns_in_player_view;
                            if(is_enemy_alerted(enemy->e.turns_in_player_view))
                            {
                                break;
                            }
                            
                            v2u pathfind_pos = next_pathfind_pos(&dungeon->pathfind, dungeon->tiles, enemy->pos, player->pos);
                            enemy->hit_chance = 30;
                            assert(player->evasion < enemy->hit_chance);
                            
                            v4u enemy_fov_rect = get_dimension_rect(dungeon, player->pos, enemy->fov);
                            
                            if(is_set(enemy->flags, EntityFlags_MagicAttacks))
                            {
                                u32 break_value = 100;
                                u32 counter = 0;
                                
                                for(;;)
                                {
                                    u32 index = random_number(&game->random, 0, enemy->e.spell_count);
                                    counter += enemy->e.spells[index].effect.chance;
                                    
                                    if(counter >= break_value)
                                    {
                                        enemy->e.spell_index = index;
                                        break;
                                    }
                                }
                                
                                // TODO(rami): Dedup needed below at some point.
                                Spell *spell = &enemy->e.spells[enemy->e.spell_index];
                                if(spell->type == SpellType_Offensive)
                                {
                                    if(is_inside_rect_and_in_spell_range(enemy_fov_rect, spell->range,
                                                                         enemy->pos, player->pos))
                                    {
                                        attack_entity(&game->random, dungeon, inventory, ui, enemy, player, spell->effect.value);
                                    }
                                }
                                else if(spell->type == SpellType_Healing)
                                {
                                    for(u32 target_index = 0; target_index < MAX_ENTITY_COUNT; ++target_index)
                                    {
                                        Entity *target = &entities[target_index];
                                        if(target_index != entity_index &&
                                           is_entity_valid_and_not_player(target->type))
                                        {
                                            if(is_inside_rect_and_in_spell_range(enemy_fov_rect, spell->range, enemy->pos, target->pos) &&
                                               is_set(target->flags, EntityFlags_Combat) &&
                                               target->hp < target->max_hp)
                                            {
                                                log_add(ui, "The %s casts %s at the %s, healing it for %u health.", enemy->name, get_spell_name(spell->id), target->name, spell->effect.value);
                                                heal_entity(target, spell->effect.value);
                                                
                                                break;
                                            }
                                        }
                                    }
                                }
                                else if(spell->type == SpellType_Buff)
                                {
                                    for(u32 target_index = 0; target_index < MAX_ENTITY_COUNT; ++target_index)
                                    {
                                        Entity *target = &entities[target_index];
                                        if(target_index != entity_index &&
                                           is_entity_valid_and_not_player(target->type))
                                        {
                                            if(is_inside_rect_and_in_spell_range(enemy_fov_rect, spell->range, enemy->pos, target->pos) &&
                                               is_set(target->flags, EntityFlags_Combat) &&
                                               !is_entity_under_status_effect(target, spell->effect.type))
                                            {
                                                log_add(ui, "The %s casts %s on the %s.", enemy->name, get_spell_name(spell->id), target->name);
                                                start_entity_status_effect(target, spell->effect);
                                                
                                                break;
                                            }
                                        }
                                    }
                                }
                            }
                            else if(is_inside_rect(enemy_fov_rect, player->pos) &&
                                    (is_set(enemy->flags, EntityFlags_RangedAttacks) ||
                                     equal_v2u(pathfind_pos, player->pos)))
                            {
                                attack_entity(&game->random, dungeon, inventory, ui, enemy, player, enemy->e.damage);
                            }
                            else
                            {
                                enemy->new_pos = pathfind_pos;
                            }
                        }
                        else
                        {
                            if(is_set(entity->flags, EntityFlags_Combat) &&
                               !is_enemy_alerted(enemy->e.turns_in_player_view))
                            {
                                if(!is_set(enemy->flags, EntityFlags_Pathfinding))
                                {
                                    start_entity_pathfind(enemy, dungeon, items, player->pos);
                                    //printf("Enemy Pathfind: Target %u, %u\n", enemy->pathfind_target.x, enemy->pathfind_target.y);
                                }
                                
                                enemy->new_pos = next_pathfind_pos(&enemy->pathfind, dungeon->tiles, enemy->pos, enemy->pathfind_target);
                                //printf("Enemy Pathfind: New Pos %u, %u\n", enemy->new_pos.x, enemy->new_pos.y);
                                
                                if(equal_v2u(enemy->new_pos, enemy->pathfind_target))
                                {
                                    //printf("Enemy Pathfind: Target Reached\n");
                                    
                                    unset(enemy->flags, EntityFlags_Combat | EntityFlags_Pathfinding);
                                    enemy->e.turns_in_player_view = 0;
                                }
                            }
                            else
                            {
                                unset(enemy->flags, EntityFlags_Combat);
                                enemy->e.turns_in_player_view = 0;
                                
                                enemy->new_direction = get_random_direction(&game->random);
                                enemy->new_pos = get_direction_pos(enemy->pos, enemy->new_direction);
                                
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
                        
                        // Before calling move_entity(), we set pos_save_for_ghost
                        // based on if the new enemy pos is seen. Enemy rendering
                        // code will then use pos_save_for_ghost.
                        if(is_tile_seen(dungeon->tiles, enemy->new_pos))
                        {
                            enemy->e.saved_pos_for_ghost = enemy->new_pos;
                        }
                        else
                        {
                            enemy->e.saved_flipped_for_ghost = is_set(enemy->flags, EntityFlags_Flipped);
                            enemy->e.saved_pos_for_ghost = enemy->pos;
                        }
                        
                        if(is_entity_under_status_effect(enemy, StatusEffectType_Confusion))
                        {
                            get_confused_move_pos(&game->random, dungeon, ui, enemy);
                        }
                        
                        if(is_tile_traversable_and_not_occupied(dungeon->tiles, enemy->new_pos))
                        {
                            move_entity(enemy, dungeon->tiles, enemy->new_pos);
                        }
                        
                        update_entity_status_effects(game, dungeon, ui, enemy);
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
            
            if(!is_set(enemy->flags, EntityFlags_Invisible))
            {
                if(is_tile_seen(dungeon->tiles, enemy->pos))
                {
                    set(enemy->flags, EntityFlags_HasBeenSeen);
                    unset(enemy->flags, EntityFlags_GhostEnabled);
                    
                    v4u src = get_tile_rect(enemy->tile_pos);
                    v4u dest = get_game_dest(game, enemy->pos);
                    SDL_RenderCopyEx(game->renderer, assets->tileset.tex, (SDL_Rect *)&src, (SDL_Rect *)&dest, 0, 0, is_set(enemy->flags, EntityFlags_Flipped));
                    
                    // Additional things to render on enemy tile.
                    if(enemy->e.turns_in_player_view == 1)
                    {
                        v4u status_src = get_tile_rect(make_v2u(8, 15));
                        SDL_RenderCopy(game->renderer, assets->tileset.tex, (SDL_Rect *)&status_src, (SDL_Rect *)&dest);
                    }
                    else if(is_entity_under_any_status_effect(enemy))
                    {
                        v4u status_src = get_tile_rect(make_v2u(7, 15));
                        SDL_RenderCopy(game->renderer, assets->tileset.tex, (SDL_Rect *)&status_src, (SDL_Rect *)&dest);
                    }
                    
                    if(is_set(enemy->flags, EntityFlags_Combat))
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
                    if(is_set(enemy->flags, EntityFlags_HasBeenSeen))
                    {
                        if(is_set(enemy->flags, EntityFlags_GhostEnabled))
                        {
                            if(is_tile_seen(dungeon->tiles, enemy->e.ghost_pos))
                            {
                                unset(enemy->flags, EntityFlags_HasBeenSeen | EntityFlags_GhostEnabled);
                            }
                            else
                            {
                                v4u src = get_tile_rect(enemy->tile_pos);
                                v4u dest = get_game_dest(game, enemy->e.ghost_pos);
                                render_texture_half_color(game->renderer, assets->tileset.tex, src, dest, is_set(enemy->flags, EntityFlags_GhostFlipped));
                            }
                        }
                        else
                        {
                            if(is_tile_seen(dungeon->tiles, enemy->e.saved_pos_for_ghost))
                            {
                                // Old enemy pos is seen, so enemy ghost is placed on
                                // new enemy pos, this means the enemy moved.
                                enemy->e.ghost_pos = enemy->new_pos;
                            }
                            else
                            {
                                // Old enemy pos is not seen, so enemy ghost is placed on
                                // the old enemy pos, this means the player moved.
                                enemy->e.ghost_pos = enemy->e.saved_pos_for_ghost;
                            }
                            
                            set(enemy->flags, EntityFlags_GhostEnabled);
                            
                            if(enemy->e.saved_flipped_for_ghost)
                            {
                                set(enemy->flags, EntityFlags_GhostFlipped);
                            }
                            else
                            {
                                unset(enemy->flags, EntityFlags_GhostFlipped);
                            }
                        }
                    }
                }
            }
        }
    }
}

internal void
add_player_entity(Random *random, Entity *player)
{
    player->id = EntityID_Player;
    strcpy(player->name, "Name");
    
    player->max_hp = 80;
    player->hp = player->max_hp;
    
    //player->hp = 1;
    //player->hp = 1000000;
    
    player->w = player->h = 32;
    player->tile_pos = get_entity_tile_pos(player->id);
    player->remains = EntityRemains_RedBlood;
    player->type = EntityType_Player;
    
    player->strength = 10;
    player->intelligence = 10;
    player->dexterity = 10;
    
    player->evasion = 10;
    player->fov = 8;
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
            enemy->fov = 8;
            enemy->e.level = entity_levels[id];
            
            set_tile_occupied(tiles, enemy->pos, true);
            
            switch(id)
            {
                case EntityID_Dummy:
                {
                    // This is a dummy entity for testing purposes.
                    strcpy(enemy->name, "Dummy");
                    enemy->max_hp = enemy->hp = U32_MAX;
                    
                    enemy->evasion = 10;
                    enemy->action_count = 1.0f;
                } break;
                
                case EntityID_SkeletonWarrior:
                {
                    strcpy(enemy->name, "Skeleton Warrior");
                    enemy->max_hp = enemy->hp = 18;
                    
                    enemy->e.damage = 5;
                    enemy->evasion = 5;
                    enemy->action_count = 1.0f;
                } break;
                
                case EntityID_SkeletonArcher:
                {
                    strcpy(enemy->name, "Skeleton Archer");
                    enemy->max_hp = enemy->hp = 15;
                    
                    enemy->e.damage = 4;
                    enemy->evasion = 6;
                    enemy->action_count = 1.0f;
                    
                    set(enemy->flags, EntityFlags_RangedAttacks);
                } break;
                
                case EntityID_SkeletonMage:
                {
                    strcpy(enemy->name, "Skeleton Mage");
                    enemy->max_hp = enemy->hp = 15;
                    
                    enemy->evasion = 4;
                    enemy->action_count = 0.5f;
                    
                    set(enemy->flags, EntityFlags_MagicAttacks);
                    add_enemy_spell(enemy, SpellID_DarkBolt);
                } break;
                
                case EntityID_Bat:
                {
                    strcpy(enemy->name, "Bat");
                    enemy->max_hp = enemy->hp = 14;
                    
                    enemy->e.damage = 1;
                    enemy->evasion = 14;
                    enemy->action_count = 3.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_Rat:
                {
                    strcpy(enemy->name, "Rat");
                    enemy->max_hp = enemy->hp = 11;
                    
                    enemy->e.damage = 1;
                    enemy->evasion = 12;
                    enemy->action_count = 2.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_KoboldWarrior:
                {
                    strcpy(enemy->name, "Kobold Warrior");
                    enemy->max_hp = enemy->hp = 22;
                    
                    enemy->e.damage = 5;
                    enemy->evasion = 8;
                    enemy->action_count = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_KoboldShaman:
                {
                    strcpy(enemy->name, "Kobold Shaman");
                    enemy->max_hp = enemy->hp = 18;
                    
                    enemy->evasion = 7;
                    enemy->action_count = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                    
                    set(enemy->flags, EntityFlags_MagicAttacks);
                    
                    add_enemy_spell(enemy, SpellID_Bolster);
                    add_enemy_spell(enemy, SpellID_LesserHeal);
                } break;
                
                case EntityID_Snail:
                {
                    strcpy(enemy->name, "Snail");
                    enemy->max_hp = enemy->hp = 28;
                    
                    enemy->e.damage = 10;
                    enemy->evasion = 3;
                    enemy->action_count = 0.5f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_Slime:
                {
                    strcpy(enemy->name, "Slime");
                    enemy->max_hp = enemy->hp = 20;
                    
                    enemy->e.damage = 4;
                    enemy->evasion = 6;
                    enemy->action_count = 1.0f;
                    enemy->remains = EntityRemains_GreenBlood;
                } break;
                
                case EntityID_Dog:
                {
                    strcpy(enemy->name, "Dog");
                    enemy->max_hp = enemy->hp = 18;
                    
                    enemy->e.damage = 3;
                    enemy->evasion = 10;
                    enemy->action_count = 2.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_OrcWarrior:
                {
                    strcpy(enemy->name, "Orc Warrior");
                    enemy->max_hp = enemy->hp = 28;
                    
                    enemy->e.damage = 8;
                    enemy->evasion = 10;
                    enemy->action_count = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_OrcArcher:
                {
                    strcpy(enemy->name, "Orc Archer");
                    enemy->max_hp = enemy->hp = 26;
                    
                    enemy->e.damage = 6;
                    enemy->evasion = 8;
                    enemy->action_count = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                    
                    set(enemy->flags, EntityFlags_RangedAttacks);
                } break;
                
                case EntityID_OrcShaman:
                {
                    strcpy(enemy->name, "Orc Shaman");
                    enemy->max_hp = enemy->hp = 26;
                    
                    enemy->evasion = 8;
                    enemy->action_count = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                    
                    set(enemy->flags, EntityFlags_MagicAttacks);
                    
                    // TODO(rami): Lesser Heal
                    // TODO(rami): Bolster / Reinforce (maybe not for this enemy)
                } break;
                
                case EntityID_Python:
                {
                    strcpy(enemy->name, "Python");
                    enemy->max_hp = enemy->hp = 15;
                    
                    enemy->e.damage = 4;
                    enemy->evasion = 11;
                    enemy->action_count = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                    
                    enemy->e.poison.is_enabled = true;
                    enemy->e.poison.type = StatusEffectType_Poison;
                    enemy->e.poison.value = 2;
                    enemy->e.poison.chance = 33;
                    enemy->e.poison.duration = 8;
                } break;
                
                case EntityID_Shade:
                {
                    strcpy(enemy->name, "Shade");
                    enemy->max_hp = enemy->hp = 18;
                    
                    enemy->e.damage = 3;
                    enemy->evasion = 16;
                    enemy->action_count = 1.0f;
                    
                    set(enemy->flags, EntityFlags_MagicAttacks);
                } break;
                
                case EntityID_ElfKnight:
                {
                    strcpy(enemy->name, "Elf Knight");
                    enemy->max_hp = enemy->hp = 32;
                    
                    enemy->e.damage = 12;
                    enemy->defence = 3;
                    enemy->evasion = 2;
                    enemy->action_count = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_ElfArbalest:
                {
                    strcpy(enemy->name, "Elf Arbalest");
                    enemy->max_hp = enemy->hp = 28;
                    
                    enemy->e.damage = 14;
                    enemy->evasion = 4;
                    enemy->action_count = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                    
                    set(enemy->flags, EntityFlags_RangedAttacks);
                } break;
                
                case EntityID_ElfMage:
                {
                    strcpy(enemy->name, "Elf Mage");
                    enemy->max_hp = enemy->hp = 28;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_count = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                    
                    set(enemy->flags, EntityFlags_MagicAttacks);
                    
                    // TODO(rami): Blood mage? blood siphon?
                    // TODO(rami): Weaken
                } break;
                
                case EntityID_GiantSlime:
                {
                    strcpy(enemy->name, "Giant Slime");
                    enemy->max_hp = enemy->hp = 40;
                    
                    enemy->e.damage = 6;
                    enemy->evasion = 1;
                    enemy->action_count = 1.0f;
                    enemy->remains = EntityRemains_GreenBlood;
                    
                    // TODO(rami): Leaves smaller versions of itself?
                    // TODO(rami): Leaves pools of acid?
                } break;
                
                case EntityID_Spectre:
                {
                    strcpy(enemy->name, "Spectre");
                    enemy->max_hp = enemy->hp = 25;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_count = 1.0f;
                    
                    set(enemy->flags, EntityFlags_MagicAttacks);
                    
                    // TODO(rami): Slow and invisible?
                    // TODO(rami): Slow and blink?
                } break;
                
                case EntityID_OrcAssassin:
                {
                    strcpy(enemy->name, "Orc Assassin");
                    enemy->max_hp = enemy->hp = 28;
                    
                    enemy->e.damage = 7;
                    enemy->evasion = 10;
                    enemy->action_count = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_OrcSorcerer:
                {
                    strcpy(enemy->name, "Orc Sorcerer");
                    enemy->max_hp = enemy->hp = 26;
                    
                    enemy->evasion = 5;
                    enemy->action_count = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                    
                    set(enemy->flags, EntityFlags_MagicAttacks);
                    
                    // TODO(rami): Focused on damage
                } break;
                
                case EntityID_Minotaur:
                {
                    strcpy(enemy->name, "Minotaur");
                    enemy->max_hp = enemy->hp = 34;
                    
                    enemy->e.damage = 14;
                    enemy->evasion = 4;
                    enemy->action_count = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_Treant:
                {
                    strcpy(enemy->name, "Treant");
                    enemy->max_hp = enemy->hp = 40;
                    
                    enemy->e.damage = 4;
                    enemy->defence = 6;
                    enemy->evasion = 0;
                    enemy->action_count = 0.5f;
                } break;
                
                case EntityID_Viper:
                {
                    strcpy(enemy->name, "Viper");
                    enemy->max_hp = enemy->hp = 18;
                    
                    enemy->e.damage = 4;
                    enemy->evasion = 5;
                    enemy->action_count = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                    
                    enemy->e.poison.is_enabled = true;
                    enemy->e.poison.type = StatusEffectType_Poison;
                    enemy->e.poison.value = 3;
                    enemy->e.poison.chance = 33;
                    enemy->e.poison.duration = 8;
                } break;
                
                case EntityID_CentaurWarrior:
                {
                    strcpy(enemy->name, "Centaur Warrior");
                    enemy->max_hp = enemy->hp = 35;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 4;
                    enemy->action_count = 2.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_CentaurSpearman:
                {
                    strcpy(enemy->name, "Centaur Spearman");
                    enemy->max_hp = enemy->hp = 35;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 4;
                    enemy->action_count = 2.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_CentaurArcher:
                {
                    strcpy(enemy->name, "Centaur Archer");
                    enemy->max_hp = enemy->hp = 32;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 3;
                    enemy->action_count = 2.0f;
                    enemy->remains = EntityRemains_RedBlood;
                    
                    set(enemy->flags, EntityFlags_RangedAttacks);
                } break;
                
                case EntityID_CursedSkull:
                {
                    strcpy(enemy->name, "Cursed Skull");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_count = 1.0f;
                    
                    set(enemy->flags, EntityFlags_MagicAttacks);
                    
                    // TODO(rami): Spell
                } break;
                
                case EntityID_Wolf:
                {
                    strcpy(enemy->name, "Wolf");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_count = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_OgreWarrior:
                {
                    strcpy(enemy->name, "Ogre Warrior");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_count = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_OgreArcher:
                {
                    strcpy(enemy->name, "Ogre Archer");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_count = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                    
                    set(enemy->flags, EntityFlags_RangedAttacks);
                    
                    // TODO(rami): Switch from ranger to something special?
                } break;
                
                case EntityID_OgreMage:
                {
                    strcpy(enemy->name, "Ogre Mage");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_count = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                    
                    set(enemy->flags, EntityFlags_MagicAttacks);
                } break;
                
                case EntityID_Cyclops:
                {
                    strcpy(enemy->name, "Cyclops");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_count = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_ShadowWalker:
                {
                    strcpy(enemy->name, "Shadow Walker");
                    enemy->max_hp = enemy->hp = 10;
                    
                    enemy->e.damage = 5;
                    enemy->evasion = 0;
                    enemy->action_count = 1.0f;
                    
                    set(enemy->flags, EntityFlags_Invisible);
                } break;
                
                case EntityID_DwarwenWarrior:
                {
                    strcpy(enemy->name, "Dwarwen Warrior");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_count = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_DwarwenSorcerer:
                {
                    strcpy(enemy->name, "Dwarwen Sorcerer");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_count = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                    
                    set(enemy->flags, EntityFlags_MagicAttacks);
                } break;
                
                case EntityID_DwarwenPriest:
                {
                    strcpy(enemy->name, "Dwarwen Priest");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_count = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                    
                    set(enemy->flags, EntityFlags_MagicAttacks);
                } break;
                
                case EntityID_ScarletSnake:
                {
                    strcpy(enemy->name, "Scarlet Snake");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_count = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_Lich:
                {
                    strcpy(enemy->name, "Lich");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_count = 1.0f;
                    
                    set(enemy->flags, EntityFlags_MagicAttacks);
                } break;
                
                case EntityID_AbyssalFiend:
                {
                    strcpy(enemy->name, "Abyssal Fiend");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_count = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                    
                    set(enemy->flags, EntityFlags_RangedAttacks);
                } break;
                
                case EntityID_BloodTroll:
                {
                    strcpy(enemy->name, "Blood Troll");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_count = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_IronGolem:
                {
                    strcpy(enemy->name, "Iron Golem");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_count = 1.0f;
                } break;
                
                case EntityID_Griffin:
                {
                    strcpy(enemy->name, "Griffin");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_count = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_Imp:
                {
                    strcpy(enemy->name, "Imp");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_count = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_BlackKnight:
                {
                    strcpy(enemy->name, "Black Knight");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_count = 1.0f;
                } break;
                
                case EntityID_GiantDemon:
                {
                    strcpy(enemy->name, "Giant Demon");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_count = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_Hellhound:
                {
                    strcpy(enemy->name, "Hellhound");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_count = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_AbyssalHexmaster:
                {
                    strcpy(enemy->name, "Abyssal Hexmaster");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_count = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                    
                    set(enemy->flags, EntityFlags_MagicAttacks);
                } break;
                
                case EntityID_Mahjarrat:
                {
                    strcpy(enemy->name, "Mahjarrat");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_count = 1.0f;
                    
                    set(enemy->flags, EntityFlags_MagicAttacks);
                } break;
                
                invalid_default_case;
            }
            
            return;
        }
    }
    
    assert(false);
}