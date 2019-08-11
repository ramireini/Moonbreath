internal void
move_item(u32 src_index, u32 dest_index)
{
    item_t *dest_slot = &inventory.slot[dest_index];
    item_t *src_slot = &inventory.slot[src_index];
    
    if(dest_slot->id)
    {
        item_t buffer_slot = *dest_slot;
        *dest_slot = *src_slot;
        *src_slot = buffer_slot;
    }
    else
    {
        *dest_slot = *src_slot;
        memset(src_slot, 0, sizeof(item_t));
    }
    
    inventory.item_is_moving = false;
}

internal void
render_items()
{
    for(u32 i = 0; i < ITEM_COUNT; ++i)
    {
        if(item[i].id && !item[i].in_inventory)
        {
            v2u pos = get_game_position(item[i].pos);
            
            v4u src = V4u(tile_mul(item_info[item[i].id - 1].tile.x),
                          tile_mul(item_info[item[i].id - 1].tile.y),
                          32, 32);
            
            v4u dest = V4u(pos.x, pos.y, 32, 32);
            
            if(is_seen(item[i].pos))
            {
                SDL_RenderCopy(game.renderer, texture[tex_item_tileset],
                               (SDL_Rect *)&src, (SDL_Rect *)&dest);
            }
        }
    }
}

// TODO(rami): Do we want to pick dropped items in the reverse order?
// Or do we want to give a list of items on that spot so you can choose?
// Or something else?
internal void
drop_item(b32 print_drop_message)
{
    if(inventory.item_count)
    {
        for(u32 i = 0; i < ITEM_COUNT; ++i)
        {
            if(item[i].in_inventory)
            {
                u32 inventory_index = get_inventory_pos_index();
                
                if(item[i].unique_id ==
                   inventory.slot[inventory_index].unique_id)
                {
                    item[i].in_inventory = false;
                    item[i].equipped = false;
                    item[i].pos = player.pos;
                    
                    item_t *slot = &inventory.slot[inventory_index];
                    slot->id = 0;
                    slot->unique_id = 0;
                    slot->pos = V2u(0, 0);
                    slot->in_inventory = false;
                    slot->equipped = false;
                    
                    if(print_drop_message)
                    {
                        add_console_message("You drop the %s", color_white,
                                            item_info[item[i].id - 1].name);
                    }
                    
                    --inventory.item_count;
                    break;
                }
            }
        }
    }
    else
    {
        add_console_message("You have nothing to drop", color_white);
    }
}

internal void
remove_item(u32 i)
{
    item[i].id = id_none;
    item[i].in_inventory = false;
    item[i].equipped = false;
    item[i].pos = V2u(0, 0);
}

internal void
consume_item()
{
    for(u32 i = 0; i < ITEM_COUNT; ++i)
    {
        if(item[i].in_inventory &&
           item_info[item[i].id - 1].category == category_consumable)
        {
            u32 inventory_index = get_inventory_pos_index();
            if(item[i].unique_id ==
               inventory.slot[inventory_index].unique_id)
            {
                if(heal_player(item_info[item[i].id - 1].heal_amount))
                {
                    add_console_message("You drink the potion and feel slightly better", color_green);
                    drop_item(0);
                    remove_item(i);
                }
                else
                {
                    add_console_message("You do not feel the need to drink this", color_white);
                }
                
                break;
            }
        }
    }
}

internal void
add_item_stats(u32 item_info_index)
{
    if(item_info[item_info_index].category == category_weapon)
    {
        player.damage += item_info[item_info_index].damage;
    }
    else if(item_info[item_info_index].category == category_armor)
    {
        player.armor += item_info[item_info_index].armor;
    }
}

internal void
remove_item_stats(u32 item_info_index)
{
    if(item_info[item_info_index].category == category_weapon)
    {
        player.damage -= item_info[item_info_index].damage;
    }
    else if(item_info[item_info_index].category == category_armor)
    {
        player.armor -= item_info[item_info_index].armor;
    }
}

internal return_data_t
get_item_index_from_unique_id(u32 unique_id)
{
    return_data_t data = {0};
    
    for(u32 i = 0; i < ITEM_COUNT; ++i)
    {
        if(item[i].unique_id == unique_id)
        {
            data.success = 1;
            data.value = i;
            break;
        }
    }
    
    return(data);
}

internal u32
is_item_slot_occupied(item_slot slot)
{
    u32 result = 0;
    
    for(u32 i = 0; i < INVENTORY_SLOT_COUNT; ++i)
    {
        u32 info_index = inventory.slot[i].id - 1;
        
        if(inventory.slot[i].equipped &&
           item_info[info_index].slot == slot)
        {
            result = 1;
            break;
        }
    }
    
    return(result);
}

internal void
toggle_equipped_item()
{
    for(u32 i = 0; i < ITEM_COUNT; ++i)
    {
        if(item[i].in_inventory &&
           (item_info[item[i].id - 1].category == category_weapon ||
            item_info[item[i].id - 1].category == category_armor))
        {
            u32 inventory_index = get_inventory_pos_index();
            
            if(item[i].unique_id ==
               inventory.slot[inventory_index].unique_id)
            {
                if(item[i].equipped &&
                   inventory.slot[inventory_index].equipped)
                {
                    item[i].equipped = false;
                    inventory.slot[inventory_index].equipped = false;
                    
                    remove_item_stats(item[i].id - 1);
                    add_console_message("You unequip the %s", color_white,
                                        item_info[item[i].id - 1].name);
                }
                else
                {
                    // If the item slot already has something in it,
                    // unequip whatever item is there to make space for the new item
                    item_slot_data_t slot = get_item_equip_slot_data(inventory_index);
                    if(slot.occupied)
                    {
                        return_data_t ret = get_item_index_from_unique_id(inventory.slot[slot.index].unique_id);
                        item[ret.value].equipped = false;
                        inventory.slot[slot.index].equipped = false;
                        
                        remove_item_stats(inventory.slot[slot.index].id - 1);
                    }
                    
                    item[i].equipped = true;
                    inventory.slot[inventory_index].equipped = true;
                    
                    add_item_stats(item[i].id - 1);
                    add_console_message("You equip the %s", color_white,
                                        item_info[item[i].id - 1].name);
                }
                
                break;
            }
        }
    }
}

internal void
add_item(item_id item_id, v2u pos)
{
    for(u32 i = 0; i < ITEM_COUNT; ++i)
    {
        if(!item[i].id)
        {
            printf("Item added\n");
            
            item[i].id = item_id;
            item[i].in_inventory = false;
            item[i].equipped = false;
            item[i].pos = pos;
            return;
        }
    }
    
    assert(0, "Item array is full");
}

internal void
pick_up_item()
{
    for(u32 i = 0; i < ITEM_COUNT; ++i)
    {
        if(!item[i].in_inventory)
        {
            if(V2u_equal(item[i].pos, player.pos))
            {
                for(u32 inventory_i = 0; inventory_i < INVENTORY_SLOT_COUNT; ++inventory_i)
                {
                    if(!inventory.slot[inventory_i].id)
                    {
                        item[i].in_inventory = true;
                        inventory.slot[inventory_i] = item[i];
                        add_console_message("You pick up the %s", color_white,
                                            item_info[item[i].id - 1].name);
                        
                        return;
                    }
                }
                
                add_console_message("Your inventory is full right now", color_white);
            }
        }
    }
    
    add_console_message("You find nothing to pick up", color_white);
}