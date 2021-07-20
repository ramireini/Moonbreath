internal Status *
get_active_status(Status *statuses, StatusType type)
{
    Status *result = 0;
    
    for(u32 index = 0; index < MAX_ENTITY_STATUS_COUNT; ++index)
    {
        Status *status = &statuses[index];
        
        if(status->type && status->type == type)
        {
            result = status;
            break;
        }
    }
    
    return(result);
}

internal void
log_add_new_player_view_string(char *single, char *multiple, u32 count, UI *ui)
{
    assert(single);
    assert(multiple);
    assert(count);
    assert(ui);
    
    if(count > 1)
    {
        log_add("New %s come into view!", ui, multiple);
    }
    else
    {
        log_add("A new %s comes into view!", ui, single);
    }
}

internal b32
log_add_player_view_has_enemies(EntityState *entities, Dungeon *dungeon, UI *ui)
{
    assert(entities);
    assert(dungeon);
    assert(ui);
    
    b32 result = false;
    
    u32 new_enemy_count = get_player_view_enemy_count(entities, dungeon);
    if(new_enemy_count)
    {
        result = true;
        log_add_new_player_view_string("enemy", "enemies", new_enemy_count, ui);
    }
    
    return(result);
    }

internal void
update_player_entity_statuses(Game *game, PathfindMap *enemy_pathfind_map, Entity *player, Inventory *inventory, Dungeon *dungeon, UI *ui)
{
    assert(game);
    assert(is_entity_valid(player));
    assert(inventory);
    assert(dungeon);
    assert(ui);
    
    for(u32 count = 0; count < player->action_time; ++count)
    {
        update_entity_statuses(game, enemy_pathfind_map, player, dungeon, inventory, ui);
    }
}

internal void
change_stat(u32 *value, s32 change, b32 is_apply)
{
    assert(*value);
    assert(value);
    assert(change);
    
    if(is_apply)
    {
        *value += change;
    }
    else
    {
        *value += (-change);
    if(is_zero(*value)) *value = 0;
    }
    }

internal b32
update_entity_regen(Entity *entity)
{
    assert(is_entity_valid(entity));
    
    b32 result = false;
    
    if(entity->hp < entity->max_hp)
    {
        EntityRegen *regen = &entity->regen;
        
        assert(regen);
        assert(regen->hp_increase);
        assert(regen->max);
        assert(regen->advance);
        
        regen->current += regen->advance;
        if(regen->current >= regen->max)
        {
            regen->next_turn = false;
            regen->current = 0;
            
            entity->hp += regen->hp_increase;
            if(entity->hp > entity->max_hp) entity->hp = entity->max_hp;
        }
        else if(regen->current + regen->advance >= regen->max)
        {
            regen->next_turn = true;
            if(entity->hp == entity->max_hp - 1) result = true;
            }
    }
    
    return(result);
}

internal void
summon_entity(Random *random, EntityState *entities, Entity *entity, Dungeon *dungeon, UI *ui, u32 summon_flags)
{
    assert(random);
    assert(entities);
    assert(is_entity_valid(entity));
    assert(dungeon);
    assert(ui);
    
    if(summon_flags)
    {
        for(;;)
            {
            EntityInfo summon_info = get_random_enemy_entity_info(random);
            
            if(entity->id != summon_info.id &&
                   is_set(summon_info.flags, summon_flags) &&
                   entity_level_fits_dungeon_level(summon_info.level, dungeon->level))
            {
                v2u summon_pos = get_random_dungeon_traversable_unoccupied_rect_pos(random, dungeon, entity->e.view_rect);
                Entity *summon_enemy = add_enemy_entity(entities, dungeon, summon_info.id, summon_pos.x, summon_pos.y);
                
                log_add("The %s summons a %s", ui, entity->name.s, summon_enemy->name.s);
                break;
            }
        }
    }
    else
    {
        EntityID summon_id = get_random_enemy_id_for_dungeon_level(random, dungeon->level);
        v4u summon_rect = get_dungeon_dimension_rect(dungeon->size, entity->pos, 2);
        v2u summon_pos = get_random_dungeon_traversable_unoccupied_rect_pos(random, dungeon, summon_rect);
        
        add_enemy_entity(entities, dungeon, summon_id, summon_pos.x, summon_pos.y);
        log_add("You hear an odd sound and something appears next to you!", ui);
    }
    }

internal Spell *
set_random_entity_spell(Random *random, Entity *entity)
{
    assert(random);
    assert(is_enemy_entity_valid(entity));
    
    Spell *result = 0;
    
    u32 counter = 0;
    while(counter < 100)
    {
        u32 index = get_random(random, 0, entity->e.spell_count - 1);
        Spell *spell = &entity->e.spells[index];
        
        assert(spell->type);
        assert(spell->chance);
        
        result = spell;
        counter += spell->chance;
    }
    
    entity->e.spell = result;
    
    assert(result);
    return(result);
}

internal b32
is_in_spell_range(v2u start_pos, v2u end_pos, u32 spell_range)
{
    assert(!is_v2u_zero(start_pos));
    assert(!is_v2u_zero(end_pos));
    
    b32 result = (!spell_range || cardinal_and_ordinal_distance(start_pos, end_pos) <= spell_range);
    return(result);
}

internal b32
entity_in_view_and_spell_range(v4u attacker_view, v2u attacker_pos, v2u defender_pos, u32 spell_range)
{
    assert(!is_v4u_zero(attacker_view));
    assert(!is_v2u_zero(attacker_pos));
    assert(!is_v2u_zero(defender_pos));
    
    b32 result = (is_dungeon_pos_inside_rect(defender_pos, attacker_view) &&
                  is_in_spell_range(attacker_pos, defender_pos, spell_range));
    
    return(result);
}

internal void
select_and_cast_entity_spell(Random *random,
                             EntityState *entities,
                             u32 caster_index,
                             Entity *caster,
                             Entity *target,
                             Dungeon *dungeon,
                             UI *ui)
{
    assert(random);
    assert(entities);
    assert(caster_index);
    assert(is_entity_valid(caster));
    assert(is_entity_valid(target));
    assert(dungeon);
    assert(ui);
    
    // We loop until the caster uses a spell
    for(;;)
    {
        Spell *spell = set_random_entity_spell(random, caster);
        
        if(spell->type == StatusType_Damage &&
           entity_in_view_and_spell_range(caster->e.view_rect, caster->pos, target->pos, spell->range))
        {
            attack_entity(random, &entities->enemy_pathfind_map, caster, target, dungeon, ui, spell->damage);
            break;
        }
        else if(spell->type == StatusType_Stat)
        {
            Spell *spell = caster->e.spell;
            
            for(u32 stat_entity_index = 0; stat_entity_index < MAX_ENTITY_COUNT; ++stat_entity_index)
            {
                Entity *stat_entity = &entities->array[stat_entity_index];
                
                if(caster_index != stat_entity_index &&
                   is_entity_valid(stat_entity))
                {
                    // Make sure the target is valid for the spell target type
                    b32 valid_target = false;
                    switch(spell->target_type)
                    {
                        case EntityType_Player:
                        {
                            if(is_player_entity_valid(stat_entity))
                            {
                                valid_target = true;
                            }
                        } break;
                        
                        case EntityType_Enemy:
                        {
                            if(is_enemy_entity_valid(stat_entity) &&
                               is_set(stat_entity->flags, EntityFlag_InCombat))
                            {
                                valid_target = true;
                            }
                        } break;
                        
                        invalid_default_case;
                    }
                    
                    // Make sure the spell is valid for the target stats
                    b32 valid_spell = true;
                    if(spell->type == StatusType_Stat)
                    {
                        switch(spell->stat_type)
                        {
                            case StatType_Str: valid_spell = ((s32)stat_entity->str > 0); break;
                            case StatType_Int: valid_spell = ((s32)stat_entity->intel > 0); break;
                            case StatType_Dex: valid_spell = ((s32)stat_entity->dex > 0); break;
                            case StatType_Def: valid_spell = ((s32)stat_entity->def > 0); break;
                            case StatType_EV: valid_spell = ((s32)stat_entity->ev > 0); break;
                            
                            case StatType_StrIntDex:
                            {
                                valid_spell = ((s32)stat_entity->str > 0 &&
                                                   (s32)stat_entity->intel > 0 &&
                                                   (s32)stat_entity->dex > 0);
                            } break;
                            
                            invalid_default_case;
                        }
                    }
                    
#if 1
                    printf("valid_spell: %u\n", valid_spell);
                    printf("valid_target: %u\n", valid_target);
                    printf("spell->value.min: %d\n", spell->value.min);
                    printf("spell->value.max: %d\n", spell->value.max);
                    printf("spell->target_type: %s\n", get_entity_type_string(spell->target_type));
                    printf("get_total_stat_status_value(): %d\n\n", get_total_stat_status_value(target->statuses, spell->stat_type));
#endif
                    
                    if(valid_target && valid_spell &&
                       entity_in_view_and_spell_range(caster->e.view_rect, caster->pos, stat_entity->pos, spell->range))
                    {
                        cast_entity_spell(random, caster, stat_entity, dungeon, ui);
                        break;
                    }
                }
            }
            
            break;
        }
        else if((spell->type == StatusType_Bind ||
                             spell->type == StatusType_Burn || 
                             spell->type == StatusType_Confusion) &&
                    entity_in_view_and_spell_range(caster->e.view_rect, caster->pos, target->pos, spell->range) &&
                    !get_active_status(target->statuses, spell->type))
        {
            cast_entity_spell(random, caster, target, dungeon, ui);
            break;
            }
        else if(spell->type == StatusType_Summon)
        {
            summon_entity(random, entities, caster, dungeon, ui, spell->summon_flags);
            break;
        }
        }
        
    // Nothing should use this until we set it again.
    caster->e.spell = 0;
}

internal void
log_add_entity_attack_string(Random *random,
                             Entity *attacker,
                             Entity *defender,
                             UI *ui,
                             u32 value,
                             ResistAmount resist_amount)
{
    assert(random);
    assert(is_entity_valid(attacker));
    assert(is_entity_valid(defender));
    
    String256 attack_string = {0};
    String64 attack_text = {0};
    
    if(attacker->type == EntityType_Player)
    {
        Item *player_weapon = attacker->p.weapon;
        
        if(player_weapon)
        {
            switch(player_weapon->id)
            {
                case ItemID_Dagger:
                case ItemID_Sword:
                {
                    switch(get_random(random, 1, 6))
                    {
                        case 1: snprintf(attack_text.s, sizeof(attack_text.s), "stab"); break;
                        case 2: snprintf(attack_text.s, sizeof(attack_text.s), "pierce"); break;
                        case 3: snprintf(attack_text.s, sizeof(attack_text.s), "puncture"); break;
                        case 4: snprintf(attack_text.s, sizeof(attack_text.s), "slash"); break;
                        case 5: snprintf(attack_text.s, sizeof(attack_text.s), "lacerate"); break;
                        case 6: snprintf(attack_text.s, sizeof(attack_text.s), "cleave"); break;
                        
                        invalid_default_case;
                    }
                } break;
                
                case ItemID_Club:
                case ItemID_Warhammer:
                {
                    switch(get_random(random, 1, 6))
                    {
                        case 1: snprintf(attack_text.s, sizeof(attack_text.s), "smash"); break;
                        case 2: snprintf(attack_text.s, sizeof(attack_text.s), "bash"); break;
                        case 3: snprintf(attack_text.s, sizeof(attack_text.s), "strike"); break;
                        case 4: snprintf(attack_text.s, sizeof(attack_text.s), "pummel"); break;
                        case 5: snprintf(attack_text.s, sizeof(attack_text.s), "pound"); break;
                        case 6: snprintf(attack_text.s, sizeof(attack_text.s), "crush"); break;
                        
                        invalid_default_case;
                    }
                } break;
                
                case ItemID_Battleaxe:
                {
                    switch(get_random(random, 1, 6))
                    {
                        case 1: snprintf(attack_text.s, sizeof(attack_text.s), "hack"); break;
                        case 2: snprintf(attack_text.s, sizeof(attack_text.s), "rend"); break;
                        case 3: snprintf(attack_text.s, sizeof(attack_text.s), "chop"); break;
                        case 4: snprintf(attack_text.s, sizeof(attack_text.s), "slash"); break;
                        case 5: snprintf(attack_text.s, sizeof(attack_text.s), "lacerate"); break;
                        case 6: snprintf(attack_text.s, sizeof(attack_text.s), "cleave"); break;
                        
                        invalid_default_case;
                    }
                } break;
                
                case ItemID_Spear:
                {
                    switch(get_random(random, 1, 4))
                    {
                        case 1: snprintf(attack_text.s, sizeof(attack_text.s), "stab"); break;
                        case 2: snprintf(attack_text.s, sizeof(attack_text.s), "pierce"); break;
                        case 3: snprintf(attack_text.s, sizeof(attack_text.s), "puncture"); break;
                        case 4: snprintf(attack_text.s, sizeof(attack_text.s), "strike"); break;
                        
                        invalid_default_case;
                    }
                } break;
                
                invalid_default_case;
            }
        }
        else
        {
            switch(get_random(random, 1, 2))
            {
                case 1: snprintf(attack_text.s, sizeof(attack_text.s), "punch"); break;
                case 2: snprintf(attack_text.s, sizeof(attack_text.s), "kick"); break;
                
                invalid_default_case;
            }
        }
    }
    
    // The rest figures out how to place the parts of the entity attack string.
    b32 is_spell = false;
    Spell *spell = 0;
    if(attacker->type == EntityType_Enemy)
    {
        spell = attacker->e.spell;
        if(spell)
        {
            is_spell = true;
                snprintf(attack_text.s, sizeof(attack_text.s), "casts %s on", spell->name.s);
        }
        else if(!attack_text.s[0])
        {
            if(is_set(attacker->flags, EntityFlag_UsesRangedAttacks))
            {
                snprintf(attack_text.s, sizeof(attack_text.s), "fires an arrow at");
            }
            else
            {
                snprintf(attack_text.s, sizeof(attack_text.s), "attacks");
            }
        }
    }
    
    String64 attacker_name = {0};
    if(is_set(attacker->flags, EntityFlag_Invisible))
    {
        snprintf(attacker_name.s, sizeof(attacker_name.s), "Something");
    }
    else
    {
        if(attacker->type == EntityType_Player)
        {
            snprintf(attacker_name.s, sizeof(attacker_name.s), "You");
        }
        else
        {
            snprintf(attacker_name.s, sizeof(attacker_name.s), "The %s", attacker->name.s);
        }
    }
    
    String64 defender_name = {0};
    if(is_set(defender->flags, EntityFlag_Invisible))
    {
        if(defender->type == EntityType_Player)
        {
            snprintf(defender_name.s, sizeof(defender_name.s), "you");
        }
        else
        {
            snprintf(defender_name.s, sizeof(defender_name.s), "the Something");
        }
    }
    else
    {
        if(defender->type == EntityType_Player)
        {
            snprintf(defender_name.s, sizeof(defender_name.s), "you");
        }
        else
        {
            snprintf(defender_name.s, sizeof(defender_name.s), "the %s", defender->name.s);
        }
    }
    
    if(resist_amount == ResistAmount_Partial)
    {
        snprintf(attack_string.s, sizeof(attack_string.s), "%s %s %s, it resists some of the damage, %u damage.", attacker_name.s, attack_text.s, defender_name.s, value);
    }
    else if(resist_amount == ResistAmount_Full)
    {
        snprintf(attack_string.s, sizeof(attack_string.s), "%s %s %s, it resists all the damage!", attacker_name.s, attack_text.s, defender_name.s);
    }
    else
    {
        if(is_spell && spell->type != StatusType_Damage)
        {
            snprintf(attack_string.s, sizeof(attack_string.s), "%s %s %s.", attacker_name.s, attack_text.s, defender_name.s);
        }
        else
        {
        snprintf(attack_string.s, sizeof(attack_string.s), "%s %s %s, %u damage.", attacker_name.s, attack_text.s, defender_name.s, value);
        }
        }
    
    log_add(attack_string.s, ui);
}

internal void
log_add_entity_resistance_action(Entity *entity, UI *ui, ResistAmount resist_amount)
{
    assert(is_entity_valid(entity));
    assert(ui);
    
    String32 text = {0};
    
    switch(resist_amount)
    {
        case ResistAmount_Partial: strcpy(text.s, "partially"); break;
        case ResistAmount_Full: strcpy(text.s, "fully"); break;
        
        invalid_default_case;
    }
    
    if(entity->type == EntityType_Player)
    {
        log_add("%sYou %s resist the attack!", ui, start_color(Color_LightGray), text.s);
    }
    else
    {
        assert(entity->type == EntityType_Enemy);
        log_add("%sThe %s %s resists the attack!", ui, start_color(Color_LightGray), entity->name.s, text.s);
    }
}

