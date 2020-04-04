internal u32
item_info_index_from_item_index(u32 item_index)
{
    u32 result = items[item_index].id - 1;
    return(result);
}

internal u32
item_info_index_from_inventory_index(u32 inventory_index)
{
    u32 result = inventory.slots[inventory_index].id - 1;
    return(result);
}

internal u32_t
get_equipped_item_inventory_index(item_slot slot)
{
    u32_t result = {0};
    
    for(u32 inventory_index = 0;
        inventory_index < (inventory.w * inventory.h);
        ++inventory_index)
    {
        u32 item_info_index = item_info_index_from_inventory_index(inventory_index);
        item_info_t *item_info = &item_information[item_info_index];
        
        if(inventory.slots[inventory_index].id &&
           inventory.slots[inventory_index].is_equipped &&
           item_info->slot == slot)
        {
            result.success = true;
            result.value = inventory_index;
            break;
        }
    }
    
    return(result);
}

internal void
move_item_in_inventory(u32 src_inventory_index, u32 dest_inventory_index)
{
    item_t *dest_inventory_slot = &inventory.slots[dest_inventory_index];
    item_t *src_inventory_slot = &inventory.slots[src_inventory_index];
    
    if(dest_inventory_slot->id)
    {
        item_t temp_slot = *dest_inventory_slot;
        *dest_inventory_slot = *src_inventory_slot;
        *src_inventory_slot = temp_slot;
    }
    else
    {
        *dest_inventory_slot = *src_inventory_slot;
        memset(src_inventory_slot, 0, sizeof(item_t));
    }
    
    inventory.item_is_being_moved = false;
}

