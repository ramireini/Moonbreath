internal void
reset_inventory_multiple_pickup(ItemState *items, u32 *inventory_flags, u32 dungeon_level, Letter *select_letters)
{
    assert(items);
    assert(inventory_flags);
    assert(is_dungeon_level_valid(dungeon_level));
    assert(select_letters);
    
    reset_letters(select_letters);
    unset_item_selections(items, dungeon_level);
    unset(*inventory_flags, InventoryFlag_MultiplePickup);
}

internal Entity *
get_player_entity(EntityState *entities)
{
    Entity *result = &entities->array[0];
    return(result);
}

internal char *
get_status_type_text(StatusType type)
{
    char *result = 0;
    
    switch(type)
    {
        case StatusType_Damage: result = "Damage"; break;
        case StatusType_Heal: result = "Heal"; break;
        case StatusType_Stat: result = "Stat"; break;
        case StatusType_Bind: result = "Bind"; break;
        case StatusType_Confusion: result = "Confusion"; break;
        case StatusType_Poison: result = "Poison"; break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal char *
get_stat_type_text(StatType type)
{
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
    if(!is_set(player_flags, EntityFlag_IsPathfinding))
    {
        set(*flags, flag);
    }
}

internal void
reset_entity_action_time(f32 *action_time)
{
    *action_time = 0.0f;
}

internal char *
get_entity_type_text(EntityType type)
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
get_entity_remains_text(EntityRemains type)
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
force_move_entity(Entity *entity, Dungeon *dungeon, v2u pos, u32 dungeon_level)
{
    set_dungeon_pos_occupied(dungeon->tiles, entity->pos, false);
    
    entity->pos = pos;
    entity->new_pos = pos;
    entity->dungeon_level = dungeon_level;
    
    set_dungeon_pos_occupied(dungeon->tiles, entity->pos, true);
    
    if(entity->type == EntityType_Player)
    {
        update_fov(entity, dungeon);
    }
}

internal b32
is_pos_seen_and_flag_not_set(DungeonTiles tiles, v2u pos, u32 flags, u32 flag)
{
    b32 result = (is_tile_seen(tiles, pos) &&
                      !is_set(flags, flag));
    
    return(result);
}

internal b32
are_new_items_in_player_view(ItemState *items, Dungeon *dungeon)
{
    b32 result = false;
    
    for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
    {
        Item *item = &items->array[index];
        
        if(is_item_valid_and_not_in_inventory(item, dungeon->level) &&
               is_pos_seen_and_flag_not_set(dungeon->tiles, item->pos, item->flags,
                                            ItemFlag_HasBeenSeen))
        {
            set(item->flags, ItemFlag_HasBeenSeen);
            
            // We loop and set flag for all in view
              result = true;
        }
    }
    
    return(result);
}

internal b32
are_new_dungeon_traps_in_player_view(Dungeon *dungeon)
{
    b32 result = false;
    
    DungeonTraps *traps = &dungeon->traps;
    
    for(u32 index = 0; index < traps->count; ++index)
    {
         DungeonTrap *trap = &traps->array[index];
        
        if(is_pos_seen_and_flag_not_set(dungeon->tiles, trap->pos, trap->flags,
                                        DungeonTrapFlag_HasBeenSeen))
        {
            set(trap->flags, DungeonTrapFlag_HasBeenSeen);
            
            // We loop and set flag for all in view
            result = true;
        }
    }
    
    return(result);
}

internal b32
are_new_enemies_in_player_view(EntityState *entities, Dungeon *dungeon)
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
    v2u pos = get_random_dungeon_traversable_unoccupied_pos(random, dungeon);
    force_move_entity(entity, dungeon, pos, dungeon->level);
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
        
        #if 0
        printf("\nDamage: %u (%s)\n", result, get_damage_type_text(damage_type));
        printf("Defender Resistance: %dp (%.01f%%)\n", resistance, resistance_percentage * 100.0f);
        printf("Damage After: %d (%d old; %d change)\n\n", result - damage_change, result, damage_change);
        #endif
        
        result = damage;
        
        if(resistance == 5)
        {
            log_add("%sYou fully resist the attack!", ui, start_color(Color_LightGray));
        }
        else
        {
            log_add("%sYou partially resist the attack!", ui, start_color(Color_LightGray));
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

internal s32
get_statuses_stat_value(Status *statuses, StatType stat_type)
{
    assert(statuses);
    assert(stat_type);
    
    s32 result = 0;
    
    for(u32 index = 0; index < MAX_ENTITY_STATUS_COUNT; ++index)
    {
        Status *status = &statuses[index];
        
        if(status->type == StatusType_Stat &&
               status->stat_type == stat_type)
        {
            assert(status->value != 0);
            
            result += status->value;
        }
    }
    
    return(result);
}

internal Status *
get_status_from_statuses(Status *statuses, StatusType type)
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

internal b32
is_entity_in_spell_range(v2u caster_pos, v2u target_pos, u32 spell_range)
{
    assert(!is_v2u_zero(caster_pos));
    assert(!is_v2u_zero(target_pos));
    
    b32 result = (!spell_range || get_cardinal_and_ordinal_distance(caster_pos, target_pos) <= spell_range);
    return(result);
}

internal b32
is_entity_in_view_and_spell_range(v4u caster_view_rect, v2u caster_pos, v2u target_pos, u32 spell_range)
{
    assert(!is_v4u_zero(caster_view_rect));
    assert(!is_v2u_zero(caster_pos));
    assert(!is_v2u_zero(target_pos));
    
    b32 result = (is_dungeon_pos_inside_rect(caster_view_rect, target_pos) &&
        is_entity_in_spell_range(caster_pos, target_pos, spell_range));
    
    return(result);
}

internal void
update_player_pathfind(Game *game,
                       Entity *player,
                       EntityState *entities,
                       ItemState *items,
                       Dungeon *dungeon,
                       UI *ui)
{
    b32 can_update = true;
    
    if(are_new_items_in_player_view(items, dungeon))
    {
        can_update = false;
        log_add("%sNew items come into view..", ui, start_color(Color_LightGray));
    }
    else if(are_new_dungeon_traps_in_player_view(dungeon))
    {
        can_update = false;
        log_add("%sNew dungeon traps come into view..", ui, start_color(Color_LightGray));
    }
    else if(are_new_enemies_in_player_view(entities, dungeon))
    {
        can_update = false;
        log_add("%sNew enemies come into view..", ui, start_color(Color_LightGray));
    }
    
    if(can_update)
    {
        player->new_pos = get_pathfind_pos(&entities->player_pathfind_map, dungeon->tiles, player->pos, player->pathfind_target_pos);
        
#if 0
        printf("Auto Explore: Pathfind Target Pos %u, %u\n", player->pathfind_target_pos.x, player->pathfind_target_pos.y);
        printf("Auto Explore: New Pos %u, %u\n\n", player->new_pos.x, player->new_pos.y);
#endif
        
        // Only set the pathfind trail if we will move to player->new_pos in the future.
        // This means if the target is a door we won't place a trail under the player.
        if(is_dungeon_pos_traversable(dungeon->tiles, player->new_pos))
        {
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
        }
        
        // Stop pathfind if target has been reached
        if(is_v2u_equal(player->new_pos, player->pathfind_target_pos))
        {
            //printf("Auto Explore: Destination Reached\n");
            unset(player->flags, EntityFlag_IsPathfinding);
        }
        
        game->should_update = true;
        
#if 0
        // Print player pathfind trail
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
        
    }
    else
    {
        unset(player->flags, EntityFlag_IsPathfinding);
    }
    }

internal void
update_examine_pos(Examine *examine, Direction move_direction, Dungeon *dungeon)
{
    v2u new_pos = get_direction_pos(examine->pos, move_direction);
    
#if MOONBREATH_SLOW
    if(fkey_active[2])
    {
        if(is_pos_inside_dungeon(dungeon->size, new_pos))
        {
            examine->pos = new_pos;
        }
        
        return;
    }
#endif
    
    if(is_pos_inside_dungeon(dungeon->size, new_pos) &&
       has_tile_been_seen(dungeon->tiles, new_pos))
    {
        examine->pos = new_pos;
    }
}

internal b32
is_entity_valid(Entity *entity)
{
    b32 result = false;
    
    if(entity &&
           entity->id &&
           entity->type &&
           is_dungeon_level_valid(entity->dungeon_level))
    {
        result = true;
    }
    
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

 internal u32
get_enemy_normal_turn_count(f32 *current_time, f32 enemy_action_time)
{
    u32 result = *current_time / enemy_action_time;
    *current_time -= enemy_action_time;
    
    return(result);
}

internal u32
get_enemy_turn_count(DungeonTiles tiles, Entity *enemy, f32 *current_time)
{
    assert(is_entity_valid_and_enemy(enemy));
    
    u32 result = 0;
    
    if(is_dungeon_pos_water(tiles, enemy->pos))
    {
        if(is_set(enemy->flags, EntityFlag_MovesNormallyOnWater))
        {
            result = get_enemy_normal_turn_count(current_time, enemy->action_time);
        }
        else
        {
            f32 slow_enemy_action_time = enemy->action_time * 2.0f;
            
            result = *current_time / slow_enemy_action_time;
            *current_time -= slow_enemy_action_time;
        }
    }
    else
    {
        result = get_enemy_normal_turn_count(current_time, enemy->action_time);
    }
    
    return(result);
}

internal u32
get_dungeon_pos_entity_count(EntityState *entities, u32 dungeon_level, v2u pos, b32 enemy_only)
{
    u32 result = 0;
    
    for(u32 index = 0; index < MAX_ENTITY_COUNT; ++index)
    {
        Entity *entity = &entities->array[index];
        
        if(enemy_only && !is_entity_valid_and_enemy(entity))
        {
            continue;
        }
        
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
        
        if(enemy_only && !is_entity_valid_and_enemy(entity))
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
        
        set(entity->flags, EntityFlag_IsPathfinding);
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
                         u32 dungeon_level,
                         ItemID item_id,
                         u32 x, u32 y)
{
    Item *item = 0;
    
    if(is_item_id_weapon(item_id))
    {
        item = add_weapon_item(&game->random, items, dungeon_level, item_id, ItemRarity_Common, x, y, false);
        set(item->flags, ItemFlag_IsEquipped);
    }
    else if(is_item_id_potion(item_id))
    {
        item = add_consumable_item(&game->random, items, dungeon_level, item_id, x, y, 1);
        set_potion_or_scroll_as_known(items, item->id);
    }
    
    assert(item);
    
    item->enchantment_level = 0;
    
    set(item->flags, ItemFlag_IsIdentified);
    unset(item->flags, ItemFlag_IsCursed);
    
    add_item_to_inventory(game, player, item, items, inventory, ui, dungeon_level, false);
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

internal Spell *
get_random_entity_spell(Random *random, Entity *entity)
{
    assert(is_entity_valid_and_enemy(entity));
    
    Spell *result = 0;
    u32 counter = 0;
    
    while(counter < 100)
    {
        u32 spell_index = get_random_number(random, 0, entity->e.spell_count - 1);
        Spell *spell = &entity->e.spells[spell_index];
        
        assert(spell->type);
        assert(spell->chance);
        
        result = spell;
        counter += spell->chance;
    }
    
    assert(result);
    return(result);
    }

internal void
add_entity_attack_spell(Entity *entity,
                        char *name,
                        DamageType damage_type,
                        u32 value,
                        u32 chance)
{
    assert(is_entity_valid_and_enemy(entity));
    assert(is_chance_valid(chance));
    assert(value > 0);
    
    for(u32 index = 0; index < MAX_ENTITY_SPELL_COUNT; ++index)
    {
        Spell *spell = &entity->e.spells[index];
        
        if(!spell->type)
        {
            strcpy(spell->name.s, name);
            
            spell->type = StatusType_Damage;
            spell->damage_type = damage_type;
            
            spell->value = value;
            spell->chance = chance;
            
            sprintf(spell->description.s, "Deals %u %s damage to the target.", spell->value, get_damage_type_text(damage_type));
            
            ++entity->e.spell_count;
            return;
        }
    }
    
    assert(false);
}

internal void
add_entity_heal_spell(Entity *entity,
                        char *name,
                        u32 value,
                        u32 chance)
{
    assert(is_entity_valid_and_enemy(entity));
    assert(is_chance_valid(chance));
    assert(value > 0);
    
    for(u32 index = 0; index < MAX_ENTITY_SPELL_COUNT; ++index)
    {
        Spell *spell = &entity->e.spells[index];
        
        if(!spell->type)
        {
            strcpy(spell->name.s, name);
            spell->type = StatusType_Heal;
            
            spell->value = value;
            spell->chance = chance;
            
            sprintf(spell->description.s, "Heals the target for %u health.", spell->value);
            
            ++entity->e.spell_count;
            return;
        }
    }
    
    assert(false);
}

internal void
add_entity_stat_spell(Entity *entity,
                      char *name,
                      StatType stat_type,
                      u32 value,
                      u32 duration,
                      u32 chance)
{
    assert(is_entity_valid_and_enemy(entity));
    assert(is_chance_valid(chance));
    assert(stat_type);
    
    for(u32 index = 0; index < MAX_ENTITY_SPELL_COUNT; ++index)
    {
        Spell *spell = &entity->e.spells[index];
        
        if(!spell->type)
        {
            strcpy(spell->name.s, name);
            spell->type = StatusType_Stat;
            spell->stat_type = stat_type;
            
            spell->value = value;
            spell->chance = chance;
            spell->duration = duration;
            
            char *action = "Increases";
            if(spell->value < 0)
            {
                action = "Decreases";
            }
            
            char *stat = 0;
            switch(spell->stat_type)
            {
                case StatType_Def: stat = "Defence"; break;
                
                invalid_default_case;
            }
            
            sprintf(spell->description.s, "%s the targets %s by %u for %u turns.", action, stat, get_absolute(spell->value), spell->duration);
            
            ++entity->e.spell_count;
            return;
        }
    }
    
    assert(false);
}

internal void
add_entity_status(Status *statuses, Status *new_status)
{
    assert(new_status->type);
    
    for(u32 index = 0; index < MAX_ENTITY_STATUS_COUNT; ++index)
    {
        Status *status = &statuses[index];
        
        if(!status->type)
        {
            *status = *new_status;
            return;
        }
    }
    
    assert(0);
}

internal void
update_entity_stat_status(Entity *entity, Status *status, UI *ui)
{
    assert(status->duration);
    assert(status->value != 0);
    
    if(!status->was_value_used)
    {
        status->was_value_used = true;
        if(status->start_message.s[0]) log_add(status->start_message.s, ui);
        
        switch(status->stat_type)
        {
            case StatType_Str:
            {
                entity->strength += status->value;
            } break;
            
            case StatType_Int:
            {
                entity->intelligence += status->value;
            } break;
            
            case StatType_Dex:
            {
                entity->dexterity += status->value;
            } break;
            
            // Stat EV is applied in player update
            case StatType_EV: break;
            
            case StatType_Def:
            {
                entity->defence += status->value;
            } break;
            
            case StatType_StrIntDex:
            {
                entity->strength += status->value;
                entity->intelligence += status->value;
                entity->dexterity += status->value;
            } break;
            
            invalid_default_case;
        }
    }
}

internal void
update_entity_statuses(Game *game,
                             Entity *entity,
                             Dungeon *dungeon,
                             Inventory *inventory,
                             UI *ui)
{
    assert(is_entity_valid(entity));
    
    for(u32 index = 0; index < MAX_ENTITY_STATUS_COUNT; ++index)
    {
        Status *status = &entity->statuses[index];
        
        if(status->type)
        {
            switch(status->type)
            {
                case StatusType_Damage:
                {
                    assert(status->value);
                    assert(status->damage_type);
                    
                    attack_entity(&game->random, 0, entity, dungeon, ui, status->value, status->damage_type);
                } break;
                
                case StatusType_Heal:
                {
                    assert(status->value > 0);
                    
                    if(entity->hp == entity->max_hp &&
                           status->max_message.s[0])
                    {
                        log_add(status->max_message.s, ui);
                    }
                    else
                    {
                        log_add(status->start_message.s, ui);
                        heal_entity(entity, status->value);
                    }
                    } break;
                
                case StatusType_Stat: update_entity_stat_status(entity, status, ui); break;
                
                case StatusType_Bind:
                {
                    assert(status->duration);
                    
                    if(!status->was_value_used &&
                           status->start_message.s[0])
                    {
                        status->was_value_used = true;
                        log_add(status->start_message.s, ui);
                    }
                    
                } break;
                
                case StatusType_Confusion:
                {
                    assert(status->duration);
                    
                    assert(0);
                } break;
                
                case StatusType_Poison:
                {
                    assert(status->duration);
                    
                    assert(0);
                    
                    // TODO(rami): This belongs here
                    // log_add(ui, "%sYou don't feel sick anymore.", start_color(Color_LightGray));
                    
                } break;
                
                invalid_default_case;
            }
            
            if(status->duration) --status->duration;
            
            // End status
                if(!status->duration)
            {
                if(status->end_message.s[0]) log_add(status->end_message.s, ui);
                
                    if(status->stat_type)
                {
                    if(status->end_message.s[0]) log_add(status->end_message.s, ui);
                    
                        switch(status->stat_type)
                        {
                        case StatType_Str: entity->strength -= status->value; break;
                        case StatType_Int: entity->intelligence -= status->value; break;
                        case StatType_Dex: entity->dexterity -= status->value; break;
                        case StatType_EV: break; // EV is applied in entity update
                        case StatType_Def: entity->defence -= status->value;
                        
                            case StatType_StrIntDex:
                            {
                                entity->strength -= status->value;
                                entity->intelligence -= status->value;
                                entity->dexterity -= status->value;
                            } break;
                            
                            invalid_default_case;
                        }
                }
                    
                    zero_struct(*status);
                }
        }
    }
    }

internal void
add_entity_status_from_spell(Game *game,
                             Entity *entity,
                             Dungeon *dungeon,
                             Inventory *inventory,
                             UI *ui,
                             Spell *spell)
{
    Status *statuses = entity->statuses;
    
    assert(statuses);
    assert(spell);
    assert(spell->type);
    
    Status status = {0};
    strcpy(status.name.s, spell->name.s);
    strcpy(status.description.s, spell->description.s);
    status.type = spell->type;
    status.damage_type = spell->damage_type;
    status.stat_type = spell->stat_type;
    status.spell = spell;
    status.value = spell->value;
    status.chance = spell->chance;
    status.duration = spell->duration;
    
    update_entity_stat_status(entity, &status, ui);
    add_entity_status(statuses, &status);
}

internal b32
will_entity_hit(Random *random, Entity *attacker, Entity *defender)
{
    assert(attacker);
    assert(defender);
    
    b32 result = false;
    
    // Attacking an invisible entity has a lower chance of hitting.
    u32 attacker_hit_chance = attacker->hit_chance;
    if(is_set(defender->flags, EntityFlag_IsInvisible))
    {
        attacker_hit_chance /= 2;
    }
    
    // Defending an attack from an invisible entity has a higher chance of hitting.
    u32 defender_evasion = defender->evasion;
    if(is_set(attacker->flags, EntityFlag_IsInvisible))
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
entity_can_move(Entity *entity, UI *ui, b32 add_to_log)
{
    assert(is_entity_valid(entity));
    
    b32 result = true;
     
    for(u32 index = 0; index < MAX_ENTITY_STATUS_COUNT; ++index)
    {
        Status *status = &entity->statuses[index];
        
        if(status->type && status->type == StatusType_Bind)
        {
            result = false;
            
            if(add_to_log)
            {
                log_add("You attempt to move, but are not able to!", ui);
            }
            
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
    assert(is_entity_valid_and_player(player));
    assert(dungeons && is_dungeon_level_valid(new_dungeon_level));
    
    Dungeon *new_dungeon = get_dungeon_from_level(dungeons, new_dungeon_level);
    if(new_dungeon->level)
    {
        dungeons->current_level = new_dungeon_level;
        force_move_entity(player, new_dungeon, pos, new_dungeon_level);
        
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
    assert(is_entity_valid_and_player(player));
    
    u32 new_dungeon_level = dungeons->current_level;
    if(passage->type == DungeonPassageType_Up)
    {
        --new_dungeon_level;
        
        log_add("%sYou go up the passage..", ui, start_color(Color_LightGray));
    }
    else if(passage->type == DungeonPassageType_Down)
    {
        ++new_dungeon_level;
        
        log_add("%sYou go down the passage..", ui, start_color(Color_LightGray));
    }
    
    move_entity_between_dungeons(game, player, entities, dungeons, items, inventory, ui, passage->dest_pos, new_dungeon_level);
    }

internal b32
move_entity(Random *random, Entity *entity, Dungeon *dungeon, UI *ui, v2u pos)
{
    assert(is_entity_valid(entity));
    assert(!is_v2u_zero(pos));
    
    b32 result = entity_can_move(entity, ui, true);
    
    if(result)
    {
        // If the entity is confused, attempt to stumble in a direction that is different from
        // the intended move direction.
        
        Status *confusion_status = get_status_from_statuses(entity->statuses, StatusType_Confusion);
        if(confusion_status)
    {
            assert(confusion_status->chance);
        
        if(hit_random_chance(random, confusion_status->chance))
        {
            Direction confused_direction = get_random_direction(random);
            v2u confused_move_pos = get_direction_pos(entity->pos, confused_direction);
            
            if(confused_direction != entity->new_direction &&
                       is_dungeon_pos_traversable_and_unoccupied(dungeon->tiles, confused_move_pos))
            {
                
#if 0
                printf("\nconfused_direction: %s\n", get_direction_string(confused_direction));
                printf("confused_move_pos: %u, %u\n", confused_move_pos.x, confused_move_pos.y);
#endif
                
                pos = confused_move_pos;
                
                if(entity->type == EntityType_Player)
                {
                        log_add("%sYou stumble slightly..", ui, start_color(Color_LightGray));
                }
                else if(entity->type == EntityType_Enemy)
                {
                        log_add("%sThe %s stumbles slightly..", ui, start_color(Color_LightGray), entity->name.s);
                }
            }
        }
    }
    
        //printf("Moved entity to pos: %u, %u\n", pos.x, pos.y);
        force_move_entity(entity, dungeon, pos, dungeon->level);
        
        switch(entity->new_direction)
        {
            case Direction_Left:
            case Direction_UpLeft:
            case Direction_DownLeft: set(entity->flags, EntityFlag_IsFlipped); break;
            
            case Direction_Right:
            case Direction_UpRight:
            case Direction_DownRight: unset(entity->flags, EntityFlag_IsFlipped); break;
        }
    }
    
    return(result);
}

internal b32
heal_entity(Entity *entity, u32 value)
{
    assert(entity);
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

internal String256
get_entity_attack_text(Random *random,
                       Entity *attacker,
                       Entity *defender,
                       u32 value)
{
    assert(random);
    assert(attacker);
    assert(defender);
    assert(value);
    
    String256 result = {0};
    
    if(attacker->type == EntityType_Player)
    {
        Item *player_weapon = attacker->p.weapon;
        char *attack_text = 0;
        
        if(player_weapon)
        {
            switch(player_weapon->id)
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
        
        if(is_set(defender->flags, EntityFlag_IsInvisible))
        {
            snprintf(result.s, sizeof(result.s), "You attack the something, dealing %u damage.", value);
        }
        else
        {
            snprintf(result.s, sizeof(result.s), "You %s the %s, dealing %u damage.", attack_text, defender->name.s, value);
        }
    }
    else if(attacker->type == EntityType_Enemy)
    {
        if(attacker->e.spell_count)
        {
            Spell *enemy_spell = attacker->e.spell;
            assert(enemy_spell);
            
            if(defender->type == EntityType_Player)
            {
                snprintf(result.s, sizeof(result.s), "The %s casts %s at you, dealing %u damage.", attacker->name.s, enemy_spell->name.s, value);
            }
            else
            {
                snprintf(result.s, sizeof(result.s), "The %s casts %s at the %s, healing it for %u health.", attacker->name.s, enemy_spell->name.s, defender->name.s, value);
            }
        }
        else if(is_set(attacker->flags, EntityFlag_UsesRangedAttacks))
        {
            snprintf(result.s, sizeof(result.s), "The %s fires an arrow at you, dealing %u damage.", attacker->name.s, value);
        }
        else if(is_set(attacker->flags, EntityFlag_IsInvisible))
        {
            snprintf(result.s, sizeof(result.s), "Something attacks you, dealing %u damage.", value);
        }
        else
        {
            snprintf(result.s, sizeof(result.s), "The %s attacks you, dealing %u damage.", attacker->name.s, value);
        }
    }
    
    return(result);
}

internal void
remove_entity(Entity *entity)
{
    assert(entity);
    zero_struct(*entity);
}

internal void
kill_entity(Random *random, Entity *entity, Dungeon *dungeon, UI *ui)
{
    assert(random);
    assert(entity);
    assert(dungeon);
    assert(ui);
    
    if(entity->type == EntityType_Player)
    {
        // TODO(rami): Character dumps, on death create a text file and put all of the player
        // information in there.
        
        log_add("Oh no, you are dead!", ui);
        entity->hp = 0;
    }
    else if(entity->type == EntityType_Enemy)
    {
        log_add("%sThe %s dies!", ui, start_color(Color_LightRed), entity->name.s);
        
        // Place entity death remains
        if(entity->remains && can_place_dungeon_remains_on_pos(dungeon->tiles, entity->pos))
        {
            DungeonTileID remains = DungeonTileID_None;
            
            switch(entity->remains)
            {
                case EntityRemains_RedBlood:
                {
                    remains = get_random_number(random,
                                                    DungeonTileID_RedBloodGroundMedium1,
                                                    DungeonTileID_RedBloodGroundLarge2);
                } break;
                
                case EntityRemains_GreenBlood:
                {
                    remains = get_random_number(random,
                                                    DungeonTileID_GreenBloodGroundMedium1,
                                                    DungeonTileID_GreenBloodGroundLarge2);
                } break;
                
                invalid_default_case;
            }
            
            set_dungeon_pos_remains(dungeon->tiles, entity->pos, remains);
        }
        
        set_dungeon_pos_occupied(dungeon->tiles, entity->pos, false);
        remove_entity(entity);
    }
}

internal void
attack_entity(Random *random,
              Entity *attacker,
              Entity *defender,
              Dungeon *dungeon,
              UI *ui,
              u32 damage,
              DamageType damage_type)
{
    assert(random);
    assert(defender);
    assert(dungeon);
    assert(ui);
    assert(damage);
    assert(damage_type);
    
#if MOONBREATH_SLOW
    // Skip dummies during debugging
    if(defender->id == EntityID_Dummy) return;
    
    // Player Hit Test
    if(is_entity_valid_and_player(attacker) && fkey_active[4])
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
    
    if(!attacker || will_entity_hit(random, attacker, defender))
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
                if(attacker)
                {
                log_add(get_entity_attack_text(random, attacker, defender, damage).s, ui);
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
                        if(can_place_dungeon_remains_on_pos(dungeon->tiles, remains_pos) &&
                               !is_dungeon_pos_traversable(dungeon->tiles, remains_pos))
                        {
                            DungeonTileID remains = DungeonTileID_None;
                            
                            if(is_dungeon_pos_wall(dungeon->tiles, remains_pos))
                            { 
                                switch(defender->remains)
                                {
                                    case EntityRemains_RedBlood:
                                    {
                                        if(remains_direction == Direction_Up)
                                        {
                                            remains = get_random_number(random,
                                                                            DungeonTileID_RedBloodWallUp1,
                                                                            DungeonTileID_RedBloodWallUp3);
                                        }
                                        else if(remains_direction == Direction_Down)
                                        {
                                            remains = get_random_number(random,
                                                                            DungeonTileID_RedBloodWallDown1,
                                                                            DungeonTileID_RedBloodWallDown3);
                                        }
                                        else if(remains_direction == Direction_Left)
                                        {
                                            remains = get_random_number(random,
                                                                            DungeonTileID_RedBloodWallLeft1,
                                                                            DungeonTileID_RedBloodWallLeft3);
                                        }
                                        else if(remains_direction == Direction_Right)
                                        {
                                            remains = get_random_number(random,
                                                                        DungeonTileID_RedBloodWallRight1,
                                                                        DungeonTileID_RedBloodWallRight3);
                                        }
                                    } break;
                                    
                                    case EntityRemains_GreenBlood:
                                    {
                                        if(remains_direction == Direction_Up)
                                        {
                                            remains = get_random_number(random,
                                                                            DungeonTileID_GreenBloodWallUp1,
                                                                            DungeonTileID_GreenBloodWallUp3);
                                        }
                                        else if(remains_direction == Direction_Down)
                                        {
                                            remains = get_random_number(random,
                                                                            DungeonTileID_GreenBloodWallDown1,
                                                                            DungeonTileID_GreenBloodWallDown3);
                                        }
                                        else if(remains_direction == Direction_Left)
                                        {
                                            remains = get_random_number(random,
                                                                            DungeonTileID_GreenBloodWallLeft1,
                                                                            DungeonTileID_GreenBloodWallLeft3);
                                        }
                                        else if(remains_direction == Direction_Right)
                                        {
                                            remains = get_random_number(random,
                                                                            DungeonTileID_GreenBloodWallRight1,
                                                                            DungeonTileID_GreenBloodWallRight3);
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
                                        remains = get_random_number(random,
                                                                        DungeonTileID_RedBloodGroundSmall1,
                                                                        DungeonTileID_RedBloodGroundSmall3);
                                    } break;
                                    
                                    case EntityRemains_GreenBlood:
                                    {
                                        remains = get_random_number(random,
                                                                        DungeonTileID_GreenBloodGroundSmall1,
                                                                        DungeonTileID_GreenBloodGroundSmall3);
                                    } break;
                                    
                                    invalid_default_case;
                                }
                            }
                            
                            set_dungeon_pos_remains(dungeon->tiles, remains_pos, remains);
                        }
                    }
                    
                    // Apply attacker poison to defender
                    if(attacker)
                    {
                        Status *attacker_poison = &attacker->e.poison;
                        
                        if(attacker_poison->type &&
                               !get_status_from_statuses(defender->statuses, StatusType_Poison) &&
                               hit_random_chance(random, attacker_poison->chance))
                        {
                            if(is_entity_immune(defender, DamageType_Poison))
                            {
                                log_add("%sYour resistance stopped you from being poisoned.", ui, start_color(Color_DarkGreen));
                            }
                            else
                            {
                                switch(defender->type)
                                {
                                    case EntityType_Player: log_add("%sYou start feeling unwell.", ui, start_color(Color_DarkGreen)); break;
                                    case EntityType_Enemy: log_add("%sThe %s starts to look unwell.", ui, start_color(Color_DarkGreen)); break;
                                    
                                    invalid_default_case;
                                }
                                
                                add_entity_status(defender->statuses, attacker_poison);
                            }
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
                log_add("%sYour armor blocks the attack.", ui, start_color(Color_LightGray));
            }
            else if(defender->type == EntityType_Enemy)
            {
                log_add("%sThe %s's armor blocks your attack.", ui, start_color(Color_LightGray), defender->name.s);
            }
        }
    }
    else
    {
        if(attacker->type == EntityType_Player)
        {
            if(is_set(defender->flags, EntityFlag_IsInvisible))
            {
                log_add("%sYou miss the something.", ui, start_color(Color_LightGray));
            }
            else
            {
                log_add("%sYou miss the %s.", ui, start_color(Color_LightGray), defender->name.s);
            }
        }
        else if(attacker->type == EntityType_Enemy)
        {
            log_add("%sYou dodge the attack.", ui, start_color(Color_LightGray));
        }
    }
}

internal void
handle_asking_player(Game *game,
                     Input *input,
                     ItemState *items,
                     Inventory *inventory,
                     UI *ui,
                     u32 dungeon_level)
{
    if(was_pressed(&input->GameKey_Yes))
    {
        if(inventory->used_item_type)
        {
            log_add("%sThe scroll turns illegible, you discard it.", ui, start_color(Color_LightGray));
            inventory->used_item_type = UsedItemType_None;
        }
        
        end_consumable_use(inventory->examine_item, items, inventory, dungeon_level);
        unset(inventory->flags, InventoryFlag_AskingPlayer | InventoryFlag_ReadyForKeypress);
    }
    else if(was_pressed(&input->GameKey_No))
    {
        log_add_okay(ui);
        unset(inventory->flags, InventoryFlag_AskingPlayer | InventoryFlag_ReadyForKeypress);
    }
}

internal b32
was_pressed_core(InputState *state)
{
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
    
    #if MOONBREATH_SLOW
    if(fkey_active[3] && state->is_down)
    {
        return(true);
    }
#endif
    
    b32 result = was_pressed_core(state);
    return(result);
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
    Examine *examine = &game->examine;
    Dungeon *dungeon = get_dungeon_from_level(dungeons, dungeons->current_level);
    
    game->should_update = false;
    reset_entity_action_time(&game->action_time);
    
    set(player->flags, EntityFlag_NotifyAboutMultipleItems);
    player->new_pos = player->pos;
    player->new_direction = Direction_None;
    
    if(is_set(player->flags, EntityFlag_IsPathfinding))
    {
        update_player_pathfind(game, player, entities, items, dungeon, ui);
    }
    else if(is_set(inventory->flags, InventoryFlag_AskingPlayer))
    {
        handle_asking_player(game, input, items, inventory, ui, dungeon->level);
    }
    else
    {
        
#if MOONBREATH_SLOW
        if(was_pressed_core(&input->fkeys[1]))
        {
            // Toggle debug state visibility
            DebugState *debug = &game->debug;
            debug->is_shown = !debug->is_shown;
            
            // Stop the active hot interaction
            if(debug->hot_interaction.type)
            {
                debug->hot_interaction.tree->is_moving = false;
                zero_struct(debug->hot_interaction);
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
                
                if(is_item_valid_and_not_in_inventory(item, dungeon->level))
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
                
                if(is_item_valid(item, dungeon->level))
                {
                    toggle(item->flags, ItemFlag_IsIdentified);
                }
            }
            
            return;
        }
        else if(was_pressed(&input->fkeys[5]))
        {
            printf("F5: Item mark test string.\n");
            strcpy(items->temp_mark.array, "My name is Jonathan Blow and I've been a programmer for awhile.");
            
            return;
        }
        else if(was_pressed(&input->fkeys[6]))
        {
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
#endif
        
            b32 was_direction_pressed = false;
            for(Direction direction = Direction_Up; direction <= Direction_DownRight; ++direction)
            {
                u32 index = direction - 1;
                if(was_pressed(&input->game_keys[index]) || examine->key_pressed[index])
            {
                if(is_set(examine->flags, ExamineFlag_Open) && !examine->type)
                    {
                        update_examine_pos(examine, direction, dungeon);
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
            if(!is_set(inventory->flags, InventoryFlag_Open) &&
               was_pressed(&input->GameKey_OpenInventory))
            {
                if(other_windows_are_closed(examine, inventory, ui))
                    {
                    unset(inventory->flags, InventoryFlag_AskingPlayer);
                    
                    set(inventory->flags, InventoryFlag_Open);
                    set_view_at_start_and_reset_all_view_moves(&inventory->window.view);
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
                                reset_letters(ui->select_letters);
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
                    else if(inventory->used_item_type)
                    {
                            ask_for_item_cancel(input, game, ui, inventory);
                        }
                        else if(is_set(inventory->flags, InventoryFlag_Open))
                    {
                            unset(inventory->flags, InventoryFlag_Open | InventoryFlag_ReadyForKeypress);
                    }
                    else if(is_set(inventory->flags, InventoryFlag_MultiplePickup))
                    {
                        reset_inventory_multiple_pickup(items, &inventory->flags, dungeon->level, ui->select_letters);
                    }
                    else if(is_set(inventory->flags, InventoryFlag_MultipleExamine))
                    {
                        reset_letters(ui->select_letters);
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
                        set_view_at_start_and_reset_all_view_moves(&items->pickup_window.view);
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
                        game->action_time = player->p.turn_action_time * 3.0f;
                    }
                    }
                }
            else if(was_pressed(&input->GameKey_AutoExplore) &&
                    other_windows_are_closed(examine, inventory, ui))
            {
                // Start player pathfinding
                
                if(are_new_enemies_in_player_view(entities, dungeon))
                    {
                        log_add("There are enemies near!", ui);
                    }
                    else
                {
                        if(entity_can_move(player, ui, true))
                        {
                            assert(!is_set(player->flags, EntityFlag_IsPathfinding));
                            
                            b32 pathfind_target_pos_set = false;
                            v2u pathfind_target_pos = {0};
                            
                            while(is_dungeon_explorable(dungeon))
                            {
                                pathfind_target_pos = get_random_dungeon_pos(&game->random, dungeon->size);
                                if(is_dungeon_pos_traversable_and_has_not_been_seen(dungeon->tiles, pathfind_target_pos))
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
                                log_add("Nothing more to explore.", ui);
                            }
                        }
                    }
                }
                else if(was_pressed(&input->GameKey_Examine))
                {
                if(other_windows_are_closed(examine, inventory, ui))
                {
                    set(examine->flags, ExamineFlag_Open | ExamineFlag_CameraFollow);
                    examine->pos = player->pos;
                    }
                }
                else if(was_pressed(&input->GameKey_Log))
                {
                if(other_windows_are_closed(examine, inventory, ui))
                {
                    ui->full_log_open = true;
                    ui->start_full_log_from_end = true;
                    
                    set_view_at_start_and_reset_all_view_moves(&ui->full_log.view);
                    }
                }
                else if(was_pressed(&input->GameKey_Wait))
                {
                if(other_windows_are_closed(examine, inventory, ui))
                {
                    log_add("%sYou wait for a moment..", ui, start_color(Color_LightGray));
                    unset(player->flags, EntityFlag_NotifyAboutMultipleItems);
                        
                        game->should_update = true;
                    game->action_time = player->p.turn_action_time;
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
                            update_item_adjust(input, examine_item, items, inventory, ui, dungeon->level);
                            }
                            else
                            {
                                if(was_pressed(&input->Key_A))
                                {
                                set(inventory->flags, InventoryFlag_Adjust);
                                log_add("%sAdjust to which letter? (%s to cancel)", ui, start_color(Color_Yellow), get_printable_key(input, game->keybinds[GameKey_Back]).s);
                                }
                                else if(was_pressed(&input->Key_E))
                                {
                                equip_item(game, player, examine_item, inventory, ui);
                                }
                            else if(was_pressed(&input->Key_U) &&
                                    unequip_item(game, player, examine_item, ui))
                                {
                                    log_add_item_action_text(examine_item, ui, ItemActionType_Unequip);
                                }
                            else if((was_pressed(&input->Key_C) ||
                                     was_pressed(&input->Key_R)) &&
                                        is_item_consumable(examine_item->type))
                            {
                                use_consumable(game, player, dungeon, examine_item, items, inventory, ui);
                                }
                                else if(was_pressed(&input->Key_D))
                            {
                                drop_item_from_inventory(game, player, examine_item, items, inventory, dungeon, ui);
                                }
                                else if(was_pressed(&input->Key_M))
                                {
                                // Start examine item mark
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
                                if(inventory->used_item_type)
                                    {
                                    use_inventory_item(&game->random, pressed, examine_item, items, inventory, ui, dungeon->level);
                                    }
                                    else
                                {
                                    // Examine item
                                    Letter *letter = get_letter(inventory->item_letters, pressed);
                                    
                                    if(letter->parent_type)
                                    {
                                        assert(letter->parent_type == LetterParentType_Item);
                                        
                                        set(inventory->flags, InventoryFlag_Examine);
                                        inventory->examine_item = letter->item;
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
                                if(is_item_valid_and_selected(item, dungeon->level) &&
                                       !add_item_to_inventory(game, player, item, items, inventory, ui, dungeon->level, true))
                                {
                                        break;
                                }
                            }
                            
                            reset_inventory_multiple_pickup(items, &inventory->flags, dungeon->level, ui->select_letters);
                            }
                            else if(pressed)
                        {
                            // Select and unselect the item in the pickup window
                            Letter *letter = get_letter(ui->select_letters, pressed);
                            
                            if(letter->parent_type)
                            {
                                assert(letter->parent_type == LetterParentType_Item);
                                
                                toggle(letter->item->flags, ItemFlag_IsSelected);
                            }
                            }
                    }
                    else if(is_set(inventory->flags, InventoryFlag_MultipleExamine) &&
                                pressed &&
                                !examine->type)
                    {
                        if(is_set(inventory->flags, InventoryFlag_ReadyForKeypress))
                        {
                            Letter *letter = get_letter(ui->select_letters, pressed);
                            if(letter->parent_type)
                            {
                                set(examine->flags, ExamineFlag_Open);
                                unset(examine->flags, ExamineFlag_CameraFollow);
                                
                                switch(letter->parent_type)
                                {
                                    case LetterParentType_Entity:
                                    {
                                        examine->type = ExamineType_Entity;
                                        examine->entity = letter->entity;
                                    } break;
                                    
                                    case LetterParentType_Item:
                                    {
                                        examine->type = ExamineType_Item;
                                        examine->item = letter->item;
                                    } break;
                                    
                                    case LetterParentType_DungeonTrap:
                                    {
                                        examine->type = ExamineType_Trap;
                                        examine->trap = letter->trap;
                                    } break;
                                    
                                    invalid_default_case;
                                }
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
    Dungeon *dungeon = get_dungeon_from_level(dungeons, dungeons->current_level);
    
    // Update player
    Entity *player = get_player_entity(entities);
    assert(is_entity_valid_and_player(player));
    
    update_player_input(game, input, player, entities, items, inventory, dungeons, assets, ui);
    
    player->defence = 0;
    player->p.weight = 0;
    
    // Apply player armor stats
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
                player->defence += (item->a.defence + item->enchantment_level);
                player->p.weight += item->a.weight;
            }
        }
    }
    
    player->evasion = 10 - (player->p.weight / player->p.weight_to_evasion_ratio);
    player->evasion += get_statuses_stat_value(player->statuses, StatType_EV);
    
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
            force_move_entity(player, dungeon, player->new_pos, dungeon->level);
        }
        
        else
#endif
        
        if(!is_v2u_equal(player->pos, player->new_pos))
        {
            if(!is_v2u_equal(player->pos, player->new_pos) && is_dungeon_pos_occupied(dungeon->tiles, player->new_pos))
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
                        
                        Item *player_weapon = player->p.weapon;
                        if(player_weapon)
                        {
                            player_damage = player_weapon->w.damage + player_weapon->enchantment_level;
                            player_accuracy = player_weapon->w.accuracy + player_weapon->enchantment_level;
                            player_attack_speed = player_weapon->w.speed;
                            player_damage_type = player_weapon->w.first_damage_type;
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
                        
                        set(target->flags, EntityFlag_InCombat);
                        attack_entity(&game->random, player, target, dungeon, ui, modified_player_damage, player_damage_type);
                        
                        game->action_time = player_attack_speed;
                    }
                }
            } 
            else
            {
                if(is_dungeon_pos_closed_door(dungeon->tiles, player->new_pos))
                {
                    set_dungeon_pos_open_door(dungeon->tiles, player->new_pos);
                    game->action_time = player->p.turn_action_time;
                }
                else if(is_dungeon_pos_traversable(dungeon->tiles, player->new_pos))
                {
                    if(is_dungeon_pos_water(dungeon->tiles, player->pos))
                    {
                        game->action_time = player->p.turn_action_time * 2.0f;
                        log_add("%sYou wade through the water..", ui, start_color(Color_LightGray));
                    }
                    else
                    {
                        game->action_time = player->p.turn_action_time;
                    }
                    
                    if(move_entity(&game->random, player, dungeon, ui, player->new_pos))
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
                                DamageType spike_damage_type = DamageType_Physical;
                                u32 spike_damage = get_random_number_from_v2u(&game->random, spec->spike_trap_value);
                                
                                    log_add("Sharp spikes pierce you, dealing %u damage!", ui, spike_damage);
                                attack_entity(&game->random, 0, player, dungeon, ui, spike_damage, spike_damage_type);
                            } break;
                            
                            case DungeonTrapType_Sword:
                            {
                                DamageType sword_damage_type = DamageType_Physical;
                                u32 sword_damage = get_random_number_from_v2u(&game->random, spec->sword_trap_value);
                                
                                    log_add("Swords lacerate you, dealing %u damage!", ui, sword_damage);
                                    attack_entity(&game->random, 0, player, dungeon, ui, sword_damage, sword_damage_type);
                                
                            } break;
                            
                            case DungeonTrapType_Arrow:
                            {
                                DamageType arrow_damage_type = DamageType_Physical;
                                
                                u32 arrow_damage = get_random_number_from_v2u(&game->random, spec->arrow_trap_value);
                                
                                    log_add("Multiple arrows fire at you, dealing %u damage!", ui, arrow_damage);
                                    attack_entity(&game->random, 0, player, dungeon, ui, arrow_damage, arrow_damage_type);
                            } break;
                            
                            case DungeonTrapType_Magic:
                            {
                                DamageType magic_damage_type = get_random_damage_type(&game->random);
                                assert(magic_damage_type != DamageType_Physical);
                                
                                u32 magic_damage = get_random_number_from_v2u(&game->random, spec->magic_trap_value);
                                
                                    log_add("A magical trap explodes below you, dealing %u %s damage!", ui, magic_damage, get_damage_type_text(magic_damage_type));
                                    attack_entity(&game->random, 0, player, dungeon, ui, magic_damage, magic_damage_type);
                            } break;
                            
                            case DungeonTrapType_Bind:
                                {
                                        Status bind_status = {0};
                                        bind_status.type = StatusType_Bind;
                                        bind_status.duration = get_random_number_from_v2u(&game->random, spec->bind_trap_value);
                                    sprintf(bind_status.start_message.s, "You feel like you can't move!");
                                    sprintf(bind_status.end_message.s, "%sYou feel like you can move again.", start_color(Color_LightGray));
                                        
                                    add_entity_status(player->statuses, &bind_status);
                            } break;
                            
                            case DungeonTrapType_Shaft:
                            {
                                assert(!is_set(player->flags, EntityFlag_IsPathfinding));
                                
                                log_add("You fall into a shaft!", ui);
                                
                                if(!trap->is_shaft_set)
                                { 
                                    trap->shaft_depth = get_random_number_from_v2u(&game->random, spec->shaft_trap_value);
                                    u32 shaft_dungeon_level = dungeons->current_level + trap->shaft_depth;
                                        
                                        Dungeon *shaft_dungeon = get_dungeon_from_level(dungeons, shaft_dungeon_level);
                                        trap->shaft_dest = get_random_dungeon_traversable_unoccupied_pos(&game->random, shaft_dungeon);
                                        move_entity_between_dungeons(game, player, entities, dungeons, items, inventory, ui, trap->shaft_dest, shaft_dungeon_level);
                                        
                                        trap->is_shaft_set = true;
                                        
                                        //printf("Set shaft data - Depth: %u - Destination: %u, %u.\n", trap->shaft_depth, trap->shaft_dest.x, trap->shaft_dest.y);
                                }
                            } break;
                            
                            case DungeonTrapType_Summon:
                            {
                                log_add("You hear an odd sound and something appears next to you!", ui);
                                
                                EntityID enemy_id = get_random_enemy_suitable_for_dungeon_level(&game->random, dungeon->level);
                                    v4u summon_rect = get_dungeon_dimension_rect(dungeon->size, player->pos, 2);
                                    v2u summon_pos = get_random_dungeon_traversable_unoccupied_rect_pos(&game->random, dungeon, summon_rect);
                                
                                add_enemy_entity(entities, dungeon, enemy_id, summon_pos.x, summon_pos.y);
                                
#if 0
                                printf("ID: %u\n", enemy_id);
                                printf("Summon Pos: %u, %u\n", summon_pos.x, summon_pos.y);
#endif
                                
                            } break;
                            
                            case DungeonTrapType_Teleport:
                            {
                                log_add("You take a step and find yourself in a different place!", ui);
                                teleport_entity(&game->random, player, dungeon, ui);
                            } break;
                            
                            invalid_default_case;
                            }
                    }
                    }
                }
            }
        }
    }
    
    if(game->action_time)
    {
        game->time += game->action_time;
        player->action_time = game->action_time;
        
        for(u32 action_count = 0; action_count < player->action_time; ++action_count)
        {
            update_entity_statuses(game, player, dungeon, inventory, ui);
        }
        
        // Tell the player about multiple items
        if(is_set(player->flags, EntityFlag_NotifyAboutMultipleItems) &&
               other_windows_are_closed(&game->examine, inventory, ui) &&
               get_dungeon_pos_item_count(items, dungeon->level, player->pos) > 1)
        {
                log_add("There are multiple items here.", ui);
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
        
        if(is_entity_valid_and_enemy(enemy))
        {
            if(game->action_time)
            {
                
#if MOONBREATH_SLOW
                    if(enemy->id == EntityID_Dummy) continue;
#endif
                
                enemy->e.action_timer += game->action_time;
                f32 current_time = enemy->e.action_timer;
                
                //printf("enemy->e.action_timer: %.01f\n", enemy->e.action_timer);
                
                for(;;)
                {
                    u32 enemy_turn_count = get_enemy_turn_count(dungeon->tiles, enemy, &current_time);
                    
                    //printf("\ncurrent_time: %.01f\n", current_time);
                    //printf("enemy_turn_count: %u\n", enemy_turn_count);
                    
                    if(!enemy_turn_count)
                    {
                        break;
                    }
                    
                    reset_entity_action_time(&enemy->e.action_timer);
                    
#if MOONBREATH_SLOW
                        if(is_tile_seen(dungeon->tiles, enemy->pos) && !fkey_active[1])
#else
                        if(is_tile_seen(dungeon->tiles, enemy->pos))
#endif
                        {
                        set(enemy->flags, EntityFlag_InCombat);
                        unset(enemy->flags, EntityFlag_IsPathfinding);
                            
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
                            
                        enemy->hit_chance = 30;
                        assert(player->evasion < enemy->hit_chance);
                        
                        v4u enemy_view_rect = get_dungeon_dimension_rect(dungeon->size, enemy->pos, enemy->fov);
                        v2u pathfind_pos = get_pathfind_pos(&entities->enemy_pathfind_map, dungeon->tiles, enemy->pos, player->pos);
                            
                        if(enemy->e.spell_count)
                        {
                            enemy->e.spell = get_random_entity_spell(&game->random, enemy);
                            Spell *enemy_spell = enemy->e.spell;
                            
                            if(enemy_spell->type == StatusType_Damage &&
                                   is_entity_in_view_and_spell_range(enemy_view_rect, enemy->pos, player->pos, enemy_spell->range))
                                {
                                attack_entity(&game->random, enemy, player, dungeon, ui, enemy_spell->value, enemy_spell->damage_type);
                            }
                            #if 0
                            else if(spell->type == StatusType_Heal)
                                {
                                    for(u32 target_index = 0; target_index < MAX_ENTITY_COUNT; ++target_index)
                                    {
                                        Entity *target = &entities->array[target_index];
                                        if(target_index != enemy_index &&
                                           is_entity_valid_and_enemy(target))
                                        {
                                            if(is_inside_rect_and_spell_range(enemy_fov_rect, spell->range, enemy->pos, target->pos) &&
                                               is_set(target->flags, EntityFlag_InCombat) &&
                                               target->hp < target->max_hp)
                                            {
                                            log_add(ui, "The %s casts %s at the %s, healing it for %u health.", enemy->name, spell->name.s, target->name, spell->value);
                                                heal_entity(target, spell->value);
                                                
                                                break;
                                            }
                                        }
                                    }
                            }
                            #endif
                            
#if 1
                            else if(enemy_spell->type == StatusType_Stat)
                            {
                                // TODO(rami): If repeated, get_entity_from_type(caster_exclude_index)
                                    for(u32 target_index = 0; target_index < MAX_ENTITY_COUNT; ++target_index)
                                    {
                                    Entity *target = &entities->array[target_index];
                                    
                                        if(target_index != enemy_index &&
                                           is_entity_valid_and_enemy(target))
                                    {
                                        if(is_entity_in_view_and_spell_range(enemy_view_rect, enemy->pos, target->pos, enemy_spell->range) &&
                                               is_set(target->flags, EntityFlag_InCombat) &&
                                               !get_statuses_stat_value(target->statuses, enemy_spell->stat_type))
                                        {
                                            log_add("The %s casts %s on the %s.", ui, enemy->name.s, enemy_spell->name.s, target->name.s);
                                            add_entity_status_from_spell(game, target, dungeon, inventory, ui, enemy_spell);
                                                
                                                break;
                                        }
                                        }
                                    }
                            }
                            
                            enemy->e.spell = 0;
                            #endif
                            }
                        else if((is_pos_inside_rect(enemy_view_rect, player->pos) &&
                                 is_set(enemy->flags, EntityFlag_UsesRangedAttacks)) ||
                                    is_v2u_equal(pathfind_pos, player->pos))
                            {
                                attack_entity(&game->random, enemy, player, dungeon, ui, enemy->e.damage, enemy->e.damage_type);
                            }
                            else
                            {
                                enemy->new_pos = pathfind_pos;
                            }
                        }
                        else
                        {
                        if(is_set(enemy->flags, EntityFlag_InCombat) &&
                               !is_enemy_alerted(enemy->e.turns_in_player_view))
                            {
                            if(!is_set(enemy->flags, EntityFlag_IsPathfinding))
                                {
                                make_entity_pathfind(enemy, items, dungeon, &entities->enemy_pathfind_map, player->pos);
                                    //printf("Enemy Pathfind: Target %u, %u\n", enemy->pathfind_target.x, enemy->pathfind_target.y);
                                }
                                
                            enemy->new_pos = get_pathfind_pos(&entities->enemy_pathfind_map, dungeon->tiles, enemy->pos, enemy->pathfind_target_pos);
                                //printf("Enemy Pathfind: New Pos %u, %u\n", enemy->new_pos.x, enemy->new_pos.y);
                                
                                if(is_v2u_equal(enemy->new_pos, enemy->pathfind_target_pos))
                                {
                                    //printf("Enemy Pathfind: Target Reached\n");
                                    
                                unset(enemy->flags, EntityFlag_InCombat | EntityFlag_IsPathfinding);
                                    enemy->e.turns_in_player_view = 0;
                                }
                            }
                            else
                            {
                            unset(enemy->flags, EntityFlag_InCombat);
                                enemy->e.turns_in_player_view = 0;
                                
                                enemy->new_direction = get_random_direction(&game->random);
                                enemy->new_pos = get_direction_pos(enemy->pos, enemy->new_direction);
                                
                            if(is_dungeon_pos_traversable(dungeon->tiles, enemy->new_pos))
                                {
#if MOONBREATH_SLOW
                                    if(is_tile_seen(dungeon->tiles, enemy->new_pos) && !fkey_active[1])
#else
                                    if(is_tile_seen(dungeon->tiles, enemy->new_pos))
#endif
                                {
                                    if(!enemy_turn_count)
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
                        enemy->e.saved_flipped_for_ghost = is_set(enemy->flags, EntityFlag_IsFlipped);
                            enemy->e.saved_pos_for_ghost = enemy->pos;
                        }
                    
                    if(is_dungeon_pos_traversable_and_unoccupied(dungeon->tiles, enemy->new_pos))
                    {
                        move_entity(&game->random, enemy, dungeon, ui, enemy->new_pos);
                        }
                        
                    update_entity_statuses(game, enemy, dungeon, inventory, ui);
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
                EntityState *entities,
                Inventory *inventory,
                 Dungeon *dungeon,
                Assets *assets)
{
        { // Render player
        Entity *player = get_player_entity(entities);
        assert(is_entity_valid_and_player(player));
        
            v4u dest = get_game_dest(game, player->pos);
            SDL_RenderCopy(game->renderer, assets->tileset.tex, (SDL_Rect *)&player->tile_src, (SDL_Rect *)&dest);
            
            // Render player items
            for(u32 slot_index = 1; slot_index < ItemSlot_Count; ++slot_index)
            {
                for(u32 inventory_index = 0; inventory_index < MAX_INVENTORY_SLOT_COUNT; ++inventory_index)
                {
                    Item *item = inventory->slots[inventory_index];
                if(is_item_valid_and_in_inventory(item, dungeon->level) &&
                       is_set(item->flags, ItemFlag_IsEquipped) &&
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
                        case Direction_Up: trail_src = get_dungeon_tileset_rect(DungeonTileID_FootstepsUp); break;
                        case Direction_Down: trail_src = get_dungeon_tileset_rect(DungeonTileID_FootstepsDown); break;
                        case Direction_Left: trail_src = get_dungeon_tileset_rect(DungeonTileID_FootstepsLeft); break;
                        case Direction_Right: trail_src = get_dungeon_tileset_rect(DungeonTileID_FootstepsRight); break;
                            
                        case Direction_UpLeft: trail_src = get_dungeon_tileset_rect(DungeonTileID_FootstepsUpLeft); break;
                        case Direction_UpRight: trail_src = get_dungeon_tileset_rect(DungeonTileID_FootstepsUpRight); break;
                        case Direction_DownLeft: trail_src = get_dungeon_tileset_rect(DungeonTileID_FootstepsDownLeft); break;
                        case Direction_DownRight: trail_src = get_dungeon_tileset_rect(DungeonTileID_FootstepsDownRight); break;
                            
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
                if(!is_set(enemy->flags, EntityFlag_IsInvisible))
            {
                if(is_tile_seen(dungeon->tiles, enemy->pos))
                {
                    set(enemy->flags, EntityFlag_HasBeenSeen);
                        unset(enemy->flags, EntityFlag_IsGhostEnabled);
                    
                    v4u dest = get_game_dest(game, enemy->pos);
                        SDL_RenderCopyEx(game->renderer, assets->tileset.tex, (SDL_Rect *)&enemy->tile_src, (SDL_Rect *)&dest, 0, 0, is_set(enemy->flags, EntityFlag_IsFlipped));
                    
                    // Additional things to render on enemy tile
                    if(enemy->e.turns_in_player_view == 1)
                    {
                            v4u status_src = get_dungeon_tileset_rect(DungeonTileID_EntityAlerted);
                        SDL_RenderCopy(game->renderer, assets->tileset.tex, (SDL_Rect *)&status_src, (SDL_Rect *)&dest);
                    }
                    else
                        {
                            for(u32 index = 0; index < MAX_ENTITY_STATUS_COUNT; ++index)
                            {
                                 Status *status = &enemy->statuses[index];
                                
                                if(status->type)
                                {
                                    v4u status_src = get_dungeon_tileset_rect(DungeonTileID_EntityStatus);
                                    SDL_RenderCopy(game->renderer, assets->tileset.tex, (SDL_Rect *)&status_src, (SDL_Rect *)&dest);
                                    
                                    break;
                                }
                            }
                    }
                    
                        if(is_set(enemy->flags, EntityFlag_InCombat))
                    {
                        // HP Bar Outside
                        v4u hp_bar_outside = {dest.x, dest.y + 33, 32, 4};
                            render_outline_rect(game, hp_bar_outside, Color_Black);
                        
                        // HP Bar Inside
                        u32 hp_bar_inside_w = get_ratio(enemy->hp, enemy->max_hp, 30);
                        v4u hp_bar_inside = {dest.x + 1, dest.y + 34, hp_bar_inside_w, 2};
                        render_fill_rect(game, hp_bar_inside, Color_DarkRed);
                    }
                }
                else
                {
                    if(is_set(enemy->flags, EntityFlag_HasBeenSeen))
                    {
                            if(is_set(enemy->flags, EntityFlag_IsGhostEnabled))
                        {
                            if(is_tile_seen(dungeon->tiles, enemy->e.ghost_pos))
                            {
                                    unset(enemy->flags, EntityFlag_HasBeenSeen | EntityFlag_IsGhostEnabled);
                            }
                            else
                            {
                                v4u dest = get_game_dest(game, enemy->e.ghost_pos);
                                    render_texture_half_color(game->renderer, assets->tileset.tex, enemy->tile_src, dest, is_set(enemy->flags, EntityFlag_IsGhostFlipped));
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
                            
                                set(enemy->flags, EntityFlag_IsGhostEnabled);
                            
                            if(enemy->e.saved_flipped_for_ghost)
                            {
                                    set(enemy->flags, EntityFlag_IsGhostFlipped);
                            }
                            else
                            {
                                    unset(enemy->flags, EntityFlag_IsGhostFlipped);
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
add_player_entity(Entity *player)
{
    player->id = EntityID_Player;
    strcpy(player->name.s, "Name");
    
    player->max_hp = 80;
    player->hp = player->max_hp;
    
    player->width = player->height = 32;
    player->tile_src = get_dungeon_tile_rect(get_entity_tile_pos(player->id));
    player->remains = EntityRemains_RedBlood;
    player->type = EntityType_Player;
    
    player->strength = 10;
    player->intelligence = 10;
    player->dexterity = 10;
    
    player->evasion = 10;
    player->fov = 8;
    
    player->p.turn_action_time = 1.0f;
    player->p.weight_to_evasion_ratio = 3;
    
    //player->max_hp = 10000000;
    //player->resistances[DamageType_Fire] = 5;
    //player->resistances[DamageType_Poison] = 5;
}

internal u32
get_enemy_level_from_entity_id(EntityID id)
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
        
        case EntityID_DwarwenWarrior: result = 8; break;
        case EntityID_DwarwenSorcerer: result = 0; break;
        case EntityID_DwarwenPriest: result = 8; break;
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
        case EntityID_Mahjarrat: result = 10; break;
        
        invalid_default_case;
    }
    
    return(result);
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
            enemy->width = enemy->height = 32;
            enemy->tile_src = get_dungeon_tile_rect(get_entity_tile_pos(enemy->id));
            enemy->type = EntityType_Enemy;
            enemy->fov = 8;
            enemy->e.level = get_enemy_level_from_entity_id(id);
            
            force_move_entity(enemy, dungeon, enemy->new_pos, dungeon->level);
            
            #if 1
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
                    // This is a dummy entity for testing purposes
                    strcpy(enemy->name.s, "Dummy");
                    enemy->max_hp = enemy->hp = U32_MAX;
                } break;
                
                case EntityID_SkeletonWarrior:
                {
                    strcpy(enemy->name.s, "Skeleton Warrior");
                    enemy->max_hp = enemy->hp = 18;
                    
                    enemy->e.damage_type = DamageType_Physical;
                    enemy->e.damage = 5;
                    enemy->evasion = 5;
                    enemy->action_time = 1.0f;
                } break;
                
                case EntityID_SkeletonArcher:
                {
                    strcpy(enemy->name.s, "Skeleton Archer");
                    enemy->max_hp = enemy->hp = 15;
                    
                    enemy->e.damage_type = DamageType_Physical;
                    enemy->e.damage = 4;
                    enemy->evasion = 6;
                    enemy->action_time = 1.0f;
                    
                    set(enemy->flags, EntityFlag_UsesRangedAttacks);
                } break;
                
                case EntityID_SkeletonMage:
                {
                    strcpy(enemy->name.s, "Skeleton Mage");
                    enemy->max_hp = enemy->hp = 15;
                    
                    enemy->evasion = 4;
                    enemy->action_time = 1.0f;
                    
                    add_entity_attack_spell(enemy, "Dark Bolt", DamageType_Dark, 5, 100);
                    } break;
                
                case EntityID_Bat:
                {
                    strcpy(enemy->name.s, "Bat");
                    enemy->max_hp = enemy->hp = 14;
                    
                    enemy->e.damage_type = DamageType_Physical;
                    enemy->e.damage = 1;
                    enemy->evasion = 14;
                    enemy->action_time = 3.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_Rat:
                {
                    strcpy(enemy->name.s, "Rat");
                    enemy->max_hp = enemy->hp = 11;
                    
                    enemy->e.damage_type = DamageType_Physical;
                    enemy->e.damage = 1;
                    enemy->evasion = 12;
                    enemy->action_time = 0.5f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_KoboldWarrior:
                {
                    strcpy(enemy->name.s, "Kobold Warrior");
                    enemy->max_hp = enemy->hp = 22;
                    
                    enemy->e.damage_type = DamageType_Physical;
                    enemy->e.damage = 5;
                    enemy->evasion = 8;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_KoboldShaman:
                {
                    strcpy(enemy->name.s, "Kobold Shaman");
                    enemy->max_hp = enemy->hp = 18;
                    
                    enemy->evasion = 7;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                    
                    //add_entity_spell(enemy, SpellID_Bolster, 0);
                    //add_entity_spell(enemy, SpellID_LesserHeal, 0);
                } break;
                
                case EntityID_Snail:
                {
                    strcpy(enemy->name.s, "Snail");
                    enemy->max_hp = enemy->hp = 28;
                    
                    enemy->e.damage_type = DamageType_Physical;
                    enemy->e.damage = 10;
                    enemy->evasion = 3;
                    enemy->action_time = 0.5f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_Slime:
                {
                    strcpy(enemy->name.s, "Slime");
                    enemy->max_hp = enemy->hp = 20;
                    
                    enemy->e.damage_type = DamageType_Physical;
                    enemy->e.damage = 4;
                    enemy->evasion = 6;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_GreenBlood;
                } break;
                
                case EntityID_Dog:
                {
                    strcpy(enemy->name.s, "Dog");
                    enemy->max_hp = enemy->hp = 18;
                    
                    enemy->e.damage_type = DamageType_Physical;
                    enemy->e.damage = 3;
                    enemy->evasion = 10;
                    enemy->action_time = 2.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_OrcWarrior:
                {
                    strcpy(enemy->name.s, "Orc Warrior");
                    enemy->max_hp = enemy->hp = 28;
                    
                    enemy->e.damage_type = DamageType_Physical;
                    enemy->e.damage = 8;
                    enemy->evasion = 10;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_OrcArcher:
                {
                    strcpy(enemy->name.s, "Orc Archer");
                    enemy->max_hp = enemy->hp = 26;
                    
                    enemy->e.damage_type = DamageType_Physical;
                    enemy->e.damage = 6;
                    enemy->evasion = 8;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                    
                    set(enemy->flags, EntityFlag_UsesRangedAttacks);
                } break;
                
                case EntityID_OrcShaman:
                {
                    strcpy(enemy->name.s, "Orc Shaman");
                    enemy->max_hp = enemy->hp = 26;
                    
                    enemy->evasion = 8;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                    
                    add_entity_attack_spell(enemy, "Lightning Whip", DamageType_Lightning, 6, 70);
                    add_entity_stat_spell(enemy, "Protective Shout", StatType_Def, 3, 5, 30);
                    //add_entity_heal_spell(enemy, "Healing Wave", 8, 20);
                    } break;
                
                case EntityID_Python:
                {
                    strcpy(enemy->name.s, "Python");
                    enemy->max_hp = enemy->hp = 15;
                    
                    enemy->e.damage_type = DamageType_Physical;
                    enemy->e.damage = 4;
                    enemy->evasion = 11;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                    
                    Status *poison = &enemy->e.poison;
                    poison->type = StatusType_Poison;
                    poison->value = 2;
                    poison->chance = 33;
                    poison->duration = 8;
                } break;
                
                case EntityID_Shade:
                {
                    strcpy(enemy->name.s, "Shade");
                    enemy->max_hp = enemy->hp = 18;
                    
                    enemy->e.damage_type = DamageType_Dark;
                    enemy->e.damage = 3;
                    enemy->evasion = 16;
                    enemy->action_time = 1.0f;
                } break;
                
                case EntityID_ElfKnight:
                {
                    strcpy(enemy->name.s, "Elf Knight");
                    enemy->max_hp = enemy->hp = 32;
                    
                    enemy->e.damage_type = DamageType_Physical;
                    enemy->e.damage = 12;
                    enemy->defence = 3;
                    enemy->evasion = 2;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_ElfArbalest:
                {
                    strcpy(enemy->name.s, "Elf Arbalest");
                    enemy->max_hp = enemy->hp = 28;
                    
                    enemy->e.damage = 14;
                    enemy->evasion = 4;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                    
                    set(enemy->flags, EntityFlag_UsesRangedAttacks);
                } break;
                
                case EntityID_ElfMage:
                {
                    strcpy(enemy->name.s, "Elf Mage");
                    enemy->max_hp = enemy->hp = 28;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                    
                    // TODO(rami): Blood mage? blood siphon?
                    // TODO(rami): Weaken
                } break;
                
                case EntityID_GiantSlime:
                {
                    strcpy(enemy->name.s, "Giant Slime");
                    enemy->max_hp = enemy->hp = 40;
                    
                    enemy->e.damage_type = DamageType_Physical;
                    enemy->e.damage = 6;
                    enemy->evasion = 1;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_GreenBlood;
                    
                    // TODO(rami): Leaves smaller versions of itself?
                    // TODO(rami): Leaves pools of acid?
                } break;
                
                case EntityID_Spectre:
                {
                    strcpy(enemy->name.s, "Spectre");
                    enemy->max_hp = enemy->hp = 25;
                    
                    enemy->e.damage_type = DamageType_Dark;
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    
                    // TODO(rami): Slow and invisible?
                    // TODO(rami): Slow and blink?
                } break;
                
                case EntityID_OrcAssassin:
                {
                    strcpy(enemy->name.s, "Orc Assassin");
                    enemy->max_hp = enemy->hp = 28;
                    
                    enemy->e.damage_type = DamageType_Physical;
                    enemy->e.damage = 7;
                    enemy->evasion = 10;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_OrcSorcerer:
                {
                    strcpy(enemy->name.s, "Orc Sorcerer");
                    enemy->max_hp = enemy->hp = 26;
                    
                    enemy->evasion = 5;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                    
                    // TODO(rami): Focused on damage
                } break;
                
                case EntityID_Minotaur:
                {
                    strcpy(enemy->name.s, "Minotaur");
                    enemy->max_hp = enemy->hp = 34;
                    
                    enemy->e.damage_type = DamageType_Physical;
                    enemy->e.damage = 14;
                    enemy->evasion = 4;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_Treant:
                {
                    strcpy(enemy->name.s, "Treant");
                    enemy->max_hp = enemy->hp = 40;
                    
                    enemy->e.damage_type = DamageType_Physical;
                    enemy->e.damage = 4;
                    enemy->defence = 6;
                    enemy->evasion = 0;
                    enemy->action_time = 0.5f;
                } break;
                
                case EntityID_Viper:
                {
                    strcpy(enemy->name.s, "Viper");
                    enemy->max_hp = enemy->hp = 18;
                    
                    enemy->e.damage_type = DamageType_Physical;
                    enemy->e.damage = 4;
                    enemy->evasion = 5;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                    
                    Status *poison = &enemy->e.poison;
                    poison->type = StatusType_Poison;
                    poison->value = 2;
                    poison->chance = 33;
                    poison->duration = 8;
                } break;
                
                case EntityID_CentaurWarrior:
                {
                    strcpy(enemy->name.s, "Centaur Warrior");
                    enemy->max_hp = enemy->hp = 35;
                    
                    enemy->e.damage_type = DamageType_Physical;
                    enemy->e.damage = 0;
                    enemy->evasion = 4;
                    enemy->action_time = 2.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_CentaurSpearman:
                {
                    strcpy(enemy->name.s, "Centaur Spearman");
                    enemy->max_hp = enemy->hp = 35;
                    
                    enemy->e.damage_type = DamageType_Physical;
                    enemy->e.damage = 0;
                    enemy->evasion = 4;
                    enemy->action_time = 2.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_CentaurArcher:
                {
                    strcpy(enemy->name.s, "Centaur Archer");
                    enemy->max_hp = enemy->hp = 32;
                    
                    enemy->e.damage_type = DamageType_Physical;
                    enemy->e.damage = 0;
                    enemy->evasion = 3;
                    enemy->action_time = 2.0f;
                    enemy->remains = EntityRemains_RedBlood;
                    
                    set(enemy->flags, EntityFlag_UsesRangedAttacks);
                } break;
                
                case EntityID_CursedSkull:
                {
                    strcpy(enemy->name.s, "Cursed Skull");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage_type = DamageType_Physical;
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    
                    // TODO(rami): Maybe these attack physically and have a chance
                    // on hit to give you a status?
                } break;
                
                case EntityID_Wolf:
                {
                    strcpy(enemy->name.s, "Wolf");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage_type = DamageType_Physical;
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_OgreWarrior:
                {
                    strcpy(enemy->name.s, "Ogre Warrior");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage_type = DamageType_Physical;
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_OgreArcher:
                {
                    strcpy(enemy->name.s, "Ogre Archer");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage_type = DamageType_Physical;
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                    
                    set(enemy->flags, EntityFlag_UsesRangedAttacks);
                    
                    // TODO(rami): Switch from ranger to something special?
                } break;
                
                case EntityID_OgreMage:
                {
                    strcpy(enemy->name.s, "Ogre Mage");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_Cyclops:
                {
                    strcpy(enemy->name.s, "Cyclops");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage_type = DamageType_Physical;
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_ShadowWalker:
                {
                    strcpy(enemy->name.s, "Shadow Walker");
                    enemy->max_hp = enemy->hp = 10;
                    
                    enemy->e.damage_type = DamageType_Dark;
                    enemy->e.damage = 5;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    
                    set(enemy->flags, EntityFlag_IsInvisible);
                } break;
                
                case EntityID_DwarwenWarrior:
                {
                    strcpy(enemy->name.s, "Dwarwen Warrior");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage_type = DamageType_Physical;
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_DwarwenSorcerer:
                {
                    strcpy(enemy->name.s, "Dwarwen Sorcerer");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_DwarwenPriest:
                {
                    strcpy(enemy->name.s, "Dwarwen Priest");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_ScarletSnake:
                {
                    strcpy(enemy->name.s, "Scarlet Snake");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage_type = DamageType_Physical;
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_Lich:
                {
                    strcpy(enemy->name.s, "Lich");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                } break;
                
                case EntityID_AbyssalFiend:
                {
                    strcpy(enemy->name.s, "Abyssal Fiend");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage_type = DamageType_Physical;
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                    
                    set(enemy->flags, EntityFlag_UsesRangedAttacks);
                } break;
                
                case EntityID_BloodTroll:
                {
                    strcpy(enemy->name.s, "Blood Troll");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage_type = DamageType_Physical;
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_IronGolem:
                {
                    strcpy(enemy->name.s, "Iron Golem");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage_type = DamageType_Physical;
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                } break;
                
                case EntityID_Griffin:
                {
                    strcpy(enemy->name.s, "Griffin");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage_type = DamageType_Physical;
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_Imp:
                {
                    strcpy(enemy->name.s, "Imp");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage_type = DamageType_Physical;
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_BlackKnight:
                {
                    strcpy(enemy->name.s, "Black Knight");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage_type = DamageType_Physical;
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                } break;
                
                case EntityID_GiantDemon:
                {
                    strcpy(enemy->name.s, "Giant Demon");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage_type = DamageType_Physical;
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_Hellhound:
                {
                    strcpy(enemy->name.s, "Hellhound");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage_type = DamageType_Physical;
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_AbyssalHexmaster:
                {
                    strcpy(enemy->name.s, "Abyssal Hexmaster");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                    enemy->remains = EntityRemains_RedBlood;
                } break;
                
                case EntityID_Mahjarrat:
                {
                    strcpy(enemy->name.s, "Mahjarrat");
                    enemy->max_hp = enemy->hp = 0;
                    
                    enemy->e.damage = 0;
                    enemy->evasion = 0;
                    enemy->action_time = 1.0f;
                } break;
                
                invalid_default_case;
            }
            
            break;
        }
    }
    
    assert(result);
    return(result);
}