internal void
place_entity_remains(Random *random, Entity *entity, Dungeon *dungeon, EntityRemainsType type)
{
    assert(random);
    assert(is_entity_valid(entity));
    assert(dungeon);
    assert(type);
    
    v2u remains_pos = {0};
    DungeonTileID remains = 0;
    
    if(entity->remains)
    {
        if(type == EntityRemainsType_Wound)
        {
            Direction direction = get_random_direction(random);
            remains_pos = get_direction_pos(entity->pos, direction);
            
            if(can_place_dungeon_remains_on_pos(dungeon, remains_pos))
            {
                //if(entity->type == EntityType_Enemy) printf("Wound, %s\n", get_direction_string(direction));
                
                if(is_dungeon_pos_wall(dungeon->tiles, remains_pos))
                { 
                    switch(entity->remains)
                    {
                        case EntityRemains_RedBlood:
                        {
                            if(direction == Direction_Up)
                            {
                                remains= get_random(random,
                                                    DungeonTileID_RedBloodWallUp1,
                                                    DungeonTileID_RedBloodWallUp3);
                            }
                            else if(direction == Direction_Down)
                            {
                                remains = get_random(random,
                                                     DungeonTileID_RedBloodWallDown1,
                                                     DungeonTileID_RedBloodWallDown3);
                            }
                            else if(direction == Direction_Left)
                            {
                                remains = get_random(random,
                                                     DungeonTileID_RedBloodWallLeft1,
                                                     DungeonTileID_RedBloodWallLeft3);
                            }
                            else if(direction == Direction_Right)
                            {
                                remains = get_random(random,
                                                     DungeonTileID_RedBloodWallRight1,
                                                     DungeonTileID_RedBloodWallRight3);
                            }
                        } break;
                        
                        case EntityRemains_GreenBlood:
                        {
                            if(direction == Direction_Up)
                            {
                                remains = get_random(random,
                                                     DungeonTileID_GreenBloodWallUp1,
                                                     DungeonTileID_GreenBloodWallUp3);
                            }
                            else if(direction == Direction_Down)
                            {
                                remains = get_random(random,
                                                     DungeonTileID_GreenBloodWallDown1,
                                                     DungeonTileID_GreenBloodWallDown3);
                            }
                            else if(direction == Direction_Left)
                            {
                                remains = get_random(random,
                                                     DungeonTileID_GreenBloodWallLeft1,
                                                     DungeonTileID_GreenBloodWallLeft3);
                            }
                            else if(direction == Direction_Right)
                            {
                                remains = get_random(random,
                                                     DungeonTileID_GreenBloodWallRight1,
                                                     DungeonTileID_GreenBloodWallRight3);
                            }
                        } break;
                        
                        invalid_default_case;
                    }
                }
                else
                {
                    assert(is_dungeon_pos_traversable(dungeon->tiles, remains_pos));
                    
                    switch(entity->remains)
                    {
                        case EntityRemains_RedBlood:
                        {
                            remains = get_random(random,
                                                 DungeonTileID_RedBloodGroundSmall1,
                                                 DungeonTileID_RedBloodGroundSmall3);
                        } break;
                        
                        case EntityRemains_GreenBlood:
                        {
                            remains = get_random(random,
                                                 DungeonTileID_GreenBloodGroundSmall1,
                                                 DungeonTileID_GreenBloodGroundSmall3);
                        } break;
                        
                        invalid_default_case;
                    }
                }
            }
        }
        else
        {
            assert(type == EntityRemainsType_Kill);
            assert(is_dungeon_pos_traversable(dungeon->tiles, entity->pos));
            
            if(can_place_dungeon_remains_on_pos(dungeon, entity->pos))
            {
                remains_pos = entity->pos;
                
                switch(entity->remains)
                {
                    case EntityRemains_RedBlood:
                    {
                        remains = get_random(random,
                                             DungeonTileID_RedBloodGroundMedium1,
                                             DungeonTileID_RedBloodGroundLarge2);
                    } break;
                    
                    case EntityRemains_GreenBlood:
                    {
                        remains = get_random(random,
                                             DungeonTileID_GreenBloodGroundMedium1,
                                             DungeonTileID_GreenBloodGroundLarge2);
                    } break;
                    
                    invalid_default_case;
                }
            }
        }
        
        if(remains) set_dungeon_pos_remains(dungeon->tiles, remains_pos, remains);
    }
    }

internal b32
is_entity_evasion_valid(u32 evasion)
{
    b32 result = (evasion >= 0 && evasion <= 21);
    return(result);
}

internal b32
is_damage_valid(DamageInfo damage)
{
    b32 result = (damage.min &&
                  damage.max &&
                  damage.type);
    
    return(result);
}

internal b32
is_spell_valid(Spell *spell)
{
    b32 result = (spell && spell->type);
    return(result);
}

internal b32
is_status_valid(Status *status)
{
    b32 result = (status && status->type);
    return(result);
}

internal b32
is_resistance_valid(s32 value)
{
    b32 result = (value >= -5 && value <= 5);
    return(result);
}

internal b32
entity_can_see_pos(DungeonTiles tiles, v4u view_rect, v2u pos, v2u target)
{
    b32 result = true;
    
    assert(!is_v2u_zero(pos));
    assert(!is_v4u_zero(view_rect));
    assert(!is_v2u_zero(target));
    assert(!is_v2u_equal(pos, target));
    
    if(is_pos_inside_dungeon_rect(target, view_rect))
    {
        // Bresenham's line algorithm
        s32 dx = absolute(target.x - pos.x);
        s32 sx = pos.x < target.x ? 1 : -1;
        
        s32 dy = absolute(target.y - pos.y);
        s32 sy = pos.y < target.y ? 1 : -1;
        
        s32 error = (dx > dy ? dx : -dy) / 2;
        s32 error_two = 0;
        
        for(;;)
        {
            //printf("pos: %u, %u\n", pos.x, pos.y);
            
            error_two = error;
            if(error_two > -dx)
            {
                error -= dy;
                pos.x += sx;
            }
            
            if(error_two < dy)
            {
                error += dx;
                pos.y += sy;
            }
            
            if(is_v2u_equal(pos, target))
            {
                break;
            }
            else if(!is_dungeon_pos_traversable(tiles, pos) ||
                    is_dungeon_pos_occupied(tiles, pos))
            {
                assert(!is_v2u_equal(pos, target));
                
                result = false;
                break;
            }
            
        }
    }
    else
    {
        result = false;
    }
        
    return(result);
}

internal EntityInfo
get_random_enemy_entity_info(Random *random)
{
    assert(random);
    
    EntityID enemy_id = get_random_enemy_entity_id(random);
    EntityInfo result = get_entity_info(enemy_id);
    
    return(result);
}

