internal void
set_item_info_data()
{
    // TODO(rami): Since we know the item we are setting the information for,
    // we could skip all the things that item doesn't care about because
    // the item array is initialized to zero.
    
    item_info_t *info = &item_info[0];
    info->id = 1;
    strcpy(info->name, "Ring of Protection");
    info->slot = slot_ring;
    strcpy(info->description, "A worn ring with unknown glyphs engraved on it.");
    info->tile = V2u(7, 1);
    info->type = type_armor;
    info->general.strength = 0;
    info->general.defence = 2;
    info->general.hp = 3;
    
    info = &item_info[1];
    info->id = 2;
    strcpy(info->name, "Iron Sword");
    info->slot = slot_first_hand;
    strcpy(info->description, "A sharp sword made of iron.");
    info->tile = V2u(4, 1);
    info->type = type_weapon;
    info->general.strength = 2;
    info->general.defence = 0;
    info->general.hp = 0;
    
    info = &item_info[2];
    info->id = 3;
    strcpy(info->name, "Lesser Health Potion");
    info->slot = slot_ring;
    strcpy(info->description, "Heals minor cuts.");
    info->tile = V2u(8, 0);
    info->type = type_consumable;
    info->consumable.effect = effect_heal;
    strcpy(info->consumable.effect_text, "Restores 2 health");
    info->consumable.effect_amount = 2;
}

internal u32
get_item_info_index(u32 item_index)
{
    u32 result = items[item_index].id - 1;
    return(result);
}

internal u32
get_inventory_info_index(u32 inventory_index)
{
    u32 result = inventory.slots[inventory_index].id - 1;
    return(result);
}

internal u32
get_inventory_pos_index()
{
    u32 result = (inventory.current_slot.y * INVENTORY_WIDTH) + inventory.current_slot.x;
    return(result);
}