internal void
render_items()
{
    for(u32 item_index = 0;
        item_index < array_count(items);
        ++item_index)
    {
        if(items[item_index].id &&
           !items[item_index].in_inventory &&
           is_seen(items[item_index].pos))
        {
            v2u pos = get_game_pos(items[item_index].pos);
            
            u32 item_info_index = item_info_index_from_item_index(item_index);
            v4u src =
            {
                tile_mul(item_information[item_info_index].tile.x),
                tile_mul(item_information[item_info_index].tile.y),
                32,
                32
            };
            
            v4u dest = {pos.x, pos.y, 32, 32};
            SDL_RenderCopy(game.renderer, textures.item_tileset.tex, (SDL_Rect *)&src, (SDL_Rect *)&dest);
            
            // TODO(rami): Added a line around items, would like to make this
            // something you can toggle in the future.
#if 1
            set_render_color(color_green);
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
    if(inventory.item_is_being_moved)
    {
        inventory.moved_item_dest_index = index_from_v2u(inventory.current_slot, inventory.w);
        if(inventory.moved_item_src_index != inventory.moved_item_dest_index)
        {
            move_item_in_inventory(inventory.moved_item_src_index, inventory.moved_item_dest_index);
        }
        
        inventory.item_is_being_moved = false;
        inventory.moved_item_src_index = (u32)-1;
        inventory.moved_item_dest_index = (u32)-1;
    }
    else
    {
        u32 index = index_from_v2u(inventory.current_slot, inventory.w);
        if(inventory.slots[index].id)
        {
            inventory.item_is_being_moved = true;
            inventory.moved_item_src_index = index;
        }
    }
}

internal void
remove_inventory_item(b32 print_drop_text)
{
    if(inventory.item_count)
    {
        for(u32 item_index = 0;
            item_index < array_count(items);
            ++item_index)
        {
            item_t *item = &items[item_index];
            if(item->in_inventory)
            {
                u32 inventory_index = index_from_v2u(inventory.current_slot, inventory.w);
                item_t *inventory_item = &inventory.slots[inventory_index];
                if(inventory_item->unique_id == item->unique_id)
                {
                    item->in_inventory = false;
                    item->is_equipped = false;
                    item->pos = player.pos;
                    
                    u32 item_info_index = item_info_index_from_item_index(item_index);
                    if(inventory_item->is_equipped)
                    {
#if 0
                        remove_item_stats(item_info_index);
#endif
                    }
                    
                    if(print_drop_text)
                    {
                        add_log_message("You drop the %c%u %s.", color_white, (inventory_item->enchantment_level >= 0) ? '+' : '-', abs(inventory_item->enchantment_level), item_information[item_info_index].name);
                    }
                    
                    memset(inventory_item, 0, sizeof(item_t));
                    --inventory.item_count;
                    break;
                }
            }
        }
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
consume_item()
{
    for(u32 item_index = 0;
        item_index < array_count(items);
        ++item_index)
    {
        u32 info_index = item_info_index_from_item_index(item_index);
        if(items[item_index].in_inventory && item_information[info_index].type == type_consumable)
        {
            u32 inventory_index = index_from_v2u(inventory.current_slot, inventory.w);
            if(items[item_index].unique_id == inventory.slots[inventory_index].unique_id)
            {
                // TODO(rami): We need to handle all the effects somehow.
                if(item_information[info_index].consume_effect == effect_healing)
                {
                    if(heal_player(item_information[info_index].effect_amount))
                    {
                        add_log_message("The potion heals you for %d hitpoints.", color_green, item_information[info_index].effect_amount);
                        
                        remove_inventory_item(0);
                        remove_game_item(&items[item_index]);
                    }
                    else
                    {
                        add_log_message("You do not feel the need to drink this.", color_white);
                    }
                }
                
                break;
            }
        }
    }
}

internal u32_t
item_index_from_inventory_index(u32 inventory_index)
{
    u32_t result = {0};
    
    for(u32 item_index = 0;
        item_index < array_count(items);
        ++item_index)
    {
        if(items[item_index].unique_id ==
           inventory.slots[inventory_index].unique_id)
        {
            result.success = true;
            result.value = item_index;
            break;
        }
    }
    
    return(result);
}

internal void
unequip_item(u32 inventory_index)
{
    u32_t item_index = item_index_from_inventory_index(inventory_index);
    assert(item_index.success);
    
    item_t *item = &items[item_index.value];
    item_t *inventory_item = &inventory.slots[inventory_index];
    
    item->is_equipped = false;
    inventory_item->is_equipped = false;
    
    u32 item_info_index = item_info_index_from_item_index(item_index.value);
    add_log_message("You unequip the %c%u %s.", color_white,
                    (inventory_item->enchantment_level >= 0) ? '+' : '-',
                    abs(inventory_item->enchantment_level),
                    item_information[item_info_index].name);
}

internal void
equip_item(u32 inventory_index)
{
    u32_t item_index = item_index_from_inventory_index(inventory_index);
    assert(item_index.success);
    
    item_t *item = &items[item_index.value];
    item_t *inventory_item = &inventory.slots[inventory_index];
    
    item->is_equipped = true;
    inventory_item->is_equipped = true;
    
    u32 item_info_index = item_info_index_from_item_index(item_index.value);
    add_log_message("You equip the %c%u %s.", color_white,
                    (inventory_item->enchantment_level >= 0) ? '+' : '-',
                    abs(inventory_item->enchantment_level),
                    item_information[item_info_index].name);
}

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
        
        if(item->id &&
           !item->in_inventory &&
           V2u_equal(item->pos, player.pos))
        {
            for(u32 inventory_index = 0;
                inventory_index < (inventory.w * inventory.h);
                ++inventory_index)
            {
                item_t *inventory_item = &inventory.slots[inventory_index];
                if(!inventory_item->id)
                {
                    item->in_inventory = true;
                    *inventory_item = *item;
                    
                    u32 item_info_index = item_info_index_from_item_index(item_index);
                    add_log_message("You pick up the %c%u %s.", color_white, (inventory_item->enchantment_level >= 0) ? '+' : '-', abs(inventory_item->enchantment_level), item_information[item_info_index].name);
                    return;
                }
            }
            
            add_log_message("Your inventory is full right now.", color_white);
            return;
        }
    }
    
    add_log_message("You find nothing to pick up.", color_white);
}