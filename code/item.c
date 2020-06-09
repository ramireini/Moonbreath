internal b32
is_item_used(use_type use, u32 slot_index, inventory_t *inventory)
{
    b32 result = (inventory->use_item_type == use &&
                  inventory->use_item_src_index == slot_index);
    
    return(result);
}

internal b32
is_item_not_used(u32 slot_index, inventory_t *inventory)
{
    b32 result = (inventory->use_item_type == use_type_none &&
                  inventory->use_item_src_index != slot_index);
    
    return(result);
}

internal void
identify_items_with_id(item id, item_t *items)
{
    for(u32 item_index = 0;
        item_index < MAX_ITEMS;
        ++item_index)
    {
        item_t *item = &items[item_index];
        if(item->id == id)
        {
            item->is_identified = true;
        }
    }
}

internal void
set_consumable_as_known(item id, item_t *items, consumable_data_t *cdata)
{
    switch(id)
    {
        case item_potion_of_might: cdata->is_known[consumable_might] = true;
        case item_potion_of_wisdom: cdata->is_known[consumable_wisdom] = true;
        case item_potion_of_agility: cdata->is_known[consumable_agility] = true;
        case item_potion_of_awareness: cdata->is_known[consumable_awareness] = true;
        case item_potion_of_fortitude: cdata->is_known[consumable_fortitude] = true;
        case item_potion_of_resistance: cdata->is_known[consumable_resistance] = true;
        case item_potion_of_healing: cdata->is_known[consumable_healing] = true;
        case item_potion_of_haste: cdata->is_known[consumable_haste] = true;
        case item_potion_of_curing: cdata->is_known[consumable_curing] = true;
        case item_potion_of_vulnerability: cdata->is_known[consumable_vulnerability] = true;
        case item_potion_of_clumsiness: cdata->is_known[consumable_clumsiness] = true;
        case item_potion_of_poison: cdata->is_known[consumable_poison] = true;
        case item_potion_of_weakness: cdata->is_known[consumable_weakness] = true;
        case item_potion_of_flight: cdata->is_known[consumable_flight] = true;
        
        case item_scroll_of_identify: cdata->is_known[consumable_identify] = true; break;
        case item_scroll_of_infuse_weapon: cdata->is_known[consumable_infuse_weapon] = true; break;
        case item_scroll_of_enchant_weapon: cdata->is_known[consumable_enchant_weapon] = true; break;
        case item_scroll_of_enchant_armor: cdata->is_known[consumable_enchant_armor] = true; break;
        case item_scroll_of_magic_mapping: cdata->is_known[consumable_magic_mapping] = true; break;
        case item_scroll_of_teleportation: cdata->is_known[consumable_teleportation] = true; break;
        
        invalid_default_case;
    }
}

internal void
set_consumable_as_known_and_identify_items_with_id(item id,
                                                   item_t *items,
                                                   consumable_data_t *cdata)
{
    set_consumable_as_known(id, items, cdata);
    identify_items_with_id(id, items);
}

internal void
ask_for_item_cancel(game_state_t *game, string_128_t *log, inventory_t *inventory)
{
    log_text(log, "Cancel and waste the item?, [%c] Yes [%c] No.", game->keybinds[key_yes], game->keybinds[key_no]);
    inventory->is_asking_player = true;
}

internal void
reset_inventory_item_use(inventory_t *inventory)
{
    inventory->use_item_type = use_type_none;
    inventory->use_item_src_index = MAX_U32;
    inventory->use_item_dest_index = MAX_U32;
}

internal u32
get_inventory_slot_index(v2u pos)
{
    u32 result = (pos.y * INVENTORY_WIDTH) + pos.x;
    return(result);
}

internal item_t *
get_inventory_slot_item(inventory_t *inventory, v2u pos)
{
    u32 slot_index = get_inventory_slot_index(pos);
    item_t *result = inventory->slots[slot_index];
    return(result);
}

internal slot_t
get_slot_from_pos(inventory_t *inventory, v2u pos)
{
    slot_t result =
    {
        get_inventory_slot_index(pos),
        get_inventory_slot_item(inventory, pos)
    };
    
    return(result);
}

