internal void
set_item_info_data()
{
    // TODO(rami): Since we know the item we are setting the information for,
    // we could skip all the things that item doesn't care about because
    // the item array is initialized to zero
    
    item_info_t *info = &item_info[0];
    info->id = 1;
    strcpy(info->name, "Lesser Health Potion");
    info->category = category_consumable;
    info->slot = slot_none;
    info->tile = V2u(8, 0);
    strcpy(info->use, "Restores 2 health");
    info->heal_amount = 2;
    info->damage = 0;
    info->armor = 0;
    strcpy(info->description, "");
    
    info = &item_info[1];
    info->id = 2;
    strcpy(info->name, "Iron Sword");
    info->category = category_weapon;
    info->slot = slot_first_hand;
    info->tile = V2u(4, 1);
    info->use[0] = 0;
    info->heal_amount = 0;
    info->damage = 2;
    info->armor = 0;
    strcpy(info->description, "");
    
    info = &item_info[2];
    info->id = 3;
    strcpy(info->name, "Rune Helmet");
    info->category = category_armor;
    info->slot = slot_head;
    info->tile = V2u(0, 1);
    info->use[0] = 0;
    info->heal_amount = 0;
    info->damage = 0;
    info->armor = 1;
    strcpy(info->description, "");
    
    info = &item_info[3];
    info->id = 4;
    strcpy(info->name, "Rune Chestplate");
    info->category = category_armor;
    info->slot = slot_body;
    info->tile = V2u(1, 1);
    info->use[0] = 0;
    info->heal_amount = 0;
    info->damage = 0;
    info->armor = 1;
    strcpy(info->description, "");
    
    info = &item_info[4];
    info->id = 5;
    strcpy(info->name, "Rune Platelegs");
    info->category = category_armor;
    info->slot = slot_legs;
    info->tile = V2u(2, 1);
    info->use[0] = 0;
    info->heal_amount = 0;
    info->damage = 0;
    info->armor = 1;
    strcpy(info->description, "");
    
    info = &item_info[5];
    info->id = 6;
    strcpy(info->name, "Rune Boots");
    info->category = category_armor;
    info->slot = slot_feet;
    info->tile = V2u(3, 1);
    info->use[0] = 0;
    info->heal_amount = 0;
    info->damage = 0;
    info->armor = 1;
    strcpy(info->description, "");
    
    info = &item_info[6];
    info->id = 7;
    strcpy(info->name, "Rune Shield");
    info->category = category_armor;
    info->slot = slot_second_hand;
    info->tile = V2u(5, 1);
    info->use[0] = 0;
    info->heal_amount = 0;
    info->damage = 0;
    info->armor = 1;
    strcpy(info->description, "");
    
    info = &item_info[7];
    info->id = 8;
    strcpy(info->name, "Rune Amulet");
    info->category = category_armor;
    info->slot = slot_amulet;
    info->tile = V2u(6, 1);
    info->use[0] = 0;
    info->heal_amount = 0;
    info->damage = 0;
    info->armor = 1;
    strcpy(info->description, "");
    
    info = &item_info[8];
    info->id = 9;
    strcpy(info->name, "Rune Ring");
    info->category = category_armor;
    info->slot = slot_ring;
    info->tile = V2u(7, 1);
    info->use[0] = 0;
    info->heal_amount = 0;
    info->damage = 0;
    info->armor = 1;
    strcpy(info->description, "");
    
    info = &item_info[9];
    info->id = 10;
    strcpy(info->name, "Red Chestplate");
    info->category = category_armor;
    info->slot = slot_body;
    info->tile = V2u(1, 2);
    info->use[0] = 0;
    info->heal_amount = 0;
    info->damage = 0;
    info->armor = 3;
    strcpy(info->description, "");
    
    info = &item_info[10];
    info->id = 11;
    strcpy(info->name, "Red Sword");
    info->category = category_weapon;
    info->slot = slot_first_hand;
    info->tile = V2u(4, 2);
    info->use[0] = 0;
    info->heal_amount = 0;
    info->damage = 3;
    info->armor = 0;
    strcpy(info->description, "");
}

internal void
move_item(u32 src_index, u32 dest_index)
{
    item_t *dest_slot = &inventory.slots[dest_index];
    item_t *src_slot = &inventory.slots[src_index];
    
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
    for(u32 i = 0; i < array_count(items); ++i)
    {
        if(items[i].id && !items[i].in_inventory)
        {
            if(is_seen(items[i].pos))
            {
                v2u pos = get_game_position(items[i].pos);
                
                v4u src = V4u(tile_mul(item_info[items[i].id - 1].tile.x), tile_mul(item_info[items[i].id - 1].tile.y), 32, 32);
                v4u dest = V4u(pos.x, pos.y, 32, 32);
                SDL_RenderCopy(game.renderer, textures[tex_item_tileset].tex, (SDL_Rect *)&src, (SDL_Rect *)&dest);
            }
        }
    }
}