internal u32
get_entity_level(EntityID id)
{
    u32 result = 0;
    
    switch(id)
    {
        case EntityID_Dummy: result = 1; break;
        case EntityID_SkeletonWarrior: result = 1; break;
        case EntityID_SkeletonArcher: result = 1; break;
        case EntityID_SkeletonMage: result = 1; break;
        case EntityID_Bat: result = 1; break;
        case EntityID_Rat: result = 1; break;
        
        case EntityID_KoboldWarrior: result = 2; break;
        case EntityID_KoboldShaman: result = 2; break;
        case EntityID_Snail: result = 2; break;
        case EntityID_Slime: result = 2; break;
        case EntityID_Dog: result = 2; break;
        
        case EntityID_OrcWarrior: result = 3; break;
        case EntityID_OrcArcher: result = 3; break;
        case EntityID_OrcShaman: result = 3; break;
        case EntityID_Python: result = 3; break;
        case EntityID_Shade: result = 3; break;
        
        case EntityID_ElfKnight: result = 4; break;
        case EntityID_ElfArbalest: result = 4; break;
        case EntityID_ElfMage: result = 4; break;
        case EntityID_GiantSlime: result = 4; break;
        case EntityID_Spectre: result = 4; break;
        
        case EntityID_OrcAssassin: result = 5; break;
        case EntityID_OrcSorcerer: result = 5; break;
        case EntityID_Minotaur: result = 5; break;
        case EntityID_Treant: result = 5; break;
        case EntityID_Viper: result = 5; break;
        
        case EntityID_CentaurWarrior: result = 6; break;
        case EntityID_CentaurSpearman: result = 6; break;
        case EntityID_CentaurArcher: result = 6; break;
        case EntityID_CursedSkull: result = 6; break;
        case EntityID_Wolf: result = 6; break;
        
        case EntityID_OgreWarrior: result = 7; break;
        case EntityID_OgreArcher: result = 7; break;
        case EntityID_OgreMage: result = 7; break;
        case EntityID_Cyclops: result = 7; break;
        case EntityID_ShadowWalker: result = 7; break;
        
        case EntityID_DwarfKnight: result = 8; break;
        case EntityID_DwarfSoldier: result = 8; break;
        case EntityID_DwarfTinkerer: result = 8; break;
        case EntityID_ScarletSnake: result = 8; break;
        case EntityID_Lich: result = 8; break;
        
        case EntityID_AbyssalFiend: result = 9; break;
        case EntityID_BloodTroll: result = 9; break;
        case EntityID_IronGolem: result = 9; break;
        case EntityID_Griffin: result = 9; break;
        case EntityID_Imp: result = 9; break;
        
        case EntityID_BlackKnight: result = 10; break;
        case EntityID_GiantDemon: result = 10; break;
        case EntityID_Hellhound: result = 10; break;
        case EntityID_AbyssalHexmaster: result = 10; break;
        case EntityID_Zarimahar: result = 10; break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal u32
get_entity_flags(EntityID id)
{
    u32 result = 0;
    
    switch(id)
    {
        case EntityID_SkeletonWarrior:
        {
            set(result, EntityFlag_Undead);
        } break;
        
        case EntityID_SkeletonArcher:
        {
            set(result, EntityFlag_Undead);
            set(result, EntityFlag_UsesRangedAttacks);
        } break;
        
        case EntityID_SkeletonMage:
        {
            set(result, EntityFlag_Undead);
        } break;
        
        case EntityID_Bat:
        {
        } break;
        
        case EntityID_Rat:
        {
        } break;
        
        case EntityID_KoboldWarrior:
        {
        } break;
        
        case EntityID_KoboldShaman:
        {
        } break;
        
        case EntityID_Snail:
        {
        } break;
        
        case EntityID_Slime:
        {
        } break;
        
        case EntityID_Dog:
        {
        } break;
        
        case EntityID_OrcWarrior:
        {
        } break;
        
        case EntityID_OrcArcher:
        {
            set(result, EntityFlag_UsesRangedAttacks);
        } break;
        
        case EntityID_OrcShaman:
        {
        } break;
        
        case EntityID_Python:
        {
        } break;
        
            case EntityID_Shade:
        {
            set(result, EntityFlag_Undead);
        } break;
        
        case EntityID_ElfKnight:
        {
        } break;
        
        case EntityID_ElfArbalest:
        {
            set(result, EntityFlag_UsesRangedAttacks);
        } break;
        
        case EntityID_ElfMage:
        {
        } break;
        
        case EntityID_GiantSlime:
        {
        } break;
        
        case EntityID_Spectre:
        {
            set(result, EntityFlag_Undead);
            set(result, EntityFlag_UsesRangedAttacks);
        } break;
        
        case EntityID_OrcAssassin:
        {
        } break;
        
        case EntityID_OrcSorcerer:
        {
        } break;
        
        case EntityID_Minotaur:
        {
        } break;
        
        case EntityID_Treant:
        {
        } break;
        
        case EntityID_Viper:
        {
        } break;
        
        case EntityID_CentaurWarrior:
        {
        } break;
        
        case EntityID_CentaurSpearman:
        {
        } break;
        
        case EntityID_CentaurArcher:
        {
            set(result, EntityFlag_UsesRangedAttacks);
        } break;
        
        case EntityID_CursedSkull:
        {
        } break;
        
        case EntityID_Wolf:
        {
        } break;
        
        case EntityID_OgreWarrior:
        {
        } break;
        
        case EntityID_OgreArcher:
        {
            set(result, EntityFlag_UsesRangedAttacks);
        } break;
        
        case EntityID_OgreMage:
        {
        } break;
        
        case EntityID_Cyclops:
        {
        } break;
        
        case EntityID_ShadowWalker:
        {
            set(result, EntityFlag_Undead);
            set(result, EntityFlag_UsesRangedAttacks);
        } break;
        
        case EntityID_DwarfKnight:
        {
        } break;
        
        case EntityID_DwarfSoldier:
        {
        } break;
        
        case EntityID_DwarfTinkerer:
        {
        } break;
        
        case EntityID_ScarletSnake:
        {
        } break;
        
        case EntityID_Lich:
        {
            set(result, EntityFlag_Undead);
        } break;
        
        case EntityID_AbyssalFiend:
        {
            set(result, EntityFlag_UsesRangedAttacks);
        } break;
        
        case EntityID_BloodTroll:
        {
        } break;
        
        case EntityID_IronGolem:
        {
        } break;
        
        case EntityID_Griffin:
        {
        } break;
        
        case EntityID_Imp:
        {
        } break;
        
        case EntityID_BlackKnight:
        {
        } break;
        
        case EntityID_GiantDemon:
        {
        } break;
        
        case EntityID_Hellhound:
        {
        } break;
        
        case EntityID_AbyssalHexmaster:
        {
        } break;
        
        case EntityID_Zarimahar:
        {
        } break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal EntityID
get_random_enemy_entity_id(Random *random)
{
    assert(random);
    
    EntityID result = get_random(random, ENEMY_ID_START, ENEMY_ID_END);
    return(result);
}

internal EntityInfo
get_entity_info(EntityID id)
{
    assert(id);
    
    EntityInfo result = {0};
    result.id = id;
    result.level = get_entity_level(id);
    result.flags = get_entity_flags(id);
    
    return(result);
}

internal b32
is_entity_under_status(Entity *entity, StatusType type)
{
    assert(is_entity_valid(entity));
    assert(type);
    
    b32 result = false;
    
    for(u32 index = 0; index < MAX_ENTITY_STATUS_COUNT; ++index)
    {
        Status *status = &entity->statuses[index];
        
        if(status->type == type)
        {
            result = true;
            break;
        }
    }
    
    return(result);
}

internal b32
does_entity_have_any_status(Entity *entity)
{
    assert(is_entity_valid(entity));
    
    b32 result = false;
    
    for(u32 index = 0; index < MAX_ENTITY_STATUS_COUNT; ++index)
    {
        Status *status = &entity->statuses[index];
        
        if(status->type)
        {
            result = true;
            break;
        }
    }
    
    return(result);
}

internal void
attack_entity_with_status(Game *game, PathfindMap *enemy_pathfind_map, Entity *entity, Dungeon *dungeon, UI *ui, Status *status)
{
    assert(game);
    assert(is_entity_valid(entity));
    assert(dungeon);
    assert(ui);
    assert(is_status_valid(status));
    assert(status->chance);
    assert(status->duration);
    
        assert(status->player_active.s[0]);
        assert(status->player_active_color);
        
        u32 damage = attack_entity(&game->random, enemy_pathfind_map, 0, entity, dungeon, ui, status->damage);
        
        log_add(status->player_active.s, ui,
                start_color(status->player_active_color),
                damage);
    }

internal b32
end_inventory_item_interact(Inventory *inventory)
{
    assert(inventory);
    
    // This is used by render_inventory() to update the inventory view after an interaction has ended
    inventory->update_view_after_interact = true;
    inventory->interact_type = InteractType_None;
}

internal void
process_inventory_item_interact(Random *random, Inventory *inventory, Item *item, UI *ui)
{
    assert(random);
    assert(inventory);
    assert(item);
    assert(ui);
    
    switch(inventory->interact_type)
    {
        case InteractType_Identify:
        {
            assert(!is_set(item->flags, ItemFlag_IsIdentified));
            
            set(item->flags, ItemFlag_IsIdentified);
            log_add("You identify the %s%s.", ui, get_item_status_color(&item->flags, item->rarity),
                    get_full_item_name(item).s);
        } break;
        
        case InteractType_EnchantWeapon:
        {
            ++item->enchant_level;
            
            switch(get_random(random, 1, 4))
            {
                case 1: log_add("%sThe %s glows blue for a moment..", ui, start_color(Color_LightBlue), get_full_item_name(item).s); break;
                case 2: log_add("%sThe %s seems sharper than before..", ui, start_color(Color_LightBlue), get_full_item_name(item).s); break;
                case 3: log_add("%sThe %s vibrates slightly..", ui, start_color(Color_LightBlue), get_full_item_name(item).s); break;
                case 4: log_add("%sThe %s starts shimmering..", ui, start_color(Color_LightBlue), get_full_item_name(item).s); break;
                
                invalid_default_case;
            }
        } break;
        
        case InteractType_EnchantArmor:
        {
            ++item->enchant_level;
            
            switch(get_random(random, 1, 4))
            {
                case 1: log_add("%sThe %s glows white for a moment..", ui, start_color(Color_LightBlue), get_full_item_name(item).s); break;
                case 2: log_add("%sThe %s looks sturdier than before..", ui, start_color(Color_LightBlue), get_full_item_name(item).s); break;
                case 3: log_add("%sThe %s feels warm for a moment..", ui, start_color(Color_LightBlue), get_full_item_name(item).s); break;
                case 4: log_add("%sThe %s feels different than before..", ui, start_color(Color_LightBlue), get_full_item_name(item).s); break;
                
                invalid_default_case;
            }
        } break;
        
        case InteractType_Uncurse:
        {
            assert(is_set(item->flags, ItemFlag_IsIdentified));
            
            unset(item->flags, ItemFlag_IsCursed);
                log_add("The %s feels slightly different now.", ui, get_full_item_name(item).s);
            } break;
        
        invalid_default_case;
    }
    
    end_inventory_item_interact(inventory);
}

internal void
set_view_depeding_on_scrolling(View *view)
{
    assert(view);
    
    if(is_view_scrolling(*view))
    {
        // Set view at end if we're scrolling
        set_view_at_end(view);
    }
    else
    {
        // Set view at start if we aren't scrolling
        set_view_at_start_and_reset_view_moves(view);
    }
}

internal void
log_add_enemies_nearby(UI *ui)
{
    assert(ui);
    log_add("There are enemies nearby!", ui);
}

internal b32
can_player_auto_explore(u32 *examine_flags, EntityState *entities, Dungeon *dungeon, UI *ui)
{
    assert(examine_flags);
    assert(entities);
    assert(dungeon);
    assert(ui);
    
    b32 result = true;
    
    if(get_player_view_enemy_count(entities, dungeon))
    {
        result = false;
        
        log_add_enemies_nearby(ui);
        unset(*examine_flags, ExamineFlag_Open); // We want this unset if we came here through the examine mode.
        }
    
    return(result);
}

internal void
add_entity_attack_status(Entity *entity,
                        StatusType type,
                        u32 value,
                        u32 duration,
                        u32 chance)
{
    assert(is_enemy_entity_valid(entity));
    assert(type);
    assert(duration);
    assert(is_chance_valid(chance));
    
    Status *status = &entity->e.attack_status;
    assert(!is_status_valid(status));
    
    switch(type)
    {
        case StatusType_Poison:
        {
            assert(value);
            status->value.type = DamageType_Poison;
        } break;
        
        case StatusType_Burn:
        {
            assert(value);
            status->value.type = DamageType_Fire;
        } break;
        
        case StatusType_Bleed:
        {
            assert(value);
            status->value.type = DamageType_Physical;
        } break;
        
        case StatusType_Sightless: assert(!value); break;
        case StatusType_BrokenArmor: assert(!value); break;
        
        invalid_default_case;
    }
    
    status->type = type;
    status->value.min = 1;
    status->value.max = value;
    status->chance = chance;
    status->duration = duration;
    }

internal void
start_examine_from_owner(Examine *examine, Owner *owner)
{
    assert(examine);
    assert(owner);
    
    set(examine->flags, ExamineFlag_Open);
    unset(examine->flags, ExamineFlag_CameraOnExamine);
    
    switch(owner->type)
    {
        case OwnerType_Item:
        {
            examine->type = ExamineType_Item;
            examine->item = owner->item;
        } break;
        
        case OwnerType_Entity:
        {
            examine->type = ExamineType_Entity;
            examine->entity = owner->entity;
        } break;
        
        case OwnerType_Trap:
        {
            examine->type = ExamineType_Trap;
            examine->trap = owner->trap;
        } break;
        
        invalid_default_case;
    }
}

internal void
reset_inventory_multiple_pickup(ItemState *items,
                                u32 *inventory_flags,
                                u32 dungeon_level,
                                Owner *temp_owners)
{
    assert(items);
    assert(inventory_flags);
    assert(is_dungeon_level_valid(dungeon_level));
    assert(temp_owners);
    
    reset_all_owner_select_letters(temp_owners);
    unset_item_selections(items, dungeon_level);
    unset(*inventory_flags, InventoryFlag_MultiplePickup);
}

internal char *
get_status_type_string(StatusType type)
{
    assert(type);
    
    char *result = 0;
    
    switch(type)
    {
        case StatusType_Damage: result = "Damage"; break;
        case StatusType_Heal: result = "Heal"; break;
        case StatusType_Stat: result = "Stat"; break;
        case StatusType_Poison: result = "Poison"; break;
        case StatusType_Burn: result = "Burn"; break;
        case StatusType_Bind: result = "Bind"; break;
        case StatusType_Bleed: result = "Bleed"; break;
        case StatusType_BrokenArmor: result = "Broken Armor"; break;
        case StatusType_Sightless: result = "Sightless"; break;
        case StatusType_Confusion: result = "Confusion"; break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal char *
get_stat_type_string(StatType type)
{
    assert(type);
    
    char *result = 0;
    
    switch(type)
    {
        case StatType_Str: result = "Str"; break;
        case StatType_Int: result = "Int"; break;
        case StatType_Dex: result = "Dex"; break;
        case StatType_Def: result = "Def"; break;
        case StatType_EV: result = "EV"; break;
        case StatType_StrIntDex: result = "StrIntDex"; break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal void
set_flag_if_player_is_not_pathfinding(u32 player_flags, u32 *flags, u32 flag)
{
    // Pathfinding needs to be able to set HasBeenSeen manually
    if(!is_set(player_flags, EntityFlag_Pathfinding))
    {
        set(*flags, flag);
    }
}

internal char *
get_entity_type_string(EntityType type)
{
    char *result = 0;
    
    switch(type)
    {
        case EntityType_Player: result = "Player"; break;
        case EntityType_Enemy: result = "Enemy"; break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal char *
get_entity_remains_string(EntityRemains type)
{
    char *result = 0;
    
    switch(type)
    {
        case EntityRemains_RedBlood: result = "Red Blood"; break;
        case EntityRemains_GreenBlood: result = "Green Blood"; break;
        
        invalid_default_case;
    }
    
    return(result);
    }

internal void
force_move_entity(PathfindMap *enemy_pathfind_map, Entity *entity, Dungeon *dungeon, v2u pos, u32 dungeon_level)
{
    set_dungeon_pos_occupied(dungeon->tiles, entity->pos, false);
    
    entity->pos = pos;
    entity->new_pos = pos;
    entity->dungeon_level = dungeon_level;
    
    set_dungeon_pos_occupied(dungeon->tiles, entity->pos, true);
    
    if(entity->type == EntityType_Player)
    {
        update_player_view(entity, dungeon);
        
        // Teleporting the player and other situations demand that this happens here.
        update_pathfind_map(dungeon, enemy_pathfind_map, entity->pos);
        }
    else
    {
        assert(entity->type == EntityType_Enemy);
        entity->e.view_rect = get_dungeon_dimension_rect(dungeon->size, pos, entity->view_range);
    }
}

internal b32
is_pos_seen_and_flag_not_set(DungeonTiles tiles, v2u pos, u32 flags, u32 flag)
{
    b32 result = (is_tile_seen(tiles, pos) &&
                      !is_set(flags, flag));
    
    return(result);
}

internal u32
get_player_view_new_dungeon_trap_count(Dungeon *dungeon)
{
    u32 result = false;
    
    DungeonTraps *traps = &dungeon->traps;
    
    for(u32 index = 0; index < traps->count; ++index)
    {
         DungeonTrap *trap = &traps->array[index];
        
        if(is_pos_seen_and_flag_not_set(dungeon->tiles, trap->pos, trap->flags,
                                        DungeonTrapFlag_HasBeenSeen))
        {
            set(trap->flags, DungeonTrapFlag_HasBeenSeen);
            ++result;
        }
    }
    
    return(result);
}

internal u32
get_player_view_enemy_count(EntityState *entities, Dungeon *dungeon)
{
    assert(entities);
    assert(dungeon);
    
    u32 result = false;
    
    for(u32 index = 0; index < MAX_ENTITY_COUNT; ++index)
    {
        Entity *entity = &entities->array[index];
        
        if(is_enemy_entity_valid(entity) &&
           is_tile_seen(dungeon->tiles, entity->pos))
        {
            ++result;
        }
    }
    
    return(result);
}

internal void
teleport_entity(Random *random,
                PathfindMap *enemy_pathfind_map,
                Entity *entity,
                Dungeon *dungeon,
                UI *ui,
                DungeonRandomPosType type)
{
    assert(random);
    assert(is_entity_valid(entity));
    assert(dungeon);
    assert(ui);
    
    v2u pos = {0};
    switch(type)
    {
        case DungeonRandomPosType_Traversable: pos = get_random_dungeon_traversable_unoccupied_pos(random, dungeon); break;
        
        case DungeonRandomPosType_TraversableRect:
        {
            assert(entity->type == EntityType_Enemy);
            pos = get_random_dungeon_traversable_unoccupied_rect_pos(random, dungeon, entity->e.view_rect); 
        } break;
        
        invalid_default_case;
    }
    
    force_move_entity(enemy_pathfind_map, entity, dungeon, pos, dungeon->level);
    }

internal ResistInfo
get_resisted_entity_damage(Entity *entity, UI *ui, u32 damage, DamageType damage_type)
{
    assert(is_entity_valid(entity));
    assert(ui);
    assert(damage);
    assert(damage_type);
    
    ResistInfo result = {0};
    result.damage_after = damage;
    
    s32 resistance = entity->resistances[damage_type];
    if(resistance)
    {
        assert(is_resistance_valid(resistance));
        
        f32 resistance_percentage = (f32)resistance * 0.20f;
        u32 damage_change = (u32)((f32)damage * resistance_percentage);
        
        // Sometimes damage and resistance are low and damage_change will be zero, this is when we'll set
        // it to a minimum.
        if(is_zero(damage_change)) damage_change = 1;
        
            damage -= damage_change;
            
#if 0
            printf("\nDamage: %u (%s)\n", result, get_damage_type_string(damage_type));
            printf("Defender Resistance: %dp (%.01f%%)\n", resistance, resistance_percentage * 100.0f);
            printf("New Damage: %d (Old Damage: %d old, Change: %d)\n\n", result - damage_change, result, damage_change);
#endif
        
        result.damage_after = damage;
            if(is_zero(damage))
        {
            result.amount = ResistAmount_Full;
            }
            else
        {
            result.amount = ResistAmount_Partial;
            }
        }
    
    return(result);
}

internal b32
is_entity_immune(s32 *resistances, DamageType damage_type)
{
    assert(resistances);
    assert(damage_type);
    
    b32 result = (resistances[damage_type] == 5);
    return(result);
}

internal void
reset_player_pathfind_trail(Entity *player)
{
    assert(is_player_entity_valid(player));
    
    player->p.render_pathfind_trail = false;
    zero_array(player->p.pathfind_trail, MAX_PATHFIND_TRAIL_COUNT);
}

internal s32
get_total_stat_status_value(Status *statuses, StatType type)
{
    assert(statuses);
    assert(type);
    
    s32 result = 0;
    
    for(u32 index = 0; index < MAX_ENTITY_STATUS_COUNT; ++index)
    {
        Status *status = &statuses[index];
        
        if(status->type == StatusType_Stat &&
               status->stat_type == type)
        {
            assert(status->value.max);
            result += status->value.max;
        }
    }
    
    return(result);
}

internal b32
update_player_pathfind(Entity *player,
                       EntityState *entities,
                       ItemState *items,
                       Dungeon *dungeon,
                       UI *ui)
{
    b32 result = true;
    
    u32 new_item_count = get_player_view_new_item_count(items, dungeon);
    if(new_item_count)
    {
        result = false;
        log_add_new_player_view_string("item", "items", new_item_count, ui);
        }
    
    u32 new_dungeon_trap_count = get_player_view_new_dungeon_trap_count(dungeon);
    if(new_dungeon_trap_count)
    {
        result = false;
        log_add_new_player_view_string("trap", "traps", new_dungeon_trap_count, ui);
    }
    
    if(log_add_player_view_has_enemies(entities, dungeon, ui)) result = false;
    
    if(result)
    {
        PathfindPosInfo pathfind = get_pathfind_result(&entities->player_pathfind_map, dungeon, player->pos, player->pathfind_target_pos);
        assert(pathfind.is_valid);
        
            #if 0
            printf("pathfind.pos: %u, %u\n", pathfind.pos.x, pathfind.pos.y);
            printf("pathfind target: %u, %u\n\n", player->pathfind_target_pos.x, player->pathfind_target_pos.y);
#endif
            
            player->new_pos = pathfind.pos;
            
            // Reached pathfind target
            if(is_v2u_equal(player->new_pos, player->pathfind_target_pos))
            {
                unset(player->flags, EntityFlag_Pathfinding);
            }
            
            // Set new pathfind trail
            if(is_dungeon_pos_traversable(dungeon->tiles, player->new_pos))
            {
                player->p.render_pathfind_trail = true;
                
                for(u32 trail_index = 0; trail_index < MAX_PATHFIND_TRAIL_COUNT; ++trail_index)
                {
                    PathfindTrail *trail = &player->p.pathfind_trail[trail_index];
                    if(is_v2u_zero(trail->pos))
                    {
                        assert(!is_v2u_equal(player->new_pos, player->pos));
                        
                        trail->direction = get_direction_moved_from(player->new_pos, player->pos);
                        trail->pos = player->pos;
                        
                        break;
                    }
                }
            }
    }
    else
    {
        unset(player->flags, EntityFlag_Pathfinding);
    }
    
    return(result);
    }

internal b32
is_entity_valid(Entity *entity)
{
    b32 result = (entity &&
        entity->id &&
        entity->type &&
        is_dungeon_level_valid(entity->dungeon_level));
    
    return(result);
}

internal b32
is_player_entity_valid(Entity *entity)
{
    b32 result = (is_entity_valid(entity) && entity->type == EntityType_Player);
    return(result);
}

internal b32
is_enemy_entity_valid(Entity *entity)
{
    b32 result = (is_entity_valid(entity) && entity->type == EntityType_Enemy);
    return(result);
}

 internal u32
get_enemy_turn_count(f32 *available_time, f32 action_time)
{
    assert(available_time);
    assert(action_time);
    
    u32 result = *available_time / action_time;
    *available_time -= action_time;
    
    return(result);
}

internal u32
get_dungeon_pos_entity_count(EntityState *entities, u32 dungeon_level, v2u pos, b32 enemy_only)
{
    assert(entities);
    assert(is_dungeon_level_valid(dungeon_level));
    assert(!is_v2u_zero(pos));
    
    u32 result = 0;
    
    for(u32 index = 0; index < MAX_ENTITY_COUNT; ++index)
    {
        Entity *entity = &entities->array[index];
        if(enemy_only && !is_enemy_entity_valid(entity)) continue;
        
        if(is_entity_valid(entity) &&
           entity->dungeon_level == dungeon_level &&
           is_v2u_equal(entity->pos, pos))
        {
            ++result;
        }
    }
    
    return(result);
}

internal Entity *
get_dungeon_pos_entity(EntityState *entities, u32 dungeon_level, v2u pos, b32 enemy_only)
{
    Entity *result = 0;
    
    for(u32 index = 0; index < MAX_ENTITY_COUNT; ++index)
    {
        Entity *entity = &entities->array[index];
        
        if(enemy_only && !is_enemy_entity_valid(entity))
        {
            continue;
        }
        
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
other_windows_are_closed(Examine *examine, Inventory *inventory, UI *ui)
{
    b32 result = (!examine->type &&
                  !is_set(examine->flags, ExamineFlag_Open) &&
                      !is_set(inventory->flags, InventoryFlag_Open) &&
                      !is_set(inventory->flags, InventoryFlag_MultiplePickup) &&
                      !is_set(inventory->flags, InventoryFlag_MultipleExamine) &&
                      !ui->full_log_open);
    
    return(result);
}

internal void
make_entity_pathfind(Entity *entity,
                     ItemState *items,
                     Dungeon *dungeon,
                     PathfindMap *pathfind_map,
                     v2u pathfind_target_pos)
{
    if(!is_v2u_equal(entity->pos, pathfind_target_pos))
    {
        
#if 0
        print_v2u("Pathfind Origin", entity->pos);
        print_v2u("Pathfind Target", pathfind_target_pos);
        #endif
        
        set(entity->flags, EntityFlag_Pathfinding);
        entity->pathfind_target_pos = pathfind_target_pos;
        
        update_pathfind_map(dungeon, pathfind_map, pathfind_target_pos);
        if(entity->type == EntityType_Player) reset_player_pathfind_trail(entity);
    }
}

internal void
add_player_starting_item(Game *game,
                         Entity *player,
                         ItemState *items,
                         Inventory *inventory,
                         UI *ui,
                         u32 dungeon_level,
                         ItemID item_id,
                         u32 x, u32 y)
{
    assert(game);
    assert(is_player_entity_valid(player));
    assert(items);
    assert(inventory);
    assert(ui);
    assert(is_dungeon_level_valid(dungeon_level));
    assert(item_id);
    assert(x);
    assert(y);
    
    Random *random = &game->random;
    Item *item = 0;
    
    if(is_item_id_weapon(item_id))
    {
        item = add_weapon_item(random, items, dungeon_level, item_id, ItemRarity_Common, x, y, false);
    }
    else if(is_item_id_potion(item_id))
    {
        item = add_consumable_item(random, items, dungeon_level, item_id, x, y, 1);
        set_potion_or_scroll_as_known(items, item->id);
        
        assert(!item->enchant_level);
    }
    
    assert(item);
    assert(!is_set(item->flags, ItemFlag_IsCursed));
    
    add_item_to_inventory(game, player, item, items, inventory, ui, dungeon_level, false);
    
    if(is_item_id_weapon(item_id))
    {
        item->enchant_level = 0;
        equip_item(game, player, item, inventory, ui, false);
    }
}

internal b32
is_enemy_alerted(u32 turns_in_player_view)
{
    b32 result = (turns_in_player_view == 1);
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
        
        case EntityID_DwarfKnight: result = make_v2u(1, 14); break;
        case EntityID_DwarfSoldier: result = make_v2u(1, 14); break;
        case EntityID_DwarfTinkerer: result = make_v2u(1, 14); break;
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
        case EntityID_Zarimahar: result = make_v2u(1, 14); break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal void
add_entity_summon_spell(Entity *entity,
                        char *name,
                        SummonType type,
                        u32 chance,
                        u32 flags)
{
    assert(is_enemy_entity_valid(entity));
    assert(name);
    assert(type);
    assert(is_chance_valid(chance));
    assert(flags);
    
    for(u32 index = 0; index < MAX_ENTITY_SPELL_COUNT; ++index)
    {
        Spell *spell = &entity->e.spells[index];
        
        if(!spell->type)
        {
            strcpy(spell->name.s, name);
            spell->type = StatusType_Summon;
            spell->chance = chance;
            
            spell->summon_flags = flags;
            spell->summon_type = type;
            
            switch(type)
            {
                case SummonType_Undead: strcpy(spell->description.s, "Summons a random enemy of the undead type."); break;
                
                invalid_default_case;
            }
            
            ++entity->e.spell_count;
            return;
        }
    }
    
    assert(0);
}

internal void
add_entity_attack_spell(Entity *entity,
                        char *name,
                        u32 damage,
                        DamageType type,
                        u32 chance,
                        u32 range)
{
    assert(is_enemy_entity_valid(entity));
    assert(name);
    assert(type);
    assert(damage);
    assert(is_chance_valid(chance));
    
    for(u32 index = 0; index < MAX_ENTITY_SPELL_COUNT; ++index)
    {
        Spell *spell = &entity->e.spells[index];
        
        if(!spell->type)
        {
            strcpy(spell->name.s, name);
            
            spell->type = StatusType_Damage;
            spell->damage.type = type;
            spell->damage.min = 1;
            spell->damage.max = damage;
            spell->chance = chance;
            
            sprintf(spell->description.s, "Deals %u - %u %s damage to the target.",
                        spell->damage.min, spell->damage.max, get_damage_type_string(type));
            
            ++entity->e.spell_count;
            return;
        }
    }
    
    assert(0);
}

internal void
add_entity_status_spell(Entity *entity,
                         char *name,
                        StatusType status_type,
                        u32 value,
                        u32 duration,
                        u32 chance,
                        u32 range)
{
    assert(is_enemy_entity_valid(entity));
    assert(name);
    assert(status_type);
    assert(is_chance_valid(chance));
    assert(duration);
    
    for(u32 index = 0; index < MAX_ENTITY_SPELL_COUNT; ++index)
    {
        Spell *spell = &entity->e.spells[index];
        
        if(!spell->type)
        {
            strcpy(spell->name.s, name);
            
            spell->type = status_type;
            spell->chance = chance;
            spell->duration = duration;
            
            switch(status_type)
            {
                case StatusType_Bind:
                {
                    assert(!value);
                    
                    sprintf(spell->description.s, "Stops the target from moving for %u turns.", spell->duration);
                } break;
                
                case StatusType_Confusion:
                {
                    assert(!value);
                    
                    sprintf(spell->description.s, "Confuses the target for %u turns.", spell->duration);
                } break;
                
                case StatusType_Burn:
                {
                    assert(value);
                    
                    spell->value.min = 1;
                    spell->value.max = value;
                    spell->value.type = DamageType_Fire;
                    sprintf(spell->description.s, "Deals %u - %u %s damage to the target for %u turns.", spell->value.min, spell->value.max, get_damage_type_string(DamageType_Fire), spell->duration); 
                } break;
                
                invalid_default_case;
            }
            
            ++entity->e.spell_count;
            return;
        }
    }
    
    assert(0);
}

internal void
add_entity_heal_spell(Entity *entity,
                        char *name,
                      u32 value,
                      u32 chance,
                      u32 range)
{
    assert(is_enemy_entity_valid(entity));
    assert(name);
    assert(value);
    assert(is_chance_valid(chance));
    
    for(u32 index = 0; index < MAX_ENTITY_SPELL_COUNT; ++index)
    {
        Spell *spell = &entity->e.spells[index];
        
        if(!spell->type)
        {
            strcpy(spell->name.s, name);
            
            spell->type = StatusType_Heal;
            spell->value.max = value;
            spell->chance = chance;
            
            sprintf(spell->description.s, "Heals the target for %u health.", value);
            
            ++entity->e.spell_count;
            return;
        }
    }
    
    assert(0);
}

internal void
add_entity_stat_spell(Entity *entity,
                      char *name,
                      StatType stat_type,
                      s32 value,
                      u32 duration,
                      u32 chance,
                      u32 range)
{
    assert(is_enemy_entity_valid(entity));
    assert(name);
    assert(stat_type);
    assert(value);
    assert(duration);
    assert(is_chance_valid(chance));
    
    for(u32 index = 0; index < MAX_ENTITY_SPELL_COUNT; ++index)
    {
        Spell *spell = &entity->e.spells[index];
        
        if(!spell->type)
        {
            strcpy(spell->name.s, name);
            
            spell->type = StatusType_Stat;
            spell->stat_type = stat_type;
            
            // Assumes positive stat spells are towards enemies.
            if(value > 0)
            {
                spell->target_type = EntityType_Enemy;
            }
            else
            {
                spell->target_type = EntityType_Player;
            }
            
            spell->value.max = value;
            spell->chance = chance;
            spell->duration = duration;
            
            char *action = "Increases";
            if(spell->value.max < 0)
            {
                action = "Decreases";
            }
            
            char *stat = 0;
            switch(spell->stat_type)
            {
                case StatType_Str: stat = "Strength"; break;
                case StatType_Int: stat = "Intelligence"; break;
                case StatType_Dex: stat = "Dexterity"; break;
                case StatType_Def: stat = "Defence"; break;
                case StatType_EV: stat = "Evasion"; break;
                case StatType_StrIntDex: stat = "Strength, Intelligence and Dexterity"; break;
                    
                invalid_default_case;
            }
            
            sprintf(spell->description.s, "%s the targets %s by %u for %u turns.",
                        action,
                        stat,
                        absolute(spell->value.max),
                        spell->duration);
            
            ++entity->e.spell_count;
            return;
        }
    }
    
    assert(0);
}

internal void
update_entity_stats_from_status(Entity *entity,
                                UI *ui,
                                Status *status,
                                b32 is_apply)
{
    assert(is_entity_valid(entity));
    assert(ui);
    assert(is_status_valid(status));
        assert(status->value.max);
    
    if(is_apply)
    {
        assert(!status->stat_value_applied);
        status->stat_value_applied = true;
    }
    
    if(status->stat_type)
    {
        switch(status->stat_type)
        {
            case StatType_Str: change_stat(&entity->str, status->value.max, is_apply); break;
            case StatType_Int: change_stat(&entity->intel, status->value.max, is_apply); break;
            case StatType_Dex: change_stat(&entity->dex, status->value.max, is_apply); break;
        case StatType_EV: break;
            case StatType_Def: change_stat(&entity->def, status->value.max, is_apply); break;
        
        case StatType_StrIntDex:
            {
                change_stat(&entity->str, status->value.max, is_apply);
                change_stat(&entity->intel, status->value.max, is_apply);
                change_stat(&entity->dex, status->value.max, is_apply);
        } break;
        
        invalid_default_case;
    }
    }
    }

internal void
add_entity_status(Random *random,
                    Entity *entity,
                    Dungeon *dungeon,
                    UI *ui,
                    Status *new_status)
{
    assert(random);
    assert(is_entity_valid(entity));
    assert(ui);
    assert(is_status_valid(new_status));
    
    for(u32 index = 0; index < MAX_ENTITY_STATUS_COUNT; ++index)
    {
        Status *status = &entity->statuses[index];
        
        if(!status->type)
        {
            *status = *new_status;
            
            // Set status value from value range
            ItemInfo *info = status->item_info;
            if(info)
            {
                assert(info->value_range.min);
                assert(info->value_range.max);
                
                status->is_player_active_custom = true;
                status->value.max = get_random_from_v2u(random, info->value_range);
                }
            
            switch(status->type)
            {
                case StatusType_Poison:
                {
                    assert(entity->type == EntityType_Player);
                    
                    status->is_player_active_custom = true;
                    status->print_end_on_last_status = true;
                    status->player_active_color = Color_White;
                    
                    strcpy(status->player_start.s, "You feel poisoned!");
                    strcpy(status->player_end.s, "You don't feel poisoned anymore.");
                    strcpy(status->player_active.s, "%sThe poison wrecks your body, %u damage.");
                } break;
                
                case StatusType_Burn:
                {
                    assert(entity->type == EntityType_Player);
                    
                    status->is_player_active_custom = true;
                    status->print_end_on_last_status = true;
                    status->player_active_color = Color_White;
                    
                    strcpy(status->player_start.s, "Your body starts burning!");
                    strcpy(status->player_end.s, "Your body stops burning.");
                    strcpy(status->player_active.s, "%sYour body burns, %u damage.");
                    } break;
                
                case StatusType_Bleed:
                {
                    assert(entity->type == EntityType_Player);
                    
                    status->is_player_active_custom = true;
                    status->print_end_on_last_status = true;
                    status->player_active_color = Color_White;
                    
                    strcpy(status->player_start.s, "Your wounds start bleeding!");
                    strcpy(status->player_end.s, "Your wounds stop bleeding.");
                    strcpy(status->player_active.s, "%sYour wounds bleed, %u damage.");
                } break;
                
                case StatusType_Bind:
                {
                    status->print_end_on_last_status = true;
                        
                        if(entity->type == EntityType_Player)
                        {
                        strcpy(status->player_start.s, "You feel like you can't move!");
                        strcpy(status->player_end.s, "You feel like you can move again.");
                        }
                } break;
                
                case StatusType_Confusion:
                {
                    status->print_end_on_last_status = true;
                    
                    if(entity->type == EntityType_Player)
                    {
                        strcpy(status->player_start.s, "You feel very confused!");
                        strcpy(status->player_end.s, "You don't feel confused anymore.");
                    }
                } break;
                
                case StatusType_BrokenArmor:
                {
                    status->print_end_on_last_status = true;
                    
                    if(entity->type == EntityType_Player)
                    {
                        strcpy(status->player_start.s, "Your armor feels broken!");
                        strcpy(status->player_end.s, "Your armor doesn't feel broken anymore.");
                    }
                } break;
                
                case StatusType_Sightless:
                {
                    status->print_end_on_last_status = true;
                    
                    if(entity->type == EntityType_Player)
                    {
                        status->stored_value = entity->view_range;
                        
                        entity->view_range /= 2;
                        update_player_view(entity, dungeon);
                        
                        strcpy(status->player_start.s, "Your vision turns muddled!");
                        strcpy(status->player_end.s, "Your vision returns to normal.");
                    }
                } break;
            }
            
            // Print status start message
            if(status->player_start.s[0]) log_add(status->player_start.s, ui);
            
            return;
        }
    }
    
    assert(0);
}

internal void
update_entity_statuses(Game *game,
                       PathfindMap *enemy_pathfind_map,
                             Entity *entity,
                             Dungeon *dungeon,
                             Inventory *inventory,
                             UI *ui)
{
    assert(game);
    assert(is_entity_valid(entity));
    assert(dungeon);
    assert(inventory);
    assert(ui);
    
    for(u32 index = 0; index < MAX_ENTITY_STATUS_COUNT; ++index)
    {
        Status *status = &entity->statuses[index];
        
        if(status->type)
        {
            switch(status->type)
            {
                case StatusType_Damage: attack_entity(&game->random, enemy_pathfind_map, 0, entity, dungeon, ui, status->value); break;
                
                case StatusType_Heal:
                {
                    assert(entity->hp);
                    assert(entity->max_hp);
                    
                    if(entity->hp == entity->max_hp)
                    {
                        assert(status->player_max_hp.s[0]);
                        log_add(status->player_max_hp.s, ui);
                    }
                    else
                    {
                        // Custom messages are used when the status value has been predetermined.
                        if(status->is_player_active_custom)
                        {
                            assert(status->player_active.s[0]);
                            assert(status->player_active_target.s[0]);
                            assert(status->player_active_color);
                            assert(status->value.max);
                            
#if 0
                            printf("player_active: %s\n", status->player_active.s);
                            printf("player_active_target: %s\n\n", status->player_active_target.s);
                            printf("player_active_color: %s\n", get_color_string(status->player_active_color));
                            printf("status->value.max: %d\n", status->value.max);
#endif
                            
                                if(status->is_value_percentage)
                                {
                                    //printf("status->value.max: %u\n", status->value.max);
                                    
                                    // Get the percent value from the entity max health.
                                    f32 percent_value = ((f32)status->value.max / 100.0f) * (f32)entity->max_hp;
                                    status->value.max = f32_to_u32_rounded_up(percent_value);
                                }
                                
                                log_add(status->player_active.s, ui,
                                        start_color(status->player_active_color),
                                        status->player_active_target.s,
                                        status->value.max);
                        }
                        else
                        {
                            log_add(status->player_active.s, ui);
                        }
                        
                        
                        heal_entity(entity, status->value.max);
                    }
                } break;
                
                case StatusType_Stat:
                {
                    if(!status->stat_value_applied) update_entity_stats_from_status(entity, ui, status, true);
                } break;
                
                case StatusType_Burn:
                case StatusType_Poison:
                case StatusType_Bleed: attack_entity_with_status(game, enemy_pathfind_map, entity, dungeon, ui, status); break;
                
                case StatusType_Bind: break;
                case StatusType_BrokenArmor: break;
                case StatusType_Confusion: break;
                case StatusType_Sightless: break;
                
                invalid_default_case;
            }
            
            if(status->duration) --status->duration;
            
            // End status
            if(!status->duration)
            {
                b32 can_print_end = true;
                
                if(status->print_end_on_last_status)
                {
                    // Don't print the end message if there's another status that's the same.
                    for(u32 other_index = 0; other_index < MAX_ENTITY_STATUS_COUNT; ++other_index)
                    {
                        Status *other_status = &entity->statuses[other_index];
                        
                        if(other_index != index &&
                               other_status->type == status->type)
                        {
                            can_print_end = false;
                            break;
                        }
                    }
                    }
                
                // Print status end message
                if(can_print_end && status->player_end.s[0]) log_add(status->player_end.s, ui);
                
                // Undo status effect
                if(status->stat_type)
                {
                    update_entity_stats_from_status(entity, ui, status, false);
                }
                else
                {
                    switch(status->type)
                    {
                        case StatusType_Sightless:
                        {
                            assert(status->stored_value);
                            entity->view_range = status->stored_value;
                            } break;
                        
                        case StatusType_Bind: break;
                        case StatusType_Heal: break;
                        case StatusType_Burn: break;
                        case StatusType_Bleed: break;
                        case StatusType_Poison: break;
                        case StatusType_Confusion: break;
                        case StatusType_BrokenArmor: break;
                        
                        invalid_default_case;
                    }
                }
                
                zero_struct(*status);
            }
        }
        }
    }

internal void
add_entity_status_from_spell(Random *random,
                             Entity *entity,
                             Dungeon *dungeon,
                             UI *ui,
                             Spell *spell)
{
    assert(random);
    assert(is_entity_valid(entity));
    assert(dungeon);
    assert(ui);
    assert(is_spell_valid(spell));
    
    Status status = {0};
    strcpy(status.name.s, spell->name.s);
    status.type = spell->type;
    status.stat_type = spell->stat_type;
    status.value = spell->value;
    status.duration = spell->duration;
    status.chance = spell->chance;
    status.spell = spell;
    
    // Only update if the spell has a value.
    if(status.value.max) update_entity_stats_from_status(entity, ui, &status, true);
    add_entity_status(random, entity, dungeon, ui, &status);
}

internal void
cast_entity_spell(Random *random,
                     Entity *caster,
                     Entity *target,
                     Dungeon *dungeon,
                     UI *ui)
{
    assert(random);
    assert(is_enemy_entity_valid(caster));
    assert(is_spell_valid(caster->e.spell));
    assert(is_entity_valid(target));
    assert(dungeon);
    assert(ui);
    
    log_add_entity_attack_string(random, caster, target, ui, 0, ResistAmount_None);
    add_entity_status_from_spell(random, target, dungeon, ui, caster->e.spell);
}

internal b32
will_entity_hit(Random *random, Entity *attacker, Entity *defender)
{
    assert(random);
    assert(is_entity_valid(attacker));
    assert(attacker->hit_chance);
    assert(is_entity_valid(defender));
    
    b32 result = false;
    
    if(defender->ev)
    {
        // An attack against an invisible entity has a lower chance of hitting.
        u32 attacker_hit_chance = attacker->hit_chance;
        if(is_set(defender->flags, EntityFlag_Invisible))
        {
            attacker_hit_chance /= 2;
        }
        
        // An attack from an invisible entity has a higher chance of hitting.
        u32 defender_evasion = defender->ev;
        if(is_set(attacker->flags, EntityFlag_Invisible))
        {
            defender_evasion /= 3;
        }
        
        if(get_random(random, 1, attacker_hit_chance) >= defender_evasion)
        {
            result = true;
        }
    }
    else
    {
        result = true;
    }
    
    return(result);
}

internal b32
entity_can_move(Entity *entity, UI *ui)
{
    assert(is_entity_valid(entity));
    assert(ui);
    
    b32 result = true;
     
    for(u32 index = 0; index < MAX_ENTITY_STATUS_COUNT; ++index)
    {
        Status *status = &entity->statuses[index];
        
        if(status->type && status->type == StatusType_Bind)
        {
            result = false;
            if(entity->type == EntityType_Player) log_add("You aren't able to move!", ui);
            
            break;
            }
    }
    
    return(result);
}

internal void
move_entity_between_dungeons(Game *game,
                             Entity *player,
                             EntityState *entities,
                             Dungeons *dungeons,
                             ItemState *items,
                             Inventory *inventory,
                             UI *ui,
                             v2u pos,
                             u32 new_dungeon_level)
{
    assert(is_player_entity_valid(player));
    assert(dungeons && is_dungeon_level_valid(new_dungeon_level));
    
    Dungeon *new_dungeon = get_dungeon_from_level(dungeons, new_dungeon_level);
    if(new_dungeon->level)
    {
        dungeons->current_level = new_dungeon_level;
        force_move_entity(&entities->enemy_pathfind_map, player, new_dungeon, pos, new_dungeon_level);
        
            reset_player_pathfind_trail(player);
    }
    else
    {
        create_dungeon(game, player, entities, dungeons, items, inventory, ui, new_dungeon_level);
    }
}

internal void
entity_use_passage(Game *game,
                   Entity *player,
                   EntityState *entities,
                   Dungeons *dungeons,
                   ItemState *items,
                   Inventory *inventory,
                   UI *ui,
                   DungeonPassage *passage)
{
    assert(is_player_entity_valid(player));
    
    u32 new_dungeon_level = dungeons->current_level;
    switch(passage->type)
    {
        case DungeonPassageType_Up:
        {
            --new_dungeon_level;
            log_add("%sYou go up the passage..", ui, start_color(Color_LightGray));
        } break;
        
        case DungeonPassageType_Down:
        {
            ++new_dungeon_level;
            log_add("%sYou go down the passage..", ui, start_color(Color_LightGray));
        } break;
        
        invalid_default_case;
    }
    
    move_entity_between_dungeons(game, player, entities, dungeons, items, inventory, ui, passage->dest_pos, new_dungeon_level);
    }

internal b32
move_entity(Random *random, PathfindMap *enemy_pathfind_map, Entity *entity, Dungeon *dungeon, UI *ui, v2u pos)
{
    assert(is_entity_valid(entity));
    assert(!is_v2u_zero(pos));
    
    b32 result = entity_can_move(entity, ui);
    if(result)
    {
        // If the entity is confused, attempt to move them in a direction that is different from the
        // original move direction.
        Status *confusion_status = get_active_status(entity->statuses, StatusType_Confusion);
        if(confusion_status)
    {
            assert(confusion_status->chance);
        
        if(hit_random_chance(random, confusion_status->chance))
        {
            Direction confused_direction = get_random_direction(random);
            v2u confused_move_pos = get_direction_pos(entity->pos, confused_direction);
            
            if(confused_direction != entity->direction &&
                       is_dungeon_pos_traversable_and_unoccupied(dungeon->tiles, confused_move_pos))
            {
                
#if 0
                    printf("\nconfused_direction: %s\n", get_direction_string(confused_direction));
                printf("confused_move_pos: %u, %u\n", confused_move_pos.x, confused_move_pos.y);
#endif
                
                pos = confused_move_pos;
                    
                    switch(entity->type)
                    {
                        case EntityType_Player: log_add("%sYou stumble around.", ui, start_color(Color_LightGray)); break;
                        case EntityType_Enemy: log_add("%sThe %s stumbles around.", ui, start_color(Color_LightGray), entity->name.s); break;
                        
                        invalid_default_case;
                    }
            }
        }
    }
        
        force_move_entity(enemy_pathfind_map, entity, dungeon, pos, dungeon->level);
        //printf("force_move_entity(): %u, %u\n", pos.x, pos.y);
        
        // We only set the flipped flag for enemies.
        if(entity->type == EntityType_Enemy)
        {
        switch(entity->direction)
        {
            case Direction_Left:
            case Direction_UpLeft:
            case Direction_DownLeft: set(entity->flags, EntityFlag_Flipped); break;
            
            case Direction_Right:
            case Direction_UpRight:
            case Direction_DownRight: unset(entity->flags, EntityFlag_Flipped); break;
        }
        }
    }
    
    return(result);
}

internal b32
heal_entity(Entity *entity, u32 value)
{
    assert(is_entity_valid(entity));
    assert(value);
    
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

internal void
remove_entity_from_game(Entity *entity, DungeonTiles tiles)
{
    assert(is_entity_valid(entity));
    
    set_dungeon_pos_occupied(tiles, entity->pos, false);
    zero_struct(*entity);
}

internal void
kill_entity(Random *random, Entity *entity, Dungeon *dungeon, UI *ui)
{
    assert(random);
    assert(is_entity_valid(entity));
    assert(dungeon);
    assert(ui);
    
    switch(entity->type)
    {
        case EntityType_Player:
        {
            // TODO(rami): Character death info dumps
            
            log_add("Oh no, you are dead!", ui);
            entity->hp = 0;
        } break;
        
        case EntityType_Enemy:
        {
            log_add("%sThe %s dies!", ui, start_color(Color_LightRed), entity->name.s);
            place_entity_remains(random, entity, dungeon, EntityRemainsType_Kill);
            remove_entity_from_game(entity, dungeon->tiles);
        } break;
        
        invalid_default_case;
    }
}

internal u32
attack_entity(Random *random,
              PathfindMap *enemy_pathfind_map,
              Entity *attacker,
              Entity *defender,
              Dungeon *dungeon,
              UI *ui,
              DamageInfo damage_info)
{
    u32 result = 0;
    
#if 1
    if(attacker) printf("Attacker: %s\n", attacker->name.s);
    printf("Defender: %s\n", defender->name.s);
    printf("Damage Info Min: %u\n", damage_info.min);
    printf("Damage Info Max: %u\n", damage_info.max);
    printf("Damage Type: %s\n", get_damage_type_string(damage_info.type));
    
    printf("\n");
    #endif
    
    assert(random);
    assert(defender);
    assert(dungeon);
    assert(ui);
    assert(damage_info.type);
    
#if MOONBREATH_SLOW
    // Skip dummies during debugging
    if(defender->id == EntityID_Dummy) return(0);
    
    // Player hit chance test
    if(is_player_entity_valid(attacker) && fkey_active[6])
    {
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
        
        printf("\n- Entity Hit Test -\n");
        
        printf("Attempts: %u\n", loop_count);
        printf("Hits: %u (%.0f%%)\n", hit_count, ((f32)hit_count / (f32)loop_count) * 100.0f);
        printf("Misses: %u (%.0f%%)\n\n", miss_count, ((f32)miss_count / (f32)loop_count) * 100.0f);
        
        printf("Attacker Hit Chance: %u\n", attacker->hit_chance);
        printf("Defender Evasion: %u\n", defender->ev);
        
        return(0);
    }
#endif
    
    if(!attacker || will_entity_hit(random, attacker, defender))
    {
        if(!is_zero(damage_info.max))
        {
            assert(damage_info.min);
            u32 damage = get_random(random, damage_info.min, damage_info.max);
            
            if(defender->def) damage -= get_random(random, 0, defender->def);
            
            if(!is_zero(damage))
            {
                ResistInfo resist_info = get_resisted_entity_damage(defender, ui, damage, damage_info.type);
                damage = resist_info.damage_after;
                
                if(attacker) log_add_entity_attack_string(random, attacker, defender, ui, damage, resist_info.amount);
                
                if(!is_zero(damage))
                {
                    result = damage;
                    
                    defender->hp -= damage;
                    if(!is_zero(defender->hp))
                    {
                        defender->regen.current = 0;
                        if(hit_random_chance(random, 30)) place_entity_remains(random, defender, dungeon, EntityRemainsType_Wound);
                        
                        // Start attack status
                        if(attacker && attacker->type == EntityType_Enemy)
                        {
                            Status *status = &attacker->e.attack_status;
                            
                            if(status->type &&
                                   !get_active_status(defender->statuses, status->type) &&
                               hit_random_chance(random, status->chance))
                            {
                                if(status->value.type &&
                                       is_entity_immune(defender->resistances, status->value.type))
                                {
                                    log_add("You resist the %s!", ui, get_damage_type_string(status->value.type));
                                }
                                else
                                {
                                    add_entity_status(random, defender, dungeon, ui, status);
                                }
                            }
                        }
                        
                        // On hit
                        if(defender &&
                               defender->type == EntityType_Enemy &&
                               hit_random_chance(random, defender->e.hit_teleport_chance))
                        {
                            teleport_entity(random, enemy_pathfind_map, defender, dungeon, ui, DungeonRandomPosType_TraversableRect);
                            log_add("The %s suddenly warps somewhere else!", ui, defender->name.s);
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
                    log_add("%sYour armor blocks the attack.", ui, start_color(Color_LightGray));
                }
                else
                {
                    assert(defender->type == EntityType_Enemy);
                    log_add("%sThe %s's armor blocks your attack.", ui, start_color(Color_LightGray), defender->name.s);
                }
            }
            }
        else
        {
            // Attacker deals zero damage
            assert(is_player_entity_valid(attacker));
            log_add_entity_attack_string(random, attacker, defender, ui, 0, ResistAmount_None);
        }
        }
    else
    {
        if(attacker->type == EntityType_Player)
        {
            if(is_set(defender->flags, EntityFlag_Invisible))
            {
                log_add("%sYou miss the something.", ui, start_color(Color_LightGray));
            }
            else
            {
                log_add("%sYou miss the %s.", ui, start_color(Color_LightGray), defender->name.s);
            }
        }
        else
        {
            assert(attacker->type == EntityType_Enemy);
            log_add("%sYou dodge the attack.", ui, start_color(Color_LightGray));
        }
    }
    
    return(result);
}

internal b32
was_pressed_core(InputState *state)
{
    assert(state);
    
    b32 result = false;
    
    if(state->is_down)
    {
        if(state->has_been_up || state->repeat)
        {
            state->has_been_up = false;
            result = true;
        }
    }
    
    return(result);
}

internal b32
was_pressed(InputState *state)
{
    assert(state);
    
    #if MOONBREATH_SLOW
    if(fkey_active[3] && state->is_down) return(true);
#endif
    
    b32 result = was_pressed_core(state);
    return(result);
}

internal void
update_enemy_entities(Game *game,
                      Entity *player,
                      EntityState *entities,
                      ItemState *items,
                      Inventory *inventory,
                      Dungeon *dungeon,
                      UI *ui)
{
    assert(game);
    assert(player);
    assert(entities);
    assert(items);
    assert(inventory);
    assert(dungeon);
    assert(ui);
    
    Random *random = &game->random;
    
    for(u32 enemy_index = 0; enemy_index < MAX_ENTITY_COUNT; ++enemy_index)
    {
        Entity *enemy = &entities->array[enemy_index];
        if(is_enemy_entity_valid(enemy))
        {
            
#if MOONBREATH_SLOW
            if(enemy->id == EntityID_Dummy) continue;
            //continue;
#endif
            
            if(game->passed_time)
            {
                // Add elapsed time to enemy action timer
                enemy->e.action_timer += game->passed_time;
                
                // Get enemy turn count from enemy action time
                f32 enemy_available_time = enemy->e.action_timer;
                u32 enemy_turn_count = get_enemy_turn_count(&enemy_available_time, enemy->action_time);
                if(is_dungeon_pos_water(dungeon->tiles, enemy->pos) &&
                   !is_set(enemy->flags, EntityFlag_NormalWaterMovement))
                {
                    enemy_turn_count = get_enemy_turn_count(&enemy_available_time, enemy->action_time * 2.0f);
                }
                
                while(enemy_turn_count)
                {
                    
#if 0
                    printf("game->action_time: %.01f\n", game->action_time);
                    printf("enemy->e.action_timer: %.01f\n", enemy->e.action_timer);
                    printf("enemy_turn_count: %u\n", enemy_turn_count);
                    printf("\n");
#endif
                    
                    --enemy_turn_count;
                    enemy->e.action_timer = 0;
                    
                    if(entity_can_see_pos(dungeon->tiles, enemy->e.view_rect, enemy->pos, player->pos))
                    {
                        set(enemy->flags, EntityFlag_InCombat);
                        unset(enemy->flags, EntityFlag_Pathfinding);
                        
                        // This is for spellcasting enemies that don't call move_entity() which would
                        // set their direction, so we set it here again.
                        if(player->pos.x < enemy->pos.x)
                        {
                            enemy->direction = Direction_Left;
                            set(enemy->flags, EntityFlag_Flipped);
                        }
                        else
                        {
                            enemy->direction = Direction_Right;
                            unset(enemy->flags, EntityFlag_Flipped);
                        }
                        
                        ++enemy->e.turns_in_player_view;
                        if(is_enemy_alerted(enemy->e.turns_in_player_view)) break;
                        
                        // Attack, cast spells etc.
                        PathfindPosInfo pathfind = get_pathfind_result(&entities->enemy_pathfind_map, dungeon, enemy->pos, player->pos);
                        assert(pathfind.is_valid);
                        
                        if(enemy->e.spell_count)
                        {
                            select_and_cast_entity_spell(random, entities, enemy_index, enemy, player, dungeon, ui);
                        }
                        else if((is_dungeon_pos_inside_rect(player->pos, enemy->e.view_rect) &&
                                 is_set(enemy->flags, EntityFlag_UsesRangedAttacks)) ||
                                is_v2u_equal(pathfind.pos, player->pos))
                        {
                            attack_entity(random, &entities->enemy_pathfind_map, enemy, player, dungeon, ui, enemy->e.damage);
                        }
                        else
                        {
                            enemy->new_pos = pathfind.pos;
                        }
                    }
                    else
                    {
                        // Update enemy combat pathfind
                        if(is_set(enemy->flags, EntityFlag_InCombat) &&
                           !is_enemy_alerted(enemy->e.turns_in_player_view))
                        {
                            if(!is_set(enemy->flags, EntityFlag_Pathfinding))
                            {
                                make_entity_pathfind(enemy, items, dungeon, &entities->enemy_pathfind_map, player->pos);
                                //printf("Enemy Pathfind: Target %u, %u\n", enemy->pathfind_target_pos.x, enemy->pathfind_target_pos.y);
                            }
                            
                            PathfindPosInfo pathfind = get_pathfind_result(&entities->enemy_pathfind_map, dungeon, enemy->pos, enemy->pathfind_target_pos);
                            assert(pathfind.is_valid);
                            
                            enemy->new_pos = pathfind.pos;
                            //printf("Enemy Pathfind: New Pos: %u, %u\n", pathfind.pos.x, pathfind.pos.y);
                            
                            if(is_v2u_equal(enemy->new_pos, enemy->pathfind_target_pos))
                            {
                                //printf("Enemy Pathfind: Target Reached\n");
                                
                                enemy->e.turns_in_player_view = 0;
                                unset(enemy->flags, EntityFlag_InCombat | EntityFlag_Pathfinding);
                            }
                        }
                        else
                        {
                            enemy->e.turns_in_player_view = 0;
                            unset(enemy->flags, EntityFlag_InCombat);
                            
                            enemy->direction = get_random_direction(random);
                            enemy->new_pos = get_direction_pos(enemy->pos, enemy->direction);
                            
                            if(is_dungeon_pos_traversable(dungeon->tiles, enemy->new_pos) &&
                               is_tile_seen(dungeon->tiles, enemy->new_pos) &&
                               !enemy_turn_count)
                            {
                                ++enemy->e.turns_in_player_view;
                            }
                        }
                    }
                    
                    // Before calling move_entity(), we set saved_pos_for_ghost
                    // based on if the new enemy pos is seen. Enemy rendering
                    // code will then use saved_pos_for_ghost.
                    if(is_tile_seen(dungeon->tiles, enemy->new_pos))
                    {
                        enemy->e.saved_pos_for_ghost = enemy->new_pos;
                    }
                    else
                    {
                        enemy->e.saved_flipped_for_ghost = is_set(enemy->flags, EntityFlag_Flipped);
                        enemy->e.saved_pos_for_ghost = enemy->pos;
                    }
                    
                    if(is_dungeon_pos_traversable_and_unoccupied(dungeon->tiles, enemy->new_pos))
                    {
                        move_entity(random, &entities->enemy_pathfind_map, enemy, dungeon, ui, enemy->new_pos);
                    }
                    
                    update_entity_statuses(game, &entities->enemy_pathfind_map, enemy, dungeon, inventory, ui);
                }
            }
        }
    }
}

internal void
update_player_input(Game *game,
                    Input *input,
                    Entity *player,
                    EntityState *entities,
                    ItemState *items,
                    Inventory *inventory,
                    Dungeons *dungeons,
                    Assets *assets,
                    UI *ui)
{
    assert(game);
    assert(input);
    assert(is_player_entity_valid(player));
    assert(entities);
    assert(items);
    assert(inventory);
    assert(dungeons);
    assert(assets);
    assert(ui);
    
    Random *random = &game->random;
    Examine *examine = &game->examine;
    Dungeon *dungeon = get_dungeon_from_level(dungeons, dungeons->current_level);
    
    game->should_update = false;
    game->passed_time = 0;
    
    player->new_pos = player->pos;
    player->direction = Direction_None;
    set(player->flags, EntityFlag_NotifyAboutMultipleItems);
    
    if(is_set(player->flags, EntityFlag_Pathfinding))
    {
        game->should_update = update_player_pathfind(player, entities, items, dungeon, ui);
    }
    else if(is_set(inventory->flags, InventoryFlag_AskingPlayer))
    {
        // Process player question response
        if(was_pressed(&input->GameKey_Yes))
        {
            if(inventory->interact_type)
            {
                log_add("The scroll turns illegible, you discard it.", ui);
                end_inventory_item_interact(inventory);
            }
            
            unset_asking_player_and_ready_for_keypress(&inventory->flags);
        }
        else if(was_pressed(&input->GameKey_No))
        {
            log_add_okay(ui);
            unset_asking_player_and_ready_for_keypress(&inventory->flags);
        }
        else if(inventory->interact_type &&
                    get_pressed_alphabet_char(input))
        {
            // Repeat the item cancel question if we're not getting the inputs we expect.
                ask_for_item_cancel(game->keybinds, input, inventory, ui);
            }
    }
    else
    {
        
#if MOONBREATH_SLOW
        if(was_pressed_core(&input->fkeys[1]))
        {
            // Toggle debug state visibility
            DebugState *debug = &game->debug;
            debug->is_shown = !debug->is_shown;
            
            // Reset debug event
            DebugEvent *event = &debug->event;
            if(event->type)
            {
                reset_debug_event(event);
            }
            
            return;
        }
        else if(was_pressed(&input->fkeys[2]))
        {
            if(is_set(inventory->flags, InventoryFlag_Open))
            {
                printf("F2: All items have been picked up.\n");
            
            for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
            {
                Item *item = &items->array[index];
                
                if(is_item_valid_and_not_in_inventory(item))
                {
                    add_item_to_inventory(game, player, item, items, inventory, ui, dungeon->level, true);
                }
            }
        }
        
            return;
        }
        else if(was_pressed(&input->fkeys[3]))
        {
            if(is_set(inventory->flags, InventoryFlag_Open))
            {
                printf("F3: All inventory items have been dropped.\n");
                
                for(u32 index = 0; index < MAX_INVENTORY_SLOT_COUNT; ++index)
                {
                    Item *item = inventory->slots[index];
                    
                    if(item)
                    {
                        drop_item_from_inventory(game, player, item, items, inventory, dungeon, ui);
                    }
                }
            }
            
            return;
        }
        else if(was_pressed(&input->fkeys[4]))
        {
            printf("F4: All items identified toggle.\n");
            
            for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
            {
                Item *item = &items->array[index];
                
                if(is_item_valid(item) &&
                   item->type != ItemType_Ration)
                {
                    toggle(item->flags, ItemFlag_IsIdentified);
                }
            }
            
            return;
        }
        else if(was_pressed(&input->fkeys[5]))
        {
            printf("F5: Item mark test string.\n");
            strcpy(items->temp_mark.array, "Donec in tellus et mauris maximus interdum in at ex. Proin pla.");
            
            return;
        }
        else if(was_pressed(&input->fkeys[6]))
        {
            fkey_active[6] = !fkey_active[6];
            printf("F6: Entity hit test %s.\n", fkey_active[6] ? "on" : "off");
            
            return;
        }
        else if(was_pressed(&input->fkeys[7]))
        {
            player->hp = player->max_hp / 2;
            
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
#endif
        
            b32 was_direction_pressed = false;
            for(Direction direction = Direction_Up; direction <= Direction_DownRight; ++direction)
            {
                u32 index = direction - 1;
                if(was_pressed(&input->game_keys[index]) || examine->key_pressed[index])
            {
                if(is_set(examine->flags, ExamineFlag_Open) && !examine->type)
                {
                    // Update examine pos
                    v2u new_pos = get_direction_pos(examine->pos, direction);
                    if(is_pos_inside_dungeon(dungeon->size, new_pos))
                    {
                        examine->pos = new_pos;
                    }
                    }
                else if(other_windows_are_closed(examine, inventory, ui))
                {
                    reset_player_pathfind_trail(player);
                    
                    v2u new_pos = get_direction_pos(player->pos, direction);
                    if(is_pos_inside_dungeon(dungeon->size, new_pos))
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
            if(was_pressed(&input->Button_Left))
            {
                
                #if 0
                printf("Mouse Player Pathfind Map Value: %u\n", get_pathfind_value(&entities->player_pathfind_map, input->mouse_tile));
                printf("Mouse Enemy Pathfind Map Value: %u\n\n", get_pathfind_value(&entities->enemy_pathfind_map, input->mouse_tile));
                #endif
                
                if(game->debug.is_shown)
                {
                    game->debug.should_update = true;
                }
                else
                {
                    // Mouse highlight click
                    Owner *highlight = &ui->mouse_highlight;
                    
                    if(highlight->type)
                    {
                        assert(highlight->type == OwnerType_Item ||
                               highlight->type == OwnerType_Entity ||
                               highlight->type == OwnerType_Trap);
                        
                        if(is_set(inventory->flags, InventoryFlag_Open))
                        {
                            if(inventory->interact_type)
                            {
                                process_inventory_item_interact(random, inventory, highlight->item, ui);
                            }
                            else
                            {
                            start_item_examine(inventory, highlight->item);
                            }
                            }
                        else if(is_set(inventory->flags, InventoryFlag_MultiplePickup))
                        {
                            toggle(highlight->item->flags, ItemFlag_IsSelected);
                        }
                        else if(is_set(inventory->flags, InventoryFlag_MultipleExamine))
                        {
                            start_examine_from_owner(examine, highlight);
                        }
                    }
                }
                }
            else if(!is_set(inventory->flags, InventoryFlag_Open) &&
               was_pressed(&input->GameKey_OpenInventory))
            {
                if(other_windows_are_closed(examine, inventory, ui))
                    {
                    set(inventory->flags, InventoryFlag_Open);
                    set_view_at_start_and_reset_view_moves(&inventory->window.view);
                        }
                }
            else if(was_pressed(&input->GameKey_Back))
                {
                    if(!is_set(inventory->flags, InventoryFlag_Mark))
                {
                        if((is_set(examine->flags, ExamineFlag_Open) || examine->type))
                        {
                            if(examine->type == ExamineType_Spell)
                        {
                                examine->type = ExamineType_Entity;
                        }
                            else
                        {
                            unset(examine->flags, ExamineFlag_Open | ExamineFlag_ReadyForKeypress);
                            
                            if(examine->type == ExamineType_Entity)
                            {
                                reset_all_owner_select_letters(ui->temp_owners);
                            }
                            
                            examine->type = ExamineType_None;
                            examine->selected_passage = 0;
                            }
                        }
                    else if(is_set(inventory->flags, InventoryFlag_Adjust))
                    {
                        unset(inventory->flags, InventoryFlag_Adjust);
                            log_add_okay(ui);
                        }
                    else if(is_set(inventory->flags, InventoryFlag_Examine))
                    {
                        unset(inventory->flags, InventoryFlag_Examine);
                        }
                    else if(inventory->interact_type)
                    {
                            ask_for_item_cancel(game->keybinds, input, inventory, ui);
                        }
                        else if(is_set(inventory->flags, InventoryFlag_Open))
                    {
                        unset(inventory->flags, InventoryFlag_Open | InventoryFlag_ReadyForKeypress);
                    }
                    else if(is_set(inventory->flags, InventoryFlag_MultiplePickup))
                    {
                        reset_inventory_multiple_pickup(items, &inventory->flags, dungeon->level, ui->temp_owners);
                    }
                    else if(is_set(inventory->flags, InventoryFlag_MultipleExamine))
                    {
                        reset_all_owner_select_letters(ui->temp_owners);
                        unset(inventory->flags, InventoryFlag_MultipleExamine);
                    }
                        else if(ui->full_log_open)
                    {
                            ui->full_log_open = false;
                    }
                    else
                    {
                        game->mode = GameMode_Quit;
                    }
                    }
                }
                else if(was_pressed(&input->GameKey_Pickup))
            {
                if(other_windows_are_closed(examine, inventory, ui))
                {
                    if(get_dungeon_pos_item_count(items, dungeon->level, player->pos) > 1)
                    {
                        set(inventory->flags, InventoryFlag_MultiplePickup);
                        set_view_at_start_and_reset_view_moves(&items->pickup_window.view);
                        }
                    else
                    {
                        // Pickup item
                        Item *item = get_dungeon_pos_item(items, dungeon->level, player->pos, 0);
                        
                        if(item)
                        {
                            add_item_to_inventory(game, player, item, items, inventory, ui, dungeon->level, true);
                        }
                        else
                        {
                            log_add("You see nothing to pick up here.", ui);
                        }
                    }
                    }
                }
            else if(was_pressed(&input->GameKey_UsePassage))
                {
                if(other_windows_are_closed(examine, inventory, ui))
                {
                    if(is_dungeon_pos_tile(dungeon->tiles, player->pos, DungeonTileID_ExitDungeon))
                    {
                        game->mode = GameMode_Quit;
                    }
                    else if(is_dungeon_pos_tile(dungeon->tiles, player->pos, DungeonTileID_StoneStaircaseUp) ||
                                is_dungeon_pos_tile(dungeon->tiles, player->pos, DungeonTileID_StoneStaircaseDown))
                    {
                        DungeonPassage *used_passage = get_dungeon_pos_passage(&dungeon->passages, player->pos);
                        entity_use_passage(game, player, entities, dungeons, items, inventory, ui, used_passage);
                        
                        game->should_update = true;
                        game->passed_time = player->p.action_time * 3.0f;
                    }
                    }
                }
            else if(was_pressed(&input->GameKey_AutoExplore) &&
                    other_windows_are_closed(examine, inventory, ui))
            {
                // Start automatic player pathfind
                    if(can_player_auto_explore(&examine->flags, entities, dungeon, ui))
                    {
                        if(entity_can_move(player, ui))
                        {
                            assert(!is_set(player->flags, EntityFlag_Pathfinding));
                            
                            b32 pathfind_target_pos_set = false;
                            v2u pathfind_target_pos = {0};
                            
                            while(is_dungeon_explorable(dungeon))
                            {
                            pathfind_target_pos = get_random_dungeon_pos(random, dungeon->size);
                            
                                if(is_pos_pathfindable(dungeon, pathfind_target_pos) &&
                                   !tile_has_been_seen(dungeon->tiles, pathfind_target_pos))
                            {
                                pathfind_target_pos_set = true;
                                    break;
                                }
                            }
                            
                            if(pathfind_target_pos_set)
                            {
                                make_entity_pathfind(player, items, dungeon, &entities->player_pathfind_map, pathfind_target_pos);
                            }
                            else
                            {
                                log_add("There's nothing more to explore.", ui);
                            }
                        }
                    }
                }
            else if(was_pressed(&input->GameKey_Examine) &&
                        other_windows_are_closed(examine, inventory, ui))
                {
                    set(examine->flags, ExamineFlag_Open | ExamineFlag_CameraOnExamine);
                    examine->pos = player->pos;
                }
            else if(was_pressed(&input->GameKey_Log) &&
                        other_windows_are_closed(examine, inventory, ui))
                {
                    ui->full_log_open = true;
                    ui->start_full_log_from_end = true;
                    
                    set_view_at_start_and_reset_view_moves(&ui->full_log.view);
                }
            else if(was_pressed(&input->GameKey_Wait) &&
                        other_windows_are_closed(examine, inventory, ui))
            {
                    log_add("%sYou wait for a moment.", ui, start_color(Color_LightGray));
                        
                        game->should_update = true;
                game->passed_time = player->p.action_time;
                    unset(player->flags, EntityFlag_NotifyAboutMultipleItems);
            }
            else if(was_pressed(&input->GameKey_Rest) &&
                        other_windows_are_closed(examine, inventory, ui))
            {
                if(get_player_view_enemy_count(entities, dungeon))
                {
                    log_add_enemies_nearby(ui);
                }
                else if(player->hp < player->max_hp)
                {
                    game->passed_time = player->p.action_time;
                    
                        log_add("%sYou take a rest.", ui, start_color(Color_LightGray));
                    unset(player->flags, EntityFlag_NotifyAboutMultipleItems);
                    
                        for(;;)
                    {
                        game->passed_time += player->p.action_time;
                            
                            b32 should_break = update_entity_regen(player);
                            update_player_entity_statuses(game, &entities->enemy_pathfind_map, player, inventory, dungeon, ui);
                            update_enemy_entities(game, player, entities, items, inventory, dungeon, ui);
                            
                            // If max HP is reached via regen and we have enemies in view then we still want
                            // to add the new enemy message to the log.
                        if(log_add_player_view_has_enemies(entities, dungeon, ui) || should_break) break;
                        }
                }
                else
                {
                    log_add("You don't have a need to rest.", ui);
                }
                }
                else if(input->Button_ScrollUp.is_down ||
                        input->Button_ScrollDown.is_down ||
                        input->Key_Home.is_down ||
                        input->Key_End.is_down)
            {
                if(is_set(inventory->flags, InventoryFlag_Mark))
                {
                    Mark *mark = &items->temp_mark;
                    
                    if(was_pressed(&input->Key_Home) && mark->cursor)
                    {
                        // Set mark at start
                        mark->cursor = 0;
                        set_view_at_start(&mark->view);
                        
                        force_render_mark_cursor(mark);
                    }
                    else if(was_pressed(&input->Key_End) && (mark->cursor < mark->view.count))
                    {
                        // Set mark at end
                        mark->cursor = mark->view.count;
                        set_view_at_end(&mark->view);
                        
                        force_render_mark_cursor(mark);
                        }
                }
                else if(inventory->interact_type)
                {
                    update_view_scroll(input, &inventory->interact_window.view);
                }
                    else if(is_set(inventory->flags, InventoryFlag_Open))
                {
                    update_view_scroll(input, &inventory->window.view);
                }
                else if(is_set(inventory->flags, InventoryFlag_MultiplePickup))
                {
                    update_view_scroll(input, &items->pickup_window.view);
                }
                else if(is_set(inventory->flags, InventoryFlag_MultipleExamine))
                {
                    update_view_scroll(input, &items->examine_window.view);
                }
                    else if(ui->full_log_open)
                {
                    update_view_scroll(input, &ui->full_log.view);
                    }
                }
            else if(is_set(inventory->flags, InventoryFlag_Open) ||
                        is_set(inventory->flags, InventoryFlag_MultiplePickup) ||
                        is_set(inventory->flags, InventoryFlag_MultipleExamine))
            {
                Item *examine_item = inventory->examine_item;
                
                if(is_set(inventory->flags, InventoryFlag_Examine))
                    {
                        if(is_set(inventory->flags, InventoryFlag_Mark))
                        {
                        update_item_mark(input, &items->temp_mark, examine_item, &inventory->flags);
                        }
                        else
                        {
                            if(is_set(inventory->flags, InventoryFlag_Adjust))
                            {
                            update_item_adjust(input, examine_item, items, inventory, dungeon->level, ui);
                            }
                            else
                            {
                                if(was_pressed(&input->Key_A))
                                {
                                set(inventory->flags, InventoryFlag_Adjust);
                                log_add("%sAdjust to which letter? (%s to cancel)", ui, start_color(Color_LightYellow), get_printable_key(input, game->keybinds[GameKey_Back]).s);
                                }
                                else if(was_pressed(&input->Key_E))
                                {
                                equip_item(game, player, examine_item, inventory, ui, true);
                                }
                            else if(was_pressed(&input->Key_U) &&
                                        unequip_item(game, player, examine_item, ui, false))
                                {
                                log_add_item_action_string(examine_item, ui, ItemActionType_Unequip);
                                }
                            else if((was_pressed(&input->Key_C) ||
                                     was_pressed(&input->Key_R)) &&
                                        is_item_consumable(examine_item->type))
                            {
                                // Consume item
                                if(examine_item->type != ItemType_Ration)
                                {
                                    set_potion_or_scroll_as_known(items, examine_item->id);
                                }
                                
                                if(examine_item->type == ItemType_Scroll)
                                {
                                    switch(examine_item->id)
                                    {
                                        case ItemID_IdentifyScroll:
                                        {
                                            log_add("You read the %s, choose an item to identify.", ui, examine_item->name.s);
                                            inventory->interact_type = InteractType_Identify;
                                        } break;
                                        
                                        case ItemID_EnchantWeaponScroll:
                                        {
                                            log_add("You read the %s, choose a weapon to enchant.", ui, examine_item->name.s);
                                            inventory->interact_type = InteractType_EnchantWeapon;
                                        } break;
                                        
                                        case ItemID_EnchantArmorScroll:
                                        {
                                            log_add("You read the %s, choose an armor to enchant.", ui, examine_item->name.s);
                                            inventory->interact_type = InteractType_EnchantArmor;
                                        } break;
                                        
                                        case ItemID_MagicMappingScroll:
                                        {
                                            log_add("You read the %s, your surroundings become clear to you.", ui, examine_item->name.s);
                                            
                                            for(u32 y = 0; y < dungeon->size.h; ++y)
                                            {
                                                for(u32 x = 0; x < dungeon->size.w; ++x)
                                                {
                                                    set_tile_has_been_seen(dungeon->tiles, make_v2u(x, y), true);
                                                }
                                            }
                                        } break;
                                        
                                        case ItemID_TeleportationScroll:
                                        {
                                            log_add("You read the %s, you find yourself in a different place.", ui, examine_item->name.s);
                                            teleport_entity(random, &entities->enemy_pathfind_map, player, dungeon, ui, DungeonRandomPosType_Traversable);
                                        } break;
                                        
                                        case ItemID_UncurseScroll:
                                        {
                                            log_add("You read the %s, choose an item to uncurse.", ui, examine_item->name.s);
                                            inventory->interact_type = InteractType_Uncurse;
                                        } break;
                                        
                                        invalid_default_case;
                                    }
                                }
                                
                                // Initialize interact window after interact_type has been set.
                                if(inventory->interact_type)
                                {
                                    set_view_at_start_and_reset_view_moves(&inventory->interact_window.view);
                                }
                                
                                // Add status from item if it has one
                                Status *item_status = &examine_item->c.status;
                                if(item_status->type)
                                {
                                    add_entity_status(random, player, dungeon, ui, item_status);
                                }
                                
                                end_consumable_use(examine_item, items, inventory, dungeon->level);
                                
                                game->passed_time = player->p.action_time;
                                unset(inventory->flags, InventoryFlag_Examine);
                                }
                                else if(was_pressed(&input->Key_D))
                            {
                                drop_item_from_inventory(game, player, examine_item, items, inventory, dungeon, ui);
                                }
                                else if(was_pressed(&input->Key_M))
                                {
                                // Start item marking
                                set(inventory->flags, InventoryFlag_Mark);
                                Mark *temp_mark = &items->temp_mark;
                                
                                if(is_set(examine_item->flags, ItemFlag_IsMarked))
                                {
                                    assert(!examine_item->mark.render_cursor);
                                    assert(!examine_item->mark.cursor);
                                    assert(!examine_item->mark.render_cursor_start_ms);
                                    
                                    temp_mark->view = examine_item->mark.view;
                                    strcpy(temp_mark->array, examine_item->mark.array);
                                    
                                    move_mark_to_end(temp_mark);
                                    assert(temp_mark->view.count);
                                }
                                else
                                {
                                    move_mark_to_start(temp_mark);
                                }
                                }
                            }
                        }
                    }
                    else
                {
                    char pressed = get_pressed_alphabet_char(input);
                    
                    if(is_set(inventory->flags, InventoryFlag_Open))
                    {
                            if(is_set(inventory->flags, InventoryFlag_ReadyForKeypress))
                            {
                                if(pressed)
                            {
                                // Inventory item interact
                                
                                if(inventory->interact_type)
                                {
                                    for(u32 index = 0; index < MAX_INVENTORY_SLOT_COUNT; ++index)
                                    {
                                        Item *item = inventory->slots[index];
                                        
                                        if(item &&
                                           item->inventory_letter == pressed &&
                                               item_fits_interact_type(item, inventory->interact_type))
                                        {
                                            assert(is_item_valid_and_in_inventory(item));
                                            process_inventory_item_interact(random, inventory, item, ui);
                                            }
                                    }
                                    }
                                    else
                                {
                                    // Inventory item examine
                                    
                                    Owner *owner = get_owner_from_letter(inventory->item_owners, pressed);
                                    if(owner && owner->type)
                                    {
                                        assert(owner->type == OwnerType_Item);
                                        assert(is_item_valid_and_in_inventory(owner->item));
                                        
                                        start_item_examine(inventory, owner->item);
                                    }
                                    }
                                }
                        }
                            else
                        {
                                set(inventory->flags, InventoryFlag_ReadyForKeypress);
                            }
                        }
                    else if(is_set(inventory->flags, InventoryFlag_MultiplePickup))
                    {
                        if(was_pressed(&input->Key_Enter))
                        {
                            // Add selected items to inventory
                            
                            for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
                            {
                                Item *item = &items->array[index];
                                
                                if(is_item_valid_and_selected(item) &&
                                       !add_item_to_inventory(game, player, item, items, inventory, ui, dungeon->level, true))
                                {
                                        break;
                                }
                            }
                            
                            reset_inventory_multiple_pickup(items, &inventory->flags, dungeon->level, ui->temp_owners);
                            }
                            else if(pressed)
                        {
                            // Toggle if an item is selected in the pickup window
                            
                            Owner *owner = get_owner_from_letter(ui->temp_owners, pressed);
                            if(owner->type)
                            {
                                assert(owner->type == OwnerType_Item);
                                toggle(owner->item->flags, ItemFlag_IsSelected);
                            }
                            }
                    }
                    else if(is_set(inventory->flags, InventoryFlag_MultipleExamine) &&
                                pressed &&
                                !examine->type)
                    {
                        // Item examine from multiple examine window
                        
                        if(is_set(inventory->flags, InventoryFlag_ReadyForKeypress))
                        {
                            Owner *owner = get_owner_from_letter(ui->temp_owners, pressed);
                            if(owner->type)
                            {
                                start_examine_from_owner(examine, owner);
                            }
                        }
                            else
                        {
                            set(inventory->flags, InventoryFlag_ReadyForKeypress);
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
                Dungeons *dungeons,
                Assets *assets,
UI *ui)
{
    assert(game);
    assert(input);
    assert(entities);
    assert(items);
    assert(inventory);
    assert(dungeons);
    assert(assets);
    assert(ui);
    
    Random *random = &game->random;
    Dungeon *dungeon = get_dungeon_from_level(dungeons, dungeons->current_level);
    
    // Update player
    Entity *player = get_player_entity();
    assert(is_player_entity_valid(player));
    
    update_player_input(game, input, player, entities, items, inventory, dungeons, assets, ui);
    
    // Update defence and weight
    player->def = 0;
    player->p.weight = 0;
    
    for(u32 slot_index = ItemSlot_None + 1; slot_index < ItemSlot_Count; ++slot_index)
    {
        for(u32 inventory_index = 0; inventory_index < MAX_INVENTORY_SLOT_COUNT; ++inventory_index)
        {
            Item *item = inventory->slots[inventory_index];
            
            if(item &&
                   is_set(item->flags, ItemFlag_IsEquipped) &&
               item->slot == slot_index &&
               item->type == ItemType_Armor)
            {
                player->def += (item->a.defence + item->enchant_level);
                player->p.weight += item->a.weight;
            }
        }
    }
    
    if(is_entity_under_status(player, StatusType_BrokenArmor)) player->def /= 2;
    player->ev = 10 - (player->p.weight / player->p.weight_to_evasion_ratio);
    player->ev += get_total_stat_status_value(player->statuses, StatType_EV);
    
    if(game->should_update)
    {
        player->direction = get_direction_moved_from(player->pos, player->new_pos);
        
#if MOONBREATH_SLOW
        if(fkey_active[2]) force_move_entity(&entities->enemy_pathfind_map, player, dungeon, player->new_pos, dungeon->level);
        
        else
#endif
        
        if(!is_v2u_equal(player->pos, player->new_pos))
        {
            if(is_dungeon_pos_occupied(dungeon->tiles, player->new_pos))
            {
                for(u32 target_index = 0; target_index < MAX_ENTITY_COUNT; ++target_index)
                {
                    Entity *target = &entities->array[target_index];
                    
                    if(target->type == EntityType_Enemy &&
                       is_v2u_equal(player->new_pos, target->pos))
                    {
                        // These are the unarmed defaults
                        DamageInfo damage = {0};
                        damage.min = 1;
                        damage.max = 2;
                        damage.type = DamageType_Physical;
                        u32 accuracy = 1;
                        f32 attack_speed = 1.0f;
                        
                        Item *weapon = player->p.weapon;
                        if(weapon)
                        {
                            assert(weapon->type == ItemType_Weapon);
                            
                            damage.max = weapon->w.damage.max + weapon->enchant_level;
                            damage.type = weapon->w.damage.type;
                            accuracy = weapon->w.accuracy + weapon->enchant_level;
                            attack_speed = weapon->w.speed;
                            
                            assert(is_damage_valid(damage));
                        }
                        
                        player->hit_chance = 15 + (player->dex / 2);
                        player->hit_chance += accuracy;
                        
                        // Apply player strength to damage
                        if(player->str < 10)
                        {
                            damage.max -= (10 - player->str);
                        }
                        else
                        {
                            damage.max += (player->str - 10);
                        }
                        
                        set(target->flags, EntityFlag_InCombat);
                        attack_entity(&game->random, &entities->enemy_pathfind_map, player, target, dungeon, ui, damage);
                        
                        game->passed_time = attack_speed;
                    }
                }
            } 
            else
            {
                if(is_dungeon_pos_closed_door(dungeon->tiles, player->new_pos))
                {
                    log_add("You open the door.", ui);
                    set_dungeon_pos_open_door(dungeon->tiles, player->new_pos);
                    
                    game->passed_time = player->p.action_time;
                }
                else if(is_dungeon_pos_traversable(dungeon->tiles, player->new_pos))
                {
                    if(is_dungeon_pos_water(dungeon->tiles, player->pos))
                    {
                        log_add("You wade through the water.", ui);
                        game->passed_time = player->p.action_time * 2.0f;
                    }
                    else
                    {
                        game->passed_time = player->p.action_time;
                    }
                    
                        //assert(0);
                    if(move_entity(random, &entities->enemy_pathfind_map, player, dungeon, ui, player->new_pos))
                    {
                        DungeonSpec *spec = &dungeon->spec;
                        
                        // Player steps on a dungeon trap
                    if(is_dungeon_pos_trap(&dungeon->traps, player->new_pos))
                    {
                        DungeonTrap *trap = get_dungeon_pos_trap(dungeon->tiles, &dungeon->traps, player->new_pos);
                        switch(trap->type)
                        {
                            case DungeonTrapType_Spike:
                                {
                                    u32 damage = attack_entity(&game->random, &entities->enemy_pathfind_map, 0, player, dungeon, ui, spec->spike_trap_damage);
                                    log_add("Sharp spikes pierce you, %u damage.", ui, damage);
                            } break;
                            
                            case DungeonTrapType_Sword:
                            {
                                    u32 damage = attack_entity(&game->random, &entities->enemy_pathfind_map, 0, player, dungeon, ui, spec->sword_trap_damage);
                                    log_add("Swords lacerate you, %u damage.", ui, damage);
                                } break;
                            
                            case DungeonTrapType_Arrow:
                            {
                                    u32 damage = attack_entity(&game->random, &entities->enemy_pathfind_map, 0, player, dungeon, ui, spec->arrow_trap_damage);
                                    log_add("Multiple arrows fire at you, %u damage.", ui, damage);
                            } break;
                            
                            case DungeonTrapType_Magic:
                                {
                                    u32 damage = attack_entity(&game->random, &entities->enemy_pathfind_map, 0, player, dungeon, ui, spec->magic_trap_damage);
                                    log_add("A magical trap explodes below you, %u %s damage.", ui, damage, get_damage_type_string(spec->magic_trap_damage.type));
                            } break;
                            
                            case DungeonTrapType_Bind:
                                {
                                        Status bind_status = {0};
                                    bind_status.type = StatusType_Bind;
                                    bind_status.duration = get_random_from_v2u(random, spec->bind_trap_value);
                                    
                                    add_entity_status(random, player, dungeon, ui, &bind_status);
                            } break;
                            
                            case DungeonTrapType_Shaft:
                            {
                                assert(!is_set(player->flags, EntityFlag_Pathfinding));
                                
                                log_add("You fall into a shaft!", ui);
                                
                                if(!trap->is_shaft_set)
                                { 
                                    trap->shaft_depth = get_random_from_v2u(random, spec->shaft_trap_value);
                                    u32 shaft_dungeon_level = dungeons->current_level + trap->shaft_depth;
                                        
                                        Dungeon *shaft_dungeon = get_dungeon_from_level(dungeons, shaft_dungeon_level);
                                        trap->shaft_dest = get_random_dungeon_traversable_unoccupied_pos(random, shaft_dungeon);
                                        move_entity_between_dungeons(game, player, entities, dungeons, items, inventory, ui, trap->shaft_dest, shaft_dungeon_level);
                                        
                                        trap->is_shaft_set = true;
                                        
                                        //printf("Set shaft data - Depth: %u - Destination: %u, %u.\n", trap->shaft_depth, trap->shaft_dest.x, trap->shaft_dest.y);
                                }
                            } break;
                            
                            case DungeonTrapType_Summon:
                            {
                                    summon_entity(random, entities, player, dungeon, ui, 0);
                            } break;
                            
                            case DungeonTrapType_Teleport:
                            {
                                log_add("You take a step and find yourself in a different place!", ui);
                                    teleport_entity(random, &entities->enemy_pathfind_map, player, dungeon, ui, DungeonRandomPosType_Traversable);
                            } break;
                            
                            invalid_default_case;
                            }
                    }
                    }
                }
            }
        }
    }
    
    if(game->passed_time)
    {
        game->time += game->passed_time;
        player->action_time = game->passed_time;
        
        update_entity_regen(player);
        update_player_entity_statuses(game, &entities->enemy_pathfind_map, player, inventory, dungeon, ui);
        
        // Notify about multiple items
        if(other_windows_are_closed(&game->examine, inventory, ui) &&
           is_set(player->flags, EntityFlag_NotifyAboutMultipleItems) &&
               get_dungeon_pos_item_count(items, dungeon->level, player->pos) > 1)
        {
                log_add("There are multiple items here.", ui);
        }
        
        update_player_view(player, dungeon);
        }
    
    update_enemy_entities(game, player, entities, items, inventory, dungeon, ui);
    }

internal void
render_entities(Game *game,
                EntityState *entities,
                Inventory *inventory,
                 Dungeon *dungeon,
                Assets *assets)
{
    assert(game);
    assert(entities);
    assert(inventory);
    assert(dungeon);
    assert(assets);
    
        { // Render player
        Entity *player = get_player_entity();
        assert(is_player_entity_valid(player));
        v4u player_dest = render_game_dest_tile(game, assets->tileset.tex, player->tile_src, player->pos, false);
        
            // Render equipped player items
            for(u32 slot_index = ItemSlot_None + 1; slot_index < ItemSlot_Count; ++slot_index)
            {
                for(u32 inventory_index = 0; inventory_index < MAX_INVENTORY_SLOT_COUNT; ++inventory_index)
                {
                Item *item = inventory->slots[inventory_index];
                
                if(item &&
                       is_set(item->flags, ItemFlag_IsEquipped) &&
                       item->slot == slot_index)
                    {
                    SDL_RenderCopy(game->renderer, assets->tileset.tex, (SDL_Rect *)&item->equip_tile_src, (SDL_Rect *)&player_dest);
                        break;
                    }
                }
            }
        
        if(does_entity_have_any_status(player))
        {
            render_tile_on_dest(game->renderer, assets->tileset.tex, player_dest, DungeonTileID_EntityStatus);
        }
        
            // Render player pathfind trail
            if(player->p.render_pathfind_trail)
            {
                for(u32 index = 0; index < MAX_PATHFIND_TRAIL_COUNT; ++index)
                {
                    PathfindTrail *trail = &player->p.pathfind_trail[index];
                    if(!is_v2u_zero(trail->pos))
                    {
                    DungeonTileID trail_id = DungeonTileID_None;
                    switch(trail->direction)
                        {
                        case Direction_Up: trail_id = DungeonTileID_FootstepsUp; break;
                        case Direction_Down: trail_id = DungeonTileID_FootstepsDown; break;
                        case Direction_Left: trail_id = DungeonTileID_FootstepsLeft; break;
                        case Direction_Right: trail_id = DungeonTileID_FootstepsRight; break;
                            
                        case Direction_UpLeft: trail_id = DungeonTileID_FootstepsUpLeft; break;
                        case Direction_UpRight: trail_id = DungeonTileID_FootstepsUpRight; break;
                        case Direction_DownLeft: trail_id = DungeonTileID_FootstepsDownLeft; break;
                        case Direction_DownRight: trail_id = DungeonTileID_FootstepsDownRight; break;
                            
                            invalid_default_case;
                        }
                    
                    v4u trail_src = get_dungeon_tileset_rect(trail_id);
                    v4u trail_dest = get_game_dest(game->camera, trail->pos);
                    if(is_tile_seen(dungeon->tiles, trail->pos))
                    {
                        SDL_RenderCopy(game->renderer, assets->tileset.tex, (SDL_Rect *)&trail_src, (SDL_Rect *)&trail_dest);
                    }
                    else if(tile_has_been_seen(dungeon->tiles, trail->pos))
                    {
                        render_texture_half_color(game->renderer, assets->tileset.tex, trail_src, trail_dest, false);
                    }
                    }
                }
        }
        }
        
        { // Render enemies
            for(u32 enemy_index = 0; enemy_index < MAX_ENTITY_COUNT; ++enemy_index)
            {
            Entity *enemy = &entities->array[enemy_index];
            
            if(is_enemy_entity_valid(enemy) &&
                enemy->dungeon_level == dungeon->level)
            {
                if(!is_set(enemy->flags, EntityFlag_Invisible))
            {
                if(is_tile_seen(dungeon->tiles, enemy->pos))
                {
                    set(enemy->flags, EntityFlag_HasBeenSeen);
                        unset(enemy->flags, EntityFlag_GhostEnabled);
                        v4u dest = render_game_dest_tile(game, assets->tileset.tex, enemy->tile_src, enemy->pos, is_set(enemy->flags, EntityFlag_Flipped));
                        
                    if(enemy->e.turns_in_player_view == 1)
                        {
                            render_tile_on_dest(game->renderer, assets->tileset.tex, dest, DungeonTileID_EntityAlerted);
                        }
                        else if(does_entity_have_any_status(enemy))
                        {
                                render_tile_on_dest(game->renderer, assets->tileset.tex, dest, DungeonTileID_EntityStatus);
                    }
                    
                        if(enemy->e.turns_in_player_view != 1 &&
                           is_set(enemy->flags, EntityFlag_InCombat))
                        {
                            v4u new_dest = dest;
                            new_dest.w = ratio(enemy->hp, enemy->max_hp, TILE_SIZE);
                            
                            u32 health_per_section = f32_to_u32_rounded_up((f32)enemy->max_hp / (f32)HEALTH_BAR_SECTION_COUNT);
                            u32 current_section = (enemy->hp / health_per_section);
                            
                            if(current_section == HEALTH_BAR_SECTION_COUNT) --current_section;
                            
                            #if 0
                            printf("enemy->hp: %u\n", enemy->hp);
                            printf("enemy->max_hp: %u\n", enemy->max_hp);
                            printf("health_per_section: %u\n", health_per_section);
                            printf("current_section: %u\n\n", current_section);
#endif
                            
                            SDL_RenderCopy(game->renderer, assets->tileset.tex, (SDL_Rect *)&assets->health_bar_sources[current_section], (SDL_Rect *)&new_dest);
                    }
                }
                else
                    {
                        // Render enemy ghost
                    if(is_set(enemy->flags, EntityFlag_HasBeenSeen))
                    {
                            if(is_set(enemy->flags, EntityFlag_GhostEnabled))
                        {
                            if(is_tile_seen(dungeon->tiles, enemy->e.ghost_pos))
                            {
                                    unset(enemy->flags, EntityFlag_HasBeenSeen | EntityFlag_GhostEnabled);
                            }
                            else
                                {
                                    // Render enemy ghost in half color
                                    render_texture_half_color(game->renderer, assets->tileset.tex,
                                                                  enemy->tile_src, get_game_dest(game->camera, enemy->e.ghost_pos),
                                                                  is_set(enemy->flags, EntityFlag_GhostFlipped));
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
                            
                                set(enemy->flags, EntityFlag_GhostEnabled);
                            
                            if(enemy->e.saved_flipped_for_ghost)
                            {
                                    set(enemy->flags, EntityFlag_GhostFlipped);
                            }
                            else
                            {
                                    unset(enemy->flags, EntityFlag_GhostFlipped);
                            }
                        }
                    }
                }
                }
                
#if MOONBREATH_SLOW
                // Render enemy view rect
                if(is_set(enemy->flags, EntityFlag_ShowViewRange))
                {
                    //print_v4u(enemy->e.view_rect);
                    
                    for(u32 y = enemy->e.view_rect.y; y < get_rect_height(enemy->e.view_rect); ++y)
                    {
                        for(u32 x = enemy->e.view_rect.x; x < get_rect_width(enemy->e.view_rect); ++x)
                        {
                            v2u pos = {x, y};
                            v4u rect = get_game_dest(game->camera, pos);
                            render_fill_rect(game->renderer, rect, Color_EntityView, true);
                        }
                    }
                }
                #endif
                
            }
            }
            }
}

internal void
add_player_entity(Entity *player)
{
    assert(player);
    
    player->id = EntityID_Player;
    strcpy(player->name.s, "Player");
    player->max_hp = player->hp = 40;
    //player->max_hp = player->hp = 1000;
    //player->hp = player->max_hp / 2;
    //player->hp -= 6;
    //player->hp = 1;
    player->dungeon_level = 1;
    player->width = player->height = 32;
    player->tile_src = get_dungeon_tile_rect(get_entity_tile_pos(player->id));
    player->remains = EntityRemains_RedBlood;
    player->type = EntityType_Player;
    player->str = 10;
    player->intel = 10;
    player->dex = 10;
    player->ev = 10;
    player->view_range = 8;
    
    EntityRegen *regen = &player->regen;
    regen->hp_increase = 1;
    regen->max = 100;
    regen->advance = 13;
    
    player->p.action_time = 1.0f;
    player->p.weight_to_evasion_ratio = 3;
    
    //player->max_hp = 10000;
    //player->resistances[DamageType_Fire] = 5;
    //player->resistances[DamageType_Poison] = 5;
}

internal Entity *
add_enemy_entity(EntityState *entities,
                 Dungeon *dungeon,
                 EntityID id,
                 u32 x,
                 u32 y)
{
    assert(entities);
    assert(dungeon);
    assert(id);
    assert(x);
    assert(y);
    
    Entity *result = 0;
    
    for(u32 index = 0; index < MAX_ENTITY_COUNT; ++index)
    {
        Entity *enemy = &entities->array[index];
        if(!enemy->type)
        {
            result = enemy;
            
            enemy->flags = get_entity_flags(id);
            enemy->id = id;
            enemy->new_pos = enemy->pos = make_v2u(x, y);
            enemy->dungeon_level = dungeon->level;
            enemy->width = enemy->height = 32;
            enemy->tile_src = get_dungeon_tile_rect(get_entity_tile_pos(id));
            enemy->type = EntityType_Enemy;
            enemy->hit_chance = 30;
            enemy->view_range = 8;
            enemy->e.level = get_entity_level(id);
            
            force_move_entity(&entities->enemy_pathfind_map, enemy, dungeon, enemy->new_pos, dungeon->level);
            
            switch(id)
            {
                case EntityID_Dummy:
                {
                    // This is a dummy entity for testing purposes
                    strcpy(enemy->name.s, "Dummy");
                    enemy->max_hp = enemy->hp = U32_MAX;
                    
                    //enemy->ev = 22;
                } break;
                
                // TODO(rami): Play the game, change data.
                
                case EntityID_SkeletonWarrior:
                {
                    strcpy(enemy->name.s, "Skeleton Warrior");
                    enemy->max_hp = enemy->hp = 15;
                    
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 5;
                    enemy->e.damage.type = DamageType_Physical;
                    enemy->ev = 8;
                    enemy->action_time = 1.0f;
                    
                    enemy->resistances[DamageType_Dark] = 1;
                    enemy->resistances[DamageType_Holy] = -1;
                } break;
                
                case EntityID_SkeletonArcher:
                {
                    strcpy(enemy->name.s, "Skeleton Archer");
                    enemy->max_hp = enemy->hp = 15;
                    
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 4;
                    enemy->e.damage.type = DamageType_Physical;
                    enemy->ev = 7;
                    enemy->action_time = 1.0f;
                    
                    enemy->resistances[DamageType_Dark] = 1;
                    enemy->resistances[DamageType_Holy] = -1;
                } break;
                
                case EntityID_SkeletonMage:
                {
                    strcpy(enemy->name.s, "Skeleton Mage");
                    enemy->max_hp = enemy->hp = 15;
                    
                    enemy->ev = 7;
                    enemy->action_time = 1.0f;
                    
                    enemy->resistances[DamageType_Dark] = 1;
                    enemy->resistances[DamageType_Holy] = -1;
                    
                    add_entity_attack_spell(enemy, "Dark Bolt", 4, DamageType_Dark, 100, enemy->view_range);
                } break;
                
                case EntityID_Bat:
                {
                    strcpy(enemy->name.s, "Bat");
                    enemy->max_hp = enemy->hp = 10;
                    
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 2;
                    enemy->e.damage.type = DamageType_Physical;
                    enemy->ev = 15;
                    enemy->action_time = 0.3f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_Rat:
                {
                    strcpy(enemy->name.s, "Rat");
                    enemy->max_hp = enemy->hp = 12;
                    
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 2;
                    enemy->e.damage.type = DamageType_Physical;
                    enemy->ev = 11;
                    enemy->action_time = 0.5f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_KoboldWarrior:
                {
                    strcpy(enemy->name.s, "Kobold Warrior");
                    enemy->max_hp = enemy->hp = 20;
                    
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 5;
                    enemy->e.damage.type = DamageType_Physical;
                    enemy->ev = 8;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_KoboldShaman:
                {
                    strcpy(enemy->name.s, "Kobold Shaman");
                    enemy->max_hp = enemy->hp = 20;
                    
                    enemy->ev = 7;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                    
                    add_entity_heal_spell(enemy, "Healing Touch", 6, 40, enemy->view_range);
                    add_entity_attack_spell(enemy, "Lightning Whip", 4, DamageType_Lightning, 60, enemy->view_range);
                } break;
                
                case EntityID_Snail:
                {
                    strcpy(enemy->name.s, "Snail");
                    enemy->max_hp = enemy->hp = 28;
                    
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 10;
                    enemy->e.damage.type = DamageType_Physical;
                    enemy->ev = 3;
                    enemy->action_time = 2.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_Slime:
                {
                    strcpy(enemy->name.s, "Slime");
                    enemy->max_hp = enemy->hp = 20;
                    
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 4;
                    enemy->e.damage.type = DamageType_Physical;
                    enemy->ev = 4;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_GreenBlood;
                    
                    enemy->resistances[DamageType_Physical] = 1;
                } break;
                
                case EntityID_Dog:
                {
                    strcpy(enemy->name.s, "Dog");
                    enemy->max_hp = enemy->hp = 18;
                    
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 3;
                    enemy->e.damage.type = DamageType_Physical;
                    enemy->ev = 9;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_OrcWarrior:
                {
                    strcpy(enemy->name.s, "Orc Warrior");
                    enemy->max_hp = enemy->hp = 26;
                    
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 8;
                    enemy->e.damage.type = DamageType_Physical;
                    enemy->ev = 8;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_OrcArcher:
                {
                    strcpy(enemy->name.s, "Orc Archer");
                    enemy->max_hp = enemy->hp = 26;
                    
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 6;
                    enemy->e.damage.type = DamageType_Physical;
                    enemy->ev = 7;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_OrcShaman:
                {
                    strcpy(enemy->name.s, "Orc Shaman");
                    enemy->max_hp = enemy->hp = 26;
                    
                    enemy->ev = 7;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                    
                    enemy->resistances[DamageType_Poison] = 5;
                    
                    add_entity_stat_spell(enemy, "Protective Shout", StatType_Def, 3, 5, 30, enemy->view_range);
                    add_entity_attack_spell(enemy, "Lightning Whip", 6, DamageType_Lightning, 70, enemy->view_range);
                } break;
                
                case EntityID_Python:
                {
                    strcpy(enemy->name.s, "Python");
                    enemy->max_hp = enemy->hp = 15;
                    
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 4;
                    enemy->e.damage.type = DamageType_Physical;
                    enemy->ev = 11;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                    
                    enemy->resistances[DamageType_Poison] = 2;
                    
                    add_entity_attack_status(enemy, StatusType_Poison, 2, 8, 100);
                } break;
                
                case EntityID_Shade:
                {
                    strcpy(enemy->name.s, "Shade");
                    enemy->max_hp = enemy->hp = 18;
                    
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 3;
                    enemy->e.damage.type = DamageType_Dark;
                    enemy->ev = 14;
                    enemy->action_time = 1.0f;
                    
                    enemy->resistances[DamageType_Dark] = 1;
                } break;
                
                case EntityID_ElfKnight:
                {
                    strcpy(enemy->name.s, "Elf Knight");
                    enemy->max_hp = enemy->hp = 32;
                    
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 12;
                    enemy->e.damage.type = DamageType_Physical;
                    enemy->def = 3;
                    enemy->ev = 2;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_ElfArbalest:
                {
                    strcpy(enemy->name.s, "Elf Arbalest");
                    enemy->max_hp = enemy->hp = 28;
                    
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 16;
                    enemy->e.damage.type = DamageType_Physical;
                    enemy->ev = 4;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_ElfMage:
                {
                    strcpy(enemy->name.s, "Elf Mage");
                    enemy->max_hp = enemy->hp = 28;
                    
                    enemy->ev = 9;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                    
                    add_entity_stat_spell(enemy, "Weaken", StatType_Str, -3, 8, 20, enemy->view_range);
                    add_entity_attack_spell(enemy, "Fire Lance", 6, DamageType_Fire, 60, enemy->view_range);
                    add_entity_attack_spell(enemy, "Frost Nova", 6, DamageType_Ice, 60, enemy->view_range);
                } break;
                
                case EntityID_GiantSlime:
                {
                    strcpy(enemy->name.s, "Giant Slime");
                    enemy->max_hp = enemy->hp = 34;
                    
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 6;
                    enemy->e.damage.type = DamageType_Physical;
                    enemy->ev = 3;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_GreenBlood;
                    
                    enemy->resistances[DamageType_Physical] = 2;
                } break;
                
                case EntityID_Spectre:
                {
                    strcpy(enemy->name.s, "Spectre");
                    enemy->max_hp = enemy->hp = 25;
                    
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 5;
                    enemy->e.damage.type = DamageType_Dark;
                    enemy->ev = 8;
                    enemy->action_time = 1.0f;
                    
                    enemy->resistances[DamageType_Dark] = 2;
                } break;
                
                case EntityID_OrcAssassin:
                {
                    strcpy(enemy->name.s, "Orc Assassin");
                    enemy->max_hp = enemy->hp = 28;
                    
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 8;
                    enemy->e.damage.type = DamageType_Physical;
                    enemy->ev = 8;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_OrcSorcerer:
                {
                    strcpy(enemy->name.s, "Orc Sorcerer");
                    enemy->max_hp = enemy->hp = 26;
                    
                    enemy->ev = 5;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                    
                    add_entity_attack_spell(enemy, "Ignite", 12, DamageType_Fire, 80, enemy->view_range);
                    add_entity_status_spell(enemy, "Bind", StatusType_Bind, 0, 4, 30, enemy->view_range);
                } break;
                
                case EntityID_Minotaur:
                {
                    strcpy(enemy->name.s, "Minotaur");
                    enemy->max_hp = enemy->hp = 32;
                    
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 13;
                    enemy->e.damage.type = DamageType_Physical;
                    enemy->ev = 4;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_Treant:
                {
                    strcpy(enemy->name.s, "Treant");
                    enemy->max_hp = enemy->hp = 44;
                    
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 8;
                    enemy->e.damage.type = DamageType_Physical;
                    enemy->def = 6;
                    enemy->ev = 0;
                    enemy->action_time = 0.5f;
                    
                    enemy->resistances[DamageType_Fire] = -4;
                } break;
                
                case EntityID_Viper:
                {
                    strcpy(enemy->name.s, "Viper");
                    enemy->max_hp = enemy->hp = 20;
                    
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 6;
                    enemy->e.damage.type = DamageType_Physical;
                    enemy->ev = 11;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                    
                    enemy->resistances[DamageType_Poison] = 2;
                    
                    add_entity_attack_status(enemy, StatusType_Poison, 4, 8, 100);
                } break;
                
                case EntityID_CentaurWarrior:
                {
                    strcpy(enemy->name.s, "Centaur Warrior");
                    enemy->max_hp = enemy->hp = 30;
                    
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 8;
                    enemy->e.damage.type = DamageType_Physical;
                    enemy->ev = 6;
                    enemy->action_time = 2.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_CentaurSpearman:
                {
                    strcpy(enemy->name.s, "Centaur Spearman");
                    enemy->max_hp = enemy->hp = 30;
                    
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 12;
                    enemy->e.damage.type = DamageType_Physical;
                    enemy->ev = 4;
                    enemy->action_time = 2.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_CentaurArcher:
                {
                    strcpy(enemy->name.s, "Centaur Archer");
                    enemy->max_hp = enemy->hp = 30;
                    
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 5;
                    enemy->e.damage.type = DamageType_Physical;
                    enemy->ev = 4;
                    enemy->action_time = 2.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_CursedSkull:
                {
                    strcpy(enemy->name.s, "Cursed Skull");
                    enemy->max_hp = enemy->hp = 30;
                    
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 5;
                    enemy->e.damage.type = DamageType_Physical;
                    enemy->ev = 6;
                    enemy->action_time = 1.0f;
                    
                    enemy->resistances[DamageType_Dark] = 3;
                    
                    add_entity_attack_status(enemy, StatusType_Sightless, 0, 8, 33);
                } break;
                
                case EntityID_Wolf:
                {
                    strcpy(enemy->name.s, "Wolf");
                    enemy->max_hp = enemy->hp = 25;
                    
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 6;
                    enemy->e.damage.type = DamageType_Physical;
                    enemy->ev = 10;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                    
                    add_entity_attack_status(enemy, StatusType_Bleed, 3, 8, 33);
                } break;
                
                case EntityID_OgreWarrior:
                {
                    strcpy(enemy->name.s, "Ogre Warrior");
                    enemy->max_hp = enemy->hp = 34;
                    
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 14;
                    enemy->e.damage.type = DamageType_Physical;
                    enemy->ev = 3;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_OgreArcher:
                {
                    strcpy(enemy->name.s, "Ogre Archer");
                    enemy->max_hp = enemy->hp = 34;
                    
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 10;
                    enemy->e.damage.type = DamageType_Physical;
                    enemy->ev = 3;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_OgreMage:
                {
                    strcpy(enemy->name.s, "Ogre Mage");
                    enemy->max_hp = enemy->hp = 34;
                    
                    enemy->ev = 3;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                    
                    add_entity_stat_spell(enemy, "Sap", StatType_EV, -2, 8, 15, enemy->view_range);
                    add_entity_stat_spell(enemy, "Warsong", StatType_Str, 1, 8, 15, enemy->view_range);
                    add_entity_attack_spell(enemy, "Poison Cloud", 4, DamageType_Poison, 80, enemy->view_range);
                } break;
                
                case EntityID_Cyclops:
                {
                    strcpy(enemy->name.s, "Cyclops");
                    enemy->max_hp = enemy->hp = 38;
                    
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 18;
                    enemy->e.damage.type = DamageType_Physical;
                    enemy->ev = 3;
                    enemy->action_time = 0.5f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_ShadowWalker:
                {
                    strcpy(enemy->name.s, "Shadow Walker");
                    enemy->max_hp = enemy->hp = 30;
                    
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 8;
                    enemy->e.damage.type = DamageType_Dark;
                    enemy->ev = 10;
                    enemy->action_time = 1.0f;
                    
                    enemy->resistances[DamageType_Dark] = 3;
                } break;
                
                case EntityID_DwarfKnight:
                {
                    // TODO(rami): Art: high armor, big morningstar
                    
                    strcpy(enemy->name.s, "Dwarf Knight");
                    enemy->max_hp = enemy->hp = 30;
                    
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 15;
                    enemy->e.damage.type = DamageType_Physical;
                    enemy->def = 8;
                    enemy->ev = 2;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_DwarfSoldier:
                {
                    // TODO(rami): Art: medium armor, hammer
                    
                    strcpy(enemy->name.s, "Dwarf Soldier");
                    enemy->max_hp = enemy->hp = 30;
                    
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 12;
                    enemy->e.damage.type = DamageType_Physical;
                    enemy->ev = 4;
                    enemy->def = 6;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_DwarfTinkerer:
                {
                    // TODO(rami): Art: low armor, wrench?
                    
                    strcpy(enemy->name.s, "Dwarf Tinkerer");
                    enemy->max_hp = enemy->hp = 30;
                    
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 6;
                    enemy->e.damage.type = DamageType_Physical;
                    enemy->ev = 6;
                    enemy->def = 2;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                    
                    add_entity_attack_status(enemy, StatusType_BrokenArmor, 0, 8, 33);
                } break;
                
                case EntityID_ScarletSnake:
                {
                    strcpy(enemy->name.s, "Scarlet Snake");
                    enemy->max_hp = enemy->hp = 22;
                    
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 8;
                    enemy->e.damage.type = DamageType_Physical;
                    enemy->ev = 11;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                    
                    enemy->resistances[DamageType_Poison] = 2;
                    
                    add_entity_attack_status(enemy, StatusType_Poison, 6, 8, 33);
                } break;
                
                case EntityID_Lich:
                {
                    strcpy(enemy->name.s, "Lich");
                    enemy->max_hp = enemy->hp = 28;
                    
                    enemy->ev = 7;
                    enemy->action_time = 1.0f;
                    
                    enemy->resistances[DamageType_Dark] = 4;
                    enemy->resistances[DamageType_Poison] = 5;
                    enemy->resistances[DamageType_Ice] = -2;
                    enemy->resistances[DamageType_Fire] = -2;
                    
                    add_entity_attack_spell(enemy, "Freezing Gaze", 10, DamageType_Ice, 80, enemy->view_range);
                    add_entity_attack_spell(enemy, "Agonizing Grip", 14, DamageType_Dark, 80, enemy->view_range);
                    add_entity_summon_spell(enemy, "Summon Undead", SummonType_Undead, 10, EntityFlag_Undead);
                } break;
                
                case EntityID_AbyssalFiend:
                {
                    strcpy(enemy->name.s, "Abyssal Fiend");
                    enemy->max_hp = enemy->hp = 22;
                    
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 14;
                    enemy->e.damage.type = DamageType_Physical;
                    enemy->ev = 17;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                    
                    enemy->resistances[DamageType_Fire] = 2;
                    enemy->resistances[DamageType_Holy] = -1;
                    enemy->resistances[DamageType_Ice] = -2;
                    
                    enemy->e.hit_teleport_chance = 20;
                    } break;
                
                case EntityID_BloodTroll:
                {
                    strcpy(enemy->name.s, "Blood Troll");
                    enemy->max_hp = enemy->hp = 38;
                    
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 14;
                    enemy->e.damage.type = DamageType_Physical;
                    enemy->ev = 4;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                    
                    enemy->resistances[DamageType_Physical] = 2;
                    enemy->resistances[DamageType_Holy] = -3;
                    enemy->resistances[DamageType_Poison] = -2;
                } break;
                
                case EntityID_IronGolem:
                {
                    strcpy(enemy->name.s, "Iron Golem");
                    enemy->max_hp = enemy->hp = 42;
                    
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 18;
                    enemy->e.damage.type = DamageType_Physical;
                    enemy->ev = 4;
                    enemy->def = 6;
                    enemy->action_time = 1.0f;
                    
                    enemy->resistances[DamageType_Physical] = 2;
                } break;
                
                case EntityID_Griffin:
                {
                    strcpy(enemy->name.s, "Griffin");
                    enemy->max_hp = enemy->hp = 46;
                    
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 16;
                    enemy->e.damage.type = DamageType_Physical;
                    enemy->ev = 8;
                    enemy->action_time = 2.0f;
                    enemy->remains = EntityRemains_RedBlood;
                    
                    enemy->resistances[DamageType_Holy] = 3;
                    enemy->resistances[DamageType_Dark] = -3;
                } break;
                
                case EntityID_Imp:
                {
                    strcpy(enemy->name.s, "Imp");
                    enemy->max_hp = enemy->hp = 22;
                    
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 6;
                    enemy->e.damage.type = DamageType_Physical;
                    enemy->ev = 7;
                    enemy->action_time = 3.0f;
                    enemy->remains = EntityRemains_RedBlood;
                    
                    enemy->resistances[DamageType_Holy] = -2;
                    enemy->resistances[DamageType_Fire] = 2;
                    enemy->resistances[DamageType_Dark] = 1;
                } break;
                
                case EntityID_BlackKnight:
                {
                    strcpy(enemy->name.s, "Black Knight");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage.min = 0;
                    enemy->e.damage.max = 0;
                    enemy->e.damage.type = DamageType_Physical;
                    enemy->ev = 0;
                    enemy->action_time = 1.0f;
                } break;
                
                case EntityID_GiantDemon:
                {
                    strcpy(enemy->name.s, "Giant Demon");
                    enemy->max_hp = enemy->hp = 42;
                    
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 20;
                    enemy->e.damage.type = DamageType_Physical;
                    enemy->ev = 6;
                    enemy->action_time = 2.0f;
                    enemy->remains = EntityRemains_RedBlood;
                    
                    enemy->resistances[DamageType_Fire] = 2;
                    enemy->resistances[DamageType_Dark] = 2;
                    enemy->resistances[DamageType_Holy] = -2;
                    enemy->resistances[DamageType_Ice] = -2;
                    
                    add_entity_attack_status(enemy, StatusType_Burn, 4, 6, 33);
                } break;
                
                case EntityID_Hellhound:
                {
                    strcpy(enemy->name.s, "Hellhound");
                    enemy->max_hp = enemy->hp = 35;
                    
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 12;
                    enemy->e.damage.type = DamageType_Physical;
                    enemy->ev = 11;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                    
                    enemy->resistances[DamageType_Fire] = 2;
                    enemy->resistances[DamageType_Ice] = -2;
                } break;
                
                case EntityID_AbyssalHexmaster:
                {
                    strcpy(enemy->name.s, "Abyssal Hexmaster");
                    enemy->max_hp = enemy->hp = 36;
                    
                    enemy->ev = 11;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                    
                    enemy->resistances[DamageType_Poison] = 4;
                    enemy->resistances[DamageType_Holy] = -2;
                    enemy->resistances[DamageType_Lightning] = -3;
                    
                    add_entity_attack_spell(enemy, "Death's Embrace", 18, DamageType_Poison, 80, enemy->view_range);
                    add_entity_status_spell(enemy, "Capture", StatusType_Bind, 0, 6, 10, enemy->view_range);
                    add_entity_status_spell(enemy, "Hallucinate", StatusType_Confusion, 0, 5, 10, enemy->view_range);
                } break;
                
                case EntityID_Zarimahar:
                {
                    strcpy(enemy->name.s, "Zarimahar");
                    enemy->max_hp = enemy->hp = 30;
                    
                    enemy->ev = 9;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                    
                    enemy->resistances[DamageType_Fire] = 1;
                    enemy->resistances[DamageType_Ice] = 1;
                    enemy->resistances[DamageType_Lightning] = 1;
                    enemy->resistances[DamageType_Physical] = -2;
                    enemy->resistances[DamageType_Poison] = -1;
                    
                    add_entity_attack_spell(enemy, "Lance of Mante", 24, DamageType_Physical, 30, enemy->view_range);
                    add_entity_attack_spell(enemy, "Thunderclap", 18, DamageType_Lightning, 30, enemy->view_range);
                    add_entity_status_spell(enemy, "immolate", StatusType_Burn, 8, 5, 100, enemy->view_range);
                } break;
                
                invalid_default_case;
            }
            
            assert(enemy->view_range);
            enemy->e.view_rect = get_dungeon_dimension_rect(dungeon->size, enemy->pos, enemy->view_range);
            
#if MOONBREATH_SLOW
            if(id != EntityID_Dummy)
            {
                if(!is_entity_evasion_valid(enemy->ev))
                {
                    printf("Enemy \"%s\" evasion is %u which is not valid.\n", enemy->name.s, enemy->ev);
                    assert(0);
                }
                
                if((!enemy->e.damage.min || !enemy->e.damage.max) &&
                   !enemy->e.spell_count)
                {
                    printf("Enemy \"%s\" has no valid damage source.\n", enemy->name.s);
                    assert(0);
                }
                
                for(u32 index = DamageType_None + 1; index < DamageType_Count; ++index)
                {
                    s32 resitance = enemy->resistances[index];
                    
                    if(!is_resistance_valid(resitance))
                    {
                        printf("Enemy \"%s\" resistance to %s is %d, which is not valid.\n",
                               enemy->name.s, get_damage_type_string(index), resitance);
                        
                        assert(0);
                    }
                }
                
                for(u32 index = 0; index < MAX_ENTITY_SPELL_COUNT; ++index)
                {
                    Spell *spell = &enemy->e.spells[index];
                    
                    if(spell->type)
                    {
                        assert(spell->range <= enemy->view_range);
                    }
                }
            }
#endif
            
            break;
        }
    }
    
    assert(result);
    return(result);
}