internal v2u
get_inventory_pos_from_index(u32 inventory_index)
{
    v2u result = {inventory_index, 0};
    
    if(inventory_index >= INVENTORY_WIDTH)
    {
        result = V2u(inventory_index % INVENTORY_WIDTH,
                     inventory_index/ INVENTORY_WIDTH);
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
    for(u32 i = 0; i < array_count(items); ++i)
    {
        if(items[i].id &&
           !items[i].in_inventory &&
           is_seen(items[i].pos))
        {
            v2u pos = get_game_position(items[i].pos);
            
            u32 item_info_index = get_item_info_index(i);
            v4u src = V4u(tile_mul(item_info[item_info_index].tile.x), tile_mul(item_info[item_info_index].tile.y), 32, 32);
            v4u dest = V4u(pos.x, pos.y, 32, 32);
            SDL_RenderCopy(game.renderer, textures[tex_item_tileset].tex, (SDL_Rect *)&src, (SDL_Rect *)&dest);
        }
    }
}

internal void
add_item_stats(u32 item_info_index)
{
    item_info_t *info = &item_info[item_info_index];
    
    if(info->general.strength)
    {
        player.strength += info->general.strength;
    }
    
    if(info->general.defence)
    {
        player.defence += info->general.defence;
    }
    
    if(info->general.hp)
    {
        player.max_hp += info->general.hp;
    }
}

internal void
remove_item_stats(u32 item_info_index)
{
    item_info_t *info = &item_info[item_info_index];
    
    if(info->general.strength)
    {
        player.strength -= info->general.strength;
    }
    
    if(info->general.defence)
    {
        player.defence -= info->general.defence;
    }
    
    if(info->general.hp)
    {
        player.max_hp -= info->general.hp;
    }
}

internal void
remove_inventory_item(b32 print_drop_text)
{
    if(inventory.item_count)
    {
        for(u32 i = 0; i < array_count(items); ++i)
        {
            if(items[i].in_inventory)
            {
                u32 inventory_index = get_inventory_pos_index();
                if(items[i].unique_id == inventory.slots[inventory_index].unique_id)
                {
                    items[i].in_inventory = false;
                    items[i].is_equipped = false;
                    items[i].pos = player.pos;
                    
                    u32 item_info_index = get_item_info_index(i);
                    if(inventory.slots[inventory_index].is_equipped)
                    {
                        remove_item_stats(item_info_index);
                    }
                    
                    if(print_drop_text)
                    {
                        add_console_text("You drop the %s.", color_white, item_info[item_info_index].name);
                    }
                    
                    memset(&inventory.slots[inventory_index], 0, sizeof(item_t));
                    --inventory.item_count;
                    break;
                }
            }
        }
    }
    else
    {
        add_console_text("There's nothing to drop there.", color_white);
    }
}

internal void
remove_game_item(item_t *item)
{
    // NOTE(rami): No memset because we don't want to erase the unique_id member.
    item->id = id_none;
    item->pos = V2u(0, 0);
    item->in_inventory = false;
    item->is_equipped = false;
}

internal void
consume_item()
{
    for(u32 i = 0; i < array_count(items); ++i)
    {
        u32 info_index = get_item_info_index(i);
        if(items[i].in_inventory && item_info[info_index].type == type_consumable)
        {
            u32 inventory_index = get_inventory_pos_index();
            if(items[i].unique_id == inventory.slots[inventory_index].unique_id)
            {
                // TODO(rami): If we have potions that do other things than heal
                // we need to check here what the consume_effect of the potion is
                // so that we can do the heal, buff or whatever it does.
                if(heal_player(item_info[info_index].consumable.effect_amount))
                {
                    add_console_text("You drink the potion and feel slightly better.", color_green);
                    
                    remove_inventory_item(0);
                    remove_game_item(&items[i]);
                }
                else
                {
                    add_console_text("You do not feel the need to drink this.", color_white);
                }
                
                break;
            }
        }
    }
}

internal u32_t
get_item_index_from_unique_id(u32 unique_id)
{
    u32_t result = {0};
    
    for(u32 i = 0; i < array_count(items); ++i)
    {
        if(items[i].unique_id == unique_id)
        {
            result.success = true;
            result.value = i;
            break;
        }
    }
    
    return(result);
}

internal u32_t
get_inventory_index_of_item_in_inventory_slot(u32 current_item_inventory_index)
{
    u32_t result = {0};
    
    u32 current_item_info_index = get_inventory_info_index(current_item_inventory_index);
    
    for(u32 inventory_index = 0;
        inventory_index < array_count(inventory.slots);
        ++inventory_index)
    {
        if(inventory.slots[inventory_index].id)
        {
            u32 item_info_index = get_inventory_info_index(inventory_index);
            
            if(inventory_index != current_item_inventory_index &&
               inventory.slots[inventory_index].is_equipped &&
               item_info[item_info_index].slot == item_info[current_item_info_index].slot)
            {
                result.success = true;
                result.value = inventory_index;
                break;
            }
        }
    }
    
    return(result);
}

internal b32
is_item_slot_occupied(item_slot slot)
{
    b32 result = false;
    
    for(u32 inventory_index = 0;
        inventory_index < array_count(inventory.slots);
        ++inventory_index)
    {
        u32 info_index = get_inventory_info_index(inventory_index);
        
        if(inventory.slots[inventory_index].is_equipped &&
           item_info[info_index].slot == slot)
        {
            result = true;
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
        u32 item_info_index = get_item_info_index(i);
        if(items[i].in_inventory &&
           (item_info[item_info_index].type == type_weapon ||
            item_info[item_info_index].type == type_armor))
        {
            u32 inventory_index = get_inventory_pos_index();
            if(items[i].unique_id == inventory.slots[inventory_index].unique_id)
            {
                if(items[i].is_equipped && inventory.slots[inventory_index].is_equipped)
                {
                    items[i].is_equipped = false;
                    inventory.slots[inventory_index].is_equipped = false;
                    
                    remove_item_stats(item_info_index);
                    add_console_text("You unequip the %s.", color_white, item_info[item_info_index].name);
                }
                else
                {
                    // NOTE(rami): If the item slot already has something in it,
                    // unequip it to make space for the new item.
                    if(is_item_slot_occupied(item_info[item_info_index].slot))
                    {
                        u32_t slot_item_inventory_index = get_inventory_index_of_item_in_inventory_slot(inventory_index);
                        if(slot_item_inventory_index.success)
                        {
                            u32_t slot_item_index = get_item_index_from_unique_id(inventory.slots[slot_item_inventory_index.value].unique_id);
                            if(slot_item_index.success)
                            {
                                items[slot_item_index.value].is_equipped = false;
                                inventory.slots[slot_item_inventory_index.value].is_equipped = false;
                                
                                remove_item_stats(get_inventory_info_index(slot_item_inventory_index.value));
                            }
                        }
                    }
                    
                    items[i].is_equipped = true;
                    inventory.slots[inventory_index].is_equipped = true;
                    
                    add_item_stats(item_info_index);
                    add_console_text("You equip the %s.", color_white, item_info[item_info_index].name);
                    break;
                }
            }
        }
    }
}

internal void
add_item(item_id id, v2u pos)
{
    for(u32 i = 0; i < array_count(items); ++i)
    {
        if(!items[i].id)
        {
            printf("Item added\n");
            
            items[i].id = id;
            items[i].in_inventory = false;
            items[i].is_equipped = false;
            items[i].pos = pos;
            return;
        }
    }
    
    assert(0, "Item array is full");
}

internal void
add_inventory_item()
{
    for(u32 i = 0; i < array_count(items); ++i)
    {
        if(!items[i].in_inventory)
        {
            if(V2u_equal(items[i].pos, player.pos))
            {
                for(u32 inventory_index = 0;
                    inventory_index < array_count(inventory.slots);
                    ++inventory_index)
                {
                    if(!inventory.slots[inventory_index].id)
                    {
                        items[i].in_inventory = true;
                        inventory.slots[inventory_index] = items[i];
                        
                        add_console_text("You pick up the %s.", color_white, item_info[get_item_info_index(i)].name);
                        return;
                    }
                }
                
                add_console_text("Your inventory is full right now.", color_white);
            }
        }
    }
    
    add_console_text("You find nothing to pick up.", color_white);
}