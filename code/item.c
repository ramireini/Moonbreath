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
            item_info_t *info = item_info_from_slot_index(slot_index);
            if(info->slot == slot && item->is_equipped)
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
            item_info_t *info = item_info_from_item_index(item_index);
            v4u src = {tile_mul(info->tile.x), tile_mul(info->tile.y), 32, 32};
            
            v2u pos = get_game_pos(item->pos);
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
            item_info_t *info = item_info_from_slot_index(slot_index);
            add_log_message("You drop the %c%u %s.", color_white, (item->enchantment_level >= 0) ? '+' : '-', abs(item->enchantment_level), info->name);
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
consume_item()
{
    u32 slot_index = index_from_v2u(inventory.current, inventory.w);
    item_t *item = inventory.slots[slot_index];
    if(item)
    {
        item_info_t *info = item_info_from_slot_index(slot_index);
        if(item->in_inventory && info->type == type_consumable)
        {
            if(info->consume_effect == effect_healing)
            {
                if(heal_player(info->effect_amount))
                {
                    add_log_message("The potion heals you for %d hitpoints.", color_green, info->effect_amount);
                    
                    remove_inventory_item(0);
                    remove_game_item(item);
                }
                else
                {
                    add_log_message("You do not feel the need to drink this.", color_white);
                }
            }
        }
    }
}

internal void
equip_item(item_t *item, item_info_t *info)
{
    item->is_equipped = true;
    add_log_message("You equip the %c%u %s.", color_white, (item->enchantment_level >= 0) ? '+' : '-', abs(item->enchantment_level), info->name);
}

internal void
unequip_item(item_t *item, item_info_t *info)
{
    item->is_equipped = false;
    add_log_message("You unequip the %c%u %s.", color_white, (item->enchantment_level >= 0) ? '+' : '-', abs(item->enchantment_level), info->name);
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
                    
                    item_info_t *info = item_info_from_item_index(item_index);
                    add_log_message("You pick up the %c%u %s.", color_white, (item->enchantment_level >= 0) ? '+' : '-', abs(item->enchantment_level), info->name);
                    return;
                }
            }
            
            add_log_message("Your inventory is full right now.", color_white);
            return;
        }
    }
    
    add_log_message("You find nothing to pick up.", color_white);
}