// TODO(rami): Do we want to pick dropped items in the reverse order?
// Or do we want to give a list of items on that spot so you can choose?
// Or something else?
internal void
drop_item(b32 print_drop_text)
{
    if(inventory.item_count)
    {
        for(u32 i = 0; i < array_count(items); ++i)
        {
            if(items[i].in_inventory)
            {
                u32 inventory_index = get_inventory_pos_index();
                
                if(items[i].unique_id ==
                   inventory.slots[inventory_index].unique_id)
                {
                    items[i].in_inventory = false;
                    items[i].equipped = false;
                    items[i].pos = player.pos;
                    
                    item_t *slot = &inventory.slots[inventory_index];
                    slot->id = 0;
                    slot->unique_id = 0;
                    slot->pos = V2u(0, 0);
                    slot->in_inventory = false;
                    slot->equipped = false;
                    
                    if(print_drop_text)
                    {
                        add_console_text("You drop the %s", color_white, item_info[items[i].id - 1].name);
                    }
                    
                    --inventory.item_count;
                    break;
                }
            }
        }
    }
    else
    {
        add_console_text("You have nothing to drop", color_white);
    }
}

internal void
remove_item(u32 i)
{
    items[i].id = id_none;
    items[i].in_inventory = false;
    items[i].equipped = false;
    items[i].pos = V2u(0, 0);
}

internal void
consume_item()
{
    for(u32 i = 0; i < array_count(items); ++i)
    {
        if(items[i].in_inventory &&
           item_info[items[i].id - 1].category == category_consumable)
        {
            u32 inventory_index = get_inventory_pos_index();
            if(items[i].unique_id ==
               inventory.slots[inventory_index].unique_id)
            {
                if(heal_player(item_info[items[i].id - 1].heal_amount))
                {
                    add_console_text("You drink the potion and feel slightly better", color_green);
                    drop_item(0);
                    remove_item(i);
                }
                else
                {
                    add_console_text("You do not feel the need to drink this", color_white);
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
    
    for(u32 i = 0; i < array_count(items); ++i)
    {
        if(items[i].unique_id == unique_id)
        {
            data.success = true;
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
    
    for(u32 i = 0; i < array_count(inventory.slots); ++i)
    {
        u32 info_index = inventory.slots[i].id - 1;
        
        if(inventory.slots[i].equipped &&
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
    for(u32 i = 0; i < array_count(items); ++i)
    {
        if(items[i].in_inventory &&
           (item_info[items[i].id - 1].category == category_weapon ||
            item_info[items[i].id - 1].category == category_armor))
        {
            u32 inventory_index = get_inventory_pos_index();
            
            if(items[i].unique_id ==
               inventory.slots[inventory_index].unique_id)
            {
                if(items[i].equipped &&
                   inventory.slots[inventory_index].equipped)
                {
                    items[i].equipped = false;
                    inventory.slots[inventory_index].equipped = false;
                    
                    remove_item_stats(items[i].id - 1);
                    add_console_text("You unequip the %s", color_white, item_info[items[i].id - 1].name);
                }
                else
                {
                    // If the item slot already has something in it,
                    // unequip whatever item is there to make space for the new item
                    item_slot_data_t slot = get_item_equip_slot_data(inventory_index);
                    if(slot.occupied)
                    {
                        return_data_t ret = get_item_index_from_unique_id(inventory.slots[slot.index].unique_id);
                        items[ret.value].equipped = false;
                        inventory.slots[slot.index].equipped = false;
                        
                        remove_item_stats(inventory.slots[slot.index].id - 1);
                    }
                    
                    items[i].equipped = true;
                    inventory.slots[inventory_index].equipped = true;
                    
                    add_item_stats(items[i].id - 1);
                    add_console_text("You equip the %s", color_white, item_info[items[i].id - 1].name);
                }
                
                break;
            }
        }
    }
}

internal void
add_item(item_id item_id, v2u pos)
{
    for(u32 i = 0; i < array_count(items); ++i)
    {
        if(!items[i].id)
        {
            printf("Item added\n");
            
            items[i].id = item_id;
            items[i].in_inventory = false;
            items[i].equipped = false;
            items[i].pos = pos;
            return;
        }
    }
    
    assert(0, "Item array is full");
}

internal void
pick_up_item()
{
    for(u32 i = 0; i < array_count(items); ++i)
    {
        if(!items[i].in_inventory)
        {
            if(V2u_equal(items[i].pos, player.pos))
            {
                for(u32 inventory_i = 0; inventory_i < array_count(inventory.slots); ++inventory_i)
                {
                    if(!inventory.slots[inventory_i].id)
                    {
                        items[i].in_inventory = true;
                        inventory.slots[inventory_i] = items[i];
                        add_console_text("You pick up the %s", color_white, item_info[items[i].id - 1].name);
                        
                        return;
                    }
                }
                
                add_console_text("Your inventory is full right now", color_white);
            }
        }
    }
    
    add_console_text("You find nothing to pick up", color_white);
}
