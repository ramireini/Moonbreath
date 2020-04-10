
// TODO(rami): important: Since there's no item info,
// this will be if'd out for now..
#if 0
internal item_info_t *
item_info_from_item_index(u32 index)
{
    item_info_t *result = &item_information[items[index].id - 1];
    return(result);
}

internal item_info_t *
item_info_from_slot_index(u32 index)
{
    item_info_t *result = &item_information[inventory.slots[index]->id - 1];
    return(result);
}
#endif

internal char *
get_item_rarity_control_code(item_rarity rarity)
{
    char *result = 0;
    
    switch(rarity)
    {
        case item_rarity_common: result = "##3 "; break;
        case item_rarity_magical: result = "##9 "; break;
        case item_rarity_mythical: result = "##E "; break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal char *
get_item_bonus_damage_type_text(item_bonus_damage_type type)
{
    char *result = 0;
    
    switch(type)
    {
        case item_bonus_damage_type_none: break;
        
        case item_bonus_damage_type_fire: result = "of Fire"; break;
        case item_bonus_damage_type_ice: result = "of Ice"; break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal string_t
get_full_item_name(item_t *item)
{
    string_t result = {0};
    
    sprintf(result.str, "%c%d %s %s",
            (item->enchantment_level >= 0) ? '+' : '-',
            abs(item->enchantment_level),
            item->name,
            get_item_bonus_damage_type_text(item->bonus_damage_type));
    
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

// TODO(rami): When you add items stats we need to also take care of
// shield weight.
#if 0
internal void
add_item_stats(u32 item_info_index)
{
    item_info_t *item_info = &item_information[item_info_index];
    
    if(item_info->stats.strength)
    {
        player.strength += item_info->stats.strength;
    }
    
    if(item_info->stats.defence)
    {
        player.defence += item_info->stats.defence;
    }
    
    if(item_info->stats.vitality)
    {
        player.max_hp += item_info->stats.vitality;
    }
}

internal void
remove_item_stats(u32 item_info_index)
{
    item_info_t *item_info = &item_information[item_info_index];
    
    if(item_info->stats.strength)
    {
        player.strength -= item_info->stats.strength;
    }
    
    if(item_info->stats.defence)
    {
        player.defence -= item_info->stats.defence;
    }
    
    if(item_info->stats.vitality)
    {
        player.max_hp -= item_info->stats.vitality;
    }
}
#endif

internal void
move_item()
{
    inventory.moving_item_dest_index = index_from_v2u(inventory.current, inventory.w);
    
    if(inventory.is_item_being_moved)
    {
        if(inventory.moving_item_src_index != inventory.moving_item_dest_index)
        {
            if(inventory.slots[inventory.moving_item_dest_index])
            {
                item_t *temp = inventory.slots[inventory.moving_item_dest_index];
                
                inventory.slots[inventory.moving_item_dest_index] = inventory.slots[inventory.moving_item_src_index];
                inventory.slots[inventory.moving_item_src_index] = temp;
            }
            else
            {
                inventory.slots[inventory.moving_item_dest_index] = inventory.slots[inventory.moving_item_src_index];
                inventory.slots[inventory.moving_item_src_index] = 0;
            }
            
            inventory.is_item_being_moved = false;
        }
        
        inventory.is_item_being_moved = false;
        inventory.moving_item_src_index = (u32)-1;
        inventory.moving_item_dest_index = (u32)-1;
    }
    else
    {
        if(inventory.slots[inventory.moving_item_dest_index])
        {
            inventory.is_item_being_moved = true;
            inventory.moving_item_src_index = inventory.moving_item_dest_index;
        }
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
        item->pos = player.pos;
        
        if(print_drop)
        {
            string_t full_item_name = get_full_item_name(item);
            add_log_string("You drop the %s%s.", get_item_rarity_control_code(item->rarity), full_item_name.str);
        }
        
#if 0
        if(item->is_equipped)
        {
            remove_item_stats(item_info_index);
        }
#endif
        
        inventory.slots[slot_index] = 0;
    }
}

internal void
remove_game_item(item_t *item)
{
    item_t empty_item = {0};
    empty_item.unique_id = item->unique_id;
    
    *item = empty_item;
}

internal void
equip_item(item_t *item)
{
    item->is_equipped = true;
    
    string_t full_item_name = get_full_item_name(item);
    add_log_string("You equip the %s%s.", get_item_rarity_control_code(item->rarity), full_item_name.str);
}

internal void
unequip_item(item_t *item)
{
    item->is_equipped = false;
    
    string_t full_item_name = get_full_item_name(item);
    add_log_string("You unequip the %s%s.", get_item_rarity_control_code(item->rarity), full_item_name.str);
}

// TODO(rami): important: Since there's no item info,
// this will be if'd out for now..
#if 0
internal u32
add_item_info(u32 info_index,
              char *name,
              char *description,
              item_slot slot,
              item_type type,
              handedness_t handedness,
              u32 tile_x,
              u32 tile_y,
              u32 damage,
              s32 accuracy,
              u32 defence,
              u32 weight,
              consume_effect_t effect,
              u32 effect_amount)
{
    assert(info_index < array_count(item_information));
    item_info_t *item_info = &item_information[info_index];
    
    item_info->id = ++info_index;
    strcpy(item_info->name, name);
    strcpy(item_info->description, description);
    item_info->slot = slot;
    item_info->type = type;
    item_info->handedness = handedness;
    item_info->tile = V2u(tile_x, tile_y);
    item_info->damage = damage;
    item_info->accuracy = accuracy;
    item_info->defence = defence;
    item_info->weight = weight;
    item_info->consume_effect = effect;
    item_info->effect_amount = effect_amount;
    
    return(info_index);
}
#endif

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
            
            switch(id)
            {
                case item_dagger:
                {
                    strcpy(item->name, "Dagger");
                    item->rarity = rarity;
                    item->handedness = item_handedness_one_handed;
                    
                    if(rarity == item_rarity_common)
                    {
                        item->tile = V2u(11, 0);
                        item->enchantment_level = random_number(0, 0);
                        item->damage = 4;
                        item->accuracy = 1;
                    }
                    else if(rarity == item_rarity_magical)
                    {
                        item->tile = V2u(11, 1);
                        item->bonus_damage_type = item_bonus_damage_type_fire;
                        item->enchantment_level = random_number(1, 1);
                        item->damage = 6;
                        item->accuracy = 1;
                    }
                    else if(rarity == item_rarity_mythical)
                    {
                        item->tile = V2u(11, 2);
                        item->bonus_damage_type = item_bonus_damage_type_ice;
                        item->enchantment_level = random_number(2, 2);
                        item->damage = 8;
                        item->accuracy = 1;
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
            
            switch(id)
            {
                case item_potion_of_might:
                {
                    strcpy(item->name, "Potion of Might");
                    item->tile = V2u(8, 1);
                    item->effect = item_effect_might;
                    item->effect_amount = 0;
                } break;
                
                case item_potion_of_wisdom:
                {
                    strcpy(item->name, "Potion of Wisdom");
                    item->tile = V2u(8, 2);
                    item->effect = item_effect_wisdom;
                    item->effect_amount = 0;
                } break;
                
                case item_potion_of_fortitude:
                {
                    strcpy(item->name, "Potion of Fortitude");
                    item->tile = V2u(8, 3);
                    item->effect = item_effect_fortitude;
                    item->effect_amount = 0;
                } break;
                
                case item_potion_of_agility:
                {
                    strcpy(item->name, "Potion of Agility");
                    item->tile = V2u(8, 4);
                    item->effect = item_effect_agility;
                    item->effect_amount = 0;
                } break;
                
                case item_potion_of_clumsiness:
                {
                    strcpy(item->name, "Potion of Clumsiness");
                    item->tile = V2u(8, 5);
                    item->effect = item_effect_clumsiness;
                    item->effect_amount = 0;
                } break;
                
                case item_potion_of_haste:
                {
                    strcpy(item->name, "Potion of Haste");
                    item->tile = V2u(8, 6);
                    item->effect = item_effect_haste;
                    item->effect_amount = 0;
                } break;
                
                case item_potion_of_resistance:
                {
                    strcpy(item->name, "Potion of Resistance");
                    item->tile = V2u(8, 7);
                    item->effect = item_effect_resistance;
                    item->effect_amount = 0;
                } break;
                
                case item_potion_of_mana:
                {
                    strcpy(item->name, "Potion of Mana");
                    item->tile = V2u(8, 8);
                    item->effect = item_effect_mana;
                    item->effect_amount = 0;
                } break;
                
                case item_potion_of_healing:
                {
                    strcpy(item->name, "Potion of Healing");
                    item->tile = V2u(8, 9);
                    item->effect = item_effect_healing;
                    item->effect_amount = 0;
                } break;
                
                case item_potion_of_flight:
                {
                    strcpy(item->name, "Potion of Flight");
                    item->tile = V2u(8, 10);
                    item->effect = item_effect_flight;
                    item->effect_amount = 0;
                } break;
                
                case item_potion_of_knowledge:
                {
                    strcpy(item->name, "Potion of Knowledge");
                    item->tile = V2u(8, 11);
                    item->effect = item_effect_knowledge;
                    item->effect_amount = 0;
                } break;
                
                case item_potion_of_poison:
                {
                    strcpy(item->name, "Potion of Poison");
                    item->tile = V2u(8, 12);
                    item->effect = item_effect_poison;
                    item->effect_amount = 0;
                } break;
                
                case item_potion_of_curing:
                {
                    strcpy(item->name, "Potion of Curing");
                    item->tile = V2u(8, 13);
                    item->effect = item_effect_curing;
                    item->effect_amount = 0;
                } break;
                
                case item_potion_of_vulnerability:
                {
                    strcpy(item->name, "Potion of Vulnerability");
                    item->tile = V2u(8, 14);
                    item->effect = item_effect_vulnerability;
                    item->effect_amount = 0;
                } break;
                
                case item_scroll_of_identify:
                {
                    strcpy(item->name, "Scroll of Identify");
                    item->tile = V2u(9, 1);
                    item->effect = item_effect_identify;
                } break;
                
                case item_scroll_of_brand_weapon:
                {
                    strcpy(item->name, "Scroll of Brand Weapon");
                    item->tile = V2u(9, 2);
                    item->effect = item_effect_brand_weapon;
                } break;
                
                case item_scroll_of_enchant_weapon:
                {
                    strcpy(item->name, "Scroll of Enchant Weapon");
                    item->tile = V2u(9, 3);
                    item->effect = item_effect_enchant_weapon;
                } break;
                
                case item_scroll_of_enchant_armor:
                {
                    strcpy(item->name, "Scroll of Enchant Armor");
                    item->tile = V2u(9, 4);
                    item->effect = item_effect_enchant_armor;
                } break;
                
                case item_scroll_of_magic_mapping:
                {
                    strcpy(item->name, "Scroll of Magic Mapping");
                    item->tile = V2u(9, 5);
                    item->effect = item_effect_magic_mapping;
                } break;
                
                invalid_default_case;
            }
            
            return;
        }
    }
    
    assert(false);
}

// TODO(rami): important:
internal void
add_item(item_id id, u32 x, u32 y)
{
    for(u32 item_index = 0;
        item_index < array_count(items);
        ++item_index)
    {
        item_t *item = &items[item_index];
        if(!item->id)
        {
            item->id = id;
            item->in_inventory = false;
            item->is_equipped = false;
            item->pos = V2u(x, y);
            
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
        if(item->id && !item->in_inventory && V2u_equal(item->pos, player.pos))
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
                    add_log_string("You pick up the %s%s.", get_item_rarity_control_code(item->rarity), full_item_name.str);
                    
                    return;
                }
            }
            
            add_log_string("Your inventory is full right now.");
            return;
        }
    }
    
    add_log_string("You find nothing to pick up.");
}