internal b32
is_item_consumable(item_type type)
{
    b32 result = (type == item_type_potion ||
                  type == item_type_scroll);
    
    return(result);
}

internal item_damage_type
random_item_damage_type(game_state_t *game)
{
    // Skips physical damage type
    item_damage_type result = random_number(&game->random,
                                            item_damage_type_none + 2,
                                            item_damage_type_count - 1);
    return(result);
}

internal char *
item_id_text(item id)
{
    char *result = 0;
    
    switch(id)
    {
        case item_potion_of_might:
        case item_potion_of_wisdom:
        case item_potion_of_agility:
        case item_potion_of_awareness:
        case item_potion_of_fortitude:
        case item_potion_of_resistance:
        case item_potion_of_healing:
        case item_potion_of_haste:
        case item_potion_of_curing:
        case item_potion_of_vulnerability:
        case item_potion_of_clumsiness:
        case item_potion_of_poison:
        case item_potion_of_weakness:
        case item_potion_of_flight: result = "Potion"; break;
        
        case item_scroll_of_identify:
        case item_scroll_of_infuse_weapon:
        case item_scroll_of_enchant_weapon:
        case item_scroll_of_enchant_armor:
        case item_scroll_of_teleportation:
        case item_scroll_of_magic_mapping: result = "Scroll"; break;
        
        case item_dagger: result = "Dagger"; break;
        case item_sword: result = "Sword"; break;
        case item_scimitar: result = "Scimitar"; break;
        case item_katana: result = "Katana"; break;
        case item_club: result = "Club"; break;
        case item_morningstar: result = "Morningstar"; break;
        case item_warhammer: result = "Warhammer"; break;
        case item_hand_axe: result = "Hand Axe"; break;
        case item_war_axe: result = "War Axe"; break;
        case item_battleaxe: result = "Battleaxe"; break;
        case item_spear: result = "Spear"; break;
        case item_halberd: result = "Halberd"; break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal char *
item_rarity_text(item_rarity rarity)
{
    char *result = "";
    
    switch(rarity)
    {
        case item_rarity_common: result = "Common "; break;
        case item_rarity_magical: result = "Magical "; break;
        case item_rarity_mythical: result = "Mythical "; break;
    }
    
    return(result);
}

internal char *
item_handedness_text(item_handedness handedness)
{
    char *result = "";
    
    switch(handedness)
    {
        case item_handedness_none: break;
        case item_handedness_one_handed: result = "One-Handed"; break;
        case item_handedness_two_handed: result = "Two-Handed"; break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal char *
item_damage_type_text(item_damage_type damage_type)
{
    char *result = 0;
    
    switch(damage_type)
    {
        case item_damage_type_physical: result = "Physical"; break;
        case item_damage_type_fire: result = "Fire"; break;
        case item_damage_type_ice: result = "Ice"; break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal char *
item_rarity_color_code(item_rarity rarity)
{
    char *result = 0;
    
    switch(rarity)
    {
        case item_rarity_common: result = start_color(color_white); break;
        case item_rarity_magical: result = start_color(color_dark_blue); break;
        case item_rarity_mythical: result = start_color(color_purple); break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal string_128_t
full_item_name(item_t *item)
{
    string_128_t result = {0};
    
    if(item->type == item_type_weapon ||
       item->type == item_type_armor)
    {
        if(item->secondary_damage_type)
        {
            sprintf(result.str, "%c%d %s of %s",
                    (item->enchantment_level >= 0) ? '+' : '-',
                    abs(item->enchantment_level),
                    item->name,
                    item_damage_type_text(item->secondary_damage_type));
        }
        else
        {
            sprintf(result.str, "%c%d %s",
                    (item->enchantment_level >= 0) ? '+' : '-',
                    abs(item->enchantment_level),
                    item->name);
        }
    }
    else
    {
        sprintf(result.str, "%s", item->name);
    }
    
    return(result);
}

internal slot_t
equipped_inventory_slot_from_item_equip_slot(item_equip_slot equip_slot, inventory_t *inventory)
{
    slot_t result = {0};
    
    for(u32 slot_index = 0;
        slot_index < INVENTORY_AREA;
        ++slot_index)
    {
        item_t *item = inventory->slots[slot_index];
        if(item)
        {
            if((item->equip_slot == equip_slot) && item->is_equipped)
            {
                result.index = slot_index;
                result.item = inventory->slots[slot_index];
                break;
            }
        }
    }
    
    return(result);
}


internal void
render_items(game_state_t *game, dungeon_t *dungeon, item_t *items, assets_t *assets)
{
    for(u32 item_index = 0;
        item_index < MAX_ITEMS;
        ++item_index)
    {
        item_t *item = &items[item_index];
        if(item->id &&
           !item->in_inventory &&
           tile_is_seen(dungeon->tiles, item->pos))
        {
            v4u src = tile_rect(item->tile);
            v4u dest = game_dest(game, item->pos);
            SDL_RenderCopy(game->renderer,
                           assets->item_tileset.tex,
                           (SDL_Rect *)&src,
                           (SDL_Rect *)&dest);
            
            // TODO(rami): Added a line around items, would like to make this
            // something you can toggle in the future.
#if 1
            set_render_color(game, color_dark_green);
            SDL_RenderDrawRect(game->renderer, (SDL_Rect *)&dest);
#endif
        }
    }
}

// TODO(rami): add_item_stats() and remove_item_stats() need to handle
// the additional stats found in mythical items.
internal void
add_item_stats(item_t *item, entity_t *player)
{
    if(item->type == item_type_weapon)
    {
        player->damage = item->w.damage + item->enchantment_level;
        player->p.accuracy = item->w.accuracy + item->enchantment_level;
        player->p.attack_speed = item->w.attack_speed;
    }
    else if(item->type == item_type_armor)
    {
        player->p.defence += item->a.defence;
        player->evasion -= item->a.weight;
    }
}

internal void
remove_item_stats(item_t *item, entity_t *player)
{
    if(item->type == item_type_weapon)
    {
        // Base player damage, accuracy and attack speed.
        player->damage = 1;
        player->p.accuracy = 2;
        player->p.attack_speed = 1.0f;
    }
    else if(item->type == item_type_armor)
    {
        player->p.defence -= item->a.defence;
        player->evasion += item->a.weight;
    }
}

internal void
remove_item_from_game(item_t *item)
{
    memset(item, 0, sizeof(item_t));
}

internal void
remove_item_from_inventory(slot_t slot,
                           entity_t *player,
                           string_128_t *log,
                           inventory_t *inventory)
{
    if(slot.item->is_equipped)
    {
        remove_item_stats(slot.item, player);
    }
    
    slot.item->in_inventory = false;
    slot.item->is_equipped = false;
    slot.item->pos = player->pos;
    
    inventory->slots[slot.index] = 0;
}

internal void
remove_item_from_inventory_and_game(slot_t slot,
                                    entity_t *player,
                                    string_128_t *log,
                                    inventory_t *inventory)
{
    remove_item_from_inventory(slot, player, log, inventory);
    remove_item_from_game(slot.item);
}

internal void
remove_used_item_from_inventory_and_game(entity_t *player,
                                         string_128_t *log,
                                         inventory_t *inventory)
{
    slot_t slot = {inventory->use_item_src_index, inventory->slots[slot.index]};
    remove_item_from_inventory_and_game(slot, player, log, inventory);
    reset_inventory_item_use(inventory);
}

internal void
equip_item(item_t *item, entity_t *player, string_128_t *log)
{
    item->is_equipped = true;
    add_item_stats(item, player);
    
#if 0
    string_t item_name = full_item_name(item);
    add_log_string(log, "You equip the %s%s%.",
                   item_rarity_color_code(item->rarity),
                   item_name.str,
                   end_color());
#endif
}

internal void
unequip_item(item_t *item, entity_t *player, string_128_t *log)
{
    item->is_equipped = false;
    remove_item_stats(item, player);
    
#if 0
    string_t item_name = full_item_name(item);
    add_log_string(log, "You unequip the %s%s%s.",
                   item_rarity_color_code(item->rarity),
                   item_name.str,
                   end_color());
#endif
}

internal void
pick_up_item(item_t *items, inventory_t *inventory, entity_t *player, string_128_t *log)
{
    for(u32 item_index = 0;
        item_index < MAX_ITEMS;
        ++item_index)
    {
        item_t *item = &items[item_index];
        if(item->id &&
           !item->in_inventory &&
           V2u_equal(item->pos, player->pos))
        {
            for(u32 slot_index = 0;
                slot_index < INVENTORY_AREA;
                ++slot_index)
            {
                if(!inventory->slots[slot_index])
                {
                    item->in_inventory = true;
                    inventory->slots[slot_index] = item;
                    
                    if(item->is_identified)
                    {
                        string_128_t item_name = full_item_name(item);
                        log_text(log, "You pick up a %s%s%s.",
                                 item_rarity_color_code(item->rarity),
                                 item_name.str,
                                 end_color());
                    }
                    else
                    {
                        log_text(log, "You pick up a %s%s%s.",
                                 item_rarity_color_code(item->rarity),
                                 item_id_text(item->id),
                                 end_color());
                    }
                    
                    return;
                }
            }
            
            log_text(log, "Your inventory is full right now.");
            return;
        }
    }
    
    log_text(log, "You find nothing to pick up.");
}

internal void
add_weapon_item(item id, item_rarity rarity, u32 x, u32 y, game_state_t *game, item_t *items)
{
    for(u32 item_index = 0;
        item_index < MAX_ITEMS;
        ++item_index)
    {
        item_t *item = &items[item_index];
        if(!item->id)
        {
            item->id = id;
            item->pos = V2u(x, y);
            item->equip_slot = item_equip_slot_first_hand;
            item->type = item_type_weapon;
            // TODO(Rami): All weapon types should set this value by themselves.
            item->w.attack_speed = 1.0f;
            item->rarity = rarity;
            item->primary_damage_type = item_damage_type_physical;
            
            switch(id)
            {
                case item_dagger:
                {
                    item->handedness = item_handedness_one_handed;
                    item->w.damage = 1;
                    item->w.accuracy = 1;
                    item->w.attack_speed = 0.5f;
                    
                    if(rarity == item_rarity_common)
                    {
                        strcpy(item->name, "Dagger");
                        item->tile = V2u(11, 0);
                        item->enchantment_level = random_number(&game->random, -2, 2);
                    }
                    else if(rarity == item_rarity_magical)
                    {
                        strcpy(item->name, "Dagger");
                        item->tile = V2u(11, 1);
                        item->secondary_damage_type = random_item_damage_type(game);
                        item->enchantment_level = random_number(&game->random, -2, 4);
                    }
                    else
                    {
                        random_name(&game->random, item->name, name_type_item);
                        item->tile = V2u(11, 2);
                        item->secondary_damage_type = random_item_damage_type(game);
                        item->enchantment_level = random_number(&game->random, -4, 8);
                        
                        // TODO(rami): Extra stats for mythical items.
                        item->extra_stat_count = random_number(&game->random, 1, 4);
                    }
                } break;
                
                case item_sword:
                {
                    item->handedness = item_handedness_one_handed;
                    item->w.damage = 6;
                    item->w.accuracy = 0;
                    
                    if(rarity == item_rarity_common)
                    {
                        strcpy(item->name, "Sword");
                        item->tile = V2u(12, 0);
                        item->enchantment_level = random_number(&game->random, -2, 2);
                    }
                    else if(rarity == item_rarity_magical)
                    {
                        strcpy(item->name, "Sword");
                        item->tile = V2u(12, 1);
                        item->secondary_damage_type = random_item_damage_type(game);
                        item->enchantment_level = random_number(&game->random, -2, 4);
                    }
                    else if(rarity == item_rarity_mythical)
                    {
                        random_name(&game->random, item->name, name_type_item);
                        item->tile = V2u(12, 2);
                        item->secondary_damage_type = random_item_damage_type(game);
                        item->enchantment_level = random_number(&game->random, -4, 8);
                        
                        // TODO(rami): Extra stats for mythical items.
                        item->extra_stat_count = random_number(&game->random, 1, 4);
                    }
                } break;
                
                case item_scimitar:
                {
                    item->handedness = item_handedness_one_handed;
                    item->w.damage = 6;
                    item->w.accuracy = 0;
                    
                    if(rarity == item_rarity_common)
                    {
                        strcpy(item->name, "Scimitar");
                        item->tile = V2u(13, 0);
                        item->enchantment_level = random_number(&game->random, -2, 2);
                    }
                    else if(rarity == item_rarity_magical)
                    {
                        strcpy(item->name, "Scimitar");
                        item->tile = V2u(13, 1);
                        item->secondary_damage_type = random_item_damage_type(game);
                        item->enchantment_level = random_number(&game->random, -2, 4);
                    }
                    else if(rarity == item_rarity_mythical)
                    {
                        random_name(&game->random, item->name, name_type_item);
                        item->tile = V2u(13, 2);
                        item->secondary_damage_type = random_item_damage_type(game);
                        item->enchantment_level = random_number(&game->random, -4, 8);
                        
                        // TODO(rami): Extra stats for mythical items.
                        item->extra_stat_count = random_number(&game->random, 1, 4);
                    }
                } break;
                
                case item_club:
                {
                    item->handedness = item_handedness_one_handed;
                    item->w.damage = 6;
                    item->w.accuracy = 0;
                    
                    if(rarity == item_rarity_common)
                    {
                        strcpy(item->name, "Club");
                        item->tile = V2u(14, 0);
                        item->enchantment_level = random_number(&game->random, -2, 2);
                    }
                    else if(rarity == item_rarity_magical)
                    {
                        strcpy(item->name, "Club");
                        item->tile = V2u(14, 1);
                        item->secondary_damage_type = random_item_damage_type(game);
                        item->enchantment_level = random_number(&game->random, -2, 4);
                    }
                    else if(rarity == item_rarity_mythical)
                    {
                        random_name(&game->random, item->name, name_type_item);
                        item->tile = V2u(14, 2);
                        item->secondary_damage_type = random_item_damage_type(game);
                        item->enchantment_level = random_number(&game->random, -4, 8);
                        
                        // TODO(rami): Extra stats for mythical items.
                        item->extra_stat_count = random_number(&game->random, 1, 4);
                    }
                } break;
                
                case item_morningstar:
                {
                    item->handedness = item_handedness_one_handed;
                    item->w.damage = 6;
                    item->w.accuracy = 0;
                    
                    if(rarity == item_rarity_common)
                    {
                        strcpy(item->name, "Morningstar");
                        item->tile = V2u(15, 0);
                        item->enchantment_level = random_number(&game->random, -2, 2);
                    }
                    else if(rarity == item_rarity_magical)
                    {
                        strcpy(item->name, "Morningstar");
                        item->tile = V2u(15, 1);
                        item->secondary_damage_type = random_item_damage_type(game);
                        item->enchantment_level = random_number(&game->random, -2, 4);
                    }
                    else if(rarity == item_rarity_mythical)
                    {
                        random_name(&game->random, item->name, name_type_item);
                        item->tile = V2u(15, 2);
                        item->secondary_damage_type = random_item_damage_type(game);
                        item->enchantment_level = random_number(&game->random, -4, 8);
                        
                        // TODO(rami): Extra stats for mythical items.
                        item->extra_stat_count = random_number(&game->random, 1, 4);
                    }
                } break;
                
                invalid_default_case;
            }
            
            return;
        }
    }
    
    assert(false);
}

internal void
add_consumable_item(item id, u32 x, u32 y, item_t *items, consumable_data_t *cdata)
{
    for(u32 item_index = 0;
        item_index < MAX_ITEMS;
        ++item_index)
    {
        item_t *item = &items[item_index];
        if(!item->id)
        {
            item->id = id;
            item->pos = V2u(x, y);
            item->rarity = item_rarity_common;
            
            switch(id)
            {
                case item_potion_of_might:
                {
                    strcpy(item->name, "Potion of Might");
                    strcpy(item->description, "Potion Description");
                    item->tile = cdata->tiles[consumable_might];
                    item->type = item_type_potion;
                    item->c.effect = item_effect_might;
                    item->c.effect_amount = 0;
                    item->is_identified = cdata->is_known[consumable_might];
                } break;
                
                case item_potion_of_wisdom:
                {
                    strcpy(item->name, "Potion of Wisdom");
                    strcpy(item->description, "Potion Description");
                    item->tile = cdata->tiles[consumable_wisdom];
                    item->type = item_type_potion;
                    item->c.effect = item_effect_wisdom;
                    item->c.effect_amount = 0;
                    item->is_identified = cdata->is_known[consumable_wisdom];
                } break;
                
                case item_potion_of_agility:
                {
                    strcpy(item->name, "Potion of Agility");
                    strcpy(item->description, "Potion Description");
                    item->tile = cdata->tiles[consumable_agility];
                    item->type = item_type_potion;
                    item->c.effect = item_effect_agility;
                    item->c.effect_amount = 0;
                    item->is_identified = cdata->is_known[consumable_agility];
                } break;
                
                case item_potion_of_awareness:
                {
                    strcpy(item->name, "Potion of Awareness");
                    strcpy(item->description, "Potion Description");
                    item->tile = cdata->tiles[consumable_awareness];
                    item->type = item_type_potion;
                    item->c.effect = item_effect_awareness;
                    item->c.effect_amount = 0;
                    item->is_identified = cdata->is_known[consumable_awareness];
                } break;
                
                case item_potion_of_fortitude:
                {
                    strcpy(item->name, "Potion of Fortitude");
                    strcpy(item->description, "Potion Description");
                    item->tile = cdata->tiles[consumable_fortitude];
                    item->type = item_type_potion;
                    item->c.effect = item_effect_fortitude;
                    item->c.effect_amount = 0;
                    item->is_identified = cdata->is_known[consumable_fortitude];
                } break;
                
                case item_potion_of_resistance:
                {
                    strcpy(item->name, "Potion of Resistance");
                    strcpy(item->description, "Potion Description");
                    item->tile = cdata->tiles[consumable_resistance];
                    item->type = item_type_potion;
                    item->c.effect = item_effect_resistance;
                    item->c.effect_amount = 0;
                    item->is_identified = cdata->is_known[consumable_resistance];
                } break;
                
                case item_potion_of_healing:
                {
                    strcpy(item->name, "Potion of Healing");
                    strcpy(item->description, "Potion Description");
                    item->tile = cdata->tiles[consumable_healing];
                    item->type = item_type_potion;
                    item->c.effect = item_effect_healing;
                    item->c.effect_amount = 0;
                    item->is_identified = cdata->is_known[consumable_healing];
                } break;
                
                case item_potion_of_haste:
                {
                    strcpy(item->name, "Potion of Haste");
                    strcpy(item->description, "Potion Description");
                    item->tile = cdata->tiles[consumable_haste];
                    item->type = item_type_potion;
                    item->c.effect = item_effect_haste;
                    item->c.effect_amount = 0;
                    item->is_identified = cdata->is_known[consumable_haste];
                } break;
                
                case item_potion_of_curing:
                {
                    strcpy(item->name, "Potion of Curing");
                    strcpy(item->description, "Potion Description");
                    item->tile = cdata->tiles[consumable_curing];
                    item->type = item_type_potion;
                    item->c.effect = item_effect_curing;
                    item->c.effect_amount = 0;
                    item->is_identified = cdata->is_known[consumable_curing];
                } break;
                
                case item_potion_of_vulnerability:
                {
                    strcpy(item->name, "Potion of Vulnerability");
                    strcpy(item->description, "Potion Description");
                    item->tile = cdata->tiles[consumable_vulnerability];
                    item->type = item_type_potion;
                    item->c.effect = item_effect_vulnerability;
                    item->c.effect_amount = 0;
                    item->is_identified = cdata->is_known[consumable_vulnerability];
                } break;
                
                case item_potion_of_clumsiness:
                {
                    strcpy(item->name, "Potion of Clumsiness");
                    strcpy(item->description, "Potion Description");
                    item->tile = cdata->tiles[consumable_clumsiness];
                    item->type = item_type_potion;
                    item->c.effect = item_effect_clumsiness;
                    item->c.effect_amount = 0;
                    item->is_identified = cdata->is_known[consumable_clumsiness];
                } break;
                
                case item_potion_of_poison:
                {
                    strcpy(item->name, "Potion of Poison");
                    strcpy(item->description, "Potion Description");
                    item->tile = cdata->tiles[consumable_poison];
                    item->type = item_type_potion;
                    item->c.effect = item_effect_poison;
                    item->c.effect_amount = 0;
                    item->is_identified = cdata->is_known[consumable_poison];
                } break;
                
                case item_potion_of_weakness:
                {
                    strcpy(item->name, "Potion of Weakness");
                    strcpy(item->description, "Potion Description");
                    item->tile = cdata->tiles[consumable_weakness];
                    item->type = item_type_potion;
                    item->c.effect = item_effect_weakness;
                    item->c.effect_amount = 0;
                    item->is_identified = cdata->is_known[consumable_weakness];
                } break;
                
                case item_potion_of_flight:
                {
                    strcpy(item->name, "Potion of Flight");
                    strcpy(item->description, "Potion Description");
                    item->tile = cdata->tiles[consumable_flight];
                    item->type = item_type_potion;
                    item->c.effect = item_effect_flight;
                    item->c.effect_amount = 0;
                    item->is_identified = cdata->is_known[consumable_flight];
                } break;
                
                case item_scroll_of_identify:
                {
                    strcpy(item->name, "Scroll of Identify");
                    strcpy(item->description, "Scroll Description");
                    item->tile = cdata->tiles[consumable_identify];
                    item->type = item_type_scroll;
                    item->c.effect = item_effect_identify;
                    item->is_identified = cdata->is_known[consumable_identify];
                } break;
                
                case item_scroll_of_infuse_weapon:
                {
                    strcpy(item->name, "Scroll of Infuse Weapon");
                    strcpy(item->description, "Scroll Description");
                    item->tile = cdata->tiles[consumable_infuse_weapon];
                    item->type = item_type_scroll;
                    item->c.effect = item_effect_infuse_weapon;
                    item->is_identified = cdata->is_known[consumable_infuse_weapon];
                } break;
                
                case item_scroll_of_enchant_weapon:
                {
                    strcpy(item->name, "Scroll of Enchant Weapon");
                    strcpy(item->description, "Scroll Description");
                    item->tile = cdata->tiles[consumable_enchant_weapon];
                    item->type = item_type_scroll;
                    item->c.effect = item_effect_enchant_weapon;
                    item->is_identified = cdata->is_known[consumable_enchant_weapon];
                } break;
                
                case item_scroll_of_enchant_armor:
                {
                    strcpy(item->name, "Scroll of Enchant Armor");
                    strcpy(item->description, "Scroll Description");
                    item->tile = cdata->tiles[consumable_enchant_armor];
                    item->type = item_type_scroll;
                    item->c.effect = item_effect_enchant_armor;
                    item->is_identified = cdata->is_known[consumable_enchant_armor];
                } break;
                
                case item_scroll_of_magic_mapping:
                {
                    strcpy(item->name, "Scroll of Magic Mapping");
                    strcpy(item->description, "Scroll Description");
                    item->tile = cdata->tiles[consumable_magic_mapping];
                    item->type = item_type_scroll;
                    item->c.effect = item_effect_magic_mapping;
                    item->is_identified = cdata->is_known[consumable_magic_mapping];
                } break;
                
                case item_scroll_of_teleportation:
                {
                    strcpy(item->name, "Scroll of Teleportation");
                    strcpy(item->description, "Scroll Description");
                    item->tile = cdata->tiles[consumable_teleportation];
                    item->type = item_type_scroll;
                    item->c.effect = item_effect_teleportation;
                    item->is_identified = cdata->is_known[consumable_teleportation];
                } break;
                
                invalid_default_case;
            }
            
            return;
        }
    }
    
    assert(false);
}