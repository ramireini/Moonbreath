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
    // NOTE(rami): Skips physical damage type.
    item_damage_type result = random_number(&game->random,
                                            item_damage_type_none + 2,
                                            item_damage_type_count - 1);
    return(result);
}

internal char *
item_id_text(item_t *item)
{
    char *result = 0;
    
    if(item->type == item_type_potion)
    {
        result = "Potion";
    }
    else if(item->type == item_type_scroll)
    {
        result = "Scroll";
    }
    else
    {
        switch(item->id)
        {
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
            case item_trident: result = "Trident"; break;
            case item_halberd: result = "Halberd"; break;
            
            invalid_default_case;
        }
    }
    
    return(result);
}

internal char *
item_rarity_text(item_t *item)
{
    char *result = "";
    
    if(item->type == item_type_weapon ||
       item->type == item_type_armor)
    {
        switch(item->rarity)
        {
            case item_rarity_common: result = "Common "; break;
            case item_rarity_magical: result = "Magical "; break;
            case item_rarity_mythical: result = "Mythical "; break;
        }
    }
    
    return(result);
}

internal char *
item_handedness_text(item_t *item)
{
    char *result = "";
    
    if(item->type == item_type_weapon ||
       item->type == item_type_armor)
    {
        switch(item->handedness)
        {
            case item_handedness_one_handed: result = "One-Handed"; break;
            case item_handedness_two_handed: result = "Two-Handed"; break;
            
            invalid_default_case;
        }
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
        case item_rarity_common: result = "##1 "; break;
        case item_rarity_magical: result = "##9 "; break;
        case item_rarity_mythical: result = "##E "; break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal string_t
full_item_name(item_t *item)
{
    string_t result = {0};
    
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

internal u32_bool_t
equipped_item_slot_index(item_slot slot, inventory_t *inventory)
{
    u32_bool_t result = {0};
    
    for(u32 slot_index = 0;
        slot_index < INVENTORY_AREA;
        ++slot_index)
    {
        item_t *item = inventory->slots[slot_index];
        if(item)
        {
            if(item->slot == slot && item->is_equipped)
            {
                result.success = true;
                result.value = slot_index;
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
        if(item->id && !item->in_inventory && is_seen(dungeon, item->pos))
        {
            v4u src = tile_rect(item->tile);
            v4u dest = game_dest(game, item->pos);
            SDL_RenderCopy(game->renderer, assets->item_tileset.tex, (SDL_Rect *)&src, (SDL_Rect *)&dest);
            
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
        // NOTE(rami): Base player damage, accuracy and attack speed.
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
remove_inventory_item(b32 print_drop, entity_t *player, string_t *log, inventory_t *inventory)
{
    u32 slot_index = index_from_v2u(inventory->current, INVENTORY_WIDTH);
    item_t *item = inventory->slots[slot_index];
    if(item)
    {
        item->in_inventory = false;
        item->is_equipped = false;
        item->pos = player->pos;
        
        if(print_drop)
        {
            if(item->is_identified)
            {
                string_t item_name = full_item_name(item);
                add_log_string(log, "You drop the %s%s", item_rarity_color_code(item->rarity), item_name.str);
            }
            else
            {
                add_log_string(log, "You drop the %s%s", item_rarity_color_code(item->rarity), item_id_text(item));
            }
        }
        
        if(item->is_equipped)
        {
            remove_item_stats(item, player);
        }
        
        inventory->slots[slot_index] = 0;
    }
}

internal void
remove_game_item(item_t *item)
{
    memset(item, 0, sizeof(item_t));
}

internal void
equip_item(item_t *item, entity_t *player, string_t *log)
{
    item->is_equipped = true;
    add_item_stats(item, player);
    
    string_t item_name = full_item_name(item);
    add_log_string(log, "You equip the %s%s", item_rarity_color_code(item->rarity), item_name.str);
}

internal void
unequip_item(item_t *item, entity_t *player, string_t *log)
{
    item->is_equipped = false;
    remove_item_stats(item, player);
    
    string_t item_name = full_item_name(item);
    add_log_string(log, "You unequip the %s%s", item_rarity_color_code(item->rarity), item_name.str);
}

internal void
add_weapon_item(game_state_t *game, item_t *items, item id, item_rarity rarity, u32 x, u32 y)
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
            item->slot = item_slot_first_hand;
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
add_potion_item(item_t *items, item id, u32 x, u32 y)
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
                    item->tile = V2u(8, 1);
                    item->type = item_type_potion;
                    item->c.effect = item_effect_might;
                    item->c.effect_amount = 0;
                } break;
                
                case item_potion_of_wisdom:
                {
                    strcpy(item->name, "Potion of Wisdom");
                    strcpy(item->description, "Potion Description");
                    item->tile = V2u(8, 2);
                    item->type = item_type_potion;
                    item->c.effect = item_effect_wisdom;
                    item->c.effect_amount = 0;
                } break;
                
                case item_potion_of_agility:
                {
                    strcpy(item->name, "Potion of Agility");
                    strcpy(item->description, "Potion Description");
                    item->tile = V2u(8, 3);
                    item->type = item_type_potion;
                    item->c.effect = item_effect_agility;
                    item->c.effect_amount = 0;
                } break;
                
                case item_potion_of_awareness:
                {
                    strcpy(item->name, "Potion of Awareness");
                    strcpy(item->description, "Potion Description");
                    item->tile = V2u(8, 4);
                    item->type = item_type_potion;
                    item->c.effect = item_effect_awareness;
                    item->c.effect_amount = 0;
                } break;
                
                case item_potion_of_fortitude:
                {
                    strcpy(item->name, "Potion of Fortitude");
                    strcpy(item->description, "Potion Description");
                    item->tile = V2u(8, 5);
                    item->type = item_type_potion;
                    item->c.effect = item_effect_fortitude;
                    item->c.effect_amount = 0;
                } break;
                
                case item_potion_of_resistance:
                {
                    strcpy(item->name, "Potion of resistance");
                    strcpy(item->description, "Potion Description");
                    item->tile = V2u(8, 6);
                    item->type = item_type_potion;
                    item->c.effect = item_effect_resistance;
                    item->c.effect_amount = 0;
                } break;
                
                case item_potion_of_healing:
                {
                    strcpy(item->name, "Potion of Healing");
                    strcpy(item->description, "Potion Description");
                    item->tile = V2u(8, 7);
                    item->type = item_type_potion;
                    item->c.effect = item_effect_healing;
                    item->c.effect_amount = 0;
                } break;
                
                case item_potion_of_haste:
                {
                    strcpy(item->name, "Potion of Haste");
                    strcpy(item->description, "Potion Description");
                    item->tile = V2u(8, 8);
                    item->type = item_type_potion;
                    item->c.effect = item_effect_haste;
                    item->c.effect_amount = 0;
                } break;
                
                case item_potion_of_curing:
                {
                    strcpy(item->name, "Potion of Curing");
                    strcpy(item->description, "Potion Description");
                    item->tile = V2u(8, 9);
                    item->type = item_type_potion;
                    item->c.effect = item_effect_curing;
                    item->c.effect_amount = 4;
                } break;
                
                case item_potion_of_vulnerability:
                {
                    strcpy(item->name, "Potion of Vulnerability");
                    strcpy(item->description, "Potion Description");
                    item->tile = V2u(8, 10);
                    item->type = item_type_potion;
                    item->c.effect = item_effect_vulnerability;
                    item->c.effect_amount = 0;
                } break;
                
                case item_potion_of_clumsiness:
                {
                    strcpy(item->name, "Potion of Clumsiness");
                    strcpy(item->description, "Potion Description");
                    item->tile = V2u(8, 11);
                    item->type = item_type_potion;
                    item->c.effect = item_effect_clumsiness;
                    item->c.effect_amount = 0;
                } break;
                
                case item_potion_of_poison:
                {
                    strcpy(item->name, "Potion of Poison");
                    strcpy(item->description, "Potion Description");
                    item->tile = V2u(8, 12);
                    item->type = item_type_potion;
                    item->c.effect = item_effect_poison;
                    item->c.effect_amount = 0;
                } break;
                
                case item_potion_of_weakness:
                {
                    strcpy(item->name, "Potion of Weakness");
                    strcpy(item->description, "Potion Description");
                    item->tile = V2u(8, 13);
                    item->type = item_type_potion;
                    item->c.effect = item_effect_weakness;
                    item->c.effect_amount = 0;
                } break;
                
                case item_potion_of_flight:
                {
                    strcpy(item->name, "Potion of Flight");
                    strcpy(item->description, "Potion Description");
                    item->tile = V2u(8, 14);
                    item->type = item_type_potion;
                    item->c.effect = item_effect_flight;
                    item->c.effect_amount = 0;
                } break;
                
                invalid_default_case;
            }
            
            return;
        }
    }
    
    assert(false);
}

internal void
add_scroll_item(item_t *items, item id, u32 x, u32 y)
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
                case item_scroll_of_identify:
                {
                    strcpy(item->name, "Scroll of Identify");
                    strcpy(item->description, "Scroll Description");
                    item->tile = V2u(9, 1);
                    item->type = item_type_scroll;
                    item->c.effect = item_effect_identify;
                } break;
                
                case item_scroll_of_brand_weapon:
                {
                    strcpy(item->name, "Scroll of Brand Weapon");
                    strcpy(item->description, "Scroll Description");
                    item->tile = V2u(9, 2);
                    item->type = item_type_scroll;
                    item->c.effect = item_effect_brand_weapon;
                } break;
                
                case item_scroll_of_enchant_weapon:
                {
                    strcpy(item->name, "Scroll of Enchant Weapon");
                    strcpy(item->description, "Scroll Description");
                    item->tile = V2u(9, 3);
                    item->type = item_type_scroll;
                    item->c.effect = item_effect_enchant_weapon;
                } break;
                
                case item_scroll_of_enchant_armor:
                {
                    strcpy(item->name, "Scroll of Enchant Armor");
                    strcpy(item->description, "Scroll Description");
                    item->tile = V2u(9, 4);
                    item->type = item_type_scroll;
                    item->c.effect = item_effect_enchant_armor;
                } break;
                
                case item_scroll_of_magic_mapping:
                {
                    strcpy(item->name, "Scroll of Magic Mapping");
                    strcpy(item->description, "Scroll Description");
                    item->tile = V2u(9, 5);
                    item->type = item_type_scroll;
                    item->c.effect = item_effect_magic_mapping;
                } break;
                
                invalid_default_case;
            }
            
            return;
        }
    }
    
    assert(false);
}

internal void
add_inventory_item(item_t *items, inventory_t *inventory, entity_t *player, string_t *log)
{
    for(u32 item_index = 0;
        item_index < MAX_ITEMS;
        ++item_index)
    {
        item_t *item = &items[item_index];
        if(item->id && !item->in_inventory && V2u_equal(item->pos, player->pos))
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
                        string_t item_name = full_item_name(item);
                        add_log_string(log, "You pick up the %s%s", item_rarity_color_code(item->rarity), item_name.str);
                    }
                    else
                    {
                        add_log_string(log, "You pick up the %s%s", item_rarity_color_code(item->rarity), item_id_text(item));
                    }
                    
                    return;
                }
            }
            
            add_log_string(log, "Your inventory is full right now");
            return;
        }
    }
    
    add_log_string(log, "You find nothing to pick up");
}