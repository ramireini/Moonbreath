internal void
force_move_entity(Entity *entity, Tiles tiles, v2u new_pos)
{
    set_tile_occupied(tiles, entity->pos, false);
    entity->pos = entity->new_pos = new_pos;
    set_tile_occupied(tiles, entity->new_pos, true);
}

internal b32
player_view_has_enemies(EntityState *entities, Dungeon *dungeon)
{
    b32 result = false;
    
    for(u32 index = 0; index < MAX_ENTITY_COUNT; ++index)
    {
        Entity *entity = &entities->array[index];
        if(entity->type == EntityType_Enemy &&
           is_tile_seen(dungeon->tiles, entity->pos))
        {
            result = true;
            break;
        }
    }
    
    return(result);
}

internal void
teleport_entity(Random *random, Entity *entity, Dungeon *dungeon, UI *ui)
{
    for(;;)
    {
        v2u pos = get_random_dungeon_pos(random, dungeon);
        if(is_tile_traversable_and_not_occupied(dungeon->tiles, pos))
        {
            force_move_entity(entity, dungeon->tiles, pos);
            break;
        }
    }
    
    if(entity->type == EntityType_Player)
    {
    update_fov(entity, dungeon);
    }
    }

internal u32
apply_entity_resistance_to_damage(Entity *defender, UI *ui, u32 damage, DamageType damage_type)
{
    u32 result = damage;
    
    s32 resistance = defender->resistances[damage_type];
    if(resistance != 0)
    {
        assert(resistance >= -5);
        assert(resistance <= 5);
        
        f32 resistance_percentage = (f32)resistance * 0.20f;
        u32 damage_change = (u32)((f32)damage * resistance_percentage);
        damage -= damage_change;
        
        #if 1
        printf("\nDamage: %u (%s)\n", result, get_damage_type_text(damage_type));
        printf("Defender Resistance: %dp (%.01f%%)\n", resistance, resistance_percentage * 100.0f);
        printf("Damage After: %d (%d old; %d change)\n\n", result - damage_change, result, damage_change);
        #endif
        
        result = damage;
        
        if(resistance == 5)
        {
            log_add(ui, "%sYou fully resist the attack!", start_color(Color_LightGray));
        }
        else
        {
            log_add(ui, "%sYou partially resist the attack!", start_color(Color_LightGray));
        }
    }
    
    return(result);
}

internal b32
is_entity_immune(Entity *entity, DamageType damage_type)
{
    b32 result = (entity->resistances[damage_type] == 5);
    return(result);
}

internal void
reset_player_pathfind_trail(Entity *player)
{
    assert(player->type == EntityType_Player);
    
    player->p.render_pathfind_trail = false;
    zero_array(player->p.pathfind_trail, MAX_PATHFIND_TRAIL_COUNT);
}

internal b32
entity_has_status_effect(Entity *entity, StatusEffectType index)
{
    b32 result = entity->statuses[index].type;
    return(result);
}

internal b32
entity_has_any_status_effect(Entity *entity)
{
    b32 result = false;
    
    for(u32 status_index = StatusEffectType_None + 1; status_index < StatusEffectType_Count; ++status_index)
    {
        if(entity_has_status_effect(entity, status_index))
        {
            result = true;
        }
    }
    
    return(result);
}

internal v2u
get_confused_entity_move_pos(Random *random, Entity *entity, Dungeon *dungeon, UI *ui)
{
}

internal b32
in_spell_range(u32 range, v2u origin, v2u target)
{
    b32 result = (!range || get_ordinal_and_ordinal_distance(origin, target) <= range);
    return(result);
}

