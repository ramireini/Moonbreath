internal item_damage_type
get_random_item_damage_type()
{
    // NOTE(rami): Skips physical damage type.
    item_damage_type result = random_number(item_damage_type_none + 2,
                                            item_damage_type_count - 1);
    return(result);
}

internal char *
get_item_weapon_id_text(item_t *item)
{
    char *result = 0;
    
    if(item->type == item_type_consumable)
    {
        result = "Consumable";
    }
    else
    {
        switch(item->id)
        {
            case item_dagger: result = "Dagger"; break;
            case item_short_sword: result = "Short Sword"; break;
            case item_long_sword: result = "Long Sword"; break;
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
get_item_rarity_text(item_t *item)
{
    char *result = 0;
    
    if(item->type != item_type_consumable)
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
get_item_handedness_text(item_t *item)
{
    char *result = 0;
    
    if(item->type != item_type_consumable)
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
get_item_damage_type_text(item_damage_type damage_type)
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
get_item_rarity_color_code(item_rarity rarity)
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
get_full_item_name(item_t *item)
{
    string_t result = {0};
    
    if(item->type != item_type_consumable)
    {
        if(item->secondary_damage_type)
        {
            sprintf(result.str, "%c%d %s of %s",
                    (item->enchantment_level >= 0) ? '+' : '-',
                    abs(item->enchantment_level),
                    item->name,
                    get_item_damage_type_text(item->secondary_damage_type));
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

internal u32_t
get_equipped_item_slot_index(item_slot slot)
{
    u32_t result = {0};
    
    for(u32 slot_index = 0;
        slot_index < (inventory.w * inventory.h);
        ++slot_index)
    {
        item_t *item = inventory.slots[slot_index];
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
render_items()
{
    for(u32 item_index = 0;
        item_index < array_count(items);
        ++item_index)
    {
        item_t *item = &items[item_index];
        if(item->id && !item->in_inventory && is_seen(item->pos))
        {
            v2u game_pos = get_game_pos(item->pos);
            
            v4u src = {tile_mul(item->tile.x), tile_mul(item->tile.y), 32, 32};
            v4u dest = {game_pos.x, game_pos.y, 32, 32};
            SDL_RenderCopy(game.renderer, textures.item_tileset.tex, (SDL_Rect *)&src, (SDL_Rect *)&dest);
            
            // TODO(rami): Added a line around items, would like to make this
            // something you can toggle in the future.
#if 1
            set_render_color(color_dark_green);
            SDL_RenderDrawRect(game.renderer, (SDL_Rect *)&dest);
#endif
        }
    }
}

// TODO(rami): These need to handle extra stats.
internal void
add_item_stats(item_t *item)
{
    if(item->type == item_type_weapon)
    {
        player->damage = item->w.damage + item->enchantment_level;
        player->accuracy = item->w.accuracy + item->enchantment_level;
    }
    else if(item->type == item_type_armor)
    {
        player->defence += item->a.defence;
        player->evasion -= item->a.weight;
    }
}

internal void
remove_item_stats(item_t *item)
{
    if(item->type == item_type_weapon)
    {
        // NOTE(rami): Base player damage and accuracy.
        player->damage = 1;
        player->accuracy = 2;
    }
    else if(item->type == item_type_armor)
    {
        player->defence -= item->a.defence;
        player->evasion += item->a.weight;
    }
}

internal void
remove_inventory_item(b32 print_drop)
{
    u32 slot_index = index_from_v2u(inventory.current, inventory.w);
    item_t *item = inventory.slots[slot_index];
    if(item)
    {
        item->in_inventory = false;
        item->is_equipped = false;
        item->pos = player->pos;
        
        if(print_drop)
        {
            string_t full_item_name = get_full_item_name(item);
            add_log_string("You drop the %s%s", get_item_rarity_color_code(item->rarity), full_item_name.str);
        }
        
        if(item->is_equipped)
        {
            remove_item_stats(item);
        }
        
        inventory.slots[slot_index] = 0;
    }
}

internal void
remove_game_item(item_t *item)
{
    memset(item, 0, sizeof(item_t));
}

internal void
equip_item(item_t *item)
{
    item->is_equipped = true;
    add_item_stats(item);
    
    string_t full_item_name = get_full_item_name(item);
    add_log_string("You equip the %s%s", get_item_rarity_color_code(item->rarity), full_item_name.str);
}

internal void
unequip_item(item_t *item)
{
    item->is_equipped = false;
    remove_item_stats(item);
    
    string_t full_item_name = get_full_item_name(item);
    add_log_string("You unequip the %s%s", get_item_rarity_color_code(item->rarity), full_item_name.str);
}

internal void
add_weapon_item(item_id id, item_rarity rarity, u32 x, u32 y)
{
    for(u32 item_index = 0;
        item_index < array_count(items);
        ++item_index)
    {
        item_t *item = &items[item_index];
        if(!item->id)
        {
            item->id = id;
            item->pos = V2u(x, y);
            item->slot = item_slot_main_hand;
            item->type = item_type_weapon;
            item->w.speed = 1.0f;
            
            switch(id)
            {
                case item_dagger:
                {
                    item->rarity = rarity;
                    item->handedness = item_handedness_one_handed;
                    item->primary_damage_type = item_damage_type_physical;
                    item->w.damage = 4;
                    item->w.accuracy = 1;
                    
                    if(rarity == item_rarity_common)
                    {
                        strcpy(item->name, "Dagger");
                        item->tile = V2u(11, 0);
                        item->enchantment_level = random_number(-2, 2);
                    }
                    else if(rarity == item_rarity_magical)
                    {
                        strcpy(item->name, "Dagger");
                        item->tile = V2u(11, 1);
                        item->secondary_damage_type = get_random_item_damage_type();
                        item->enchantment_level = random_number(-2, 4);
                    }
                    else
                    {
                        // TODO(rami): Random mythical items have random names.
                        strcpy(item->name, "Dagger");
                        item->tile = V2u(11, 2);
                        item->secondary_damage_type = get_random_item_damage_type();
                        item->enchantment_level = random_number(-4, 8);
                        
                        // TODO(rami): Extra stats for mythical items.
                        item->extra_stat_count = random_number(1, 5);
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
add_consumable_item(item_id id, u32 x, u32 y)
{
    for(u32 item_index = 0;
        item_index < array_count(items);
        ++item_index)
    {
        item_t *item = &items[item_index];
        if(!item->id)
        {
            item->id = id;
            item->pos = V2u(x, y);
            item->type = item_type_consumable;
            item->rarity = item_rarity_common;
            
            switch(id)
            {
                case item_potion_of_might:
                {
                    strcpy(item->name, "Potion of Might");
                    item->tile = V2u(8, 1);
                    item->c.effect = item_effect_might;
                    item->c.effect_amount = 0;
                } break;
                
                case item_potion_of_wisdom:
                {
                    strcpy(item->name, "Potion of Wisdom");
                    item->tile = V2u(8, 2);
                    item->c.effect = item_effect_wisdom;
                    item->c.effect_amount = 0;
                } break;
                
                case item_potion_of_fortitude:
                {
                    strcpy(item->name, "Potion of Fortitude");
                    item->tile = V2u(8, 3);
                    item->c.effect = item_effect_fortitude;
                    item->c.effect_amount = 0;
                } break;
                
                case item_potion_of_agility:
                {
                    strcpy(item->name, "Potion of Agility");
                    item->tile = V2u(8, 4);
                    item->c.effect = item_effect_agility;
                    item->c.effect_amount = 0;
                } break;
                
                case item_potion_of_clumsiness:
                {
                    strcpy(item->name, "Potion of Clumsiness");
                    item->tile = V2u(8, 5);
                    item->c.effect = item_effect_clumsiness;
                    item->c.effect_amount = 0;
                } break;
                
                case item_potion_of_haste:
                {
                    strcpy(item->name, "Potion of Haste");
                    item->tile = V2u(8, 6);
                    item->c.effect = item_effect_haste;
                    item->c.effect_amount = 0;
                } break;
                
                case item_potion_of_resistance:
                {
                    strcpy(item->name, "Potion of Resistance");
                    item->tile = V2u(8, 7);
                    item->c.effect = item_effect_resistance;
                    item->c.effect_amount = 0;
                } break;
                
                case item_potion_of_mana:
                {
                    strcpy(item->name, "Potion of Mana");
                    item->tile = V2u(8, 8);
                    item->c.effect = item_effect_mana;
                    item->c.effect_amount = 0;
                } break;
                
                case item_potion_of_healing:
                {
                    strcpy(item->name, "Potion of Healing");
                    item->tile = V2u(8, 9);
                    item->c.effect = item_effect_healing;
                    item->c.effect_amount = 4;
                } break;
                
                case item_potion_of_flight:
                {
                    strcpy(item->name, "Potion of Flight");
                    item->tile = V2u(8, 10);
                    item->c.effect = item_effect_flight;
                    item->c.effect_amount = 0;
                } break;
                
                case item_potion_of_knowledge:
                {
                    strcpy(item->name, "Potion of Knowledge");
                    item->tile = V2u(8, 11);
                    item->c.effect = item_effect_knowledge;
                    item->c.effect_amount = 0;
                } break;
                
                case item_potion_of_poison:
                {
                    strcpy(item->name, "Potion of Poison");
                    item->tile = V2u(8, 12);
                    item->c.effect = item_effect_poison;
                    item->c.effect_amount = 0;
                } break;
                
                case item_potion_of_curing:
                {
                    strcpy(item->name, "Potion of Curing");
                    item->tile = V2u(8, 13);
                    item->c.effect = item_effect_curing;
                    item->c.effect_amount = 0;
                } break;
                
                case item_potion_of_vulnerability:
                {
                    strcpy(item->name, "Potion of Vulnerability");
                    item->tile = V2u(8, 14);
                    item->c.effect = item_effect_vulnerability;
                    item->c.effect_amount = 0;
                } break;
                
                case item_scroll_of_identify:
                {
                    strcpy(item->name, "Scroll of Identify");
                    item->tile = V2u(9, 1);
                    item->c.effect = item_effect_identify;
                } break;
                
                case item_scroll_of_brand_weapon:
                {
                    strcpy(item->name, "Scroll of Brand Weapon");
                    item->tile = V2u(9, 2);
                    item->c.effect = item_effect_brand_weapon;
                } break;
                
                case item_scroll_of_enchant_weapon:
                {
                    strcpy(item->name, "Scroll of Enchant Weapon");
                    item->tile = V2u(9, 3);
                    item->c.effect = item_effect_enchant_weapon;
                } break;
                
                case item_scroll_of_enchant_armor:
                {
                    strcpy(item->name, "Scroll of Enchant Armor");
                    item->tile = V2u(9, 4);
                    item->c.effect = item_effect_enchant_armor;
                } break;
                
                case item_scroll_of_magic_mapping:
                {
                    strcpy(item->name, "Scroll of Magic Mapping");
                    item->tile = V2u(9, 5);
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
add_inventory_item()
{
    for(u32 item_index = 0;
        item_index < array_count(items);
        ++item_index)
    {
        item_t *item = &items[item_index];
        if(item->id && !item->in_inventory && V2u_equal(item->pos, player->pos))
        {
            for(u32 slot_index = 0;
                slot_index < (inventory.w * inventory.h);
                ++slot_index)
            {
                if(!inventory.slots[slot_index])
                {
                    item->in_inventory = true;
                    inventory.slots[slot_index] = item;
                    
                    string_t full_item_name = get_full_item_name(item);
                    add_log_string("You pick up the %s%s", get_item_rarity_color_code(item->rarity), full_item_name.str);
                    return;
                }
            }
            
            add_log_string("Your inventory is full right now");
            return;
        }
    }
    
    add_log_string("You find nothing to pick up");
}