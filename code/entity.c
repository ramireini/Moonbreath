#if MOONBREATH_SLOW
internal b32
is_entity_evasion_valid(u32 evasion)
{
    b32 result = (evasion >= 0 && evasion <= 21);
    return(result);
}

internal b32
is_entity_spell_valid(Spell *spell)
{
    b32 result = (spell && spell->status_type);
    return(result);
}

internal b32
is_entity_status_valid(EntityStatus *status)
{
    b32 result = (status && status->type);
    return(result);
}

internal b32
is_entity_resist_valid(EntityResist resist)
{
 b32 result = (resist.value <= MAX_ENTITY_RESIST_VALUE &&
               resist.value >= MIN_ENTITY_RESIST_VALUE);
 
    return(result);
}

internal char *
get_entity_wandering_type_string(EnemyWanderingType type)
{
 char *result = 0;
 
 switch(type)
 {
  case EnemyWanderingType_Random: result = "Random"; break;
  case EnemyWanderingType_Travel: result = "Travel"; break;
  
  invalid_default_case;
 }
 
 return(result);
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
get_enemy_entity_state_string(EnemyEntityState type)
{
 char *result = 0;
 
 switch(type)
 {
  case EnemyEntityState_Wandering: result = "Wandering"; break;
  case EnemyEntityState_Fighting: result = "Fighting"; break;
  case EnemyEntityState_Chasing: result = "Chasing"; break;
  case EnemyEntityState_Fleeing: result = "Fleeing"; break;
  case EnemyEntityState_Cornered: result = "Cornered"; break;
  
  invalid_default_case;
 }
 
 return(result);
}

internal void
print_enemy_entity_state(EnemyEntityState type)
{
 printf("Enemy State: %s\n", get_enemy_entity_state_string(type));
}

#endif

internal void
reset_enemy_pathfind(Entity *enemy)
{
 assert(is_enemy_entity_valid(enemy));
 
 unset(enemy->flags, EntityFlag_Pathfinding);
 zero_struct(enemy->pathfind_target_pos);
}

internal char *
get_entity_name(EntityID id)
{
 char *result = 0;
 
 switch(id)
 {
  case EntityID_Dummy: result = "Dummy"; break;
  
  case EntityID_SkeletonWarrior: result = "Skeleton Warrior"; break;
  case EntityID_SkeletonArcher: result = "Skeleton Archer"; break;
  case EntityID_SkeletonMage: result = "Skeleton Mage"; break;
  case EntityID_Bat: result = "Bat"; break;
  case EntityID_Rat: result = "Rat"; break;
  
  case EntityID_KoboldWarrior: result = "Kobold Warrior"; break;
  case EntityID_KoboldShaman: result = "Kobold Shaman"; break;
  case EntityID_Snail: result = "Snail"; break;
  case EntityID_Slime: result = "Slime"; break;
  case EntityID_Dog: result = "Dog"; break;
  
  case EntityID_OrcWarrior: result = "Orc Warrior"; break;
  case EntityID_OrcArcher: result = "Orc Archer"; break;
  case EntityID_OrcShaman: result = "Orc Shaman"; break;
  case EntityID_Python: result = "Python"; break;
  case EntityID_Shade: result = "Shade"; break;
  
  case EntityID_ElfKnight: result = "Elf Knight"; break;
  case EntityID_ElfArbalest: result = "Elf Arbalest"; break;
  case EntityID_ElfMage: result = "Elf Mage"; break;
  case EntityID_GiantSlime: result = "Giant Slime"; break;
  case EntityID_Spectre: result = "Spectre"; break;
  
  case EntityID_OrcAssassin: result = "Orc Assassin"; break;
  case EntityID_OrcSorcerer: result = "Orc sorcerer"; break;
  case EntityID_Minotaur: result = "Minotaur"; break;
  case EntityID_Treant: result = "Treant"; break;
  case EntityID_Viper: result = "Viper"; break;
  
  case EntityID_CentaurWarrior: result = "Centaur Warrior"; break;
  case EntityID_CentaurSpearman: result = "Centaur Spearman"; break;
  case EntityID_CentaurArcher: result = "Centaur Archer"; break;
  case EntityID_CursedSkull: result = "Cursed Skull"; break;
  case EntityID_Wolf: result = "Wolf"; break;
  
  case EntityID_OgreWarrior: result = "Ogre Warrior"; break;
  case EntityID_OgreArcher: result = "Ogre Archer"; break;
  case EntityID_OgreMage: result = "Ogre Mage"; break;
  case EntityID_Cyclops: result = "Cyclops"; break;
  case EntityID_ShadowWalker: result = "Shadow Walker"; break;
  
  case EntityID_DwarfKnight: result = "Dwarf Knight"; break;
  case EntityID_DwarfSoldier: result = "Dwarf Soldier"; break;
  case EntityID_DwarfTinkerer: result = "Dwarf Tinkerer"; break;
  case EntityID_ScarletSnake: result = "Scarlet Snake"; break;
  case EntityID_Lich: result = "Lich"; break;
  
  case EntityID_AbyssalFiend: result = "Abyssal Fiend"; break;
  case EntityID_BloodTroll: result = "Blood Troll"; break;
  case EntityID_IronGolem: result = "Iron Golem"; break;
  case EntityID_Griffin: result = "Griffin"; break;
  case EntityID_Imp: result = "Imp"; break;
  
  case EntityID_BlackKnight: result = "Black Knight"; break;
  case EntityID_GiantDemon: result = "Giant Demon"; break;
  case EntityID_Hellhound: result = "Hellhound"; break;
  case EntityID_AbyssalHexmaster: result = "Abyssal Hexmaster"; break;
  case EntityID_Zarimahar: result = "Zarimahar"; break;
  
  invalid_default_case;
 }
 
 return(result);
}

internal f32
get_percentage(u32 count, u32 total)
{
 assert(count);
 assert(total);
 
 f32 result = ((f32)count / (f32)total) * 100.0f;
 return(result);
 }

internal void
reset_player_pathfind_trail(Entity *player)
{
 assert(is_player_entity_valid(player));
 
 player->p.render_pathfind = false;
 zero_array(player->p.pathfind_trail, MAX_PATHFIND_TRAIL_COUNT);
}

internal void
update_enemy_facing_direction(Entity *enemy, Entity *player, b32 update_from_moving)
{
    assert(is_enemy_entity_valid(enemy));
    
 if(update_from_moving)
    {
        // Update based on move direction
            switch(enemy->direction)
            {
                case Direction_Left:
                case Direction_UpLeft:
                case Direction_DownLeft: set(enemy->flags, EntityFlag_Flipped); break;
                
                case Direction_Right:
                case Direction_UpRight:
                case Direction_DownRight: unset(enemy->flags, EntityFlag_Flipped); break;
            }
    }
    else
 {
  assert(is_player_entity_valid(player));
        
        // Update based on target pos
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
    }
        }

internal u32
get_entity_status_type_count(EntityStatus *statuses, EntityStatusType type)
{
 assert(statuses);
 assert(type);
 
 u32 result = 0;
 
 for(u32 index = 0; index < MAX_ENTITY_STATUS_COUNT; ++index)
 {
  EntityStatus *status = &statuses[index];
  if(status->type == type)
  {
   ++result;
  }
 }
 
 return(result);
 }

internal EntityStatus *
get_entity_status(EntityStatus *statuses, EntityStatusType type)
{
    assert(statuses);
    assert(type);
 
 EntityStatus *result = 0;
 
    for(u32 index = 0; index < MAX_ENTITY_STATUS_COUNT; ++index)
    {
        EntityStatus *status = &statuses[index];
  if(status->type == type)
  {
   result = status;
   break;
  }
    }
    
    return(result);
}

internal String64
get_item_name_interact_string(Random *random, String32 name)
{
    assert(random);
    
    String64 result = {0};
    
    // First group
    String16 first_group[3] =
    {
        "look",
        "feel",
        "seem",
    };
 u32 first_group_index = get_random_index(random, array_count(first_group));
    
    // Add 's' if name is singular
    u32 name_length = get_string_length(name.s);
    if(name.s[name_length - 1] != 's')
    {
    u32 word_length = get_string_length(first_group[first_group_index].s);
        first_group[first_group_index].s[word_length] = 's';
    }
    
    // Second group
    String16 second_group[4] =
    {
        "different",
            "sturdier",
         "stronger",
         "improved",
    };
 u32 second_group_index = get_random_index(random, array_count(second_group));
    
    sprintf(result.s, "%s %s",
                first_group[first_group_index].s,
                second_group[second_group_index].s);
    
    return(result);
}

internal u32
get_max_enum_string_length(u32 enum_type_none, u32 enum_type_count, char *enum_to_string_callback(u32))
{
    assert(!enum_type_none);
    assert(enum_type_count);
    assert(enum_to_string_callback);
    
    u32 result = 0;
    
    for(u32 current = enum_type_none + 1; current < enum_type_count; ++current)
    {
        char *text = enum_to_string_callback(current);
  u32 length = get_string_length(text);
  
        if(result < length) result = length;
    }
    
    return(result);
    }

internal b32
windows_are_closed(ExamineMode *examine, u32 inventory_flags, u32 ui_flags)
{
    b32 result = (!examine->type &&
                  !is_set(examine->flags, ExamineFlag_Open) &&
                  !is_set(inventory_flags, InventoryFlag_Open) &&
                  !is_set(inventory_flags, InventoryFlag_MultiplePickup) &&
                      !is_set(inventory_flags, InventoryFlag_MultipleExamine) &&
                      !is_set(ui_flags, UIFlag_FullLogOpen) &&
                      !is_set(ui_flags, UIFlag_PlayerStatusOpen));
    
    return(result);
}

internal b32
was_pressed_and_windows_are_closed(InputState *state, ExamineMode *examine, u32 inventory_flags, u32 ui_flags)
{
    b32 result = was_pressed(state) && windows_are_closed(examine, inventory_flags, ui_flags);
    return(result);
    }

internal b32
set_game_passed_time_from_player_pos(f32 *game_passed_time, DungeonTiles tiles, v2u pos, f32 speed)
{
    assert(game_passed_time);
    assert(!is_v2u_zero(pos));
    assert(speed);
    
    b32 on_water = false;
    
    if(is_dungeon_pos_water(tiles, pos))
    {
         on_water = true;
        *game_passed_time = speed * 2.0f;
    }
    else
    {
        *game_passed_time = speed;
    }
    
    return(on_water);
}

internal b32
advance_game_and_player_time(Game *game, f32 *player_action_time)
{
    game->time += game->passed_time;
    *player_action_time = game->passed_time;
}

internal b32
is_entity_valid_and_not_index(Entity *entity, u32 index)
     {
    b32 result = (is_entity_valid(entity) && index != entity->index);
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
log_add_player_view_has_enemies(EntityState *entity_state, Dungeon *dungeon, UI *ui)
{
    assert(entity_state);
    assert(dungeon);
    assert(ui);
 
 b32 result = false;
 
    u32 new_enemy_count = get_player_view_enemy_count(entity_state, dungeon);
    if(new_enemy_count)
    {
  log_add_new_player_view_string("enemy", "enemies", new_enemy_count, ui);
  result = true;
    }
 
 return(result);
    }

internal void
change_entity_stat(u32 *value, s32 change, b32 is_add)
{
    assert(value);
    assert(change);
 
 if(is_add)
 {
  *value += change;
 }
 else
 {
  *value -= change;
 }
 
 if(is_zero(*value))
 {
  *value = 0;
 }
 
    }

internal void
change_entity_resist(EntityResist *resist, s32 change, b32 is_add)
{
 EntityResist final = *resist;
 
 if(is_add)
 {
  final.value += change;
  if(final.value > 5) final.value = 5;
 }
 else
 {
  final.value -= final.value;
  if(final.value < -5) final.value = -5;
 }
 
 *resist = final;
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
        assert(regen->amount);
        assert(regen->max);
        assert(regen->advance);
        assert(regen->advance <= regen->max);
        
        regen->current += regen->advance;
        if(regen->current >= regen->max)
        {
            regen->next_turn = false;
            regen->current = 0;
            
            entity->hp += regen->amount;
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
summon_entity(Random *random, EntityState *entity_state, Entity *entity, Dungeon *dungeon, UI *ui, u32 summon_flags)
{
    assert(random);
    assert(entity_state);
    assert(is_entity_valid(entity));
    assert(dungeon);
    assert(ui);
    
    if(summon_flags)
    {
        for(;;)
        {
            assert_loop_count();
            
            EntityInfo summon_info = get_random_enemy_entity_info(random);
            if(entity->id != summon_info.id &&
                   is_set(summon_info.flags, summon_flags) &&
                   entity_level_fits_dungeon_level(summon_info.level, dungeon->level))
            {
                v2u summon_pos = get_random_dungeon_traversable_unoccupied_rect_pos(random, dungeon, entity->e.view_rect);
                Entity *summon_enemy = add_enemy_entity(entity_state, dungeon, summon_info.id, summon_pos.x, summon_pos.y);
                
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
        
        add_enemy_entity(entity_state, dungeon, summon_id, summon_pos.x, summon_pos.y);
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
        
        assert(spell->status_type);
        assert(spell->chance);
        
        result = spell;
        counter += spell->chance;
    }
    
    entity->e.spell = result;
    
    assert(result);
    return(result);
}

internal b32
is_pos_in_spell_range(v2u start_pos, v2u end_pos, u32 spell_range)
{
    assert(!is_v2u_zero(start_pos));
    assert(!is_v2u_zero(end_pos));
    assert(spell_range);
    
    b32 result = cardinal_and_ordinal_distance(start_pos, end_pos) <= spell_range;
    return(result);
}

internal b32
is_entity_in_view_and_spell_range(Entity *attacker, v2u defender_pos, u32 spell_range)
{
    assert(is_enemy_entity_valid(attacker));
    assert(!is_v2u_zero(defender_pos));
    assert(spell_range);
    
    b32 result = (is_dungeon_pos_inside_rect(defender_pos, attacker->e.view_rect) &&
                      is_pos_in_spell_range(attacker->pos, defender_pos, spell_range));
    
    return(result);
}

internal EntitySpellCastResult
select_and_cast_entity_spell(Random *random,
                             EntityState *entity_state,
                             Entity *caster,
                             Dungeon *dungeon,
                             UI *ui)
{
    assert(random);
    assert(entity_state);
    assert(is_entity_valid(caster));
    assert(dungeon);
    assert(ui);
    
    EntitySpellCastResult result = {0};
    Entity *target = get_player_entity();
    
    b32 loop = true;
    while(loop) // Loop until a usable spell is found.
    {
  assert_loop_count();
  
        Spell *spell = set_random_entity_spell(random, caster);
        
        #if 0
        printf("spell->name: %s\n", spell->name.s);
        printf("spell->value.min: %u\n", spell->value.min);
        printf("spell->value.max: %u\n", spell->value.max);
        printf("spell->range: %u\n\n", spell->range);
#endif
        
        if(spell->status_type == EntityStatusType_Damage)
        {
            if(is_entity_in_view_and_spell_range(caster, target->pos, spell->range))
            {
                attack_entity(random, caster, 0, target, dungeon, ui, spell->damage);
            }
            else
            {
                assert(!is_pos_in_spell_range(caster->pos, target->pos, spell->range));
                result.target_not_in_spell_range = true;
            }
            
            loop = false;
            }
        else if(spell->status_type == EntityStatusType_Heal)
        {
            for(u32 index = 0; index < MAX_ENTITY_COUNT; ++index)
            {
                Entity *entity = &entity_state->array[index];
                if(is_entity_valid_and_not_index(entity, caster->index) &&
                       is_entity_in_view_and_spell_range(caster, entity->pos, spell->range) &&
                       entity->type == EntityType_Enemy &&
                       entity->hp != entity->max_hp)
                {
                    heal_entity(entity, get_random(random, spell->value.min, spell->value.max));
     log_add_entity_interact_string(random, caster, 0, entity, ui, 0, EntityResistInfoType_None, EntityInteractInfoType_None);
                    
                    loop = false;
                    break;
                }
            }
        }
        else if(spell->status_type == EntityStatusType_Stat)
        {
            Spell *spell = caster->e.spell;
            
            for(u32 index = 0; index < MAX_ENTITY_COUNT; ++index)
            {
                Entity *entity = &entity_state->array[index];
                if(is_entity_valid_and_not_index(entity, caster->index))
                {
                    // Make sure the target is valid for the spell target type
                    b32 valid_target = false;
                    switch(spell->target_type)
                    {
                        case EntityType_Player:
                        {
                            if(is_player_entity_valid(entity))
                            {
                                valid_target = true;
                            }
                        } break;
                        
                        case EntityType_Enemy:
                        {
       if(is_enemy_entity_valid(entity) &&
          entity->e.state == EnemyEntityState_Fighting)
                            {
                                valid_target = true;
                            }
                        } break;
                        
                        invalid_default_case;
                    }
                    
                    // Make sure the spell is valid for the target stats
                    b32 valid_spell = true;
                    if(spell->status_type == EntityStatusType_Stat)
                    {
                        switch(spell->stat_type)
                        {
                            case EntityStatusStatType_Str: valid_spell = ((s32)entity->stats.str > 0); break;
                            case EntityStatusStatType_Int: valid_spell = ((s32)entity->stats.intel > 0); break;
                            case EntityStatusStatType_Dex: valid_spell = ((s32)entity->stats.dex > 0); break;
                            case EntityStatusStatType_Def: valid_spell = ((s32)entity->stats.def > 0); break;
       case EntityStatusStatType_EV: valid_spell = ((s32)entity->stats.ev > 0); break;
       case EntityStatusStatType_FOV: valid_spell = ((s32)entity->stats.fov > 0); break;
                            
                            case EntityStatusStatType_StrIntDex:
                            {
                                valid_spell = ((s32)entity->stats.str > 0 &&
                                                   (s32)entity->stats.intel > 0 &&
                                                   (s32)entity->stats.dex > 0);
                            } break;
                            
                            invalid_default_case;
                        }
                    }
                    
#if 0
                    printf("valid_spell: %u\n", valid_spell);
                    printf("valid_target: %u\n", valid_target);
                    printf("spell->value.min: %d\n", spell->value.min);
                    printf("spell->value.max: %d\n", spell->value.max);
                    printf("spell->target_type: %s\n", get_entity_type_string(spell->target_type));
                    printf("get_total_stat_status_value(): %d\n\n", get_total_stat_status_value(target->statuses, spell->stat_type));
#endif
                    
                    if(valid_target && valid_spell &&
                           is_entity_in_view_and_spell_range(caster, entity->pos, spell->range))
                    {
                        cast_entity_spell(random, caster, entity, dungeon, ui);
                        
                        loop = false;
                        break;
                    }
                }
            }
        }
        else if((spell->status_type == EntityStatusType_Bind ||
                             spell->status_type == EntityStatusType_Burn || 
                             spell->status_type == EntityStatusType_Confusion) &&
                    is_entity_in_view_and_spell_range(caster, target->pos, spell->range) &&
                    !get_entity_status(target->statuses, spell->status_type))
        {
            // This is for statuses are that applied only when they aren't already active.
            
            cast_entity_spell(random, caster, target, dungeon, ui);
            loop = false;
            }
        else if(spell->status_type == EntityStatusType_Summon)
        {
            summon_entity(random, entity_state, caster, dungeon, ui, spell->summon_flags);
            loop = false;
            }
        }
        
    // Nothing should use this until we set it again.
    caster->e.spell = 0;
    return(result);
}

internal void
log_add_entity_interact_string(Random *random,
                             Entity *attacker,
                             DungeonTrap *trap,
                             Entity *defender,
                             UI *ui,
                             u32 value,
                               EntityResistInfoType resist_info_type,
                               EntityInteractInfoType interact_info_type)
{
    assert(random);
    assert(is_entity_valid(defender));
    assert(ui);
    
    String256 interact_string = {0};
    
    if(attacker)
    {
        String64 attack_string = {0};
        if(attacker->type == EntityType_Player)
        {
            Item *player_weapon = attacker->p.equipment[ItemSlot_Weapon];
            if(player_weapon)
            {
                switch(player_weapon->id)
                {
                    case ItemID_Dagger:
                    case ItemID_Sword:
                    {
                        switch(get_random(random, 1, 6))
                        {
                            case 1: snprintf(attack_string.s, sizeof(attack_string.s), "stab"); break;
                            case 2: snprintf(attack_string.s, sizeof(attack_string.s), "pierce"); break;
                            case 3: snprintf(attack_string.s, sizeof(attack_string.s), "puncture"); break;
                            case 4: snprintf(attack_string.s, sizeof(attack_string.s), "slash"); break;
                            case 5: snprintf(attack_string.s, sizeof(attack_string.s), "lacerate"); break;
                            case 6: snprintf(attack_string.s, sizeof(attack_string.s), "cleave"); break;
                            
                            invalid_default_case;
                        }
                    } break;
                    
                    case ItemID_Club:
                    case ItemID_Warhammer:
                    {
                        switch(get_random(random, 1, 6))
                        {
                            case 1: snprintf(attack_string.s, sizeof(attack_string.s), "smash"); break;
                            case 2: snprintf(attack_string.s, sizeof(attack_string.s), "bash"); break;
                            case 3: snprintf(attack_string.s, sizeof(attack_string.s), "strike"); break;
                            case 4: snprintf(attack_string.s, sizeof(attack_string.s), "pummel"); break;
                            case 5: snprintf(attack_string.s, sizeof(attack_string.s), "pound"); break;
                            case 6: snprintf(attack_string.s, sizeof(attack_string.s), "crush"); break;
                            
                            invalid_default_case;
                        }
                    } break;
                    
                    case ItemID_Battleaxe:
                    {
                        switch(get_random(random, 1, 6))
                        {
                            case 1: snprintf(attack_string.s, sizeof(attack_string.s), "hack"); break;
                            case 2: snprintf(attack_string.s, sizeof(attack_string.s), "rend"); break;
                            case 3: snprintf(attack_string.s, sizeof(attack_string.s), "chop"); break;
                            case 4: snprintf(attack_string.s, sizeof(attack_string.s), "slash"); break;
                            case 5: snprintf(attack_string.s, sizeof(attack_string.s), "lacerate"); break;
                            case 6: snprintf(attack_string.s, sizeof(attack_string.s), "cleave"); break;
                            
                            invalid_default_case;
                        }
                    } break;
                    
                    case ItemID_Spear:
                    {
                        switch(get_random(random, 1, 4))
                        {
                            case 1: snprintf(attack_string.s, sizeof(attack_string.s), "stab"); break;
                            case 2: snprintf(attack_string.s, sizeof(attack_string.s), "pierce"); break;
                            case 3: snprintf(attack_string.s, sizeof(attack_string.s), "puncture"); break;
                            case 4: snprintf(attack_string.s, sizeof(attack_string.s), "strike"); break;
                            
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
                    case 1: snprintf(attack_string.s, sizeof(attack_string.s), "punch"); break;
                    case 2: snprintf(attack_string.s, sizeof(attack_string.s), "kick"); break;
                    
                    invalid_default_case;
                }
            }
        }
        
        b32 is_spell = false;
        Spell *spell = 0;
        if(attacker->type == EntityType_Enemy)
        {
            spell = attacker->e.spell;
            if(spell)
            {
                is_spell = true;
                snprintf(attack_string.s, sizeof(attack_string.s), "casts %s on", spell->name.s);
            }
            else if(!attack_string.s[0])
            {
                if(is_set(attacker->flags, EntityFlag_UsesRangedAttacks))
                {
                    snprintf(attack_string.s, sizeof(attack_string.s), "fires an arrow at");
                }
                else
                {
                    snprintf(attack_string.s, sizeof(attack_string.s), "attacks");
                }
            }
        }
        
        String64 attacker_name = {0};
        if(attacker->type == EntityType_Player)
        {
            snprintf(attacker_name.s, sizeof(attacker_name.s), "You");
        }
        else if(get_entity_status(attacker->statuses, EntityStatusType_Invisible))
        {
            snprintf(attacker_name.s, sizeof(attacker_name.s), "Something");
        }
        else
        {
            snprintf(attacker_name.s, sizeof(attacker_name.s), "The %s", attacker->name.s);
        }
        
        String64 defender_name = {0};
        if(get_entity_status(defender->statuses, EntityStatusType_Invisible))
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
        
  if(interact_info_type == EntityInteractInfoType_AttackBlocked)
        {
            switch(defender->type)
            {
                case EntityType_Player:
    {
                    snprintf(interact_string.s, sizeof(interact_string.s), "%s %s %s, blocked by armor.", attacker_name.s, attack_string.s, defender_name.s);
                } break;
                
                case EntityType_Enemy:
    {
                    snprintf(interact_string.s, sizeof(interact_string.s), "%s %s %s, blocked by armor.", attacker_name.s, attack_string.s, defender_name.s);
                } break;
                
                invalid_default_case;
            }
        }
  else if(interact_info_type == EntityInteractInfoType_AttackMissed)
        {
            switch(defender->type)
            {
                case EntityType_Player:
                {
                    snprintf(interact_string.s, sizeof(interact_string.s), "%s %s %s, you dodged.", attacker_name.s, attack_string.s, defender_name.s);
                } break;
                
                case EntityType_Enemy:
                {
                    snprintf(interact_string.s, sizeof(interact_string.s), "%s %s %s, you miss.", attacker_name.s, attack_string.s, defender_name.s);
                } break;
                
                invalid_default_case;
            }
        }
  else if(resist_info_type == EntityResistInfoType_Partial)
        {
            snprintf(interact_string.s, sizeof(interact_string.s), "%s %s %s, it resists some of the damage, %u damage.", attacker_name.s, attack_string.s, defender_name.s, value);
        }
  else if(resist_info_type == EntityResistInfoType_Full)
        {
            snprintf(interact_string.s, sizeof(interact_string.s), "%s %s %s, it resists all the damage.", attacker_name.s, attack_string.s, defender_name.s);
        }
        else if(is_spell && spell->status_type != EntityStatusType_Damage)
        {
                snprintf(interact_string.s, sizeof(interact_string.s), "%s %s %s.", attacker_name.s, attack_string.s, defender_name.s);
            }
        else
        {
            snprintf(interact_string.s, sizeof(interact_string.s), "%s %s %s, %u damage.", attacker_name.s, attack_string.s, defender_name.s, value);
        }
        }
    else if(trap)
    {
        switch(trap->type)
        {
            case DungeonTrapType_Spike:
            {
    if(interact_info_type == EntityInteractInfoType_AttackBlocked)
    {
                    snprintf(interact_string.s, sizeof(interact_string.s), "Sharp spikes pierce you, blocked by armor.");
                }
                else
                {
                snprintf(interact_string.s, sizeof(interact_string.s), "Sharp spikes pierce you, %u damage.", value);
                }
            } break;
            
            case DungeonTrapType_Sword:
            {
    if(interact_info_type == EntityInteractInfoType_AttackBlocked)
                {
                    snprintf(interact_string.s, sizeof(interact_string.s), "Swords lacerate you, blocked by armor.");
                }
                else
                {
                    snprintf(interact_string.s, sizeof(interact_string.s), "Swords lacerate you, %u damage.", value);
                }
            } break;
            
            case DungeonTrapType_Arrow:
            {
    if(interact_info_type == EntityInteractInfoType_AttackBlocked)
                {
                    snprintf(interact_string.s, sizeof(interact_string.s), "Multiple arrows fire at you, blocked by armor.");
                }
                else
                {
                    snprintf(interact_string.s, sizeof(interact_string.s), "Multiple arrows fire at you, %u damage.", value);
                }
            } break;
            
            case DungeonTrapType_Magic:
            {
    if(interact_info_type == EntityInteractInfoType_AttackBlocked)
                {
                    snprintf(interact_string.s, sizeof(interact_string.s), "The magical trap explodes below you, blocked by armor.");
                }
                else
                {
                    snprintf(interact_string.s, sizeof(interact_string.s), "The magical trap explodes below you, %u %s damage.", value, get_entity_damage_type_string(trap->damage.type));
                }
            } break;
            
            invalid_default_case;
        }
        }
    
    log_add(interact_string.s, ui);
}

internal void
log_add_entity_resistance_action(Entity *entity, UI *ui, EntityResistInfoType resist_type)
{
    assert(is_entity_valid(entity));
    assert(ui);
    
    String32 text = {0};
    
    switch(resist_type)
    {
  case EntityResistInfoType_Partial: strcpy(text.s, "partially"); break;
  case EntityResistInfoType_Full: strcpy(text.s, "fully"); break;
        
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
place_entity_remains(Random *random,
                     Entity *entity,
                     Dungeon *dungeon,
                     EntityRemainsPlaceType place_type)
{
    assert(random);
    assert(is_entity_valid(entity));
    assert(dungeon);
    assert(place_type);
    
    v2u remains_pos = {0};
    DungeonTileID remains = DungeonTileID_None;
    
    if(entity->remains_type)
    {
        if(place_type == EntityRemainsPlaceType_Wound)
        {
            Direction direction = get_random_direction(random);
            remains_pos = get_direction_pos(entity->pos, direction);
            
            if(can_place_entity_remains_on_dungeon_pos(dungeon, remains_pos, place_type))
            {
                if(is_dungeon_pos_wall(dungeon->tiles, remains_pos))
                {
                    if(entity->remains_type == EntityRemainsType_RedBlood)
                    {
                        switch(direction)
                        {
                            case Direction_Up: remains = get_random(random, DungeonTileID_RedBloodWallUp1, DungeonTileID_RedBloodWallUp3); break;
                            case Direction_Down: remains = get_random(random, DungeonTileID_RedBloodWallDown1, DungeonTileID_RedBloodWallDown3); break;
                            case Direction_Left: remains = get_random(random, DungeonTileID_RedBloodWallLeft1, DungeonTileID_RedBloodWallLeft3); break;
                            case Direction_Right: remains = get_random(random, DungeonTileID_RedBloodWallRight1, DungeonTileID_RedBloodWallRight3); break;
                            case Direction_UpLeft: remains = get_random(random, DungeonTileID_RedBloodWallUpLeft1, DungeonTileID_RedBloodWallUpLeft3); break;
                            case Direction_UpRight: remains = get_random(random, DungeonTileID_RedBloodWallUpRight1, DungeonTileID_RedBloodWallUpRight3); break;
                            case Direction_DownLeft: remains = get_random(random, DungeonTileID_RedBloodWallDownLeft1, DungeonTileID_RedBloodWallDownLeft3); break;
                            case Direction_DownRight: remains = get_random(random, DungeonTileID_RedBloodWallDownRight1, DungeonTileID_RedBloodWallDownRight3); break;
                            
                            invalid_default_case;
                        }
                        }
                    else
                    {
                        assert(entity->remains_type == EntityRemainsType_GreenBlood);
                        
                        switch(direction)
                        {
                            case Direction_Up: remains = get_random(random, DungeonTileID_GreenBloodWallUp1, DungeonTileID_GreenBloodWallUp3); break;
                            case Direction_Down: remains = get_random(random, DungeonTileID_GreenBloodWallDown1, DungeonTileID_GreenBloodWallDown3); break;
                            case Direction_Left: remains = get_random(random, DungeonTileID_GreenBloodWallLeft1, DungeonTileID_GreenBloodWallLeft3); break;
                            case Direction_Right: remains = get_random(random, DungeonTileID_GreenBloodWallRight1, DungeonTileID_GreenBloodWallRight3); break;
                            case Direction_UpLeft: remains = get_random(random, DungeonTileID_GreenBloodWallUpLeft1, DungeonTileID_GreenBloodWallUpLeft3); break;
                            case Direction_UpRight: remains = get_random(random, DungeonTileID_GreenBloodWallUpRight1, DungeonTileID_GreenBloodWallUpRight3); break;
                            case Direction_DownLeft: remains = get_random(random, DungeonTileID_GreenBloodWallDownLeft1, DungeonTileID_GreenBloodWallDownLeft3); break;
                            case Direction_DownRight: remains = get_random(random, DungeonTileID_GreenBloodWallDownRight1, DungeonTileID_GreenBloodWallDownRight3); break;
                            
                            invalid_default_case;
                        }
                    }
                }
                else
                {
                    if(entity->remains_type == EntityRemainsType_RedBlood)
                    {
                        remains = get_random(random, DungeonTileID_RedBloodGroundSmall1, DungeonTileID_RedBloodGroundSmall2);
                    }
                    else
                    {
                        assert(entity->remains_type == EntityRemainsType_GreenBlood);
                        remains = get_random(random, DungeonTileID_GreenBloodGroundSmall1, DungeonTileID_GreenBloodGroundSmall2);
                    }
                }
                
                assert(remains);
            }
        }
        else if(place_type == EntityRemainsPlaceType_Kill)
        {
            assert(is_dungeon_pos_traversable(dungeon->tiles, entity->pos));
            
            if(can_place_entity_remains_on_dungeon_pos(dungeon, entity->pos, EntityRemainsPlaceType_Kill))
            {
                remains_pos = entity->pos;
                
                if(is_dungeon_pos_water(dungeon->tiles, remains_pos))
                {
                    if(entity->remains_type == EntityRemainsType_RedBlood)
                    {
                        remains = get_random(random, DungeonTileID_RedBloodWater1, DungeonTileID_RedBloodWater2);
                    }
                    else
                    {
                        assert(entity->remains_type == EntityRemainsType_GreenBlood);
                        remains = get_random(random, DungeonTileID_GreenBloodWater1, DungeonTileID_GreenBloodWater2);
                    }
                }
                else if(entity->remains_type == EntityRemainsType_RedBlood)
                {
                    remains = get_random(random, DungeonTileID_RedBloodGroundMedium1, DungeonTileID_RedBloodGroundLarge2);
                }
                else
                {
                    assert(entity->remains_type == EntityRemainsType_GreenBlood);
                    remains = get_random(random, DungeonTileID_GreenBloodGroundMedium1, DungeonTileID_GreenBloodGroundLarge2);
                }
            }
            
            assert(remains);
            }
        
        if(remains)
        {
            
#if 0
            printf("\nName: %s\n", entity->name.s);
            printf("Remains Type: %u\n\n", entity->remains_type);
            printf("Remains Place Type: %u\n", type);
            printf("Remains ID: %u\n", remains);
            printf("Remains Pos: %u, %u\n", remains_pos.x, remains_pos.y);
            printf("Is Remains Pos Wall: %u\n", is_dungeon_pos_wall(dungeon->tiles, remains_pos));
#endif
            
            set_dungeon_pos_remains_tile_id(dungeon->tiles, remains_pos, remains);
        }
    }
    }

internal b32
can_enemy_see_entity(Entity *enemy, Entity *target, DungeonTiles tiles)
{
    b32 result = false;
 
 if(enemy->dungeon_level == target->dungeon_level)
 {
  assert(is_enemy_entity_valid_in_level(enemy, target->dungeon_level));
  assert(is_entity_valid(target));
  
  if(is_pos_inside_dungeon_rect(target->pos, enemy->e.view_rect) &&
     !get_entity_status(target->statuses, EntityStatusType_Invisible))
  {
   // We use Bresenham's line algorithm here. The reason we don't use what the player uses for
   // visibility is because there are some problems that would have to get worked out. So we
   // approximate with this.
   
   v2u sight_pos = enemy->pos;
   
   s32 dx = absolute(target->pos.x - sight_pos.x);
   s32 sx = sight_pos.x < target->pos.x ? 1 : -1;
   
   s32 dy = absolute(target->pos.y - sight_pos.y);
   s32 sy = sight_pos.y < target->pos.y ? 1 : -1;
   
   s32 error = (dx > dy ? dx : -dy) / 2;
   s32 error_two = 0;
   
   for(;;)
   {
    //printf("pos: %u, %u\n", pos.x, pos.y);
    
    error_two = error;
    if(error_two > -dx)
    {
     error -= dy;
     sight_pos.x += sx;
    }
    
    if(error_two < dy)
    {
     error += dx;
     sight_pos.y += sy;
    }
    
    if(is_v2u_equal(sight_pos, target->pos))
    {
     result = true;
     break;
    }
    else if(!is_dungeon_pos_traversable(tiles, sight_pos))
    {
     assert(!is_v2u_equal(sight_pos, target->pos));
     break;
    }
   }
  }
 }
 
    return(result);
}

internal EntityInfo
get_random_enemy_entity_info(Random *random)
{
    assert(random);
    
    EntityID enemy_id = get_random_enemy_entity_id(random);
 EntityInfo result = get_enemy_entity_info(enemy_id);
    
    return(result);
}

internal u32
get_enemy_entity_level(EntityID id)
{
    u32 result = 0;
    
    switch(id)
    {
        case EntityID_Dummy:
        case EntityID_SkeletonWarrior:
        case EntityID_SkeletonArcher:
        case EntityID_SkeletonMage:
        case EntityID_Bat:
        case EntityID_Rat: result = 1; break;
        
        case EntityID_KoboldWarrior:
        case EntityID_KoboldShaman:
        case EntityID_Snail:
        case EntityID_Slime:
        case EntityID_Dog: result = 2; break;
        
        case EntityID_OrcWarrior:
        case EntityID_OrcArcher:
        case EntityID_OrcShaman:
        case EntityID_Python:
        case EntityID_Shade: result = 3; break;
        
        case EntityID_ElfKnight:
        case EntityID_ElfArbalest:
        case EntityID_ElfMage:
        case EntityID_GiantSlime:
        case EntityID_Spectre: result = 4; break;
        
        case EntityID_OrcAssassin:
        case EntityID_OrcSorcerer:
        case EntityID_Minotaur:
        case EntityID_Treant:
        case EntityID_Viper: result = 5; break;
        
        case EntityID_CentaurWarrior:
        case EntityID_CentaurSpearman:
        case EntityID_CentaurArcher:
        case EntityID_CursedSkull:
        case EntityID_Wolf: result = 6; break;
        
        case EntityID_OgreWarrior:
        case EntityID_OgreArcher:
        case EntityID_OgreMage:
        case EntityID_Cyclops:
        case EntityID_ShadowWalker: result = 7; break;
        
        case EntityID_DwarfKnight:
        case EntityID_DwarfSoldier:
        case EntityID_DwarfTinkerer:
        case EntityID_ScarletSnake:
        case EntityID_Lich: result = 8; break;
        
        case EntityID_AbyssalFiend:
        case EntityID_BloodTroll:
        case EntityID_IronGolem:
        case EntityID_Griffin:
        case EntityID_Imp: result = 9; break;
        
        case EntityID_BlackKnight:
        case EntityID_GiantDemon:
        case EntityID_Hellhound:
        case EntityID_AbyssalHexmaster:
        case EntityID_Zarimahar: result = 10; break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal u32
get_enemy_entity_flags(EntityID id)
{
    u32 result = 0;
 
    switch(id)
    {
        
        #if MOONBREATH_SLOW
        case EntityID_Dummy:
        {
        } break;
        #endif
        
        case EntityID_SkeletonWarrior:
        {
   set(result, EntityFlag_Undead);
   set(result, EntityFlag_CanOpenDoors);
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
   set(result, EntityFlag_FleeOnLowHP);
        } break;
        
        case EntityID_KoboldShaman:
  {
   set(result, EntityFlag_FleeOnLowHP);
        } break;
        
        case EntityID_Snail:
  {
   set(result, EntityFlag_FleeOnLowHP);
        } break;
        
        case EntityID_Slime:
        {
        } break;
        
        case EntityID_Dog:
  {
   set(result, EntityFlag_FleeOnLowHP);
        } break;
        
        case EntityID_OrcWarrior:
  {
   set(result, EntityFlag_FleeOnLowHP);
        } break;
        
        case EntityID_OrcArcher:
        {
   set(result, EntityFlag_FleeOnLowHP);
   set(result, EntityFlag_UsesRangedAttacks);
        } break;
        
        case EntityID_OrcShaman:
  {
   set(result, EntityFlag_FleeOnLowHP);
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
   set(result, EntityFlag_FleeOnLowHP);
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
        } break;
        
        case EntityID_OrcAssassin:
  {
   set(result, EntityFlag_FleeOnLowHP);
        } break;
        
        case EntityID_OrcSorcerer:
  {
   set(result, EntityFlag_FleeOnLowHP);
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
   set(result, EntityFlag_FleeOnLowHP);
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
   set(result, EntityFlag_FleeOnLowHP);
            set(result, EntityFlag_UsesRangedAttacks);
        } break;
        
        case EntityID_OgreMage:
  {
   set(result, EntityFlag_FleeOnLowHP);
        } break;
        
        case EntityID_Cyclops:
  {
   set(result, EntityFlag_FleeOnLowHP);
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
   set(result, EntityFlag_FleeOnLowHP);
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
   set(result, EntityFlag_FleeOnLowHP);
        } break;
        
        case EntityID_IronGolem:
        {
        } break;
        
        case EntityID_Griffin:
        {
        } break;
        
        case EntityID_Imp:
  {
   set(result, EntityFlag_FleeOnLowHP);
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
   set(result, EntityFlag_FleeOnLowHP);
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
get_enemy_entity_info(EntityID id)
{
    assert(id);
    
    EntityInfo result = {0};
    result.id = id;
 result.level = get_enemy_entity_level(id);
 result.flags = get_enemy_entity_flags(id);
    
    return(result);
}

internal b32
is_entity_under_any_status(EntityStatus *statuses)
{
    assert(statuses);
    
    for(u32 index = 0; index < MAX_ENTITY_STATUS_COUNT; ++index)
    {
        EntityStatus *status = &statuses[index];
  if(status->type)
  {
   return(true);
  }
    }
    
    return(false);
}

internal void
attack_entity_with_status(Game *game, Entity *entity, Dungeon *dungeon, UI *ui, EntityStatus *status)
{
    assert(game);
    assert(is_entity_valid(entity));
    assert(dungeon);
    assert(ui);
    assert(is_entity_status_valid(status));
    assert(status->chance);
    assert(status->duration);
    
        assert(status->player_active.s[0]);
        assert(status->player_active_color);
        
    u32 damage = attack_entity(&game->random, 0, 0, entity, dungeon, ui, status->damage);
        log_add(status->player_active.s, ui, start_color(status->player_active_color), damage);
    }

internal b32
end_inventory_item_interact(Inventory *inventory)
{
    assert(inventory);
    
    inventory->validate_view = true;
 inventory->interact_type = ItemInteractType_None;
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
  case ItemInteractType_Identify:
        {
            assert(!is_set(item->flags, ItemFlag_IsIdentified));
            
            set(item->flags, ItemFlag_IsIdentified);
            log_add("You identify the %s%s.", ui, get_item_status_color(&item->flags, item->rarity), get_full_item_name(item).s);
        } break;
        
  case ItemInteractType_EnchantWeapon:
  case ItemInteractType_EnchantArmor:
        {
            ++item->enchant_level;
            log_add("%sThe %s %s.", ui, start_color(Color_LightBlue), get_full_item_name(item).s, get_item_name_interact_string(random, item->name).s);
        } break;
        
  case ItemInteractType_Uncurse:
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
log_add_enemies_nearby(UI *ui)
{
    assert(ui);
    log_add("There are enemies nearby!", ui);
}

internal b32
can_player_auto_explore(u32 *examine_flags, EntityState *entity_state, Dungeon *dungeon, UI *ui)
{
    assert(examine_flags);
    assert(entity_state);
    assert(dungeon);
    assert(ui);
    
    b32 result = true;
    
    if(get_player_view_enemy_count(entity_state, dungeon))
    {
        result = false;
        
        log_add_enemies_nearby(ui);
        unset(*examine_flags, ExamineFlag_Open); // Unset in case we came through examine mode.
        }
    
    return(result);
}

internal void
add_entity_attack_status(EntityStatus *status,
                         EntityStatusType type,
                         s32 min_value,
                        s32 max_value,
                        u32 duration,
                        u32 chance)
{
    assert(!is_entity_status_valid(status));
    assert(type);
    assert(duration);
    assert(is_chance_valid(chance));
    
    switch(type)
    {
        case EntityStatusType_Poison:
        {
   assert(min_value > 0);
   assert(max_value > 0);
   
            status->value.type = EntityDamageType_Poison;
            status->value.skips_defence = true;
        } break;
        
        case EntityStatusType_Burn:
  {
   assert(min_value > 0);
   assert(max_value > 0);
   
            status->value.type = EntityDamageType_Fire;
            status->value.skips_defence = true;
        } break;
        
        case EntityStatusType_Bleed:
  {
   assert(min_value > 0);
   assert(max_value > 0);
   
            status->value.type = EntityDamageType_Physical;
            status->value.skips_defence = true;
        } break;
  
  case EntityStatusType_BrokenArmor:
  {
   assert(!min_value);
   assert(!max_value);
  } break;
  
  case EntityStatusType_Darkness:
  {
   assert(min_value < 0);
   assert(max_value < 0);
   
   status->stat_type = EntityStatusStatType_Darkness;
  } break;
  
        invalid_default_case;
    }
    
    status->type = type;
 status->value.min = min_value;
    status->value.max = max_value;
    status->chance = chance;
    status->duration = duration;
    }

internal void
start_examine_from_owner(ExamineMode *examine, Owner *owner)
{
    assert(examine);
    assert(owner);
    
    set(examine->flags, ExamineFlag_Open);
    unset(examine->flags, ExamineFlag_CameraOnExaminePos);
    
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
reset_inventory_multiple_pickup(ItemState *item_state,
                                u32 *inventory_flags,
                                u32 dungeon_level,
                                Owner *temp_owners)
{
    assert(item_state);
    assert(inventory_flags);
    assert(is_dungeon_level_valid(dungeon_level));
    assert(temp_owners);
    
    reset_all_owner_select_letters(temp_owners);
 unset_all_item_is_selected_flags(item_state, dungeon_level);
    unset(*inventory_flags, InventoryFlag_MultiplePickup);
}

internal char *
get_status_type_string(EntityStatusType type)
{
    char *result = 0;
    
    switch(type)
    {
        case EntityStatusType_Damage: result = "Damage"; break;
        case EntityStatusType_Heal: result = "Heal"; break;
        case EntityStatusType_Stat: result = "Stat"; break;
        case EntityStatusType_Poison: result = "Poison"; break;
        case EntityStatusType_Burn: result = "Burn"; break;
        case EntityStatusType_Bind: result = "Bind"; break;
        case EntityStatusType_Bleed: result = "Bleed"; break;
        case EntityStatusType_BrokenArmor: result = "Broken Armor"; break;
        case EntityStatusType_Confusion: result = "Confusion"; break;
  case EntityStatusType_Invisible: result = "Invisible"; break;
  case EntityStatusType_Darkness: result = "Darkness"; break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal char *
get_stat_type_string(EntityStatusStatType type)
{
    char *result = 0;
    
    switch(type)
    {
        case EntityStatusStatType_Str: result = "Str"; break;
        case EntityStatusStatType_Int: result = "Int"; break;
        case EntityStatusStatType_Dex: result = "Dex"; break;
        case EntityStatusStatType_Def: result = "Def"; break;
  case EntityStatusStatType_EV: result = "EV"; break;
  case EntityStatusStatType_FOV: result = "FOV"; break;
  case EntityStatusStatType_Darkness: result = "Darkness"; break;
  case EntityStatusStatType_StrIntDex: result = "StrIntDex"; break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal char *
get_entity_remains_string(EntityRemainsType type)
{
    char *result = 0;
    
    switch(type)
    {
        case EntityRemainsType_RedBlood: result = "Red Blood"; break;
        case EntityRemainsType_GreenBlood: result = "Green Blood"; break;
        
        invalid_default_case;
    }
    
    return(result);
    }

internal void
entity_move_force(Entity *entity,
                  Dungeon *dungeon,
                  v2u new_pos,
                  u32 dungeon_level)
{
    assert(is_entity_valid(entity));
    assert(dungeon);
    assert(is_dungeon_level_valid(dungeon_level));
    
 set_dungeon_pos_occupier(dungeon->tiles, entity->pos, 0);
 entity->new_pos = entity->pos = new_pos;
 set_dungeon_pos_occupier(dungeon->tiles, entity->new_pos, entity);
    
    if(entity->type == EntityType_Player)
    {
        entity->dungeon_level = dungeon_level;
        
  // This function is called when the player teleports etc. which means we want to update the
  // pathfind map immediately.
  assert(entity->p.pathfind_map_to_player);
  update_pathfind_map(entity, entity->p.pathfind_map_to_player, dungeon, entity->pos);
        }
    else
    {
        assert(entity->type == EntityType_Enemy);
  entity->e.view_rect = get_dungeon_dimension_rect(dungeon->size, new_pos, entity->stats.fov);
    }
}

internal u32
set_new_player_view_dungeon_traps_as_seen(DungeonTraps *traps, DungeonTiles tiles)
{
    u32 result = false;
    
 for(u32 index = 0; index < MAX_DUNGEON_TRAP_COUNT; ++index)
    {
  DungeonTrap *trap = &traps->array[index];
  
  if(is_tile_seen(tiles, trap->pos) &&
     !is_set(trap->flags, DungeonTrapFlag_HasBeenSeen))
        {
            set(trap->flags, DungeonTrapFlag_HasBeenSeen);
            ++result;
        }
    }
    
    return(result);
}

internal u32
get_player_view_enemy_count(EntityState *entity_state, Dungeon *dungeon)
{
    assert(entity_state);
    assert(dungeon);
    
    u32 result = false;
    
    for(u32 index = 0; index < MAX_ENTITY_COUNT; ++index)
    {
        Entity *enemy = &entity_state->array[index];
        
        if(is_enemy_entity_valid_in_level(enemy, dungeon->level) &&
               is_tile_seen(dungeon->tiles, enemy->pos))
        {
            ++result;
        }
    }
    
    return(result);
}

internal void
teleport_entity(Random *random,
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
    
 entity_move_force(entity, dungeon, pos, dungeon->level);
    }

internal EntityResistInfo
get_entity_resist_info_damage(Entity *entity, UI *ui, u32 damage, EntityDamageType damage_type)
{
    assert(is_entity_valid(entity));
    assert(ui);
    assert(damage);
    assert(damage_type);
    
 EntityResistInfo result = {0};
 result.damage_after = damage;
    
 EntityResist resist = entity->resists[damage_type];
    if(resist.value)
    {
  assert(is_entity_resist_valid(resist));
        
        f32 resist_percent = (f32)resist.value * (f32)(ENTITY_RESISTANCE_PER_POINT / 10);
  u32 damage_change = (u32)((f32)damage * resist_percent);
        
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
   result.type = EntityResistInfoType_Full;
            }
            else
        {
   result.type = EntityResistInfoType_Partial;
            }
        }
    
    return(result);
}

internal b32
is_entity_immune_to_damage_type(EntityResist *resists, EntityDamageType damage_type)
{
    assert(resists);
    assert(damage_type);
 
 b32 result = (resists[damage_type].value == MAX_ENTITY_RESIST_VALUE);
    return(result);
}

internal s32
get_total_stat_status_value(EntityStatus *statuses, EntityStatusStatType type)
{
    assert(statuses);
    assert(type);
    
    s32 result = 0;
    
    for(u32 index = 0; index < MAX_ENTITY_STATUS_COUNT; ++index)
    {
        EntityStatus *status = &statuses[index];
        
        if(status->type == EntityStatusType_Stat &&
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
                       EntityState *entity_state,
                       ItemState *item_state,
                       Dungeon *dungeon,
                       UI *ui)
{
 assert(is_player_entity_valid(player));
 assert(entity_state);
 assert(item_state);
 assert(dungeon);
 assert(ui);
 
    b32 result = true;
    
    u32 new_item_count = get_player_view_new_item_count(item_state, dungeon);
    if(new_item_count)
    {
    // TODO(rami): Add the name of the item based on if it's identified or not to the log.
        result = false;
        log_add_new_player_view_string("item", "items", new_item_count, ui);
        }
    
 u32 trap_count = set_new_player_view_dungeon_traps_as_seen(&dungeon->traps, dungeon->tiles);
    if(trap_count)
    {
        result = false;
        log_add_new_player_view_string("trap", "traps", trap_count, ui);
    }
    
    if(log_add_player_view_has_enemies(entity_state, dungeon, ui)) result = false;
    
    if(result)
 {
  PathfindResult pathfind_result = get_pathfind_result(player, dungeon, player->pathfind_map, PathfindMethod_Toward);
  if(pathfind_result.can_move)
   {
   player->new_pos = pathfind_result.info.pos;
   
   // Reached pathfind target
   if(is_v2u_equal(player->new_pos, player->pathfind_target_pos))
   {
    player->p.wait_before_pathfind_reset = true; // Wait before pathfind reset so it can be reset by a game time advancing player action.
    unset(player->flags, EntityFlag_Pathfinding);
   }
   
   // Add to pathfind trail
   if(is_dungeon_pos_traversable(dungeon->tiles, player->new_pos))
   {
    player->p.render_pathfind = true;
    
    for(u32 trail_index = 0; trail_index < MAX_PATHFIND_TRAIL_COUNT; ++trail_index)
    {
     PathfindTrail *trail = &player->p.pathfind_trail[trail_index];
     if(is_v2u_zero(trail->pos))
     {
      assert(!is_v2u_equal(player->new_pos, player->pos));
      
      trail->pos = player->pos;
      trail->direction = get_direction_between_positions(player->new_pos, player->pos);
      break;
     }
    }
   }
  }
  else
  {
   unset(player->flags, EntityFlag_Pathfinding);
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
    
#if 0
    if(!result)
    {
        if(entity)
        {
            printf("is_entity_valid: ID: %u\n", entity->id);
            printf("is_entity_valid: Type: %u\n", entity->type);
            printf("is_entity_valid: is_dungeon_level_valid: %u\n", is_dungeon_level_valid(entity->dungeon_level));
        }
        else
        {
            printf("is_entity_valid: Invalid pointer.\n");
        }
        
        printf("\n");
    }
    #endif
    
    return(result);
}

internal b32
is_player_entity_valid(Entity *entity)
{
 b32 result = (is_entity_valid(entity) &&
               entity->type == EntityType_Player);
 
    return(result);
}

internal b32
is_enemy_entity_valid(Entity *enemy)
{
 b32 result = (is_entity_valid(enemy) &&
               enemy->type == EntityType_Enemy &&
               enemy->e.wandering_type);
 
    return(result);
}

internal b32
is_enemy_entity_valid_in_level(Entity *enemy, u32 dungeon_level)
{
    assert(is_dungeon_level_valid(dungeon_level));
    
 b32 result = (is_enemy_entity_valid(enemy) &&
               enemy->dungeon_level == dungeon_level);
 
    return(result);
}

internal b32
is_entity_valid_in_level(Entity *entity, u32 dungeon_level)
{
    assert(is_dungeon_level_valid(dungeon_level));
    
 b32 result = (is_entity_valid(entity) &&
               entity->dungeon_level == dungeon_level);
 
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

internal Entity *
get_dungeon_pos_entity(EntityState *entity_state, u32 dungeon_level, v2u pos)
{
 assert(entity_state);
 assert(is_dungeon_level_valid(dungeon_level));
 
    for(u32 index = 0; index < MAX_ENTITY_COUNT; ++index)
    {
        Entity *entity = &entity_state->array[index];
  if(is_entity_valid_in_level(entity, dungeon_level) &&
     is_v2u_equal(entity->pos, pos))
        {
            return(entity);
        }
    }
    
    return(0);
}

internal void
init_entity_pathfind(Entity *entity, Dungeon *dungeon, PathfindMap *map, v2u target_pos)
{
 assert(is_entity_valid(entity));
 assert(dungeon);
 assert(map);
 assert(!is_v2u_zero(target_pos));
 
 if(!is_v2u_equal(entity->pos, target_pos))
    {
  set(entity->flags, EntityFlag_Pathfinding);
  entity->pathfind_target_pos = target_pos;
  update_pathfind_map(entity, map, dungeon, target_pos);
  
  if(entity->type == EntityType_Player) reset_player_pathfind_trail(entity);
    }
}

internal void
add_player_starting_item(Game *game,
                         Entity *player,
                         ItemState *item_state,
                         Inventory *inventory,
                         UI *ui,
                         ItemID item_id)
{
    assert(game);
    assert(is_player_entity_valid(player));
    assert(item_state);
    assert(inventory);
    assert(ui);
    assert(item_id);
    
 Random *random = &game->random;
 
    Item *item = 0;
    if(is_item_id_weapon(item_id))
    {
  item = add_weapon_item(random, item_state, player->dungeon_level, item_id, ItemRarity_Common, player->pos.x, player->pos.y, false);
    }
    else if(is_item_id_potion(item_id))
    {
  item = add_consumable_item(random, item_state, player->dungeon_level, item_id, player->pos.x, player->pos.y, 1);
        set_potion_or_scroll_as_known(item_state, item->id);
        
        assert(!item->enchant_level);
    }
    
    assert(item);
    assert(!is_set(item->flags, ItemFlag_IsCursed));
    
 add_item_to_inventory(game, player, item, item_state, inventory, ui, player->dungeon_level, false);
    
    if(is_item_id_weapon(item_id))
    {
        item->enchant_level = 0;
        equip_item(game, player, item, inventory, ui, false);
    }
}

internal b32
is_enemy_entity_alerted(u32 count)
{
 b32 result = (count == 1);
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
                        EntitySummonType type,
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
        
        if(!spell->status_type)
        {
            strcpy(spell->name.s, name);
            spell->status_type = EntityStatusType_Summon;
            spell->chance = chance;
            
            spell->summon_flags = flags;
            spell->summon_type = type;
            
            switch(type)
            {
    case EntitySummonType_Undead: strcpy(spell->description.s, "Summons a random undead enemy in the vicinity."); break;
                
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
                        EntityDamageType damage_type,
                        u32 chance,
                        u32 range)
{
    assert(is_enemy_entity_valid(entity));
    assert(name);
    assert(damage_type);
    assert(damage);
    assert(is_chance_valid(chance));
    
    for(u32 index = 0; index < MAX_ENTITY_SPELL_COUNT; ++index)
    {
        Spell *spell = &entity->e.spells[index];
        
        if(!spell->status_type)
        {
            strcpy(spell->name.s, name);
            
            spell->status_type = EntityStatusType_Damage;
            spell->damage.type = damage_type;
            spell->damage.min = 1;
            spell->damage.max = damage;
            spell->chance = chance;
            spell->range = range;
            
            sprintf(spell->description.s, "Deals %u-%u %s damage to the target.",
                        spell->damage.min, spell->damage.max, get_entity_damage_type_string(damage_type));
            
            ++entity->e.spell_count;
            return;
        }
    }
    
    assert(0);
}

internal void
add_entity_status_spell(Entity *entity,
                         char *name,
                        EntityStatusType status_type,
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
        
        if(!spell->status_type)
        {
            strcpy(spell->name.s, name);
            
            spell->status_type = status_type;
            spell->duration = duration;
            spell->chance = chance;
            spell->range = range;
            
            switch(status_type)
            {
                case EntityStatusType_Bind:
                {
                    assert(!value);
                    sprintf(spell->description.s, "Stops the target from moving for %u turns.", spell->duration);
                } break;
                
                case EntityStatusType_Confusion:
                {
                    assert(!value);
                    sprintf(spell->description.s, "Confuses the target for %u turns.", spell->duration);
                } break;
                
                case EntityStatusType_Burn:
                {
                    assert(value);
                    
                    spell->value.min = 1;
                    spell->value.max = value;
                    spell->value.type = EntityDamageType_Fire;
                    sprintf(spell->description.s, "Deals %u - %u %s damage to the target for %u turns.", spell->value.min, spell->value.max, get_entity_damage_type_string(EntityDamageType_Fire), spell->duration); 
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
                      u32 value_min,
                      u32 value_max,
                      u32 chance,
                      u32 range)
{
    assert(is_enemy_entity_valid(entity));
    assert(name);
    assert(value_min);
    assert(value_max);
    assert(is_chance_valid(chance));
    
    for(u32 index = 0; index < MAX_ENTITY_SPELL_COUNT; ++index)
    {
        Spell *spell = &entity->e.spells[index];
        
        if(!spell->status_type)
        {
            strcpy(spell->name.s, name);
            
            spell->status_type = EntityStatusType_Heal;
            spell->value.min = value_min;
            spell->value.max = value_max;
            spell->chance = chance;
            spell->range = range;
            
            if(value_min == value_max)
            {
                sprintf(spell->description.s, "Heals the target for %u health.", value_max);
            }
            else
            {
                sprintf(spell->description.s, "Heals the target for %u-%u health.", value_min, value_max);
            }
            
            ++entity->e.spell_count;
            return;
        }
    }
    
    assert(0);
}

internal void
add_entity_stat_spell(Entity *entity,
                      char *name,
                      EntityStatusStatType stat_type,
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
        
        if(!spell->status_type)
        {
            strcpy(spell->name.s, name);
            
            spell->status_type = EntityStatusType_Stat;
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
            spell->duration = duration;
            spell->chance = chance;
            spell->range = range;
            
            char *action = "Increases";
            if(spell->value.max < 0)
            {
                action = "Decreases";
            }
            
            char *stat = 0;
            switch(spell->stat_type)
            {
                case EntityStatusStatType_Str: stat = "Strength"; break;
                case EntityStatusStatType_Int: stat = "Intelligence"; break;
                case EntityStatusStatType_Dex: stat = "Dexterity"; break;
                case EntityStatusStatType_Def: stat = "Defence"; break;
    case EntityStatusStatType_EV: stat = "Evasion"; break;
                case EntityStatusStatType_StrIntDex: stat = "Strength, Intelligence and Dexterity"; break;
                    
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
                                EntityStatus *status,
                                b32 is_add)
{
 
    assert(is_entity_valid(entity));
    assert(ui);
    assert(is_entity_status_valid(status));
        assert(status->value.max);
    
#if 0
    printf("Entity name: %s\n", entity->name.s);
    printf("Status name: %s\n", status->name.s);
 printf("Status duration: %u\n", status->duration);
 printf("Is Add: %u\n\n", is_add);
 #endif
 
 // This function is called before status is removed after ending, we need to pass to update
 // the entity stats.
    if(!status->stat_value_applied || !is_add)
    {
  if(is_add) status->stat_value_applied = true;
  
        switch(status->stat_type)
        {
    case EntityStatusStatType_Str:
    {
     change_entity_stat(&entity->stats.str, status->value.max, is_add);
    } break;
    
    case EntityStatusStatType_Int:
    {
     change_entity_stat(&entity->stats.intel, status->value.max, is_add);
    } break;
    
    case EntityStatusStatType_Dex:
    {
     change_entity_stat(&entity->stats.dex, status->value.max, is_add);
    } break;
    
    case EntityStatusStatType_EV:
    {
     change_entity_stat(&entity->stats.ev, status->value.max, is_add);
    } break;
    
    case EntityStatusStatType_Def:
    {
     change_entity_stat(&entity->stats.def, status->value.max, is_add);
    } break;
    
    case EntityStatusStatType_FOV:
   {
    change_entity_stat(&entity->stats.fov, status->value.max, is_add);
    } break;
   
   case EntityStatusStatType_Darkness:
   {
    change_entity_stat(&entity->stats.fov, status->value.max, is_add);
   } break;
   
                case EntityStatusStatType_StrIntDex:
            {
                    change_entity_stat(&entity->stats.str, status->value.max, is_add);
                    change_entity_stat(&entity->stats.intel, status->value.max, is_add);
                    change_entity_stat(&entity->stats.dex, status->value.max, is_add);
        } break;
    }
    }
}

internal void
add_entity_status(Random *random,
                    Entity *entity,
                    Dungeon *dungeon,
                    UI *ui,
                  EntityStatus *new_status)
{
    assert(random);
    assert(is_entity_valid(entity));
    assert(ui);
    assert(is_entity_status_valid(new_status));
 
    for(u32 index = 0; index < MAX_ENTITY_STATUS_COUNT; ++index)
    {
        EntityStatus *status = &entity->statuses[index];
        if(!status->type)
        {
            *status = *new_status;
            
            // Set status value from value range
            ItemInfo *info = status->item_info;
            if(info)
            {
                assert(!is_v2u_zero(info->value_range));
                
                status->is_player_active_custom = true;
                status->value.max = get_random_from_v2u(random, info->value_range);
                }
            
            switch(status->type)
            {
                case EntityStatusType_Stat: update_entity_stats_from_status(entity, ui, status, true); break;
    
                case EntityStatusType_Poison:
                {
                    assert(entity->type == EntityType_Player);
                    
                    status->is_player_active_custom = true;
                    status->print_end_on_last_status = true;
                    status->player_active_color = Color_White;
                    
                    strcpy(status->player_start.s, "You feel poisoned!");
                    strcpy(status->player_end.s, "You don't feel poisoned anymore.");
                    strcpy(status->player_active.s, "%sThe poison wrecks your body, %u damage.");
                } break;
                
                case EntityStatusType_Burn:
                {
                    assert(entity->type == EntityType_Player);
                    
                    status->is_player_active_custom = true;
                    status->print_end_on_last_status = true;
                    status->player_active_color = Color_White;
                    
                    strcpy(status->player_start.s, "Your body starts burning!");
                    strcpy(status->player_end.s, "Your body stops burning.");
                    strcpy(status->player_active.s, "%sYour body burns, %u damage.");
                    } break;
                
                case EntityStatusType_Bleed:
                {
                    assert(entity->type == EntityType_Player);
                    
                    status->is_player_active_custom = true;
                    status->print_end_on_last_status = true;
                    status->player_active_color = Color_White;
                    
                    strcpy(status->player_start.s, "Your wounds start bleeding!");
                    strcpy(status->player_end.s, "Your wounds stop bleeding.");
                    strcpy(status->player_active.s, "%sYour wounds bleed, %u damage.");
                } break;
                
                case EntityStatusType_Bind:
                {
                    status->print_end_on_last_status = true;
                        
                        if(entity->type == EntityType_Player)
                        {
                        strcpy(status->player_start.s, "You feel like you can't move!");
                        strcpy(status->player_end.s, "You feel like you can move again.");
                        }
                } break;
                
                case EntityStatusType_Confusion:
                {
                    status->print_end_on_last_status = true;
                    
                    if(entity->type == EntityType_Player)
                    {
                        strcpy(status->player_start.s, "You feel very confused!");
                        strcpy(status->player_end.s, "You don't feel confused anymore.");
                    }
                } break;
                
                case EntityStatusType_BrokenArmor:
                {
                    status->print_end_on_last_status = true;
                    
                    if(entity->type == EntityType_Player)
                    {
                        strcpy(status->player_start.s, "Your armor feels broken!");
                        strcpy(status->player_end.s, "Your armor doesn't feel broken anymore.");
                    }
    } break;
    
    case EntityStatusType_Darkness:
    {
     update_entity_stats_from_status(entity, ui, status, true);
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
        EntityStatus *status = &entity->statuses[index];
        
        if(status->type)
        {
            switch(status->type)
            {
                case EntityStatusType_Damage: attack_entity(&game->random, 0, 0, entity, dungeon, ui, status->value); break;
                
                case EntityStatusType_Heal:
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
                            
                            if(status->is_value_percent)
                                {
                                    //printf("status->value.max: %u\n", status->value.max);
                                    
        // Get percent value from entity->max_hp.
                                    f32 percent_value = ((f32)status->value.max / 100.0f) * (f32)entity->max_hp;
                                status->value.max = get_u32_from_up_rounded_f32(percent_value);
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
                
                case EntityStatusType_Stat: update_entity_stats_from_status(entity, ui, status, true); break;
                
                case EntityStatusType_Burn:
                case EntityStatusType_Poison:
                case EntityStatusType_Bleed: attack_entity_with_status(game, entity, dungeon, ui, status); break;
                case EntityStatusType_Bind: break;
                case EntityStatusType_BrokenArmor: break;
                case EntityStatusType_Confusion: break;
    case EntityStatusType_Invisible: break;
    case EntityStatusType_Darkness: update_entity_stats_from_status(entity, ui, status, true); break;
                
                invalid_default_case;
            }
            
            if(status->duration) --status->duration;
            
            // End status
            if(!status->duration)
            {
    b32 can_print_end = true;
    
                if(status->print_end_on_last_status &&
       get_entity_status_type_count(entity->statuses, status->type) > 1)
    {
      can_print_end = false;
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
                        case EntityStatusType_Bind: break;
                        case EntityStatusType_Heal: break;
                        case EntityStatusType_Burn: break;
                        case EntityStatusType_Bleed: break;
                        case EntityStatusType_Poison: break;
                        case EntityStatusType_Confusion: break;
      case EntityStatusType_BrokenArmor: break;
      case EntityStatusType_Invisible: break;
                        
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
    assert(is_entity_spell_valid(spell));
    
    EntityStatus status = {0};
    strcpy(status.name.s, spell->name.s);
    status.type = spell->status_type;
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
    assert(is_entity_spell_valid(caster->e.spell));
    assert(is_entity_valid(target));
    assert(dungeon);
    assert(ui);
    
 log_add_entity_interact_string(random, caster, 0, target, ui, 0, EntityResistInfoType_None, EntityInteractInfoType_None);
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
    
    if(defender->stats.ev)
    {
        // An attack against an invisible entity has a lower chance of hitting.
        u32 attacker_hit_chance = attacker->hit_chance;
        if(get_entity_status(defender->statuses, EntityStatusType_Invisible))
        {
            attacker_hit_chance /= 2;
        }
        
        // An attack from an invisible entity has a higher chance of hitting.
        u32 defender_ev = defender->stats.ev;
        if(get_entity_status(attacker->statuses, EntityStatusType_Invisible))
        {
            defender_ev /= 3;
        }
        
        if(get_random(random, 1, attacker_hit_chance) >= defender_ev)
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
        EntityStatus *status = &entity->statuses[index];
        if(status->type && status->type == EntityStatusType_Bind)
        {
            result = false;
            if(entity->type == EntityType_Player) log_add("You aren't able to move!", ui);
            
            break;
            }
    }
    
    return(result);
}

internal void
move_player_between_dungeons(Game *game,
                             EntityState *entity_state,
                             DungeonState *dungeon_state,
                             ItemState *item_state,
                             Inventory *inventory,
                             UI *ui,
                             v2u new_pos,
                             u32 new_dungeon_level)
{
    assert(is_dungeon_level_valid(new_dungeon_level));
    
    // Unset player occupancy on the passage tile before moving to another dungeon level.
    Entity *player = get_player_entity();
    Dungeon *current_dungeon = get_dungeon_from_level(dungeon_state, dungeon_state->current_level);
 set_dungeon_pos_occupier(current_dungeon->tiles, player->pos, 0);
    
    Dungeon *new_dungeon = get_dungeon_from_level(dungeon_state, new_dungeon_level);
    if(new_dungeon->level)
    {
        dungeon_state->current_level = new_dungeon_level;
  entity_move_force(player, new_dungeon, new_pos, new_dungeon_level);
    }
    else
    {
        create_dungeon(game, player, entity_state, dungeon_state, item_state, inventory, ui, new_dungeon_level, player->pos);
    }
}

internal void
entity_use_passage(Game *game,
                   EntityState *entity_state,
                   DungeonState *dungeon_state,
                   ItemState *item_state,
                   Inventory *inventory,
                   UI *ui,
                   DungeonPassage *passage)
{
    assert(passage->type);
    assert(!is_v2u_zero(passage->pos));
    
    u32 new_dungeon_level = dungeon_state->current_level;
    switch(passage->type)
    {
        case DungeonPassageType_Up:
        {
            --new_dungeon_level;
            log_add("%sYou go up the passage.", ui, start_color(Color_LightGray));
        } break;
        
        case DungeonPassageType_Down:
        {
            ++new_dungeon_level;
            log_add("%sYou go down the passage.", ui, start_color(Color_LightGray));
        } break;
        
        invalid_default_case;
    }
    
    move_player_between_dungeons(game, entity_state, dungeon_state, item_state, inventory, ui, passage->dest_pos, new_dungeon_level);
    }

internal b32
entity_move(Random *random, Entity *entity, Dungeon *dungeon, UI *ui, v2u new_pos)
{
    assert(random);
    assert(is_entity_valid(entity));
    assert(!is_v2u_equal(entity->pos, new_pos));
    assert(!is_v2u_zero(new_pos));
    assert(dungeon);
    assert(ui);
    
    b32 result = false;
    
    if(entity_can_move(entity, ui))
    {
            result = true;
            
        // If the entity is confused, attempt to move them in a direction that is different from the
        // original move direction.
        EntityStatus *confused = get_entity_status(entity->statuses, EntityStatusType_Confusion);
  if(confused)
    {
   assert(confused->chance);
        
   if(hit_random_chance(random, confused->chance))
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
                
                    new_pos = confused_move_pos;
                    
                    switch(entity->type)
                    {
                        case EntityType_Player: log_add("%sYou stumble around.", ui, start_color(Color_LightGray)); break;
                        case EntityType_Enemy: log_add("%sThe %s stumbles around.", ui, start_color(Color_LightGray), entity->name.s); break;
                        
                        invalid_default_case;
                    }
            }
        }
    }
  
  #if MOONBREATH_SLOW
  if(!can_entity_move_to_pos(entity, dungeon, new_pos, false))
  {
   printf("New Pos: %u, %u\n", new_pos.x, new_pos.y);
   printf("Traversable or closed door: %u\n", is_dungeon_pos_traversable_or_closed_door(dungeon->tiles, new_pos));
   printf("Occupier: %p\n\n", get_dungeon_pos_occupier(dungeon->tiles, new_pos));
   
   assert(0);
  }
  #endif
  
  entity_move_force(entity, dungeon, new_pos, dungeon->level);
  
  if(entity->type == EntityType_Player)
  {
   set_new_player_view_dungeon_traps_as_seen(&dungeon->traps, dungeon->tiles);
  }
        else if(entity->type == EntityType_Enemy)
        {
            update_enemy_facing_direction(entity, 0, true);
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
        if(entity->hp > entity->max_hp) entity->hp = entity->max_hp;
    }
    
    return(result);
}

internal void
remove_entity(Entity *entity, DungeonTiles tiles)
{
    assert(is_entity_valid(entity));
    
 set_dungeon_pos_occupier(tiles, entity->pos, 0);
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
            if(get_entity_status(entity->statuses, EntityStatusType_Invisible))
            {
                log_add("%sThe Something dies!", ui, start_color(Color_LightRed));
            }
            else
            {
            log_add("%sThe %s dies!", ui, start_color(Color_LightRed), entity->name.s);
            }
            
            place_entity_remains(random, entity, dungeon, EntityRemainsPlaceType_Kill);
            remove_entity(entity, dungeon->tiles);
        } break;
        
        invalid_default_case;
    }
}

internal u32
attack_entity(Random *random,
              Entity *attacker,
              DungeonTrap *trap,
              Entity *defender,
              Dungeon *dungeon,
              UI *ui,
              EntityDamage damage_info)
{
    u32 result = 0;
 
 #if MOONBREATH_SLOW
 b32 print_info = 0;
 #endif
 
#if MOONBREATH_SLOW
 if(print_info)
 {
  printf("\n--------------------\n");
  
  if(attacker)
  {
   printf("Attacker: %s\n", attacker->name.s);
  }
  else
  {
   printf("Attacker: None\n");
  }
  
  printf("Defender: %s\n", defender->name.s);
  printf("Damage Info Min: %u\n", damage_info.min);
  printf("Damage Info Max: %u\n", damage_info.max);
  printf("Damage Type: %s\n", get_entity_damage_type_string(damage_info.type));
 }
    #endif
    
    assert(random);
 assert(is_entity_valid(defender));
    assert(dungeon);
    assert(ui);
    assert(damage_info.type);
 
#if MOONBREATH_SLOW
 if(attacker) assert(is_entity_valid_in_level(attacker, defender->dungeon_level));
    if(defender->id == EntityID_Dummy) return(0);
    
    // Player hit chance test
 if(is_player_entity_valid(attacker) && debug_toggles[DebugToggleType_EntityHitTest])
    {
        u32 hit_count = 0;
        u32 miss_count = 0;
        u32 test_count = 1000;
        
        for(u32 count = 0; count < test_count; ++count)
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
  
        printf("Attempts: %u\n", test_count);
  printf("Hits: %u (%.0f%%)\n", hit_count, get_percentage(hit_count, test_count));
  printf("Misses: %u (%.0f%%)\n\n", miss_count, get_percentage(miss_count, test_count));
        
        printf("Attacker Hit Chance: %u\n", attacker->hit_chance);
        printf("Defender Evasion: %u\n", defender->stats.ev);
        
        return(0);
    }
#endif
 
  if(defender->type == EntityType_Enemy)
 {
  if(defender->e.state == EnemyEntityState_Cornered)
  {
   set(defender->flags, EntityFlag_FightingFromCornered);
  }
  else if(defender->e.state != EnemyEntityState_Fleeing)
  {
   defender->e.state = EnemyEntityState_Fighting;
  }
  }
 
    if(!attacker || will_entity_hit(random, attacker, defender))
 {
  if(!is_zero(damage_info.max))
        {
            assert(damage_info.min);
            u32 damage = get_random(random, damage_info.min, damage_info.max);
   
            // Apply defender defence to damage
            if(!damage_info.skips_defence && defender->stats.def)
            {
                if(get_entity_status(defender->statuses, EntityStatusType_BrokenArmor))
                {
                    damage -= get_random(random, 0, defender->stats.def / 2);
                }
                else
                {
                    damage -= get_random(random, 0, defender->stats.def);
                }
                }
   
            if(!is_zero(damage))
            {
                // Apply defender resistances
    EntityResistInfo resist_info = get_entity_resist_info_damage(defender, ui, damage, damage_info.type);
    damage = resist_info.damage_after;
    
#if MOONBREATH_SLOW
    if(print_info)
    {
     printf("\nresist_info.type: %u\n", resist_info.type);
     printf("resist_info.damage_after: %u\n", resist_info.damage_after);
    }
    #endif
    
                log_add_entity_interact_string(random, attacker, trap, defender, ui, damage, resist_info.type, EntityInteractInfoType_None);
                
                if(!is_zero(damage))
    {
                    result = damage;
                    defender->hp -= damage;
                    
                    // Reset defender health regen
                    defender->regen.next_turn = false;
                    defender->regen.current = 0;
                    
                    if(!is_zero(defender->hp))
                    {
                        if(hit_random_chance(random, 30)) place_entity_remains(random, defender, dungeon, EntityRemainsPlaceType_Wound);
                        
                        // Start on hit status
                        if(attacker && attacker->type == EntityType_Enemy)
                        {
                            EntityStatus *status = &attacker->e.on_hit_status;
                            
                            if(status->type &&
                                   !get_entity_status(defender->statuses, status->type) &&
                               hit_random_chance(random, status->chance))
       {
        if(status->value.type && is_entity_immune_to_damage_type(defender->resists, status->value.type))
                                {
                                    log_add("You resist the %s!", ui, get_entity_damage_type_string(status->value.type));
                                }
                                else
                                {
                                    add_entity_status(random, defender, dungeon, ui, status);
                                }
                            }
                        }
                        
                        // On hit teleport
                        if(defender &&
                               defender->type == EntityType_Enemy &&
                               hit_random_chance(random, defender->e.hit_teleport_chance))
                        {
                            teleport_entity(random, defender, dungeon, ui, DungeonRandomPosType_TraversableRect);
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
                // Attacker blocked by defenders armor
    log_add_entity_interact_string(random, attacker, trap, defender, ui, damage, EntityResistInfoType_None, EntityInteractInfoType_AttackBlocked);
                }
            }
        else
  {
            // Attacker deals zero damage
            assert(is_player_entity_valid(attacker));
   log_add_entity_interact_string(random, attacker, 0, defender, ui, 0, EntityResistInfoType_None, EntityInteractInfoType_None);
  }
  }
    else
 {
        // Attacker misses defender
  log_add_entity_interact_string(random, attacker, trap, defender, ui, 0, EntityResistInfoType_None, EntityInteractInfoType_AttackMissed);
    }
 
 // Defender is fighting an invisible attacker
 if(attacker &&
    get_entity_status(attacker->statuses, EntityStatusType_Invisible) &&
    !is_set(defender->flags, EntityFlag_FightingWithInvisible))
 {
  assert(is_player_entity_valid(attacker));
  set(defender->flags, EntityFlag_FightingWithInvisible);
 }
 
#if MOONBREATH_SLOW
 if(print_info)
 {
  printf("--------------------\n");
 }
 #endif
 
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
 if(debug_toggles[DebugToggleType_SkipHasBeenUp] && state->is_down) return(true);
#endif
    
    b32 result = was_pressed_core(state);
    return(result);
}

internal UpdateEnemyPathfindType
update_enemy_pathfind(Entity *enemy, Dungeon *dungeon, PathfindMap *pathfind_map, v2u pathfind_target_pos)
{
 UpdateEnemyPathfindType result = UpdateEnemyPathfindType_None;
 
 if(!is_set(enemy->flags, EntityFlag_Pathfinding))
 {
  init_entity_pathfind(enemy, dungeon, pathfind_map, pathfind_target_pos);
 }
 assert(is_set(enemy->flags, EntityFlag_Pathfinding));
 
  PathfindResult pathfind_result = get_pathfind_result(enemy, dungeon, pathfind_map, PathfindMethod_Toward);
  if(pathfind_result.can_move)
  {
   enemy->new_pos = pathfind_result.info.pos;
   if(is_v2u_equal(enemy->new_pos, enemy->pathfind_target_pos))
   {
    unset(enemy->flags, EntityFlag_Pathfinding);
    result = UpdateEnemyPathfindType_ReachedTarget;
   }
  }
  else
  {
   unset(enemy->flags, EntityFlag_Pathfinding);
   result = UpdateEnemyPathfindType_CantMove;
  }
 
 return(result);
}

internal void
update_enemy_wandering(Random *random, Entity *enemy, Dungeon *dungeon)
{
 assert(random);
 assert(is_enemy_entity_valid(enemy));
 assert(dungeon);
 
 if(enemy->e.wandering_type == EnemyWanderingType_Random)
 {
  for(;;)
  {
  enemy->direction = get_random_direction(random);
   enemy->new_pos = get_direction_pos(enemy->pos, enemy->direction);
   
   #if 0
   printf("enemy->new_pos: %u, %u\n", enemy->new_pos.x, enemy->new_pos.y);
   printf("enemy->direction: %s\n", get_direction_string(enemy->direction));
#endif
   
   if(!get_dungeon_pos_occupier(dungeon->tiles, enemy->new_pos) &&
      can_entity_move_to_pos(enemy, dungeon, enemy->new_pos, false))
   {
    break;
   }
   }
  }
 else
 {
  assert(enemy->e.wandering_type == EnemyWanderingType_Travel);
  
  v2u pathfind_target = {0};
  if(!is_set(enemy->flags, EntityFlag_Pathfinding))
  {
   for(;;)
   {
    assert_loop_count();
    
    pathfind_target = get_random_dungeon_pos(random, dungeon->size);
    if(!is_dungeon_pos_inside_rect(pathfind_target, enemy->e.view_rect) &&
       can_entity_move_to_pos(enemy, dungeon, pathfind_target, false))
    {
     break;
    }
   }
  }
  
  update_enemy_pathfind(enemy, dungeon, enemy->pathfind_map, pathfind_target);
  }
}

internal void
update_enemy_entities(Game *game,
                      EntityState *entity_state,
                      ItemState *item_state,
                      Inventory *inventory,
                      DungeonState *dungeon_state,
                      UI *ui,
                      f32 passed_time)
{
    assert(game);
    assert(entity_state);
    assert(item_state);
    assert(inventory);
    assert(dungeon_state);
    assert(ui);
    
    Random *random = &game->random;
 Entity *player = get_player_entity();
    
    for(u32 enemy_index = 0; enemy_index < MAX_ENTITY_COUNT; ++enemy_index)
    {
        Entity *enemy = &entity_state->array[enemy_index];
        if(is_enemy_entity_valid(enemy))
        {
            
#if MOONBREATH_SLOW
   //if(enemy->id == EntityID_SkeletonWarrior) continue;
            if(enemy->id == EntityID_Dummy) continue;
            //continue;
#endif
            
            // We use passed_time instead of game->passed_time because this functions is called from places that
            // want to pass the player action count as the amount of passed time.
            if(passed_time)
            {
                Dungeon *dungeon = get_dungeon_from_level(dungeon_state, enemy->dungeon_level);
    
    // Go back to wandering if player changes dungeon level.
    if(player->dungeon_level != enemy->dungeon_level)
    {
     enemy->e.state = EnemyEntityState_Wandering;
     
     #if 0
     printf("can enemy see player: %u\n", can_enemy_see_entity(enemy, player, dungeon->tiles));
      printf("enemy name: %s\n", enemy->name.s);
     printf("enemy level: %u\n", enemy->dungeon_level);
     printf("player dungeon level: %u\n\n", player->dungeon_level);
     #endif
     
    }
    
    // Update player seen count
    if(can_enemy_see_entity(enemy, player, dungeon->tiles))
    {
     ++enemy->e.player_seen_count;
    }
    else
    {
     enemy->e.player_seen_count = 0;
    }
    
    // Get enemy turn count
                enemy->e.action_timer += passed_time;
                f32 enemy_available_time = enemy->e.action_timer;
    u32 enemy_turn_count = get_enemy_turn_count(&enemy_available_time, enemy->action_time);
    
                if(is_dungeon_pos_water(dungeon->tiles, enemy->pos) &&
                   !is_set(enemy->flags, EntityFlag_NormalWaterMovement))
                {
                    enemy_turn_count = get_enemy_turn_count(&enemy_available_time, enemy->action_time);
                }
    
                while(enemy_turn_count)
                {
                    
                    #if 0
                    printf("game->time: %.02f\n", game->time);
                    printf("enemy->e.action_timer: %.02f\n", enemy->e.action_timer);
                    printf("enemy_turn_count: %u\n", enemy_turn_count);
                    printf("\n");
#endif
                    
                    --enemy_turn_count;
                    enemy->e.action_timer = 0;
     
      #if MOONBREATH_SLOW
      b32 print_state = 0;
#endif
     
     // After changing enemy state we can jump here to update immediately.
     update_states:
     
     if(enemy->e.state == EnemyEntityState_Wandering)
     {
      
#if MOONBREATH_SLOW
      if(print_state) print_enemy_entity_state(enemy->e.state);
      #endif
      
      if(can_enemy_see_entity(enemy, player, dungeon->tiles))
      {
       reset_enemy_pathfind(enemy);
       
       enemy->e.state = EnemyEntityState_Fighting;
       goto update_states;
      }
      else
      {
       update_enemy_wandering(random, enemy, dungeon);
      }
     }
     else if(enemy->e.state == EnemyEntityState_Fighting)
     {
      
#if MOONBREATH_SLOW
      if(print_state) print_enemy_entity_state(enemy->e.state);
#endif
      
      update_enemy_facing_direction(enemy, player, false);
      
      if(!is_set(enemy->flags, EntityFlag_FightingFromCornered) && // If enemy fights out of desparation then don't go back to fleeing.
         is_set(enemy->flags, EntityFlag_FleeOnLowHP) &&
         get_percentage(enemy->hp, enemy->max_hp) <= 25.0f)
      {
       enemy->e.state = EnemyEntityState_Fleeing;
       goto update_states;
      }
      else
      {
       if(is_set(enemy->flags, EntityFlag_FightingWithInvisible))
       {
        // If direction is valid then player is in the immediate vicinity of the enemy.
        Direction direction = get_direction_between_positions(player->pos, enemy->pos);
        if(direction)
        {
         assert(is_v2u_equal(get_direction_pos(enemy->pos, direction), player->pos));
         
         // Attack
         if(enemy->e.spell_count)
         {
          select_and_cast_entity_spell(random, entity_state, enemy, dungeon, ui);
         }
         else
         {
          attack_entity(random, enemy, 0, player, dungeon, ui, enemy->e.damage);
         }
        }
        else
        {
         log_add("The %s attacks nothing but air.", ui, enemy->name.s);
         
         unset(enemy->flags, EntityFlag_FightingWithInvisible);
         enemy->e.state = EnemyEntityState_Wandering;
         goto update_states;
        }
       }
       else
       {
        if(can_enemy_see_entity(enemy, player, dungeon->tiles))
        {
         PathfindResult pathfind_result = get_pathfind_result(enemy, dungeon, enemy->e.pathfind_map_to_player, PathfindMethod_Toward);
         if(enemy->e.spell_count)
         {
          EntitySpellCastResult cast_result = select_and_cast_entity_spell(random, entity_state, enemy, dungeon, ui);
          if(cast_result.target_not_in_spell_range)
          {
           enemy->new_pos = pathfind_result.info.pos;
          }
         }
         else if((is_dungeon_pos_inside_rect(player->pos, enemy->e.view_rect) &&
                  is_set(enemy->flags, EntityFlag_UsesRangedAttacks)) ||
                 is_v2u_equal(pathfind_result.info.pos, player->pos))
         {
          attack_entity(random, enemy, 0, player, dungeon, ui, enemy->e.damage);
         }
         else if(pathfind_result.can_move)
         {
          enemy->new_pos = pathfind_result.info.pos;
         }
        }
        else
        {
         enemy->e.state = EnemyEntityState_Chasing;
         goto update_states;
        }
       }
      }
     }
     else if(enemy->e.state == EnemyEntityState_Chasing)
     {
      
#if MOONBREATH_SLOW
      if(print_state) print_enemy_entity_state(enemy->e.state);
#endif
      
      if(can_enemy_see_entity(enemy, player, dungeon->tiles))
      {
       reset_enemy_pathfind(enemy);
       
       enemy->e.state = EnemyEntityState_Fighting;
       goto update_states;
      }
      else
      {
       UpdateEnemyPathfindType type = update_enemy_pathfind(enemy, dungeon, &enemy->chase_map, player->pos);
       if(type == UpdateEnemyPathfindType_ReachedTarget)
       {
        // No goto update_states so that we actually move to the target pos.
        enemy->e.state = EnemyEntityState_Wandering;
       }
       else if(type == UpdateEnemyPathfindType_CantMove)
       {
        enemy->e.state = EnemyEntityState_Wandering;
        goto update_states;
       }
      }
      }
     else if(enemy->e.state == EnemyEntityState_Fleeing)
     {
      
      // TODO(rami): There needs to be feedback to the player that the enemy is fleeing.
      
#if MOONBREATH_SLOW
      if(print_state) print_enemy_entity_state(enemy->e.state);
      #endif
      
      update_enemy_facing_direction(enemy, player, false);
      
      PathfindResult pathfind_result = get_pathfind_result(enemy, dungeon, enemy->e.pathfind_map_to_player, PathfindMethod_Away);
      if(pathfind_result.can_move)
      {
       enemy->new_pos = pathfind_result.info.pos;
      }
      else
      {
       enemy->e.state = EnemyEntityState_Cornered;
       goto update_states;
      }
     }
     else
     {
      assert(enemy->e.state == EnemyEntityState_Cornered);
      
#if MOONBREATH_SLOW
      if(print_state) print_enemy_entity_state(enemy->e.state);
#endif
      
      update_enemy_facing_direction(enemy, player, false);
      
      if(can_enemy_see_entity(enemy, player, dungeon->tiles))
      {
       if(is_set(enemy->flags, EntityFlag_FightingFromCornered))
       {
        enemy->e.state = EnemyEntityState_Fighting;
        goto update_states;
       }
      }
      else
      {
       enemy->e.state = EnemyEntityState_Wandering;
       goto update_states;
      }
      }
     
     // new_pos will be zero for example in situations when we can't pathfind.
     if(!is_v2u_zero(enemy->new_pos) &&
        !is_v2u_equal(enemy->pos, enemy->new_pos))
     {
      // Before calling entity move, we set saved_pos_for_ghost
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
      
      if(is_dungeon_pos_closed_door(dungeon->tiles, enemy->new_pos))
      {
       set_dungeon_pos_open_door(dungeon->tiles, enemy->new_pos);
      }
      else
      {
       entity_move(random, enemy, dungeon, ui, enemy->new_pos);
      }
                    }
                    
                    update_entity_statuses(game, enemy, dungeon, inventory, ui);
                }
            }
        }
    }
}

internal b32
update_player_input(Game *game,
                    Input *input,
                    EntityState *entity_state,
                    ItemState *item_state,
                    Inventory *inventory,
                    DungeonState *dungeon_state,
                    Assets *assets,
                    UI *ui)
{
    assert(game);
    assert(input);
    assert(entity_state);
    assert(item_state);
    assert(inventory);
    assert(dungeon_state);
    assert(assets);
    assert(ui);
    
    b32 result = true;
 
 #if MOONBREATH_SLOW
 DebugState *debug = &game->debug;
 EditorMode *editor = &game->editor;
 #endif
 
    Random *random = &game->random;
    Entity *player = get_player_entity();
 ExamineMode *examine = &game->examine;
    Dungeon *dungeon = get_dungeon_from_level(dungeon_state, dungeon_state->current_level);
 
    game->should_update = false;
    game->passed_time = 0;
    
    player->new_pos = player->pos;
    player->direction = Direction_None;
    set(player->flags, EntityFlag_NotifyAboutMultipleItems);
 
#if MOONBREATH_SLOW
 // Editor inputs
 if(editor->is_open)
 {
  if(was_pressed(&input->GameKey_Back))
  {
   game->mode = GameMode_Quit;
  }
  else if(was_pressed(&input->fkeys[2]))
  {
   editor->is_open = false;
   remove_editor_actives(editor);
    
    // Reset and update tile visibility
    for(u32 y = 0; y < dungeon->size.h; ++y)
    {
     for(u32 x = 0; x < dungeon->size.w; ++x)
     {
      v2u pos = {x, y};
      set_tile_is_seen_and_has_been_seen(dungeon->tiles, pos, false);
     }
    }
  }
  else
  {
   update_editor_mode(random, editor, input, entity_state, dungeon);
  }
  
  return(result);
 }
 #endif
 
    if(is_set(player->flags, EntityFlag_Pathfinding))
    {
  game->should_update = update_player_pathfind(player, entity_state, item_state, dungeon, ui);
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
                    get_pressed_keyboard_char(input, KeyboardCharType_Alphabet))
        {
            // Repeat the item cancel question if we're not getting the inputs we expect.
                ask_for_item_cancel(game->keybinds, input, inventory, ui);
            }
    }
    else
    {
        
#if MOONBREATH_SLOW
  if(was_pressed_core(&input->fkeys[1]) &&
     windows_are_closed(examine, inventory->flags, ui->flags) &&
     !editor->is_open)
  {
            // Toggle debug state visibility
   debug->is_open = !debug->is_open;
            
            // Reset debug event
            DebugEvent *event = &debug->event;
            if(event->type)
            {
                reset_debug_event(event);
            }
            
            return(result);
        }
  else if(was_pressed(&input->fkeys[2]) &&
          windows_are_closed(examine, inventory->flags, ui->flags) &&
          !debug->is_open)
  {
   editor->is_open = true;
   editor->pos = player->pos;
   
            return(result);
        }
        else if(was_pressed(&input->fkeys[3]))
  {
   if(is_set(inventory->flags, InventoryFlag_Open))
   {
    printf("F3: All items have been picked up.\n");
    
    for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
    {
     Item *item = &item_state->array[index];
     if(is_valid_non_inventory_item(item))
     {
      add_item_to_inventory(game, player, item, item_state, inventory, ui, dungeon->level, true);
     }
    }
   }
   
            return(result);
        }
        else if(was_pressed(&input->fkeys[4]))
        {
   if(is_set(inventory->flags, InventoryFlag_Open))
   {
    printf("F4: All inventory items have been dropped.\n");
    
    for(u32 index = 0; index < MAX_INVENTORY_SLOT_COUNT; ++index)
    {
     Item *item = inventory->slots[index];
     if(item)
     {
      drop_item_from_inventory(game, player, item, item_state, inventory, dungeon, ui);
     }
    }
   }
   
            return(result);
        }
        else if(was_pressed(&input->fkeys[5]))
  {
   printf("F5: All items identified/unidentified.\n");
   
   for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
   {
    Item *item = &item_state->array[index];
    if(is_item_valid(item) && item->type != ItemType_Ration)
    {
     toggle(item->flags, ItemFlag_IsIdentified);
    }
   }
   
            return(result);
        }
        else if(was_pressed(&input->fkeys[6]))
  {
   
   #if 0
   printf("F6: Item mark test string.\n");
   strcpy(item_state->temp_mark.array, "Donec in tellus et mauris maximus interdum in at ex. Proin pla.");
#endif
   
   return(result);
        }
        else if(was_pressed(&input->fkeys[7]))
  {
   return(result);
        }
        else if(was_pressed(&input->fkeys[8]))
        {
            return(result);
        }
        else if(was_pressed(&input->fkeys[9]))
  {
   printf("F9: Set player at max health.\n");
   player->hp = player->max_hp;
   
            return(result);
        }
        else if(was_pressed(&input->fkeys[10]))
  {
   printf("F10: Set player at one health.\n");
   player->hp = 1;
   
            return(result);
        }
        else if(was_pressed(&input->fkeys[11]))
  {
   
#if 0
   printf("F11: Killed all enemy entities.\n");
   
   for(u32 index = 0; index < MAX_ENTITY_COUNT; ++index)
   {
    Entity *enemy = &entity_state->array[index];
    if(is_enemy_entity_valid_in_level(enemy, dungeon->level))
    {
     kill_entity(random, enemy, dungeon, ui);
    }
   }
#endif
   
            return(result);
        }
        else if(was_pressed(&input->fkeys[12]))
        {
   printf("F12: Entity status test.\n");
   
   #if 1
   EntityStatus test = {0};
   test.type = EntityStatusType_Stat;
   test.stat_type = EntityStatusStatType_FOV;
   test.value.max = -2;
   test.duration = 8;
   add_entity_status(random, player, dungeon, ui, &test);
#endif
   
#if 0
   EntityStatus test = {0};
   test.type = EntityStatusType_Stat;
   test.stat_type = EntityStatusStatType_EV;
   test.value.max = 3;
   test.duration = 8;
   add_entity_status(random, player, dungeon, ui, &test);
#endif
   
#if 0
            EntityStatus invisible = {0};
            invisible.type = EntityStatusType_Invisible;
            invisible.chance = 100;
            invisible.duration = 30;
            add_entity_status(random, player, dungeon, ui, &invisible);
            #endif
            
            #if 0
            EntityStatus two = {0};
            two.type = EntityStatusType_Confusion;
            two.chance = 100;
            two.duration = 10;
            add_entity_status(random, &entity_state->array[1], dungeon, ui, &two);
            #endif
            
            #if 0
            Status broken_armor_status = {0};
            broken_armor_status.type = EntityStatusType_BrokenArmor;
            broken_armor_status.chance = 100;
            broken_armor_status.duration = 10;
            add_entity_status(random, player, dungeon, ui, &broken_armor_status);
            
            Status burn_status = {0};
            burn_status.type = EntityStatusType_Burn;
            burn_status.value.min = 1;
            burn_status.value.max = 5;
            burn_status.chance = 100;
            burn_status.duration = 10;
            add_entity_status(random, player, dungeon, ui, &burn_status);
            #endif
            
            return(result);
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
                else if(windows_are_closed(examine, inventory->flags, ui->flags))
    {
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
    
    { // Debug stuff
#if 0
     printf("player->p.base_stats.str: %u\n", player->p.base_stats.str);
     printf("player->p.base_stats.intel: %u\n", player->p.base_stats.intel);
     printf("player->p.base_stats.dex: %u\n", player->p.base_stats.dex);
     printf("player->p.base_stats.ev: %u\n", player->p.base_stats.ev);
     printf("player->p.base_stats.fov: %u\n\n", player->p.base_stats.fov);
#endif
     
#if 0
     DungeonTileID remains = get_dungeon_pos_remains(dungeon->tiles, input->mouse_tile);
     printf("Remains on mouse: %u\n", remains);
#endif
     
#if 0
     printf("player_pathfind_map: %u\n", get_pathfind_value(&entity_state->player_pathfind_map, input->mouse_tile));
     printf("enemy_pathfind_map: %u\n", get_pathfind_value(&entity_state->enemy_pathfind_map, input->mouse_tile));
     printf("enemy_to_player_pathfind_map: %u\n", get_pathfind_value(&entity_state->enemy_to_player_pathfind_map, input->mouse_tile));
     printf("is_dungeon_pos_occupied: %u\n", is_dungeon_pos_occupied(dungeon->tiles, input->mouse_tile));
     printf("is_dungeon_pos_traversable: %u\n\n", is_dungeon_pos_traversable(dungeon->tiles, input->mouse_tile));
#endif
     
#if 0
     for(u32 index = 0; index < MAX_DUNGEON_PASSAGE_COUNT; ++index)
     {
      DungeonPassage *passage = &dungeon->passages.array[index];
      if(passage->type && is_v2u_equal(passage->pos, input->mouse_tile))
      {
       printf("Passage Pos: %u, %u\n", passage->pos.x, passage->pos.y);
       printf("Passage Dest Pos: %u, %u\n\n", passage->dest_pos.x, passage->dest_pos.y);
      }
     }
#endif
    }
    
    #if MOONBREATH_SLOW
    if(debug->is_open)
    {
     debug->update_state = true;
    }
    #endif
    
    { // Inventory item highlight click
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
        start_inventory_item_examine(inventory, highlight->item);
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
                        was_pressed(&input->GameKey_OpenInventory) &&
                        windows_are_closed(examine, inventory->flags, ui->flags))
            {
                    set(inventory->flags, InventoryFlag_Open);
                    set_view_and_move_at_start(&inventory->window.view);
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
                        reset_inventory_multiple_pickup(item_state, &inventory->flags, dungeon->level, ui->temp_owners);
                    }
                    else if(is_set(inventory->flags, InventoryFlag_MultipleExamine))
                    {
                        reset_all_owner_select_letters(ui->temp_owners);
                        unset(inventory->flags, InventoryFlag_MultipleExamine);
                    }
                    else if(is_set(ui->flags, UIFlag_PlayerStatusOpen))
                    {
                        unset(ui->flags, UIFlag_PlayerStatusOpen);
                    }
                    else if(is_set(ui->flags, UIFlag_FullLogOpen))
                    {
                        unset(ui->flags, UIFlag_FullLogOpen);
                    }
                    else
                    {
                        game->mode = GameMode_Quit;
                    }
                    }
                }
            else if(was_pressed_and_windows_are_closed(&input->GameKey_Pickup, examine, inventory->flags, ui->flags))
            {
                    if(get_dungeon_pos_item_count(item_state, dungeon->level, player->pos) > 1)
                    {
                        set(inventory->flags, InventoryFlag_MultiplePickup);
                        set_view_and_move_at_start(&item_state->pickup_window.view);
                        }
                    else
                    {
                        // Pickup item
                        Item *item = get_dungeon_pos_item(item_state, dungeon->level, player->pos, 0);
                        if(item)
                        {
                            add_item_to_inventory(game, player, item, item_state, inventory, ui, dungeon->level, true);
                        }
                        else
                        {
                            log_add("You see nothing to pick up here.", ui);
                        }
                    }
                }
            else if(was_pressed_and_windows_are_closed(&input->GameKey_UsePassage, examine, inventory->flags, ui->flags))
                {
                    if(is_dungeon_pos_tile(dungeon->tiles, player->pos, DungeonTileID_StoneStaircaseUp) ||
                                is_dungeon_pos_tile(dungeon->tiles, player->pos, DungeonTileID_StoneStaircaseDown))
                    {
                        DungeonPassage *used_passage = get_dungeon_pos_passage(&dungeon->passages, player->pos);
                        entity_use_passage(game, entity_state, dungeon_state, item_state, inventory, ui, used_passage);
                        
                        game->should_update = true;
                        game->passed_time = player->p.action_time * 3.0f;
                    }
                }
            else if(was_pressed_and_windows_are_closed(&input->GameKey_AutoExplore, examine, inventory->flags, ui->flags) &&
                        can_player_auto_explore(&examine->flags, entity_state, dungeon, ui) &&
                        entity_can_move(player, ui))
            {
                // Start player pathfind
                            assert(!is_set(player->flags, EntityFlag_Pathfinding));
                            
                            b32 pathfind_target_set = false;
                            v2u pathfind_target = {0};
    
    while(can_player_auto_explore_dungeon(player, dungeon))
                        {
                            assert_loop_count();
                            
     pathfind_target = get_random_dungeon_pos(random, dungeon->size);
     if(has_tile_not_been_seen_and_can_entity_move_there(player, dungeon, pathfind_target))
     {
                                pathfind_target_set = true;
                                    break;
                                }
                            }
                
                if(pathfind_target_set)
                            {
     init_entity_pathfind(player, dungeon, player->pathfind_map, pathfind_target);
                            }
                            else
                            {
                                log_add("There's nothing more to explore.", ui);
                }
                }
            else if(was_pressed_and_windows_are_closed(&input->GameKey_Examine, examine, inventory->flags, ui->flags))
                {
                set(examine->flags, ExamineFlag_Open | ExamineFlag_CameraOnExaminePos);
                    examine->pos = player->pos;
            }
                else if(was_pressed_and_windows_are_closed(&input->GameKey_Status, examine, inventory->flags, ui->flags))
            {
                set(ui->flags, UIFlag_PlayerStatusOpen);
            }
            else if(was_pressed_and_windows_are_closed(&input->GameKey_Log, examine, inventory->flags, ui->flags))
            {
                set(ui->flags, UIFlag_FullLogOpen);
                set_view_and_move_at_start(&ui->full_log.view);
                }
            else if(was_pressed_and_windows_are_closed(&input->GameKey_Wait, examine, inventory->flags, ui->flags))
            {
                    log_add("%sYou wait for a moment.", ui, start_color(Color_LightGray));
                        
                        game->should_update = true;
                game->passed_time = player->p.action_time;
                    unset(player->flags, EntityFlag_NotifyAboutMultipleItems);
            }
            else if(was_pressed_and_windows_are_closed(&input->GameKey_Rest, examine, inventory->flags, ui->flags))
            {
                if(get_player_view_enemy_count(entity_state, dungeon))
                {
                    log_add_enemies_nearby(ui);
                }
                else if(player->hp < player->max_hp)
                {
                        log_add("%sYou take a rest.", ui, start_color(Color_LightGray));
                    unset(player->flags, EntityFlag_NotifyAboutMultipleItems);
                    
                    for(;;)
                    {
                        assert_loop_count();
                        
                        game->passed_time += player->p.action_time;
                        update_entity_regen(player);
                        update_entity_statuses(game, player, dungeon, inventory, ui);
                        update_enemy_entities(game, entity_state, item_state, inventory, dungeon_state, ui, player->p.action_time);
                        
                        #if 0
                        printf("game->passed_time: %.02f\n", game->passed_time);
                        printf("player->hp: %u\n", player->hp);
                        printf("player->hp == player->max_hp: %u\n\n", player->hp == player->max_hp);
#endif
                        
                        // If max HP is reached via regen and we have enemies in view then we still want
                        // to add the new enemy message to the log.
                        if(log_add_player_view_has_enemies(entity_state, dungeon, ui) ||
                           player->hp == player->max_hp)
                        {
                            advance_game_and_player_time(game, &player->action_time);
                            return(false);
                        }
                    }
                }
                else
                {
                    log_add("You don't feel the need to rest.", ui);
                }
                }
                else if(input->Button_ScrollUp.is_down ||
                        input->Button_ScrollDown.is_down ||
                        input->Key_Home.is_down ||
                        input->Key_End.is_down)
            {
                if(is_set(inventory->flags, InventoryFlag_Mark))
                {
                    Mark *mark = &item_state->temp_mark;
                    
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
                    start_view_scrolling(input, &inventory->interact_window.view);
                }
                    else if(is_set(inventory->flags, InventoryFlag_Open))
                {
                    start_view_scrolling(input, &inventory->window.view);
                }
                else if(is_set(inventory->flags, InventoryFlag_MultiplePickup))
                {
                    start_view_scrolling(input, &item_state->pickup_window.view);
                }
                else if(is_set(inventory->flags, InventoryFlag_MultipleExamine))
                {
                    start_view_scrolling(input, &item_state->examine_window.view);
                }
                else if(is_set(ui->flags, UIFlag_FullLogOpen))
                {
                    start_view_scrolling(input, &ui->full_log.view);
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
      Mark *temp_mark = &item_state->temp_mark;
      Mark *item_mark = &examine_item->mark;
      
      if(!update_mark_input(input, temp_mark))
      {
       if(input->Key_Enter.is_down)
       {
        if(is_mark_array_valid(temp_mark->array))
        {
         item_mark->view = temp_mark->view;
         strcpy(item_mark->array, temp_mark->array);
         
         set(examine_item->flags, ItemFlag_IsMarked);
        }
        else
        {
         unset(examine_item->flags, ItemFlag_IsMarked);
        }
        
        // Reset temporary mark so it can't be seen from marking other items.
        zero_array(temp_mark->array, MAX_MARK_SIZE);
        temp_mark->view.count = 0;
        temp_mark->view.start = 0;
        
        unset(inventory->flags, InventoryFlag_Mark);
       }
        else if(input->GameKey_Back.is_down)
       {
        unset(inventory->flags, InventoryFlag_Mark);
        
        deselect_mark(temp_mark);
        zero_array(temp_mark->array, MAX_MARK_SIZE);
       }
      }
      }
                        else
                        {
                            if(is_set(inventory->flags, InventoryFlag_Adjust))
                            {
       adjust_item_letter(input, examine_item, item_state, inventory, dungeon->level, ui);
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
                            else if(was_pressed(&input->Key_U))
                            {
                                unequip_item(game, player, examine_item, ui, false);
                                }
                            else if((was_pressed(&input->Key_C) ||
                                     was_pressed(&input->Key_R)) &&
                                        is_item_consumable(examine_item->type))
       {
        ItemInteractType item_interact_type = examine_item->c.interact_type;
        if(can_item_interact_with_inventory(inventory, item_interact_type, examine_item->flags))
        {
         // Consume item
         if(examine_item->type == ItemType_Scroll)
         {
          // Apply scroll
          switch(examine_item->id)
          {
           case ItemID_IdentifyScroll:
           {
            log_add("You read the %s, choose an item to identify.", ui, examine_item->name.s);
           } break;
           
           case ItemID_EnchantWeaponScroll:
           {
            log_add("You read the %s, choose a weapon to enchant.", ui, examine_item->name.s);
           } break;
           
           case ItemID_EnchantArmorScroll:
           {
            log_add("You read the %s, choose an armor to enchant.", ui, examine_item->name.s);
           } break;
           
           case ItemID_MagicMappingScroll:
           {
            log_add("You read the %s, your surroundings become clear to you.", ui, examine_item->name.s);
            
            for(u32 y = 0; y < dungeon->size.h; ++y)
            {
             for(u32 x = 0; x < dungeon->size.w; ++x)
             {
              v2u pos = {x, y};
              set_tile_has_been_seen(dungeon->tiles, pos, true);
             }
            }
           } break;
           
           case ItemID_TeleportationScroll:
           {
            log_add("You read the %s, you find yourself in a different place.", ui, examine_item->name.s);
            teleport_entity(random, player, dungeon, ui, DungeonRandomPosType_Traversable);
           } break;
           
           case ItemID_UncurseScroll:
           {
            log_add("You read the %s, choose an item to uncurse.", ui, examine_item->name.s);
           } break;
           
           invalid_default_case;
          }
         }
         
         // Apply potion, add entity status from item
         EntityStatus *status_from_item = &examine_item->c.status;
         if(status_from_item->type)
         {
          add_entity_status(random, player, dungeon, ui, status_from_item);
         }
         
         // Set item as known
         if(is_item_id_potion_or_scroll(examine_item->id))
         {
          set_potion_or_scroll_as_known(item_state, examine_item->id);
         }
         
         // Initialize interact window
         if(item_interact_type)
         {
          inventory->interact_type = item_interact_type;
          set_view_and_move_at_start(&inventory->interact_window.view);
         }
         
         remove_consumable_from_inventory(examine_item, item_state, inventory, dungeon->level);
         
         game->passed_time = player->p.action_time;
         unset(inventory->flags, InventoryFlag_Examine);
        }
        else
        {
         log_add("You don't have anything to use that on.", ui);
        }
                                }
                                else if(was_pressed(&input->Key_D))
                            {
                                drop_item_from_inventory(game, player, examine_item, item_state, inventory, dungeon, ui);
                                }
                                else if(was_pressed(&input->Key_M))
       {
        // Open item mark
        set(inventory->flags, InventoryFlag_Mark);
        Mark *mark = &item_state->temp_mark;
        mark->is_active = true;
        
        if(is_set(examine_item->flags, ItemFlag_IsMarked))
                                {
                                    assert(!examine_item->mark.render_cursor);
                                    assert(!examine_item->mark.cursor);
         assert(!examine_item->mark.cursor_blink_start);
                                    
                                    mark->view = examine_item->mark.view;
                                    strcpy(mark->array, examine_item->mark.array);
                                    
         set_mark_cursor_at_end(mark);
                                    assert(mark->view.count);
                                }
                                else
                                {
         set_mark_cursor_at_start(mark);
        }
                                }
                            }
                        }
                    }
                    else
                {
                    char pressed = get_pressed_keyboard_char(input, KeyboardCharType_Alphabet);
                    
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
             can_interact_type_be_used_on_item(item, inventory->interact_type))
                                        {
                                            assert(is_valid_inventory_item(item));
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
                                        assert(is_valid_inventory_item(owner->item));
                                        
          start_inventory_item_examine(inventory, owner->item);
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
                                Item *item = &item_state->array[index];
                                
        if(is_valid_item_selected(item) &&
                                       !add_item_to_inventory(game, player, item, item_state, inventory, ui, dungeon->level, true))
                                {
                                        break;
                                }
                            }
                            
                            reset_inventory_multiple_pickup(item_state, &inventory->flags, dungeon->level, ui->temp_owners);
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
 
    return(result);
}

internal void
update_entities(Game *game,
                Input *input,
                EntityState *entity_state,
                ItemState *item_state,
                Inventory *inventory,
                DungeonState *dungeon_state,
                Assets *assets,
UI *ui)
{
    assert(game);
    assert(input);
    assert(entity_state);
    assert(item_state);
    assert(inventory);
    assert(dungeon_state);
    assert(assets);
    assert(ui);
    
    Random *random = &game->random;
 Entity *player = get_player_entity();
    Dungeon *dungeon = get_dungeon_from_level(dungeon_state, dungeon_state->current_level);
 
    // Update player
    if(update_player_input(game, input, entity_state, item_state, inventory, dungeon_state, assets, ui))
    {
        
    #if 0
    printf("Player Weight: %u\n", player->p.weight);
    printf("Player Weight Evasion Ratio: %u\n\n", player->p.weight_evasion_ratio);
    #endif
    
    if(game->should_update)
    {
            player->direction = get_direction_between_positions(player->pos, player->new_pos);
        
#if MOONBREATH_SLOW
   if(debug_toggles[DebugToggleType_TraverseAll])
   {
    entity_move_force(player, dungeon, player->new_pos, dungeon->level);
   }
        else
#endif
        
        if(!is_v2u_equal(player->pos, player->new_pos))
   {
    Entity *target = get_dungeon_pos_occupier(dungeon->tiles, player->new_pos);
    if(target)
    {
     assert(is_entity_valid_in_level(target, player->dungeon_level));
     assert(is_v2u_equal(target->pos, player->new_pos));
     
                            // Unarmed damage values
                            EntityDamage damage_info = {0};
                        damage_info.min = 1;
     damage_info.max = 2;
     damage_info.type = EntityDamageType_Physical;
                        u32 accuracy = 1;
                        f32 attack_speed = 1.0f;
                            
                            Item *weapon = player->p.equipment[ItemSlot_Weapon];
                            if(weapon)
     {
                                assert(is_item_valid(weapon));
                                assert(weapon->type == ItemType_Weapon);
                            
      damage_info.max = weapon->w.damage.max + weapon->enchant_level;
      damage_info.type = weapon->w.damage.type;
                                accuracy = weapon->w.accuracy + weapon->enchant_level;
                                attack_speed = weapon->w.speed;
                            
      assert(is_entity_damage_valid(damage_info));
                        }
                        
                            player->hit_chance = 15 + (player->stats.dex / 2);
                        player->hit_chance += accuracy;
     
                            { // Update player damage from strength
                                if(player->stats.str < player->p.base_stats.str)
                                {
       damage_info.max -= (player->p.base_stats.str - player->stats.str);
                                }
      else if(player->stats.str > player->p.base_stats.str)
      {
       damage_info.max += (player->stats.str - player->p.base_stats.str);
                                }
                            }
     
     attack_entity(&game->random, player, 0, target, dungeon, ui, damage_info);
                            set_game_passed_time_from_player_pos(&game->passed_time, dungeon->tiles, player->pos, attack_speed);
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
                        if(set_game_passed_time_from_player_pos(&game->passed_time, dungeon->tiles, player->pos, player->p.action_time))
                        {
                            log_add("You wade through the water.", ui);
                        }
                        
      if(entity_move(random, player, dungeon, ui, player->new_pos))
                        {
                            // Player dungeon trap interact
       DungeonTrap *trap = get_dungeon_pos_trap(&dungeon->traps, player->new_pos);
                            if(trap)
                            {
                                switch(trap->type)
                                {
                            case DungeonTrapType_Spike:
                            case DungeonTrapType_Sword:
                            case DungeonTrapType_Arrow:
                            case DungeonTrapType_Magic:
         {
                                        attack_entity(&game->random, 0, trap, player, dungeon, ui, trap->damage);
                            } break;
                            
                            case DungeonTrapType_Bind:
                                {
                                        EntityStatus bind_status = {0};
                                    bind_status.type = EntityStatusType_Bind;
          bind_status.duration = get_random_from_v2u(random, trap->value_range);
                                    
                                    add_entity_status(random, player, dungeon, ui, &bind_status);
                            } break;
                            
                            case DungeonTrapType_Shaft:
                            {
                                assert(!is_set(player->flags, EntityFlag_Pathfinding));
                                
                                log_add("You fall into a shaft!", ui);
                                
                                if(!trap->is_shaft_set)
                                { 
           trap->shaft_depth = get_random_from_v2u(random, trap->value_range);
                                            u32 shaft_dungeon_level = dungeon_state->current_level + trap->shaft_depth;
                                        
                                            Dungeon *shaft_dungeon = get_dungeon_from_level(dungeon_state, shaft_dungeon_level);
                                        trap->shaft_dest = get_random_dungeon_traversable_unoccupied_pos(random, shaft_dungeon);
                                            move_player_between_dungeons(game, entity_state, dungeon_state, item_state, inventory, ui, trap->shaft_dest, shaft_dungeon_level);
                                        
                                        trap->is_shaft_set = true;
                                        
                                        //printf("Set shaft data - Depth: %u - Destination: %u, %u.\n", trap->shaft_depth, trap->shaft_dest.x, trap->shaft_dest.y);
                                }
                            } break;
                            
                                    case DungeonTrapType_Summon: summon_entity(random, entity_state, player, dungeon, ui, 0); break;
                            
                            case DungeonTrapType_Teleport:
                            {
                                log_add("You take a step and find yourself in a different place!", ui);
                                        teleport_entity(random, player, dungeon, ui, DungeonRandomPosType_Traversable);
                            } break;
                            
                            invalid_default_case;
                                }
                    }
                    }
                }
            }
        }
    }
  
  update_player_view(player, dungeon);
  
    if(game->passed_time)
        {
            advance_game_and_player_time(game, &player->action_time);
        //printf("player->action_time: %.02f\n", player->action_time);
   
   // Update based on time passed
   for(u32 action_time = 0; action_time < player->action_time; ++action_time)
        {
            update_entity_regen(player);
                update_entity_statuses(game, player, dungeon, inventory, ui);
        }
            
   // Notify player about multiple items on their position
        if(windows_are_closed(&game->examine, inventory->flags, ui->flags) &&
           is_set(player->flags, EntityFlag_NotifyAboutMultipleItems) &&
                   get_dungeon_pos_item_count(item_state, dungeon->level, player->pos) > 1)
        {
                log_add("There are multiple items here.", ui);
        }
   
   // Reset player pathfind
   if(!is_set(player->flags, EntityFlag_Pathfinding) && player->p.render_pathfind)
   {
    if(player->p.wait_before_pathfind_reset)
    {
     player->p.wait_before_pathfind_reset = false;
    }
     else
    {
     reset_player_pathfind_trail(player);
    }
   }
            }
        
        { // Update player defence
   u32 total_item_def = 0;
            
            for(ItemSlot slot = ItemSlot_None + 1; slot < ItemSlot_Count; ++slot)
            {
                Item *item = player->p.equipment[slot];
                if(item)
    {
     // Add item defence
     if(item->type == ItemType_Armor)
     {
     total_item_def += item->a.defence + item->enchant_level;
     }
     
     total_item_def += get_item_stat_type_value(item->stats, ItemStatType_Defence);
     }
            }
            
            if(get_entity_status(player->statuses, EntityStatusType_BrokenArmor))
            {
                player->stats.def = total_item_def / 2;
            }
            else
            {
                player->stats.def = total_item_def;
            }
   
             //player->stats.def = 40; // Debug
        }
        
        update_enemy_entities(game, entity_state, item_state, inventory, dungeon_state, ui, game->passed_time);
        }
}

internal void
render_entities(Game *game,
                EntityState *entity_state,
                Inventory *inventory,
                 Dungeon *dungeon,
                Assets *assets)
{
    assert(game);
    assert(entity_state);
    assert(inventory);
    assert(dungeon);
    assert(assets);
    
        { // Render player
        Entity *player = get_player_entity();
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
     render_texture(game->renderer, assets->tileset.tex, &item->equip_tile_src, &player_dest, false, false);
                        break;
                    }
                }
            }
        
        if(is_entity_under_any_status(player->statuses)) render_tile_on_dest(game->renderer, assets->tileset.tex, player_dest, DungeonTileID_EntityStatus);
        
            // Render player pathfind trail
  if(player->p.render_pathfind)
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
      render_texture(game->renderer, assets->tileset.tex, &trail_src, &trail_dest, false, false);
                    }
                    else if(has_tile_been_seen(dungeon->tiles, trail->pos))
     {
      render_texture(game->renderer, assets->tileset.tex, &trail_src, &trail_dest, false, true);
                    }
                    }
                }
        }
        }
        
        { // Render enemies
            for(u32 enemy_index = 0; enemy_index < MAX_ENTITY_COUNT; ++enemy_index)
            {
            Entity *enemy = &entity_state->array[enemy_index];
            
            if(is_enemy_entity_valid_in_level(enemy, dungeon->level))
            {
                if(!get_entity_status(enemy->statuses, EntityStatusType_Invisible))
            {
                if(is_tile_seen(dungeon->tiles, enemy->pos))
                    {
                    set(enemy->flags, EntityFlag_HasBeenSeen);
                        unset(enemy->flags, EntityFlag_GhostEnabled);
                        v4u dest = render_game_dest_tile(game, assets->tileset.tex, enemy->tile_src, enemy->pos, is_set(enemy->flags, EntityFlag_Flipped));
                        
      if(is_enemy_entity_alerted(enemy->e.player_seen_count))
                        {
                            render_tile_on_dest(game->renderer, assets->tileset.tex, dest, DungeonTileID_EntityAlerted);
                        }
                        else if(is_entity_under_any_status(enemy->statuses))
                        {
                                render_tile_on_dest(game->renderer, assets->tileset.tex, dest, DungeonTileID_EntityStatus);
                    }
                        
                        // Render enemy healthbar
      if(!is_enemy_entity_alerted(enemy->e.player_seen_count) &&
         enemy->hp != enemy->max_hp)
                        {
                            v4u healthbar_dest = dest;
       healthbar_dest.w = ratio(enemy->hp, enemy->max_hp, TILE_SIZE);
                            
       u32 health_per_src = get_u32_from_up_rounded_f32((f32)enemy->max_hp / (f32)MAX_HEALTHBAR_SRC_COUNT);
       u32 src_index = enemy->hp / health_per_src;
                            if(src_index == MAX_HEALTHBAR_SRC_COUNT) --src_index;
       
       render_texture(game->renderer, assets->tileset.tex, &assets->healthbar_src[src_index], &healthbar_dest, false, false);
       
#if 0
       printf("enemy->hp: %u\n", enemy->hp);
       printf("enemy->max_hp: %u\n", enemy->max_hp);
       printf("health_per_src: %u\n", health_per_src);
       printf("src_index: %u\n\n", src_index);
#endif
       
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
        else if(!is_v2u_zero(enemy->e.ghost_pos)) // Render if ghost pos is valid.
                                {
         // Render enemy ghost
         b32 flipped = is_set(enemy->flags, EntityFlag_GhostFlipped);
         v4u ghost_dest = get_game_dest(game->camera, enemy->e.ghost_pos);
         render_texture(game->renderer, assets->tileset.tex, &enemy->tile_src, &ghost_dest, flipped, true);
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
add_player_entity(EntityState *entity_state)
{
    assert(entity_state);
    
    Entity *player = get_player_entity();
    
    player->id = EntityID_Player;
    strcpy(player->name.s, "Player");
    player->max_hp = player->hp = 40;
    player->dungeon_level = 1;
    player->width = player->height = 32;
 player->tile_src = get_dungeon_tile_rect(get_entity_tile_pos(player->id));
    player->remains_type = EntityRemainsType_RedBlood;
    player->type = EntityType_Player;
    
    player->p.base_stats.str = 10;
    player->p.base_stats.intel = 10;
    player->p.base_stats.dex = 10;
    player->p.base_stats.ev = 10;
 player->p.base_stats.fov = 8;
    player->stats = player->p.base_stats;
    
    EntityRegen *regen = &player->regen;
    regen->amount = 1;
    regen->max = 100;
    regen->advance = 15;
    
    player->p.action_time = 1.0f;
    player->p.weight_evasion_ratio = 4;
    
 player->pathfind_map = &entity_state->player_pathfind_map;
 player->p.pathfind_map_to_player = &entity_state->pathfind_map_to_player;
    
#if 0
    player->resists[DamageType_Physical] = 1;
    player->resists[DamageType_Fire] = 3;
    player->resists[DamageType_Ice] = 5;
    player->resists[DamageType_Lightning] = -1;
    player->resists[DamageType_Poison] = -3;
    player->resists[DamageType_Holy] = -5;
    player->resists[DamageType_Dark] = 0;
#endif
    
}

internal Entity *
add_enemy_entity(EntityState *entity_state,
                 Dungeon *dungeon,
                 EntityID id,
                 u32 x,
                 u32 y)
{
    assert(entity_state);
    assert(dungeon);
    assert(id);
    assert(x);
    assert(y);
    
    Entity *result = 0;
    
    for(u32 index = 0; index < MAX_ENTITY_COUNT; ++index)
    {
        Entity *enemy = &entity_state->array[index];
        if(!enemy->type)
        {
            result = enemy;
            
            enemy->index = index;
   enemy->flags = get_enemy_entity_flags(id);
   enemy->id = id;
   strcpy(enemy->name.s, get_entity_name(id));
            enemy->new_pos = enemy->pos = make_v2u(x, y);
            enemy->dungeon_level = dungeon->level;
   enemy->width = enemy->height = 32;
   enemy->tile_src = get_dungeon_tile_rect_from_entity_id(id);
            enemy->type = EntityType_Enemy;
            enemy->hit_chance = 30;
   enemy->stats.fov = 8;
   
   enemy->e.level = get_enemy_entity_level(id);
   enemy->e.state = EnemyEntityState_Wandering;
   
   entity_move_force(enemy, dungeon, enemy->new_pos, dungeon->level);
   
            switch(id)
            {
                case EntityID_Dummy:
                {
                    enemy->max_hp = enemy->hp = U32_MAX;
                    
     //enemy->ev = 22;
                } break;
                
                case EntityID_SkeletonWarrior:
                {
     enemy->max_hp = enemy->hp = 15;
     enemy->e.wandering_type = EnemyWanderingType_Random;
     
     enemy->e.damage.min = 1;
                    enemy->e.damage.max = 5;
                    enemy->e.damage.type = EntityDamageType_Physical;
                    enemy->stats.ev = 8;
                    enemy->action_time = 1.0f;
                    
                    enemy->resists[EntityDamageType_Dark].value = 1;
     enemy->resists[EntityDamageType_Holy].value = -1;
                } break;
                
                case EntityID_SkeletonArcher:
                {
     enemy->max_hp = enemy->hp = 15;
     enemy->e.wandering_type = EnemyWanderingType_Random;
     
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 4;
                    enemy->e.damage.type = EntityDamageType_Physical;
                    enemy->stats.ev = 7;
                    enemy->action_time = 1.0f;
                    
     enemy->resists[EntityDamageType_Dark].value = 1;
     enemy->resists[EntityDamageType_Holy].value = -1;
                } break;
                
                case EntityID_SkeletonMage:
                {
                    enemy->max_hp = enemy->hp = 15;
     enemy->e.wandering_type = EnemyWanderingType_Random;
     
                    enemy->stats.ev = 7;
                    enemy->action_time = 1.0f;
                    
     enemy->resists[EntityDamageType_Dark].value = 1;
     enemy->resists[EntityDamageType_Holy].value = -1;
                    
                    add_entity_attack_spell(enemy, "Dark Bolt", 4, EntityDamageType_Dark, 100, enemy->stats.fov);
                } break;
                
                case EntityID_Bat:
                {
                    enemy->max_hp = enemy->hp = 8;
     enemy->e.wandering_type = EnemyWanderingType_Random;
     
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 1;
                    enemy->e.damage.type = EntityDamageType_Physical;
                    enemy->stats.ev = 14;
                    enemy->action_time = 0.3f;
                    enemy->remains_type = EntityRemainsType_RedBlood;
                } break;
                
                case EntityID_Rat:
                {
                    enemy->max_hp = enemy->hp = 12;
     enemy->e.wandering_type = EnemyWanderingType_Random;
     
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 2;
                    enemy->e.damage.type = EntityDamageType_Physical;
                    enemy->stats.ev = 11;
                    enemy->action_time = 0.5f;
                    enemy->remains_type = EntityRemainsType_RedBlood;
                } break;
                
                case EntityID_KoboldWarrior:
                {
                    enemy->max_hp = enemy->hp = 20;
     enemy->e.wandering_type = EnemyWanderingType_Travel;
     
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 5;
                    enemy->e.damage.type = EntityDamageType_Physical;
                    enemy->stats.ev = 8;
                    enemy->action_time = 1.0f;
                    enemy->remains_type = EntityRemainsType_RedBlood;
                } break;
                
                case EntityID_KoboldShaman:
                {
                    enemy->max_hp = enemy->hp = 20;
     enemy->e.wandering_type = EnemyWanderingType_Travel;
     
                    enemy->stats.ev = 7;
                    enemy->action_time = 1.0f;
                    enemy->remains_type = EntityRemainsType_RedBlood;
                    
                    add_entity_heal_spell(enemy, "Healing Touch", 3, 6, 40, enemy->stats.fov);
                    add_entity_attack_spell(enemy, "Lightning Whip", 4, EntityDamageType_Lightning, 60, 4);
                } break;
                
                case EntityID_Snail:
                {
                    enemy->max_hp = enemy->hp = 28;
     enemy->e.wandering_type = EnemyWanderingType_Random;
     
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 10;
                    enemy->e.damage.type = EntityDamageType_Physical;
                    enemy->stats.ev = 3;
                    enemy->action_time = 2.0f;
                    enemy->remains_type = EntityRemainsType_RedBlood;
                } break;
                
                case EntityID_Slime:
                {
                    enemy->max_hp = enemy->hp = 20;
     enemy->e.wandering_type = EnemyWanderingType_Random;
     
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 4;
                    enemy->e.damage.type = EntityDamageType_Physical;
                    enemy->stats.ev = 4;
                    enemy->action_time = 1.0f;
                    enemy->remains_type = EntityRemainsType_GreenBlood;
                    
     enemy->resists[EntityDamageType_Physical].value = 1;
                } break;
                
                case EntityID_Dog:
                {
                    enemy->max_hp = enemy->hp = 18;
     enemy->e.wandering_type = EnemyWanderingType_Travel;
     
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 3;
                    enemy->e.damage.type = EntityDamageType_Physical;
                    enemy->stats.ev = 9;
                    enemy->action_time = 1.0f;
                    enemy->remains_type = EntityRemainsType_RedBlood;
                } break;
                
                case EntityID_OrcWarrior:
                {
                    enemy->max_hp = enemy->hp = 26;
     enemy->e.wandering_type = EnemyWanderingType_Travel;
     
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 8;
                    enemy->e.damage.type = EntityDamageType_Physical;
                    enemy->stats.ev = 8;
                    enemy->action_time = 1.0f;
                    enemy->remains_type = EntityRemainsType_RedBlood;
                } break;
                
                case EntityID_OrcArcher:
                {
                    enemy->max_hp = enemy->hp = 26;
     enemy->e.wandering_type = EnemyWanderingType_Travel;
     
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 6;
                    enemy->e.damage.type = EntityDamageType_Physical;
                    enemy->stats.ev = 7;
                    enemy->action_time = 1.0f;
                    enemy->remains_type = EntityRemainsType_RedBlood;
                } break;
                
                case EntityID_OrcShaman:
                {
                    enemy->max_hp = enemy->hp = 26;
     enemy->e.wandering_type = EnemyWanderingType_Travel;
     
                    enemy->stats.ev = 7;
                    enemy->action_time = 1.0f;
                    enemy->remains_type = EntityRemainsType_RedBlood;
                    
     enemy->resists[EntityDamageType_Poison].value = 5;
                    
                    add_entity_stat_spell(enemy, "Protective Shout", EntityStatusStatType_Def, 3, 5, 30, enemy->stats.fov);
                    add_entity_attack_spell(enemy, "Lightning Whip", 5, EntityDamageType_Lightning, 70, enemy->stats.fov);
                } break;
                
                case EntityID_Python:
                {
                    enemy->max_hp = enemy->hp = 14;
     enemy->e.wandering_type = EnemyWanderingType_Travel;
     
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 3;
                    enemy->e.damage.type = EntityDamageType_Physical;
                    enemy->stats.ev = 10;
                    enemy->action_time = 1.0f;
                    enemy->remains_type = EntityRemainsType_RedBlood;
                    
     enemy->resists[EntityDamageType_Poison].value = 2;
                    
                    add_entity_attack_status(&enemy->e.on_hit_status, EntityStatusType_Poison, 1, 2, 8, 100);
                } break;
                
                case EntityID_Shade:
                {
                    enemy->max_hp = enemy->hp = 18;
     enemy->e.wandering_type = EnemyWanderingType_Random;
     
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 3;
                    enemy->e.damage.type = EntityDamageType_Dark;
                    enemy->stats.ev = 14;
                    enemy->action_time = 1.0f;
                    
     enemy->resists[EntityDamageType_Dark].value = 1;
                } break;
                
                case EntityID_ElfKnight:
                {
                    enemy->max_hp = enemy->hp = 30;
     enemy->e.wandering_type = EnemyWanderingType_Travel;
     
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 10;
                    enemy->e.damage.type = EntityDamageType_Physical;
                    enemy->stats.def = 3;
                    enemy->stats.ev = 3;
                    enemy->action_time = 1.0f;
                    enemy->remains_type = EntityRemainsType_RedBlood;
                } break;
                
                case EntityID_ElfArbalest:
                {
                    enemy->max_hp = enemy->hp = 28;
     enemy->e.wandering_type = EnemyWanderingType_Travel;
     
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 16;
                    enemy->e.damage.type = EntityDamageType_Physical;
                    enemy->stats.ev = 4;
                    enemy->action_time = 1.0f;
                    enemy->remains_type = EntityRemainsType_RedBlood;
                } break;
                
                case EntityID_ElfMage:
                {
                    enemy->max_hp = enemy->hp = 28;
     enemy->e.wandering_type = EnemyWanderingType_Travel;
     
                    enemy->stats.ev = 9;
                    enemy->action_time = 1.0f;
                    enemy->remains_type = EntityRemainsType_RedBlood;
                    
                    add_entity_stat_spell(enemy, "Weaken", EntityStatusStatType_Str, -3, 8, 20, enemy->stats.fov);
                    add_entity_attack_spell(enemy, "Fire Lance", 6, EntityDamageType_Fire, 60, enemy->stats.fov);
                    add_entity_attack_spell(enemy, "Frost Nova", 6, EntityDamageType_Ice, 60, enemy->stats.fov);
                } break;
                
                case EntityID_GiantSlime:
                {
                    enemy->max_hp = enemy->hp = 25;
     enemy->e.wandering_type = EnemyWanderingType_Random;
     
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 5;
                    enemy->e.damage.type = EntityDamageType_Physical;
                    enemy->stats.ev = 1;
                    enemy->action_time = 1.0f;
                    enemy->remains_type = EntityRemainsType_GreenBlood;
                    
                    enemy->resists[EntityDamageType_Physical].value = 2;
                } break;
                
                case EntityID_Spectre:
                {
                    enemy->max_hp = enemy->hp = 25;
     enemy->e.wandering_type = EnemyWanderingType_Random;
     
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 5;
                    enemy->e.damage.type = EntityDamageType_Dark;
                    enemy->stats.ev = 8;
                    enemy->action_time = 1.0f;
                    
     enemy->resists[EntityDamageType_Dark].value = 2;
                } break;
                
                case EntityID_OrcAssassin:
                {
                    enemy->max_hp = enemy->hp = 28;
     enemy->e.wandering_type = EnemyWanderingType_Travel;
     
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 8;
                    enemy->e.damage.type = EntityDamageType_Physical;
                    enemy->stats.ev = 8;
                    enemy->action_time = 1.0f;
                    enemy->remains_type = EntityRemainsType_RedBlood;
                } break;
                
                case EntityID_OrcSorcerer:
                {
                    enemy->max_hp = enemy->hp = 26;
     enemy->e.wandering_type = EnemyWanderingType_Travel;
     
                    enemy->stats.ev = 5;
                    enemy->action_time = 1.0f;
                    enemy->remains_type = EntityRemainsType_RedBlood;
                    
                    add_entity_attack_spell(enemy, "Ignite", 12, EntityDamageType_Fire, 80, enemy->stats.fov);
                    add_entity_status_spell(enemy, "Bind", EntityStatusType_Bind, 0, 4, 30, enemy->stats.fov);
                } break;
                
                case EntityID_Minotaur:
                {
                    enemy->max_hp = enemy->hp = 32;
     enemy->e.wandering_type = EnemyWanderingType_Travel;
     
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 13;
                    enemy->e.damage.type = EntityDamageType_Physical;
                    enemy->stats.ev = 4;
                    enemy->action_time = 1.0f;
                    enemy->remains_type = EntityRemainsType_RedBlood;
                } break;
                
                case EntityID_Treant:
                {
                    enemy->max_hp = enemy->hp = 40;
     enemy->e.wandering_type = EnemyWanderingType_Random;
     
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 7;
                    enemy->e.damage.type = EntityDamageType_Physical;
                    enemy->stats.def = 5;
                    enemy->stats.ev = 0;
                    enemy->action_time = 2.0f;
                    
     enemy->resists[EntityDamageType_Fire].value = -4;
                } break;
                
                case EntityID_Viper:
                {
                    enemy->max_hp = enemy->hp = 20;
     enemy->e.wandering_type = EnemyWanderingType_Travel;
     
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 6;
                    enemy->e.damage.type = EntityDamageType_Physical;
                    enemy->stats.ev = 11;
                    enemy->action_time = 1.0f;
                    enemy->remains_type = EntityRemainsType_RedBlood;
                    
     enemy->resists[EntityDamageType_Poison].value = 2;
                    
                    add_entity_attack_status(&enemy->e.on_hit_status, EntityStatusType_Poison, 1, 4, 8, 100);
                } break;
                
                case EntityID_CentaurWarrior:
                {
                    enemy->max_hp = enemy->hp = 30;
     enemy->e.wandering_type = EnemyWanderingType_Travel;
     
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 8;
                    enemy->e.damage.type = EntityDamageType_Physical;
                    enemy->stats.ev = 6;
                    enemy->action_time = 2.0f;
                    enemy->remains_type = EntityRemainsType_RedBlood;
                } break;
                
                case EntityID_CentaurSpearman:
                {
                    enemy->max_hp = enemy->hp = 30;
     enemy->e.wandering_type = EnemyWanderingType_Travel;
     
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 12;
                    enemy->e.damage.type = EntityDamageType_Physical;
                    enemy->stats.ev = 4;
                    enemy->action_time = 2.0f;
                    enemy->remains_type = EntityRemainsType_RedBlood;
                } break;
                
                case EntityID_CentaurArcher:
                {
                    enemy->max_hp = enemy->hp = 30;
     enemy->e.wandering_type = EnemyWanderingType_Travel;
     
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 5;
                    enemy->e.damage.type = EntityDamageType_Physical;
                    enemy->stats.ev = 4;
                    enemy->action_time = 2.0f;
                    enemy->remains_type = EntityRemainsType_RedBlood;
                } break;
                
                case EntityID_CursedSkull:
                {
                    enemy->max_hp = enemy->hp = 30;
     enemy->e.wandering_type = EnemyWanderingType_Random;
     
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 5;
                    enemy->e.damage.type = EntityDamageType_Physical;
                    enemy->stats.ev = 6;
                    enemy->action_time = 1.0f;
                    
     enemy->resists[EntityDamageType_Dark].value = 3;
     
     add_entity_attack_status(&enemy->e.on_hit_status, EntityStatusType_Darkness, -2, -4, 8, 15);
    } break;
                
                case EntityID_Wolf:
                {
                    enemy->max_hp = enemy->hp = 25;
     enemy->e.wandering_type = EnemyWanderingType_Travel;
     
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 6;
                    enemy->e.damage.type = EntityDamageType_Physical;
                    enemy->stats.ev = 10;
                    enemy->action_time = 1.0f;
                    enemy->remains_type = EntityRemainsType_RedBlood;
                    
     add_entity_attack_status(&enemy->e.on_hit_status, EntityStatusType_Bleed, 1, 3, 8, 33);
                } break;
                
                case EntityID_OgreWarrior:
                {
                    enemy->max_hp = enemy->hp = 34;
     enemy->e.wandering_type = EnemyWanderingType_Travel;
     
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 14;
                    enemy->e.damage.type = EntityDamageType_Physical;
                    enemy->stats.ev = 3;
                    enemy->action_time = 1.0f;
                    enemy->remains_type = EntityRemainsType_RedBlood;
                } break;
                
                case EntityID_OgreArcher:
                {
                    enemy->max_hp = enemy->hp = 34;
     enemy->e.wandering_type = EnemyWanderingType_Travel;
     
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 10;
                    enemy->e.damage.type = EntityDamageType_Physical;
                    enemy->stats.ev = 3;
                    enemy->action_time = 1.0f;
                    enemy->remains_type = EntityRemainsType_RedBlood;
                } break;
                
                case EntityID_OgreMage:
                {
                    enemy->max_hp = enemy->hp = 34;
     enemy->e.wandering_type = EnemyWanderingType_Travel;
     
                    enemy->stats.ev = 3;
                    enemy->action_time = 1.0f;
                    enemy->remains_type = EntityRemainsType_RedBlood;
     
                    add_entity_stat_spell(enemy, "Sap", EntityStatusStatType_EV, -2, 8, 15, enemy->stats.fov);
                    add_entity_stat_spell(enemy, "Warsong", EntityStatusStatType_Str, 1, 8, 15, enemy->stats.fov);
                    add_entity_attack_spell(enemy, "Poison Cloud", 4, EntityDamageType_Poison, 80, enemy->stats.fov);
                } break;
                
                case EntityID_Cyclops:
                {
                    enemy->max_hp = enemy->hp = 38;
     enemy->e.wandering_type = EnemyWanderingType_Travel;
     
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 18;
                    enemy->e.damage.type = EntityDamageType_Physical;
                    enemy->stats.ev = 3;
                    enemy->action_time = 0.5f;
                    enemy->remains_type = EntityRemainsType_RedBlood;
                } break;
                
                case EntityID_ShadowWalker:
                {
                    enemy->max_hp = enemy->hp = 30;
     enemy->e.wandering_type = EnemyWanderingType_Random;
     
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 8;
                    enemy->e.damage.type = EntityDamageType_Dark;
                    enemy->stats.ev = 10;
                    enemy->action_time = 1.0f;
                    
     enemy->resists[EntityDamageType_Dark].value = 3;
                } break;
                
                case EntityID_DwarfKnight:
                {
                    // TODO(rami): Art: high armor, big morningstar
                    
                    enemy->max_hp = enemy->hp = 30;
     enemy->e.wandering_type = EnemyWanderingType_Travel;
     
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 15;
                    enemy->e.damage.type = EntityDamageType_Physical;
                    enemy->stats.def = 8;
                    enemy->stats.ev = 2;
                    enemy->action_time = 1.0f;
                    enemy->remains_type = EntityRemainsType_RedBlood;
                } break;
                
                case EntityID_DwarfSoldier:
                {
                    // TODO(rami): Art: medium armor, hammer
                    
                    enemy->max_hp = enemy->hp = 30;
     enemy->e.wandering_type = EnemyWanderingType_Travel;
     
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 12;
                    enemy->e.damage.type = EntityDamageType_Physical;
                    enemy->stats.ev = 4;
                    enemy->stats.def = 6;
                    enemy->action_time = 1.0f;
                    enemy->remains_type = EntityRemainsType_RedBlood;
                } break;
                
                case EntityID_DwarfTinkerer:
                {
                    // TODO(rami): Art: low armor, wrench?
                    
                    enemy->max_hp = enemy->hp = 30;
     enemy->e.wandering_type = EnemyWanderingType_Travel;
     
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 6;
                    enemy->e.damage.type = EntityDamageType_Physical;
                    enemy->stats.ev = 6;
                    enemy->stats.def = 2;
                    enemy->action_time = 1.0f;
                    enemy->remains_type = EntityRemainsType_RedBlood;
                    
     add_entity_attack_status(&enemy->e.on_hit_status, EntityStatusType_BrokenArmor, 1, 0, 8, 33);
                } break;
                
                case EntityID_ScarletSnake:
                {
                    enemy->max_hp = enemy->hp = 22;
     enemy->e.wandering_type = EnemyWanderingType_Travel;
     
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 8;
                    enemy->e.damage.type = EntityDamageType_Physical;
                    enemy->stats.ev = 11;
                    enemy->action_time = 1.0f;
                    enemy->remains_type = EntityRemainsType_RedBlood;
                    
     enemy->resists[EntityDamageType_Poison].value = 2;
                    
     add_entity_attack_status(&enemy->e.on_hit_status, EntityStatusType_Poison, 1, 6, 8, 33);
                } break;
                
                case EntityID_Lich:
                {
                    enemy->max_hp = enemy->hp = 28;
     enemy->e.wandering_type = EnemyWanderingType_Travel;
     
                    enemy->stats.ev = 7;
                    enemy->action_time = 1.0f;
                    
     enemy->resists[EntityDamageType_Dark].value = 4;
     enemy->resists[EntityDamageType_Poison].value = 5;
     enemy->resists[EntityDamageType_Ice].value = -2;
     enemy->resists[EntityDamageType_Fire].value = -2;
                    
                    add_entity_attack_spell(enemy, "Freezing Gaze", 10, EntityDamageType_Ice, 80, enemy->stats.fov);
                    add_entity_attack_spell(enemy, "Agonizing Grip", 14, EntityDamageType_Dark, 80, enemy->stats.fov);
     add_entity_summon_spell(enemy, "Summon Undead", EntitySummonType_Undead, 10, EntityFlag_Undead);
                } break;
                
                case EntityID_AbyssalFiend:
                {
                    enemy->max_hp = enemy->hp = 22;
     enemy->e.wandering_type = EnemyWanderingType_Random;
     
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 14;
                    enemy->e.damage.type = EntityDamageType_Physical;
                    enemy->stats.ev = 17;
                    enemy->action_time = 1.0f;
                    enemy->remains_type = EntityRemainsType_RedBlood;
                    
     enemy->resists[EntityDamageType_Fire].value = 2;
     enemy->resists[EntityDamageType_Holy].value = -1;
     enemy->resists[EntityDamageType_Ice].value = -2;
                    
                    enemy->e.hit_teleport_chance = 20;
                    } break;
                
                case EntityID_BloodTroll:
                {
                    enemy->max_hp = enemy->hp = 38;
     enemy->e.wandering_type = EnemyWanderingType_Random;
     
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 14;
                    enemy->e.damage.type = EntityDamageType_Physical;
                    enemy->stats.ev = 4;
                    enemy->action_time = 1.0f;
                    enemy->remains_type = EntityRemainsType_RedBlood;
                    
     enemy->resists[EntityDamageType_Physical].value = 2;
     enemy->resists[EntityDamageType_Holy].value = -3;
     enemy->resists[EntityDamageType_Poison].value = -2;
                } break;
                
                case EntityID_IronGolem:
                {
                    enemy->max_hp = enemy->hp = 42;
     enemy->e.wandering_type = EnemyWanderingType_Random;
     
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 18;
                    enemy->e.damage.type = EntityDamageType_Physical;
                    enemy->stats.ev = 4;
                    enemy->stats.def = 6;
                    enemy->action_time = 1.0f;
                    
     enemy->resists[EntityDamageType_Physical].value = 2;
                } break;
                
                case EntityID_Griffin:
                {
                    enemy->max_hp = enemy->hp = 46;
     enemy->e.wandering_type = EnemyWanderingType_Travel;
     
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 16;
                    enemy->e.damage.type = EntityDamageType_Physical;
                    enemy->stats.ev = 8;
                    enemy->action_time = 2.0f;
                    enemy->remains_type = EntityRemainsType_RedBlood;
                    
     enemy->resists[EntityDamageType_Holy].value = 3;
     enemy->resists[EntityDamageType_Dark].value = -3;
                } break;
                
                case EntityID_Imp:
                {
                    enemy->max_hp = enemy->hp = 22;
     enemy->e.wandering_type = EnemyWanderingType_Random;
     
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 6;
                    enemy->e.damage.type = EntityDamageType_Physical;
                    enemy->stats.ev = 7;
                    enemy->action_time = 3.0f;
                    enemy->remains_type = EntityRemainsType_RedBlood;
     
     enemy->resists[EntityDamageType_Holy].value = -2;
     enemy->resists[EntityDamageType_Fire].value = 2;
     enemy->resists[EntityDamageType_Dark].value = 1;
                } break;
                
                case EntityID_BlackKnight:
    {
                    enemy->max_hp = enemy->hp = 30;
     enemy->e.wandering_type = EnemyWanderingType_Travel;
     
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 14;
     enemy->e.damage.type = EntityDamageType_Physical;
     enemy->stats.def = 8;
                    enemy->stats.ev = 5;
                    enemy->action_time = 1.0f;
                } break;
                
                case EntityID_GiantDemon:
                {
                    enemy->max_hp = enemy->hp = 42;
     enemy->e.wandering_type = EnemyWanderingType_Travel;
     
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 20;
                    enemy->e.damage.type = EntityDamageType_Physical;
                    enemy->stats.ev = 6;
                    enemy->action_time = 2.0f;
                    enemy->remains_type = EntityRemainsType_RedBlood;
                    
     enemy->resists[EntityDamageType_Fire].value = 2;
     enemy->resists[EntityDamageType_Dark].value = 2;
     enemy->resists[EntityDamageType_Holy].value = -2;
     enemy->resists[EntityDamageType_Ice].value = -2;
                    
     add_entity_attack_status(&enemy->e.on_hit_status, EntityStatusType_Burn, 1, 4, 6, 33);
                } break;
                
                case EntityID_Hellhound:
                {
                    enemy->max_hp = enemy->hp = 35;
     enemy->e.wandering_type = EnemyWanderingType_Travel;
     
                    enemy->e.damage.min = 1;
                    enemy->e.damage.max = 12;
                    enemy->e.damage.type = EntityDamageType_Physical;
                    enemy->stats.ev = 11;
                    enemy->action_time = 1.0f;
                    enemy->remains_type = EntityRemainsType_RedBlood;
                    
     enemy->resists[EntityDamageType_Fire].value = 2;
     enemy->resists[EntityDamageType_Ice].value = -2;
                } break;
                
                case EntityID_AbyssalHexmaster:
                {
                    enemy->max_hp = enemy->hp = 36;
     enemy->e.wandering_type = EnemyWanderingType_Travel;
     
                    enemy->stats.ev = 11;
                    enemy->action_time = 1.0f;
                    enemy->remains_type = EntityRemainsType_RedBlood;
                    
     enemy->resists[EntityDamageType_Poison].value = 4;
     enemy->resists[EntityDamageType_Holy].value = -2;
     enemy->resists[EntityDamageType_Lightning].value = -3;
                    
                    add_entity_attack_spell(enemy, "Death's Embrace", 18, EntityDamageType_Poison, 80, enemy->stats.fov);
                    add_entity_status_spell(enemy, "Capture", EntityStatusType_Bind, 0, 6, 10, enemy->stats.fov);
                    add_entity_status_spell(enemy, "Hallucinate", EntityStatusType_Confusion, 0, 5, 10, enemy->stats.fov);
                } break;
                
                case EntityID_Zarimahar:
                {
                    enemy->max_hp = enemy->hp = 30;
     enemy->e.wandering_type = EnemyWanderingType_Travel;
     
                    enemy->stats.ev = 9;
                    enemy->action_time = 1.0f;
                    enemy->remains_type = EntityRemainsType_RedBlood;
                    
     enemy->resists[EntityDamageType_Fire].value = 1;
     enemy->resists[EntityDamageType_Ice].value = 1;
     enemy->resists[EntityDamageType_Lightning].value = 1;
     enemy->resists[EntityDamageType_Physical].value = -2;
     enemy->resists[EntityDamageType_Poison].value = -1;
                    
                    add_entity_attack_spell(enemy, "Lance of Mante", 24, EntityDamageType_Physical, 30, enemy->stats.fov);
                    add_entity_attack_spell(enemy, "Thunderclap", 18, EntityDamageType_Lightning, 30, enemy->stats.fov);
                    add_entity_status_spell(enemy, "immolate", EntityStatusType_Burn, 8, 5, 100, enemy->stats.fov);
                } break;
                
                invalid_default_case;
            }
            
            enemy->e.view_rect = get_dungeon_dimension_rect(dungeon->size, enemy->pos, enemy->stats.fov);
   
   switch(enemy->e.wandering_type)
   {
    case EnemyWanderingType_Random: break;
    case EnemyWanderingType_Travel: enemy->pathfind_map = &entity_state->enemy_pathfind_map; break;
    
    invalid_default_case;
   }
   enemy->e.pathfind_map_to_player = &entity_state->pathfind_map_to_player;
   
            #if 0
            enemy->resists[DamageType_Physical] = 1;
            enemy->resists[DamageType_Fire] = 3;
            enemy->resists[DamageType_Ice] = 5;
            enemy->resists[DamageType_Lightning] = -1;
            enemy->resists[DamageType_Poison] = -3;
            enemy->resists[DamageType_Holy] = -5;
            enemy->resists[DamageType_Dark] = 0;
#endif
            
#if MOONBREATH_SLOW
            if(id != EntityID_Dummy)
            {
                if(!is_entity_evasion_valid(enemy->stats.ev))
                {
                    printf("Enemy \"%s\" evasion is %u which is not valid.\n", enemy->name.s, enemy->stats.ev);
                    assert(0);
                }
                
                if((!enemy->e.damage.min || !enemy->e.damage.max) &&
                   !enemy->e.spell_count)
                {
                    printf("Enemy \"%s\" has no valid damage source.\n", enemy->name.s);
                    assert(0);
                }
                
    for(EntityDamageType index = EntityDamageType_None + 1; index < EntityDamageType_Count; ++index)
                {
                     EntityResist resist = enemy->resists[index];
                    
                    if(!is_entity_resist_valid(resist))
                    {
                        printf("Enemy \"%s\" resistance to %s is %d, which is not valid.\n",
                                   enemy->name.s, get_entity_damage_type_string(index), resist.value);
                        
                        assert(0);
                    }
                }
                
                for(u32 index = 0; index < MAX_ENTITY_SPELL_COUNT; ++index)
                {
                    Spell *spell = &enemy->e.spells[index];
                    
                    if(spell->status_type)
                    {
                        if(!(spell->range && spell->range <= enemy->stats.fov))
                        {
                            if(!spell->summon_type)
                            {
                            printf("Enemy \"%s\" spell \"%s\" has a range of %u which is not valid.\n", enemy->name.s, spell->name.s, spell->range);
                            assert(0);
                            }
                        }
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