internal b32
is_inside_rect_and_spell_range(v4u rect, u32 range, v2u origin, v2u target)
{
    b32 result = (is_pos_inside_rect(rect, target) &&
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
update_player_pathfind(Game *game,
                       Entity *player,
                       EntityState *entities,
                       ItemState *items,
                        Dungeon *dungeon)
{
    b32 found_something = false;
    
    // See if we found new items in our view
        for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
        {
            Item *item = &items->array[index];
            if(is_item_valid_and_not_in_inventory(item) &&
               is_tile_seen(dungeon->tiles, item->pos) &&
               !is_set(item->flags, ItemFlags_HasBeenSeen))
            {
                set(item->flags, ItemFlags_HasBeenSeen);
                found_something = true;
            }
        }
    
        // See if we found new entities in our view
    found_something = player_view_has_enemies(entities, dungeon);
    
    if(found_something)
    {
        unset(player->flags, EntityFlags_Pathfinding);
    }
    else
    {
        player->new_pos = get_pathfind_pos(&entities->player_pathfind_map, dungeon->tiles, player->pos, player->pathfind_target_pos);
        
        // Add to player pathfind trail
        player->p.render_pathfind_trail = true;
        for(u32 trail_index = 0; trail_index < MAX_PATHFIND_TRAIL_COUNT; ++trail_index)
        {
            PathfindTrail *trail = &player->p.pathfind_trail[trail_index];
            if(is_v2u_zero(trail->pos))
            {
                trail->direction = get_direction_moved_from(player->new_pos, player->pos);
                trail->pos = player->pos;
                
                break;
            }
        }
        
#if 0
        // Print out the playe pathfind trail information
        for(u32 trail_index = 0; trail_index < MAX_PATHFIND_TRAIL_COUNT; ++trail_index)
        {
            PathfindTrail *trail = &player->p.pathfind_trail[trail_index];
            if(!is_v2u_zero(trail->pos))
            {
                printf("Direction: %u\n", trail->direction);
                printf("Position: %u, %u\n\n", trail->pos.x, trail->pos.y);
            }
        }
        
        printf("\n\n");
        #endif
        
#if 0
        printf("Auto Explore: Pathfind Target Pos %u, %u\n", player->pathfind_target_pos.x, player->pathfind_target_pos.y);
        printf("Auto Explore: New Pos %u, %u\n\n", player->new_pos.x, player->new_pos.y);
#endif
        
        // Stop pathfind if target has been reached
        if(is_v2u_equal(player->new_pos, player->pathfind_target_pos))
        {
            //printf("Auto Explore: Destination Reached\n");
            unset(player->flags, EntityFlags_Pathfinding);
        }
        
        game->should_update = true;
    }
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
        
        return;
    }
#endif
    
    if(is_inside_dungeon(dungeon, new_pos) &&
       has_tile_been_seen(dungeon->tiles, new_pos))
    {
        examine->pos = new_pos;
    }
}

internal b32
is_entity_valid(Entity *entity)
{
    b32 result = (entity->id && entity->type);
    return(result);
}

internal b32
is_entity_valid_and_player(Entity *entity)
{
    b32 result = (is_entity_valid(entity) && entity->type == EntityType_Player);
    return(result);
}

internal b32
is_entity_valid_and_enemy(Entity *entity)
{
    b32 result = (is_entity_valid(entity) && entity->type == EntityType_Enemy);
    return(result);
}

internal Entity *
get_entity_on_pos(EntityState *entities, u32 dungeon_level, v2u pos)
{
    Entity *result = 0;
    
    for(u32 index = 0; index < MAX_ENTITY_COUNT; ++index)
    {
        Entity *entity = &entities->array[index];
        
        if(is_entity_valid(entity) &&
           entity->dungeon_level == dungeon_level &&
           is_v2u_equal(entity->pos, pos))
        {
            result = entity;
            break;
        }
    }
    
    return(result);
}

internal b32
other_windows_are_closed(Game *game, Inventory *inventory, UI *ui)
{
    b32 result = (!is_set(game->examine.flags, ExamineFlags_Open) &&
                      !is_set(inventory->flags, InventoryFlags_Open) &&
                      !is_set(inventory->flags, InventoryFlags_MultiplePickup) &&
                      !is_set(inventory->flags, InventoryFlags_MultipleExamine) &&
                      !game->examine.type &&
                  !ui->is_full_log_open);
    
    return(result);
}

internal void
make_entity_pathfind(Entity *entity, Dungeon *dungeon, ItemState *items, PathfindMap *pathfind_map, v2u pathfind_target_pos)
{
    if(!is_v2u_equal(entity->pos, pathfind_target_pos))
    {
        
#if 0
        print_v2u("Pathfind Origin", entity->pos);
        print_v2u("Pathfind Target", pathfind_target_pos);
        #endif
        
        set(entity->flags, EntityFlags_Pathfinding);
        entity->pathfind_target_pos = pathfind_target_pos;
        
        init_pathfind_map(dungeon, pathfind_map, pathfind_target_pos);
        
        if(entity->type == EntityType_Player)
        {
            reset_player_pathfind_trail(entity);
        }
    }
}

internal void
add_player_starting_item(Game *game,
                         Entity *player,
                         ItemState *items,
                         Inventory *inventory,
                         UI *ui,
                         ItemID item_id,
                         u32 x, u32 y)
{
    Item *item = 0;
    
    if(is_item_id_weapon(item_id))
    {
        item = add_weapon_item(&game->random, items, item_id, ItemRarity_Common, x, y, false);
        set(item->flags, ItemFlags_Equipped);
    }
    else if(is_item_id_potion(item_id))
    {
        item = add_consumable_item(&game->random, items, item_id, x, y, 1);
        set_as_known_and_identify_existing(items, item->id);
    }
    
    assert(item);
    
    item->enchantment_level = 0;
    set(item->flags, ItemFlags_Identified);
    unset(item->flags, ItemFlags_Cursed);
    add_item_to_inventory(game, player, item, items, inventory, ui, false);
}

internal b32
is_enemy_alerted(u32 turns_in_player_view)
{
    b32 result = (turns_in_player_view == 1);
    return(result);
}

internal b32
is_zero(u32 value)
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
add_enemy_spell(Entity *enemy, u32 *spell_chances, SpellID spell_id)
{
    assert(enemy->type == EntityType_Enemy);
    assert(is_set(enemy->flags, EntityFlags_MagicAttacks));
    
    for(u32 index = 0; index < MAX_ENTITY_SPELL_COUNT; ++index)
    {
        Spell *spell = &enemy->e.spells[index];
        if(!spell->id)
        {
            ++enemy->e.spell_count;
            spell->id = spell_id;
            
            switch(spell_id)
            {
                case SpellID_DarkBolt:
                {
                    spell->type = SpellType_Offensive;
                    spell->damage_type = DamageType_Dark;
                    spell->value = 5;
                } break;
                
                case SpellID_LesserHeal:
                {
                    spell->type = SpellType_Healing;
                    spell->value = 2;
                } break;
                
                case SpellID_Bolster:
                {
                    spell->type = SpellType_Buff;
                    spell->value = 2;
                    spell->duration = 6;
                } break;
                
                invalid_default_case;
            }
            
            spell->chance = spell_chances[spell->id];
            return;
        }
    }
    
    assert(false);
}

internal void
start_entity_status_effect(Entity *entity, StatusEffect status_effect)
{
    // Set status effect
    entity->statuses[status_effect.type] = status_effect;
    
    // Change entity stats based on status effect type
    switch(status_effect.type)
    {
        case StatusEffectType_Might: entity->strength += status_effect.value; break;
        case StatusEffectType_Wisdom: entity->intelligence += status_effect.value; break;
        case StatusEffectType_Agility: entity->dexterity += status_effect.value; break;
        
        case StatusEffectType_Decay:
        {
            entity->strength -= status_effect.value;
            entity->intelligence -= status_effect.value;
            entity->dexterity -= status_effect.value;
        } break;
        
        case StatusEffectType_Bolster: entity->defence += status_effect.value; break;
        
        case StatusEffectType_Elusion: break;
        case StatusEffectType_Confusion: break;
        case StatusEffectType_Poison: break;
        case StatusEffectType_Bind: break;
        
        invalid_default_case;
    }
}

internal void
update_entity_status_effects(Game *game,
                             Entity *entity,
                             Dungeon *dungeon,
                             Inventory *inventory,
                             UI *ui)
{
    for(u32 index = StatusEffectType_None + 1; index < StatusEffectType_Count; ++index)
    {
        StatusEffect *status = &entity->statuses[index];
        if(status->type)
        {
            if(status->duration)
            {
                switch(index)
                {
                    case StatusEffectType_Poison:
                    {
                        assert(status->value);
                        
                        switch(entity->type)
                        {
                            case EntityType_Player: log_add(ui, "%sPoison wrecks you for %u damage.", start_color(Color_DarkGreen), status->value); break;
                            case EntityType_Enemy: log_add(ui, "%sPoison wrecks the %s for %u damage.", start_color(Color_DarkGreen), entity->name, status->value); break;
                            
                            invalid_default_case;
                        }
                        
                        attack_entity(&game->random,
                                          0,
                                          entity,
                                          dungeon,
                                          inventory,
                                          ui,
                                          status->value,
                                          DamageType_Poison,
                                          true);
                    } break;
                    
                    case StatusEffectType_Might: break;
                    case StatusEffectType_Wisdom: break;
                    case StatusEffectType_Agility: break;
                    case StatusEffectType_Confusion: break;
                    case StatusEffectType_Elusion: break;
                    case StatusEffectType_Decay: break;
                    case StatusEffectType_Bolster: break;
                    case StatusEffectType_Bind: break;
                    
                    invalid_default_case;
                    }
            }
            
            --status->duration;
            
            #if 0
            printf("\nindex: %u\n", index);
            printf("status->type: %u\n", status->type);
            printf("status->duration: %u\n", status->duration);
            #endif
            
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
                    case StatusEffectType_Bind:
                    {
                        if(entity->type == EntityType_Player)
                        {
                            log_add(ui, "You feel like you can move again.");
                        }
                    } break;
                    
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
    assert(attacker);
    assert(defender);
    
    b32 result = false;
    
    // Attacking an invisible entity has a lower chance of hitting.
    u32 attacker_hit_chance = attacker->hit_chance;
    if(is_set(defender->flags, EntityFlags_Invisible))
    {
        attacker_hit_chance /= 2;
    }
    
    // Defending an attack from an invisible entity has a higher chance of hitting.
    u32 defender_evasion = defender->evasion;
    if(is_set(attacker->flags, EntityFlags_Invisible))
    {
        defender_evasion /= 3;
    }
    
    if(get_random_number(random, 1, attacker_hit_chance) >= defender_evasion)
    {
        result = true;
    }
    
    return(result);
}

internal b32
is_entity_able_to_move(Entity *entity, UI *ui, b32 should_log_add)
{
    b32 result = true;
    
    if(entity_has_status_effect(entity, StatusEffectType_Bind))
    {
        result = false;
        
        if(should_log_add)
        {
            log_add(ui, "You attempt to move, but are not able to!");
        }
    }
    
    return(result);
}

internal void
move_entity_between_dungeons(Entity *entity,
                             v2u new_pos,
                             DungeonState *dungeons,
                             Dungeon *old_dungeon,
                             Dungeon *new_dungeon)
{
    assert(entity);
    assert(entity->type == EntityType_Player);
    
    assert(old_dungeon);
    assert(new_dungeon);
    assert(old_dungeon->level != new_dungeon->level);
    
    dungeons->current_level = new_dungeon->level;
    
    force_move_entity(entity, new_dungeon->tiles, new_pos);
    entity->dungeon_level = dungeons->current_level;
    
    if(entity->type == EntityType_Player)
    {
        update_fov(entity, new_dungeon);
        reset_player_pathfind_trail(entity);
    }
}

internal void
entity_use_passage(Entity *entity,
                   DungeonState *dungeons,
                   Dungeon *dungeon,
                   Passage *passage,
                   UI *ui)
{
    assert(entity->type == EntityType_Player);
    
    u32 new_dungeon_level = dungeon->level;
    if(passage->type == PassageType_Up)
    {
        --new_dungeon_level;
        
        log_add(ui, "You go up the passage..");
    }
    else if(passage->type == PassageType_Down)
    {
        ++new_dungeon_level;
        
        log_add(ui, "You go down the passage..");
    }
    
    assert(new_dungeon_level != dungeon->level);
    
    Dungeon *new_dungeon = get_dungeon_from_level(dungeons, new_dungeon_level);
    move_entity_between_dungeons(entity, passage->dest_pos, dungeons, dungeon, new_dungeon);
    }

internal b32
move_entity(Random *random,
            Entity *entity,
            Tiles tiles,
            UI *ui,
            v2u new_pos)
{
    assert(entity->type == EntityType_Player || entity->type == EntityType_Enemy);
    
    b32 result = is_entity_able_to_move(entity, ui, true);
    
    if(result)
    {
    // If the entity is confused, we attempt to find a direction we could stumble to,
    // that is different from the original move direction.
    if(entity_has_status_effect(entity, StatusEffectType_Confusion))
    {
        StatusEffect *confused_status = &entity->statuses[StatusEffectType_Confusion];
        assert(confused_status->chance);
        
        if(hit_random_chance(random, confused_status->chance))
        {
            Direction confused_direction = get_random_direction(random);
            v2u confused_move_pos = get_direction_pos(entity->pos, confused_direction);
            
            if(confused_direction != entity->new_direction &&
               is_tile_traversable_and_not_occupied(tiles, confused_move_pos))
            {
                
#if 0
                printf("\nconfused_direction: %s\n", get_direction_string(confused_direction));
                printf("confused_move_pos: %u, %u\n", confused_move_pos.x, confused_move_pos.y);
#endif
                
                new_pos = confused_move_pos;
                
                if(entity->type == EntityType_Player)
                {
                    log_add(ui, "%sYou stumble slightly..", start_color(Color_LightGray));
                }
                else if(entity->type == EntityType_Enemy)
                {
                    log_add(ui, "%sThe %s stumbles slightly..", start_color(Color_LightGray), entity->name);
                }
            }
        }
    }
    
        //printf("Moved entity to pos: %u, %u\n", new_pos.x, new_pos.y);
        force_move_entity(entity, tiles, new_pos);
        
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
    
    return(result);
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
get_entity_attack_text(Random *random,
                       Inventory *inventory,
                       Entity *attacker,
                       Entity *defender,
                       u32 value)
{
    assert(attacker);
    assert(defender);
    assert(value);
    
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
                    switch(get_random_number(random, 1, 6))
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
                    switch(get_random_number(random, 1, 6))
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
                    switch(get_random_number(random, 1, 6))
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
                    switch(get_random_number(random, 1, 4))
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
            switch(get_random_number(random, 1, 2))
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
kill_entity(Random *random, Entity *entity, Dungeon *dungeon, UI *ui)
{
    if(entity->type == EntityType_Player)
    {
        // TODO(rami): Death message
        // TODO(rami): Character dumps
        log_add(ui, "Oh no, you are dead!");
        entity->hp = 0;
    }
    else if(entity->type == EntityType_Enemy)
    {
        log_add(ui, "%sThe %s dies!", start_color(Color_LightRed), entity->name);
        
        // Place entity death remains.
        if(entity->remains && can_place_remains_on_pos(dungeon->tiles, entity->pos))
        {
            TileID remains_id = TileID_None;
            
            switch(entity->remains)
            {
                case EntityRemains_RedBlood:
                {
                    remains_id = get_random_number(random,
                                                   TileID_RedBloodGroundMedium1,
                                                   TileID_RedBloodGroundLarge2);
                } break;
                
                case EntityRemains_GreenBlood:
                {
                    remains_id = get_random_number(random,
                                                   TileID_GreenBloodGroundMedium1,
                                                   TileID_GreenBloodGroundLarge2);
                } break;
                
                invalid_default_case;
            }
            
            set_tile_remains(dungeon->tiles, entity->pos, remains_id);
        }
        
        set_tile_occupied(dungeon->tiles, entity->pos, false);
        remove_entity(entity);
    }
}

internal void
attack_entity(Random *random,
              Entity *attacker,
              Entity *defender,
              Dungeon *dungeon,
              Inventory *inventory,
              UI *ui,
              u32 damage,
              DamageType damage_type,
              b32 has_no_attacker)
{
    assert(defender);
        assert(damage);
    assert(damage_type);
    
#if MOONBREATH_SLOW
    // Skip dummies during debugging.
    if(defender->id == EntityID_Dummy) return;
    
    // Player Hit Test
    if(attacker && attacker->type == EntityType_Player && fkey_active[4])
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
    
    if(has_no_attacker || will_entity_hit(random, attacker, defender))
    {
        if(defender->defence)
        {
            damage -= get_random_number(random, 0, defender->defence);
        }
        
        if(!is_zero(damage))
        {
            damage = apply_entity_resistance_to_damage(defender, ui, damage, damage_type);
            
            if(!is_zero(damage))
            {
                if(!has_no_attacker)
                {
                log_add(ui, get_entity_attack_text(random, inventory, attacker, defender, damage).str);
                }
                
                defender->hp -= damage;
                if(!is_zero(defender->hp))
                {
                    // Place defender entity remains splatter
                    u32 remains_chance = 30;
                    if(defender->remains && hit_random_chance(random, remains_chance))
                    {
                        Direction remains_direction = get_random_number(random, Direction_None, Direction_DownRight);
                        v2u remains_pos = get_direction_pos(defender->pos, remains_direction);
                        //printf("remains_pos: %u, %u\n", remains_pos.x, remains_pos.y);
                        
                        // What remains_id is set to depends on the tile it's placed on.
                        if(can_place_remains_on_pos(dungeon->tiles, remains_pos) &&
                           !is_tile_occupied(dungeon->tiles, remains_pos))
                        {
                            TileID remains_id = TileID_None;
                            
                            if(is_tile_wall(dungeon->tiles, remains_pos))
                            { 
                                switch(defender->remains)
                                {
                                    case EntityRemains_RedBlood:
                                    {
                                        if(remains_direction == Direction_Up)
                                        {
                                            remains_id = get_random_number(random, TileID_RedBloodWallUp1, TileID_RedBloodWallUp3);
                                        }
                                        else if(remains_direction == Direction_Down)
                                        {
                                            remains_id = get_random_number(random, TileID_RedBloodWallDown1, TileID_RedBloodWallDown3);
                                        }
                                        else if(remains_direction == Direction_Left)
                                        {
                                            remains_id = get_random_number(random, TileID_RedBloodWallLeft1, TileID_RedBloodWallLeft3);
                                        }
                                        else if(remains_direction == Direction_Right)
                                        {
                                            remains_id = get_random_number(random, TileID_RedBloodWallRight1, TileID_RedBloodWallRight3);
                                        }
                                    } break;
                                    
                                    case EntityRemains_GreenBlood:
                                    {
                                        if(remains_direction == Direction_Up)
                                        {
                                            remains_id = get_random_number(random, TileID_GreenBloodWallUp1, TileID_GreenBloodWallUp3);
                                        }
                                        else if(remains_direction == Direction_Down)
                                        {
                                            remains_id = get_random_number(random, TileID_GreenBloodWallDown1, TileID_GreenBloodWallDown3);
                                        }
                                        else if(remains_direction == Direction_Left)
                                        {
                                            remains_id = get_random_number(random, TileID_GreenBloodWallLeft1, TileID_GreenBloodWallLeft3);
                                        }
                                        else if(remains_direction == Direction_Right)
                                        {
                                            remains_id = get_random_number(random, TileID_GreenBloodWallRight1, TileID_GreenBloodWallRight3);
                                        }
                                    } break;
                                    
                                    invalid_default_case;
                                }
                            }
                            else
                            {
                                switch(defender->remains)
                                {
                                    case EntityRemains_RedBlood:
                                    {
                                        remains_id = get_random_number(random,
                                                                   TileID_RedBloodGroundSmall1,
                                                                   TileID_RedBloodGroundSmall3);
                                    } break;
                                    
                                    case EntityRemains_GreenBlood:
                                    {
                                        remains_id = get_random_number(random,
                                                                   TileID_GreenBloodGroundSmall1,
                                                                   TileID_GreenBloodGroundSmall3);
                                    } break;
                                    
                                    invalid_default_case;
                                }
                            }
                            
                            set_tile_remains(dungeon->tiles, remains_pos, remains_id);
                        }
                    }
                    
                    // Apply poison status effect from attacker to defender if possible
                    if(attacker &&
                       attacker->e.poison.type &&
                           !entity_has_status_effect(defender, StatusEffectType_Poison) &&
                               hit_random_chance(random, attacker->e.poison.chance))
                    {
                        if(is_entity_immune(defender, DamageType_Poison))
                        {
                            log_add(ui, "%sYour resistance stopped you from being poisoned.", start_color(Color_DarkGreen));
                            }
                        else
                        {
                            switch(defender->type)
                            {
                                case EntityType_Player: log_add(ui, "%sYou start feeling unwell.", start_color(Color_DarkGreen)); break;
                                case EntityType_Enemy: log_add(ui, "%sThe %s starts to look unwell.", start_color(Color_DarkGreen)); break;
                                
                                invalid_default_case;
                            }
                            
                            start_entity_status_effect(defender, attacker->e.poison);
                        }
                    }
                }
                else
                {
                    kill_entity(random, defender, dungeon, ui);
                }
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

internal void
handle_asking_player(Game *game,
                     Input *input,
                     ItemState *items,
                     Inventory *inventory,
                     UI *ui)
{
    if(was_pressed(&input->GameKey_Yes))
    {
        if(inventory->item_use_type)
        {
            log_add(ui, "%sThe scroll turns illegible, you discard it.", start_color(Color_LightGray));
            inventory->item_use_type = UsingItemType_None;
        }
        
        end_consumable_use(inventory->examine_item, items, inventory);
        unset(inventory->flags, InventoryFlags_AskingPlayer | InventoryFlags_ReadyForKeypress);
    }
    else if(was_pressed(&input->GameKey_No))
    {
        log_add_okay(ui);
        unset(inventory->flags, InventoryFlags_AskingPlayer | InventoryFlags_ReadyForKeypress);
    }
}

internal b32
was_pressed(InputState *state)
{
    b32 result = false;
    
    if(state->is_down)
    {
        
#if MOONBREATH_SLOW
        if(fkey_active[3])
        {
            return(true);
        }
#endif
        
        if(state->has_been_up || state->repeat)
        {
            state->has_been_up = false;
            result = true;
        }
    }
    
    return(result);
}

internal void
update_player_input(Game *game,
                    Input *input,
                    Entity *player,
                    EntityState *entities,
                    ItemState *items,
                    Inventory *inventory,
                    DungeonState *dungeons,
                    Assets *assets,
                    UI *ui)
{
    Dungeon *dungeon = get_dungeon_from_level(dungeons, dungeons->current_level);
    
    game->should_update = false;
    game->action_count = 0.0f;
    
    set(player->flags, EntityFlags_MultipleItemNotify);
    player->new_pos = player->pos;
    player->new_direction = Direction_None;
    
    if(is_set(player->flags, EntityFlags_Pathfinding))
    {
        update_player_pathfind(game, player, entities, items, dungeon);
    }
    else if(is_set(inventory->flags, InventoryFlags_AskingPlayer))
    {
        handle_asking_player(game, input, items, inventory, ui);
    }
    else
    {
        
#if MOONBREATH_SLOW
        if(input->fkeys[1].is_down &&
                input->fkeys[1].has_been_up)
        {
            game->should_update = true;
            game->action_count = 1.0f;
            
            input->fkeys[1].has_been_up = false;
            fkey_active[1] = !fkey_active[1];
            
            return;
        }
        //else if(was_pressed(&input->fkeys[2]))
        else if(input->fkeys[2].is_down &&
                input->fkeys[2].has_been_up)
        {
            game->should_update = true;
            
            input->fkeys[2].has_been_up = false;
            fkey_active[2] = !fkey_active[2];
            
            return;
        }
        else if(input->fkeys[3].is_down &&
        input->fkeys[3].has_been_up)
        {
            game->should_update = true;
            
            input->fkeys[3].has_been_up = false;
            fkey_active[3] = !fkey_active[3];
            
            return;
        }
        else if(input->fkeys[4].is_down &&
                input->fkeys[4].has_been_up)
        {
            input->fkeys[4].has_been_up = false;
            fkey_active[4] = !fkey_active[4];
            return;
        }
        else if(was_pressed(&input->fkeys[5]))
        {
            for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
            {
                Item *item = &items->array[index];
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
            
            return;
        }
        else if(was_pressed(&input->fkeys[6]))
        {
            for(u32 index = 0; index < dungeon->rooms_count; ++index)
            {
                Room *room = dungeon->rooms;
                if(is_inside_room(room->rect, player->pos))
                {
                    printf("Room Index: %u\n", index);
                    print_v4u(room->rect);
                    printf("enemy_count: %u\n", room->enemy_count);
                    printf("item_count: %u\n\n", room->item_count);
                    
                    break;
                }
            }
            
            return;
        }
        else if(was_pressed(&input->fkeys[7]))
        {
            return;
        }
        else if(was_pressed(&input->fkeys[8]))
        {
            return;
        }
        else if(was_pressed(&input->fkeys[9]))
        {
            return;
        }
        else if(was_pressed(&input->fkeys[10]))
        {
            return;
        }
        else if(was_pressed(&input->fkeys[11]))
        {
            return;
        }
        else if(was_pressed(&input->fkeys[12]))
        {
            return;
        }
        else if(was_pressed(&input->Button_Right))
        {
            b32 found_something = false;
            
            for(u32 index = 0; index < MAX_ENTITY_COUNT; ++index)
            {
                Entity *entity = &entities->array[index];
                if(is_entity_valid_and_enemy(entity) &&
                   is_v2u_equal(entity->pos, input->mouse_tile_pos))
                {
                    found_something = true;
                    
                    printf("Entity Name: %s\n", entity->name);
                    printf("Entity Defence: %u\n", entity->defence);
                    
                    break;
                }
            }
            
            if(!found_something)
            {
                for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
                {
                    Item *item = &items->array[index];
                    if(is_v2u_equal(item->pos, input->mouse_tile_pos))
                    {
                        found_something = true;
                        
                        if(is_item_consumable(item->type))
                        {
                            printf("Item Name: %s\n", item->name);
                        }
                        else
                        {
                            printf("Item Name: %c%d %s\n",
                                       get_sign(item->enchantment_level),
                                       get_absolute(item->enchantment_level),
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
            
            if(!found_something)
            {
                for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
                {
                    Item *item = &items->array[index];
                    if(is_item_valid(item))
                    {
                        printf("Name: %s\n", item->name);
                        printf("Pos: %u, %u\n\n", item->pos.x, item->pos.y);
                    }
                }
            }
            
            return;
        }
#endif
        
            b32 was_direction_pressed = false;
            for(Direction direction = Direction_Up; direction <= Direction_DownRight; ++direction)
            {
                u32 index = direction - 1;
                if(was_pressed(&input->game_keys[index]) || game->examine.key_pressed[index])
            {
                    if(is_set(game->examine.flags, ExamineFlags_Open) && !game->examine.type)
                    {
                        update_examine_pos(&game->examine, direction, dungeon);
                    }
                else if(other_windows_are_closed(game, inventory, ui))
                {
                    reset_player_pathfind_trail(player);
                    
                    v2u new_pos = get_direction_pos(player->pos, direction);
                    if(is_inside_dungeon(dungeon, new_pos))
                    {
                        player->new_pos = new_pos;
                        game->should_update = true;
                    }
                    }
                    
                    was_direction_pressed = true;
                    break;
                }
            }
            
            if(!was_direction_pressed)
        {
            if(!is_set(inventory->flags, InventoryFlags_Open) &&
               was_pressed(&input->GameKey_OpenInventory))
            {
                if(other_windows_are_closed(game, inventory, ui))
                    {
                        set_view_at_start(&inventory->view);
                        
                        set(inventory->flags, InventoryFlags_Open);
                    unset(inventory->flags, InventoryFlags_AskingPlayer);
                }
                
                }
            else if(was_pressed(&input->GameKey_Back))
                {
                    Examine *examine = &game->examine;
                
                    if(!is_set(inventory->flags, InventoryFlags_Marking))
                    {
                        if((is_set(game->examine.flags, ExamineFlags_Open) || examine->type))
                        {
                            if(examine->type == ExamineType_EntitySpell)
                            {
                                examine->type = ExamineType_Entity;
                            }
                            else
                            {
                                unset(game->examine.flags, ExamineFlags_Open | ExamineFlags_ReadyForKeypress);
                                examine->type = ExamineType_None;
                            }
                        }
                        else if(is_set(inventory->flags, InventoryFlags_Adjusting))
                    {
                            unset(inventory->flags, InventoryFlags_Adjusting);
                            log_add_okay(ui);
                        }
                    else if(is_set(inventory->flags, InventoryFlags_Examining))
                    {
                        unset(inventory->flags, InventoryFlags_Examining);
                        }
                    else if(inventory->item_use_type)
                    {
                            ask_for_item_cancel(input, game, ui, inventory);
                        }
                        else if(is_set(inventory->flags, InventoryFlags_Open))
                    {
                            unset(inventory->flags, InventoryFlags_Open | InventoryFlags_ReadyForKeypress);
                    }
                    else if(is_set(inventory->flags, InventoryFlags_MultiplePickup))
                    {
                        reset_item_selections(items);
                        unset(inventory->flags, InventoryFlags_MultiplePickup);
                    }
                    else if(is_set(inventory->flags, InventoryFlags_MultipleExamine))
                    {
                        reset_item_selections(items);
                        unset(inventory->flags, InventoryFlags_MultipleExamine);
                    }
                        else if(ui->is_full_log_open)
                    {
                            ui->is_full_log_open = false;
                    }
                    else
                    {
                        game->mode = GameMode_Quit;
                    }
                    }
                }
                else if(was_pressed(&input->GameKey_Pickup))
            {
                if(other_windows_are_closed(game, inventory, ui))
                {
                    if(get_pos_item_count(items, player->pos) > 1)
                    {
                        set(inventory->flags, InventoryFlags_MultiplePickup);
                        set_view_at_start(&inventory->pickup_view);
                        }
                    else
                    {
                        // Pickup item
                        
                        Item *item = get_item_on_pos(items, dungeon->level, player->pos, 0);
                        if(item)
                        {
                            add_item_to_inventory(game, player, item, items, inventory, ui, true);
                        }
                        else
                        {
                            log_add(ui, "You see nothing to pick up here.");
                        }
                    }
                    }
                }
            else if(was_pressed(&input->GameKey_UsePassage))
                {
                if(other_windows_are_closed(game, inventory, ui))
                {
                    if(is_tile_id(dungeon->tiles, player->pos, TileID_ExitDungeon))
                    {
                        game->mode = GameMode_Quit;
                    }
                    else if(is_tile_id(dungeon->tiles, player->pos, TileID_StoneStaircaseUp) ||
                                is_tile_id(dungeon->tiles, player->pos, TileID_StoneStaircaseDown))
                    {
                        Passage *used_passage = get_passage_on_pos(&dungeon->passages, player->pos);
                        entity_use_passage(player, dungeons, dungeon, used_passage, ui);
                        
                        game->should_update = true;
                        game->action_count = 3.0f;
                    }
                    }
                }
                else if(was_pressed(&input->GameKey_AutoExplore))
            {
                // Attempt to start player pathfinding
                if(other_windows_are_closed(game, inventory, ui))
                {
                    if(player_view_has_enemies(entities, dungeon))
                    {
                        log_add(ui, "There are enemies near!");
                    }
                    else
                    {
                        if(is_entity_able_to_move(player, ui, true))
                        {
                            assert(!is_set(player->flags, EntityFlags_Pathfinding));
                            
                            b32 pathfind_target_pos_set = false;
                            v2u pathfind_target_pos = {0};
                            
                            while(is_dungeon_explorable(dungeon))
                            {
                                pathfind_target_pos = get_random_dungeon_pos(&game->random, dungeon);
                                if(is_tile_traversable_and_has_not_been_seen(dungeon->tiles, pathfind_target_pos))
                                {
                                    pathfind_target_pos_set = true;
                                    break;
                                }
                            }
                            
                            if(pathfind_target_pos_set)
                            {
                                make_entity_pathfind(player, dungeon, items, &entities->player_pathfind_map, pathfind_target_pos);
                            }
                            else
                            {
                                log_add(ui, "Nothing more to explore.");
                            }
                        }
                    }
                    }
                }
                else if(was_pressed(&input->GameKey_Examine))
                {
                if(other_windows_are_closed(game, inventory, ui))
                {
                    Examine *examine = &game->examine;
                    
                    set(examine->flags, ExamineFlags_Open);
                    examine->pos = player->pos;
                    
                    examine->selected_passage = 0;
                    }
                }
                else if(was_pressed(&input->GameKey_Log))
                {
                if(other_windows_are_closed(game, inventory, ui))
                    {
                        ui->is_full_log_open = true;
                        ui->is_full_log_view_set_at_end = false;
                        
                        set_view_at_start(&ui->full_log_view);
                    }
                }
                else if(was_pressed(&input->GameKey_Wait))
                {
                if(other_windows_are_closed(game, inventory, ui))
                {
                    log_add(ui, "%sA moment goes by..", start_color(Color_LightGray));
                    unset(player->flags, EntityFlags_MultipleItemNotify);
                        
                        game->should_update = true;
                    game->action_count = 1.0f;
                    }
                }
                else if(input->Button_ScrollUp.is_down ||
                        input->Button_ScrollDown.is_down ||
                        input->Key_PageUp.is_down ||
                        input->Key_PageDown.is_down)
            {
                    if(is_set(inventory->flags, InventoryFlags_Open))
                    {
                        update_view_scrolling(&inventory->view, input);
                }
                else if(is_set(inventory->flags, InventoryFlags_MultiplePickup))
                {
                    update_view_scrolling(&inventory->pickup_view, input);
                }
                    else if(ui->is_full_log_open)
                    {
                        update_view_scrolling(&ui->full_log_view, input);
                    }
                }
            else if(is_set(inventory->flags, InventoryFlags_Open) ||
                        is_set(inventory->flags, InventoryFlags_MultiplePickup) ||
                        is_set(inventory->flags, InventoryFlags_MultipleExamine))
                {
                Item *examine_item = inventory->examine_item;
                
                if(is_set(inventory->flags, InventoryFlags_Examining))
                    {
                        if(is_set(inventory->flags, InventoryFlags_Marking))
                        {
                            update_item_marking(input, examine_item, inventory, ui);
                        }
                        else
                        {
                            if(is_set(inventory->flags, InventoryFlags_Adjusting))
                            {
                            update_item_adjusting(input, examine_item, items, inventory, ui);
                            }
                            else
                            {
                                if(was_pressed(&input->Key_A))
                                {
                                set(inventory->flags, InventoryFlags_Adjusting);
                                log_add(ui, "%sAdjust to which letter? (%s to quit).", start_color(Color_Yellow), get_printable_key(input, game->keybinds[GameKey_Back]).str);
                                }
                                else if(was_pressed(&input->Key_E))
                                {
                                    equip_item(game, examine_item, inventory, ui);
                                }
                                else if(was_pressed(&input->Key_U))
                                {
                                    if(unequip_item(game, ui, examine_item))
                                {
                                    log_add_item_action_text(ui, examine_item, ItemActionType_Unequip);
                                    }
                                }
                                else if(was_pressed(&input->Key_R))
                                {
                                    if(examine_item->type == ItemType_Scroll)
                                    {
                                    read_scroll(game, player, examine_item, items, inventory, dungeon, ui);
                                    }
                                }
                                else if(was_pressed(&input->Key_C))
                                {
                                    if(examine_item->type == ItemType_Potion ||
                                       examine_item->type == ItemType_Ration)
                                    {
                                    consume_consumable(game, player, examine_item, items, inventory, ui);
                                    }
                                }
                                else if(was_pressed(&input->Key_D))
                            {
                                drop_item_from_inventory(game, player, examine_item, items, inventory, dungeon, ui);
                                }
                                else if(was_pressed(&input->Key_M))
                                {
                                // Begin marking the examine item
                                set(inventory->flags, InventoryFlags_Marking);
                                Mark *mark = &ui->mark;
                                
                                if(is_set(examine_item->flags, ItemFlags_Marked))
                                {
                                    assert(!examine_item->mark.should_render);
                                    assert(!examine_item->mark.cursor_index);
                                    assert(!examine_item->mark.render_start);
                                    
                                    ui->mark.view = examine_item->mark.view;
                                    strcpy(ui->mark.array, examine_item->mark.array);
                                    
                                    set_mark_view_and_cursor_to_end(mark);
                                    assert(mark->view.count);
                                }
                                else
                                {
                                    set_mark_view_and_cursor_to_start(mark);
                                }
                                }
                            }
                        }
                    }
                    else
                {
                    char pressed = get_pressed_alphabet_char(input);
                    if(is_set(inventory->flags, InventoryFlags_Open))
                    {
                            if(is_set(inventory->flags, InventoryFlags_ReadyForKeypress))
                            {
                                if(pressed)
                            {
                                if(inventory->item_use_type)
                                    {
                                    use_inventory_item(&game->random, pressed, examine_item, items, inventory, ui);
                                    }
                                    else
                                {
                                    // Examine the item from inventory if we find it
                                    Item *item = get_item_from_letter(items, pressed, LetterType_Letter, true);
                                    if(item)
                                    {
                                        set(inventory->flags, InventoryFlags_Examining);
                                        inventory->examine_item = item;
                                    }
                                    }
                                }
                            }
                            else
                            {
                                set(inventory->flags, InventoryFlags_ReadyForKeypress);
                            }
                        }
                    else if(is_set(inventory->flags, InventoryFlags_MultiplePickup))
                    {
                        if(was_pressed(&input->Key_Enter))
                        {
                            // Add selected items to inventory
                            for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
                            {
                                Item *item = &items->array[index];
                                if(is_item_valid_and_selected(item))
                                {
                                    if(!add_item_to_inventory(game, player, item, items, inventory, ui, true))
                                    {
                                        break;
                                    }
                                }
                            }
                            
                            reset_item_selections(items);
                            unset(inventory->flags, InventoryFlags_MultiplePickup);
                            }
                            else if(pressed)
                            {
                                // Select and unselect the item in the pickup window
                            Item *item = get_item_from_letter(items, pressed, LetterType_SelectLetter, false);
                                if(item)
                                {
                                if(is_set(item->flags, ItemFlags_Selected))
                                    {
                                    unset(item->flags, ItemFlags_Selected);
                                    }
                                    else
                                    {
                                    set(item->flags, ItemFlags_Selected);
                                    }
                                }
                            }
                    }
                    else if(is_set(inventory->flags, InventoryFlags_MultipleExamine))
                    {
                        if(pressed)
                        {
                            Item *item = get_item_from_letter(items, pressed, LetterType_SelectLetter, false);
                            if(item)
                            {
                                set(inventory->flags, InventoryFlags_Examining);
                                inventory->examine_item = item;
                            }
                        }
                    }
                    }
            }
            }
    }
}

internal void
update_entities(Game *game,
                Input *input,
                EntityState *entities,
                ItemState *items,
                Inventory *inventory,
                DungeonState *dungeons,
                Assets *assets,
UI *ui)
{
    Dungeon *dungeon = get_dungeon_from_level(dungeons, dungeons->current_level);
    
    // Update player
    Entity *player = &entities->array[0];
    assert(is_entity_valid_and_player(player));
    
    update_player_input(game, input, player, entities, items, inventory, dungeons, assets, ui);
    
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
    if(player->statuses[StatusEffectType_Elusion].type)
    {
        player->evasion += 2;
    }
    
#if 0
    printf("Player Defence: %u\n", player->defence);
    printf("Player Weight: %u\n", player->p.weight);
    printf("Player Evasion: %u\n\n", player->evasion);
#endif
    
    if(game->should_update)
    {
        player->new_direction = get_direction_moved_from(player->pos, player->new_pos);
        
#if MOONBREATH_SLOW
        if(fkey_active[2])
        {
            force_move_entity(player, dungeon->tiles, player->new_pos);
            update_fov(player, dungeon);
        }
        
        else
#endif
        
        if(!is_v2u_equal(player->pos, player->new_pos))
        {
            if(!is_v2u_equal(player->pos, player->new_pos) && is_tile_occupied(dungeon->tiles, player->new_pos))
            {
                for(u32 target_index = 0; target_index < MAX_ENTITY_COUNT; ++target_index)
                {
                    Entity *target = &entities->array[target_index];
                    if(target->type == EntityType_Enemy &&
                       is_v2u_equal(player->new_pos, target->pos))
                    {
                        u32 player_damage = 2;
                        u32 player_accuracy = 1;
                        f32 player_attack_speed = 1.0f;
                        DamageType player_damage_type = DamageType_Physical;
                        
                        Item *item = get_equipped_item_from_slot(ItemSlot_FirstHand, inventory);
                        if(item)
                        {
                            player_damage = item->w.damage + item->enchantment_level;
                            player_accuracy = item->w.accuracy + item->enchantment_level;
                            player_attack_speed = item->w.speed;
                            player_damage_type = item->first_damage_type;
                        }
                        
#if 0
                        printf("player_damage: %u\n", player_damage);
                        printf("player_accuracy: %u\n", player_accuracy);
                        printf("player_attack_speed: %.01f\n", player_attack_speed);
#endif
                        
                        player->hit_chance = 15 + (player->dexterity / 2);
                        player->hit_chance += player_accuracy;
                        
                        // Apply strength bonus to damage
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
                        attack_entity(&game->random, player, target, dungeon, inventory, ui, modified_player_damage, player_damage_type, false);
                        
                        game->action_count = player_attack_speed;
                    }
                }
            }
            else
            {
                if(is_tile_id(dungeon->tiles, player->new_pos, TileID_StoneDoorClosed))
                {
                    set_tile_id(dungeon->tiles, player->new_pos, TileID_StoneDoorOpen);
                    game->action_count = 1.0f;
                }
                else if(is_tile_traversable(dungeon->tiles, player->new_pos) &&
                            move_entity(&game->random, player, dungeon->tiles, ui, player->new_pos))
                {
                    game->action_count = 1.0f;
                    
                    // TODO(rami): Maybe a specific color for trap trigger messages
                    
                    // Player stepped on a trap
                    if(is_tile_trap(&dungeon->traps, player->new_pos))
                    {
                        Trap *trap = get_trap_on_pos(dungeon->tiles, &dungeon->traps, player->new_pos);
                        switch(trap->type)
                        {
                            case TrapType_Spike:
                            {
                                DamageType spike_damage_type = DamageType_Physical;
                                u32 spike_damage = get_random_number_from_v2u(&game->random, dungeon->spike_trap_value);
                                
                                log_add(ui, "Sharp spikes pierce you from below, dealing %u damage.", spike_damage);
                                attack_entity(&game->random, 0, player, dungeon, inventory, ui, spike_damage, spike_damage_type, true);
                            } break;
                            
                            case TrapType_Sword:
                            {
                                DamageType sword_damage_type = DamageType_Physical;
                                u32 sword_damage = get_random_number_from_v2u(&game->random, dungeon->sword_trap_value);
                                
                                log_add(ui, "Multiple swords lacerate you from below, dealing %u damage.", sword_damage);
                                attack_entity(&game->random, 0, player, dungeon, inventory, ui, sword_damage, sword_damage_type, true);
                            } break;
                            
                            case TrapType_Arrow:
                            {
                                DamageType arrow_damage_type = DamageType_Physical;
                                
                                u32 arrow_damage = get_random_number_from_v2u(&game->random, dungeon->arrow_trap_value);
                                
                                log_add(ui, "Multiple arrows shoot up from below, dealing %u damage.", arrow_damage);
                                attack_entity(&game->random, 0, player, dungeon, inventory, ui, arrow_damage, arrow_damage_type, true);
                            } break;
                            
                            case TrapType_Magic:
                            {
                                DamageType magic_damage_type = get_random_damage_type(&game->random);
                                assert(magic_damage_type != DamageType_Physical);
                                
                                u32 magic_damage = get_random_number_from_v2u(&game->random, dungeon->magic_trap_value);
                                
                                log_add(ui, "A magical trap explodes below you, dealing %u %s damage.", magic_damage, get_damage_type_text(magic_damage_type));
                                attack_entity(&game->random, 0, player, dungeon, inventory, ui, magic_damage, magic_damage_type, true);
                            } break;
                            
                            case TrapType_Bind:
                            {
                                log_add(ui, "You feel like you can't move!");
                                
                                StatusEffect bind_status_effect = {0};
                                bind_status_effect.type = StatusEffectType_Bind;
                                bind_status_effect.duration = get_random_number_from_v2u(&game->random, dungeon->bind_trap_value);
                                
                                start_entity_status_effect(player, bind_status_effect);
                                
                            } break;
                            
                            case TrapType_Shaft:
                            {
                                log_add(ui, "You fall into a shaft!");
                                
                                if(!trap->is_shaft_set)
                                { 
                                    trap->shaft_depth = get_random_number_from_v2u(&game->random, dungeon->shaft_trap_value);
                                    
                                    u32 trap_shaft_dungeon_level = dungeons->current_level + trap->shaft_depth;
                                    assert(trap_shaft_dungeon_level <= MAX_DUNGEON_LEVELS);
                                    
                                    Dungeon *shaft_dungeon = get_dungeon_from_level(dungeons, trap_shaft_dungeon_level);
                                    
                                    for(;;)
                                    {
                                        trap->shaft_destination = get_random_dungeon_pos(&game->random, shaft_dungeon);
                                        if(is_tile_traversable_and_not_occupied(shaft_dungeon->tiles, trap->shaft_destination))
                                        {
                                            move_entity_between_dungeons(player, trap->shaft_destination, dungeons, dungeon, shaft_dungeon);
                                            break;
                                        }
                                    }
                                    
                                    trap->is_shaft_set = true;
                                    //printf("Set shaft data - Depth: %u - Destination: %u, %u.\n", trap->shaft_depth, trap->shaft_destination.x, trap->shaft_destination.y);
                                }
                            } break;
                            
                            case TrapType_Summon:
                            {
                                log_add(ui, "You hear an odd sound and something appears next to you.");
                                
                                // Find summon position
                                v4u summon_rect = get_dimension_rect(dungeon, player->pos, 2);
                                v2u summon_pos = {0};
                                for(;;)
                                {
                                    summon_pos = get_random_rect_pos(&game->random, summon_rect);
                                    if(is_tile_traversable_and_not_occupied(dungeon->tiles, summon_pos))
                                    {
                                        break;
                                    }
                                }
                                
                                EntityID enemy_id = get_random_enemy_from_level(&game->random, entities, dungeon);
                                add_enemy_entity(entities, dungeon, enemy_id, summon_pos.x, summon_pos.y);
                                
#if 0
                                printf("ID: %u\n", enemy_id);
                                printf("Summon Pos: %u, %u\n", summon_pos.x, summon_pos.y);
#endif
                                
                            } break;
                            
                            case TrapType_Teleport:
                            {
                                log_add(ui, "You take a step and find yourself in a different place.");
                                teleport_entity(&game->random, player, dungeon, ui);
                            } break;
                            
                            invalid_default_case;
                        }
                    }
                }
            }
        }
    }
    
    if(game->action_count)
    {
        game->time += game->action_count;
        player->action_count = game->action_count;
        
        for(u32 action_count = 0; action_count < player->action_count; ++action_count)
        {
            update_entity_status_effects(game, player, dungeon, inventory, ui);
        }
        
        // Inform the player if there are multiple items on their position
        if(is_set(player->flags, EntityFlags_MultipleItemNotify))
        {
            if(get_pos_item_count(items, player->pos) > 1)
            {
                log_add(ui, "There are multiple items here.");
            }
        }
        
        // Enemy pathfind map is created out of the player position everytime the
        // player does an action. Enemies will find the player by using the map.
        init_pathfind_map(dungeon, &entities->enemy_pathfind_map, player->pos);
        update_fov(player, dungeon);
    }
    
    // Update enemies
    for(u32 enemy_index = 0; enemy_index < MAX_ENTITY_COUNT; ++enemy_index)
    {
        Entity *enemy = &entities->array[enemy_index];
        
        // TODO(rami): In the future, update if the enemy is on a level that the player has
        // been in. b32 visited_by_player or something in the dungeon struct.
        if(is_entity_valid_and_enemy(enemy))
        {
            if(game->action_count)
            {
                
#if MOONBREATH_SLOW
                    if(enemy->id == EntityID_Dummy) continue;
#endif
                    
                    enemy->e.action_count_timer += enemy->action_count;
                    u32 enemy_action_count = enemy->e.action_count_timer / game->action_count;
                    
#if 0
                    printf("enemy->action_count: %.1f\n", enemy->action_count);
                    printf("action_count_timer: %.1f\n", enemy->e.action_count_timer);
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
                            
                            v2u pathfind_pos = get_pathfind_pos(&entities->enemy_pathfind_map, dungeon->tiles, enemy->pos, player->pos);
                            enemy->hit_chance = 30;
                            assert(player->evasion < enemy->hit_chance);
                            
                            v4u enemy_fov_rect = get_dimension_rect(dungeon, player->pos, enemy->fov);
                            
                            if(is_set(enemy->flags, EntityFlags_MagicAttacks))
                            {
                                enemy->e.spell_index = get_random_with_chances(&game->random, entities->spell_chances, 0, enemy->e.spell_count, RandomChanceType_Normal);
                                
                                // TODO(rami): Duplication
                                Spell *spell = &enemy->e.spells[enemy->e.spell_index];
                                if(spell->type == SpellType_Offensive)
                                {
                                    if(is_inside_rect_and_spell_range(enemy_fov_rect, spell->range, enemy->pos, player->pos))
                                    {
                                        attack_entity(&game->random, enemy, player, dungeon, inventory, ui, spell->value, spell->damage_type, false);
                                    }
                                }
                                else if(spell->type == SpellType_Healing)
                                {
                                    for(u32 target_index = 0; target_index < MAX_ENTITY_COUNT; ++target_index)
                                    {
                                        Entity *target = &entities->array[target_index];
                                        if(target_index != enemy_index &&
                                           is_entity_valid_and_enemy(target))
                                        {
                                            if(is_inside_rect_and_spell_range(enemy_fov_rect, spell->range, enemy->pos, target->pos) &&
                                               is_set(target->flags, EntityFlags_Combat) &&
                                               target->hp < target->max_hp)
                                            {
                                                log_add(ui, "The %s casts %s at the %s, healing it for %u health.", enemy->name, get_spell_name(spell->id), target->name, spell->value);
                                                heal_entity(target, spell->value);
                                                
                                                break;
                                            }
                                        }
                                    }
                                }
                                else if(spell->type == SpellType_Buff)
                                {
                                    for(u32 target_index = 0; target_index < MAX_ENTITY_COUNT; ++target_index)
                                    {
                                        Entity *target = &entities->array[target_index];
                                        if(target_index != enemy_index &&
                                           is_entity_valid_and_enemy(target))
                                        {
                                            if(is_inside_rect_and_spell_range(enemy_fov_rect, spell->range, enemy->pos, target->pos) &&
                                               is_set(target->flags, EntityFlags_Combat) &&
                                               !entity_has_status_effect(target, spell->type))
                                            {
                                                log_add(ui, "The %s casts %s on the %s.", enemy->name, get_spell_name(spell->id), target->name);
                                                start_entity_status_effect(target, spell->status_effect);
                                                
                                                break;
                                            }
                                        }
                                    }
                                }
                            }
                            else if(is_pos_inside_rect(enemy_fov_rect, player->pos) &&
                                    (is_set(enemy->flags, EntityFlags_RangedAttacks) ||
                                     is_v2u_equal(pathfind_pos, player->pos)))
                            {
                                attack_entity(&game->random, enemy, player, dungeon, inventory, ui, enemy->e.damage, enemy->e.damage_type, false);
                            }
                            else
                            {
                                enemy->new_pos = pathfind_pos;
                            }
                        }
                        else
                        {
                            if(is_set(enemy->flags, EntityFlags_Combat) &&
                               !is_enemy_alerted(enemy->e.turns_in_player_view))
                            {
                                if(!is_set(enemy->flags, EntityFlags_Pathfinding))
                                {
                                    make_entity_pathfind(enemy, dungeon, items, &entities->enemy_pathfind_map, player->pos);
                                    //printf("Enemy Pathfind: Target %u, %u\n", enemy->pathfind_target.x, enemy->pathfind_target.y);
                                }
                                
                                enemy->new_pos = get_pathfind_pos(&entities->enemy_pathfind_map, dungeon->tiles, enemy->pos, enemy->pathfind_target_pos);
                                //printf("Enemy Pathfind: New Pos %u, %u\n", enemy->new_pos.x, enemy->new_pos.y);
                                
                                if(is_v2u_equal(enemy->new_pos, enemy->pathfind_target_pos))
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
                        
                        if(is_tile_traversable_and_not_occupied(dungeon->tiles, enemy->new_pos))
                        {
                        move_entity(&game->random, enemy, dungeon->tiles, ui, enemy->new_pos);
                        }
                        
                        update_entity_status_effects(game, enemy, dungeon, inventory, ui);
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

internal void
render_entities(Game *game,
                EntityState *entities,
                Inventory *inventory,
                 Dungeon *dungeon,
                Assets *assets)
{
        { // Render player
            Entity *player = &entities->array[0];
        assert(is_entity_valid_and_player(player));
        
            v4u dest = get_game_dest(game, player->pos);
            SDL_RenderCopy(game->renderer, assets->tileset.tex, (SDL_Rect *)&player->tile_src, (SDL_Rect *)&dest);
            
            // Render player items
            for(u32 slot_index = 1; slot_index < ItemSlot_Count; ++slot_index)
            {
                for(u32 inventory_index = 0; inventory_index < MAX_INVENTORY_SLOT_COUNT; ++inventory_index)
                {
                    Item *item = inventory->slots[inventory_index];
                    if(is_item_valid_and_in_inventory(item) &&
                       is_set(item->flags, ItemFlags_Equipped) &&
                       item->slot == slot_index)
                    {
                        SDL_RenderCopy(game->renderer, assets->tileset.tex, (SDL_Rect *)&item->equip_tile_src, (SDL_Rect *)&dest);
                        
                        break;
                    }
                }
            }
            
            // Render player pathfind trail
            if(player->p.render_pathfind_trail)
            {
                for(u32 index = 0; index < MAX_PATHFIND_TRAIL_COUNT; ++index)
                {
                    PathfindTrail *trail = &player->p.pathfind_trail[index];
                    if(!is_v2u_zero(trail->pos))
                    {
                        v4u trail_src = {0};
                        
                        switch(trail->direction)
                        {
                            case Direction_Up: trail_src = get_tileset_rect(TileID_FootstepsUp); break;
                            case Direction_Down: trail_src = get_tileset_rect(TileID_FootstepsDown); break;
                            case Direction_Left: trail_src = get_tileset_rect(TileID_FootstepsLeft); break;
                            case Direction_Right: trail_src = get_tileset_rect(TileID_FootstepsRight); break;
                            
                            case Direction_UpLeft: trail_src = get_tileset_rect(TileID_FootstepsUpLeft); break;
                            case Direction_UpRight: trail_src = get_tileset_rect(TileID_FootstepsUpRight); break;
                            case Direction_DownLeft: trail_src = get_tileset_rect(TileID_FootstepsDownLeft); break;
                            case Direction_DownRight: trail_src = get_tileset_rect(TileID_FootstepsDownRight); break;
                            
                            invalid_default_case;
                        }
                        
                        v4u trail_dest = get_game_dest(game, trail->pos);
                        SDL_RenderCopy(game->renderer, assets->tileset.tex, (SDL_Rect *)&trail_src, (SDL_Rect *)&trail_dest);
                    }
                }
            }
        }
        
        { // Render enemies
            for(u32 enemy_index = 0; enemy_index < MAX_ENTITY_COUNT; ++enemy_index)
            {
            Entity *enemy = &entities->array[enemy_index];
            
                if(is_entity_valid_and_enemy(enemy) &&
                enemy->dungeon_level == dungeon->level)
            {
            if(!is_set(enemy->flags, EntityFlags_Invisible))
            {
                if(is_tile_seen(dungeon->tiles, enemy->pos))
                {
                    set(enemy->flags, EntityFlags_HasBeenSeen);
                    unset(enemy->flags, EntityFlags_GhostEnabled);
                    
                    v4u dest = get_game_dest(game, enemy->pos);
                    SDL_RenderCopyEx(game->renderer, assets->tileset.tex, (SDL_Rect *)&enemy->tile_src, (SDL_Rect *)&dest, 0, 0, is_set(enemy->flags, EntityFlags_Flipped));
                    
                    // Additional things to render on enemy tile.
                    if(enemy->e.turns_in_player_view == 1)
                    {
                        v4u status_src = get_tileset_rect(TileID_StatusMark);
                        SDL_RenderCopy(game->renderer, assets->tileset.tex, (SDL_Rect *)&status_src, (SDL_Rect *)&dest);
                    }
                    else if(entity_has_any_status_effect(enemy))
                    {
                        v4u status_src = get_tileset_rect(TileID_QuestionMark);
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
                                v4u dest = get_game_dest(game, enemy->e.ghost_pos);
                                render_texture_half_color(game->renderer, assets->tileset.tex, enemy->tile_src, dest, is_set(enemy->flags, EntityFlags_GhostFlipped));
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
}

internal void
add_player_entity(Random *random, Entity *player)
{
    player->id = EntityID_Player;
    strcpy(player->name, "Name");
    
    player->max_hp = 80;
    player->hp = player->max_hp;
    
    player->w = player->h = 32;
    player->tile_src = get_tile_rect(get_entity_tile_pos(player->id));
    player->remains = EntityRemains_RedBlood;
    player->type = EntityType_Player;
    
    player->strength = 10;
    player->intelligence = 10;
    player->dexterity = 10;
    
    player->evasion = 10;
    player->fov = 8;
    player->p.weight_to_evasion_ratio = 3;
    
    //player->max_hp = 10000000;
    //player->resistances[DamageType_Fire] = 5;
    //player->resistances[DamageType_Poison] = 5;
}

internal Entity *
add_enemy_entity(EntityState *entities,
                 Dungeon *dungeon,
                 EntityID id,
                 u32 x, u32 y)
{
    Entity *result = 0;
    
    for(u32 index = 0; index < MAX_ENTITY_COUNT; ++index)
    {
        Entity *enemy = &entities->array[index];
        if(!enemy->type)
        {
            result = enemy;
            
            enemy->id = id;
            enemy->new_pos = enemy->pos = make_v2u(x, y);
            enemy->dungeon_level = dungeon->level;
            enemy->w = enemy->h = 32;
            enemy->tile_src = get_tile_rect(get_entity_tile_pos(enemy->id));
            enemy->type = EntityType_Enemy;
            enemy->fov = 8;
            enemy->e.level = entities->levels[id];
            
            force_move_entity(enemy, dungeon->tiles, enemy->new_pos);
            
            #if 0
            StatusEffect status_effect = {0};
            status_effect.type = StatusEffectType_Bolster;
            status_effect.value = 2;
            status_effect.chance = 100;
            status_effect.duration = 5;
            start_entity_status_effect(enemy, status_effect);
            
            enemy->resistances[DamageType_Physical] = -5;
            enemy->resistances[DamageType_Fire] = -3;
            enemy->resistances[DamageType_Ice] = -1;
            enemy->resistances[DamageType_Lightning] = -2;
            enemy->resistances[DamageType_Poison] = 1;
            enemy->resistances[DamageType_Holy] = 3;
            enemy->resistances[DamageType_Dark] = 5;
#endif
            
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
                    
                    enemy->e.damage_type = DamageType_Physical;
                    enemy->e.damage = 5;
                    enemy->evasion = 5;
                    enemy->action_count = 1.0f;
                } break;
                
                case EntityID_SkeletonArcher:
                {
                    strcpy(enemy->name, "Skeleton Archer");
                    enemy->max_hp = enemy->hp = 15;
                    
                    enemy->e.damage_type = DamageType_Physical;
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
                    enemy->action_count = 1.0f;
                    
                    set(enemy->flags, EntityFlags_MagicAttacks);
                    add_enemy_spell(enemy, entities->spell_chances, SpellID_DarkBolt);
                    } break;
                
                case EntityID_Bat:
                {
                    strcpy(enemy->name, "Bat");
                    enemy->max_hp = enemy->hp = 14;
                    
                    enemy->e.damage_type = DamageType_Physical;
                    enemy->e.damage = 1;
                    enemy->evasion = 14;
                    enemy->action_count = 3.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_Rat:
                {
                    strcpy(enemy->name, "Rat");
                    enemy->max_hp = enemy->hp = 11;
                    
                    enemy->e.damage_type = DamageType_Physical;
                    enemy->e.damage = 1;
                    enemy->evasion = 12;
                    enemy->action_count = 2.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_KoboldWarrior:
                {
                    strcpy(enemy->name, "Kobold Warrior");
                    enemy->max_hp = enemy->hp = 22;
                    
                    enemy->e.damage_type = DamageType_Physical;
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
                    
                    add_enemy_spell(enemy, entities->spell_chances, SpellID_Bolster);
                    add_enemy_spell(enemy, entities->spell_chances, SpellID_LesserHeal);
                } break;
                
                case EntityID_Snail:
                {
                    strcpy(enemy->name, "Snail");
                    enemy->max_hp = enemy->hp = 28;
                    
                    enemy->e.damage_type = DamageType_Physical;
                    enemy->e.damage = 10;
                    enemy->evasion = 3;
                    enemy->action_count = 0.5f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_Slime:
                {
                    strcpy(enemy->name, "Slime");
                    enemy->max_hp = enemy->hp = 20;
                    
                    enemy->e.damage_type = DamageType_Physical;
                    enemy->e.damage = 4;
                    enemy->evasion = 6;
                    enemy->action_count = 1.0f;
                    enemy->remains = EntityRemains_GreenBlood;
                } break;
                
                case EntityID_Dog:
                {
                    strcpy(enemy->name, "Dog");
                    enemy->max_hp = enemy->hp = 18;
                    
                    enemy->e.damage_type = DamageType_Physical;
                    enemy->e.damage = 3;
                    enemy->evasion = 10;
                    enemy->action_count = 2.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_OrcWarrior:
                {
                    strcpy(enemy->name, "Orc Warrior");
                    enemy->max_hp = enemy->hp = 28;
                    
                    enemy->e.damage_type = DamageType_Physical;
                    enemy->e.damage = 8;
                    enemy->evasion = 10;
                    enemy->action_count = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_OrcArcher:
                {
                    strcpy(enemy->name, "Orc Archer");
                    enemy->max_hp = enemy->hp = 26;
                    
                    enemy->e.damage_type = DamageType_Physical;
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
                    
                    enemy->e.damage_type = DamageType_Physical;
                    enemy->e.damage = 4;
                    enemy->evasion = 11;
                    enemy->action_count = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                    
                    enemy->e.poison.type = StatusEffectType_Poison;
                    enemy->e.poison.value = 2;
                    enemy->e.poison.chance = 33;
                    enemy->e.poison.duration = 8;
                } break;
                
                case EntityID_Shade:
                {
                    strcpy(enemy->name, "Shade");
                    enemy->max_hp = enemy->hp = 18;
                    
                    enemy->e.damage_type = DamageType_Dark;
                    enemy->e.damage = 3;
                    enemy->evasion = 16;
                    enemy->action_count = 1.0f;
                    
                    set(enemy->flags, EntityFlags_MagicAttacks);
                } break;
                
                case EntityID_ElfKnight:
                {
                    strcpy(enemy->name, "Elf Knight");
                    enemy->max_hp = enemy->hp = 32;
                    
                    enemy->e.damage_type = DamageType_Physical;
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
                    
                    enemy->e.damage_type = DamageType_Physical;
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
                    
                    enemy->e.damage_type = DamageType_Dark;
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
                    
                    enemy->e.damage_type = DamageType_Physical;
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
                    
                    enemy->e.damage_type = DamageType_Physical;
                    enemy->e.damage = 14;
                    enemy->evasion = 4;
                    enemy->action_count = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_Treant:
                {
                    strcpy(enemy->name, "Treant");
                    enemy->max_hp = enemy->hp = 40;
                    
                    enemy->e.damage_type = DamageType_Physical;
                    enemy->e.damage = 4;
                    enemy->defence = 6;
                    enemy->evasion = 0;
                    enemy->action_count = 0.5f;
                } break;
                
                case EntityID_Viper:
                {
                    strcpy(enemy->name, "Viper");
                    enemy->max_hp = enemy->hp = 18;
                    
                    enemy->e.damage_type = DamageType_Physical;
                    enemy->e.damage = 4;
                    enemy->evasion = 5;
                    enemy->action_count = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                    
                    enemy->e.poison.type = StatusEffectType_Poison;
                    enemy->e.poison.value = 3;
                    enemy->e.poison.chance = 33;
                    enemy->e.poison.duration = 8;
                } break;
                
                case EntityID_CentaurWarrior:
                {
                    strcpy(enemy->name, "Centaur Warrior");
                    enemy->max_hp = enemy->hp = 35;
                    
                    enemy->e.damage_type = DamageType_Physical;
                    enemy->e.damage = 0;
                    enemy->evasion = 4;
                    enemy->action_count = 2.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_CentaurSpearman:
                {
                    strcpy(enemy->name, "Centaur Spearman");
                    enemy->max_hp = enemy->hp = 35;
                    
                    enemy->e.damage_type = DamageType_Physical;
                    enemy->e.damage = 0;
                    enemy->evasion = 4;
                    enemy->action_count = 2.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_CentaurArcher:
                {
                    strcpy(enemy->name, "Centaur Archer");
                    enemy->max_hp = enemy->hp = 32;
                    
                    enemy->e.damage_type = DamageType_Physical;
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
                    
                    enemy->e.damage_type = DamageType_Physical;
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_count = 1.0f;
                    
                    // TODO(rami): Maybe these attack physically and have a chance
                    // on hit to give you a status effect?
                } break;
                
                case EntityID_Wolf:
                {
                    strcpy(enemy->name, "Wolf");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage_type = DamageType_Physical;
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_count = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_OgreWarrior:
                {
                    strcpy(enemy->name, "Ogre Warrior");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage_type = DamageType_Physical;
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_count = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_OgreArcher:
                {
                    strcpy(enemy->name, "Ogre Archer");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage_type = DamageType_Physical;
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
                    
                    enemy->e.damage_type = DamageType_Physical;
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_count = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_ShadowWalker:
                {
                    strcpy(enemy->name, "Shadow Walker");
                    enemy->max_hp = enemy->hp = 10;
                    
                    enemy->e.damage_type = DamageType_Dark;
                    enemy->e.damage = 5;
                    enemy->evasion = 0;
                    enemy->action_count = 1.0f;
                    
                    set(enemy->flags, EntityFlags_Invisible);
                } break;
                
                case EntityID_DwarwenWarrior:
                {
                    strcpy(enemy->name, "Dwarwen Warrior");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage_type = DamageType_Physical;
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
                    
                    enemy->e.damage_type = DamageType_Physical;
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
                    
                    enemy->e.damage_type = DamageType_Physical;
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
                    
                    enemy->e.damage_type = DamageType_Physical;
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_count = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_IronGolem:
                {
                    strcpy(enemy->name, "Iron Golem");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage_type = DamageType_Physical;
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_count = 1.0f;
                } break;
                
                case EntityID_Griffin:
                {
                    strcpy(enemy->name, "Griffin");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage_type = DamageType_Physical;
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_count = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_Imp:
                {
                    strcpy(enemy->name, "Imp");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage_type = DamageType_Physical;
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_count = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_BlackKnight:
                {
                    strcpy(enemy->name, "Black Knight");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage_type = DamageType_Physical;
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_count = 1.0f;
                } break;
                
                case EntityID_GiantDemon:
                {
                    strcpy(enemy->name, "Giant Demon");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage_type = DamageType_Physical;
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_count = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_Hellhound:
                {
                    strcpy(enemy->name, "Hellhound");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage_type = DamageType_Physical;
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
            
            break;
        }
    }
    
    assert(result);
    return(result);
}