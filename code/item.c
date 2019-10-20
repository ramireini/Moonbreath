internal void
set_item_info_data()
{
    // TODO(rami): Since we know the item we are setting the information for,
    // we could skip all the things that item doesn't care about because
    // the item array is initialized to zero.
    
    /*
    item_info_t *info = &item_info[0];
    info->id = 1;
    strcpy(info->name, "Lesser Health Potion");
    info->type = type_consumable;
    info->slot = slot_none;
    info->tile = V2u(8, 0);
    strcpy(info->use, "Restores 2 health");
    info->heal_amount = 2;
    info->strength = 0;
    info->defence = 0;
    strcpy(info->description, "");
    
    info = &item_info[1];
    info->id = 2;
    strcpy(info->name, "Iron Sword");
    info->type = type_weapon;
    info->slot = slot_first_hand;
    info->tile = V2u(4, 1);
    info->use[0] = 0;
    info->heal_amount = 0;
    info->strength = 2;
    info->defence = 0;
    strcpy(info->description, "");
    
    info = &item_info[2];
    info->id = 3;
    strcpy(info->name, "Rune Helmet");
    info->type = type_armor;
    info->slot = slot_head;
    info->tile = V2u(0, 1);
    info->use[0] = 0;
    info->heal_amount = 0;
    info->strength = 0;
    info->defence = 1;
    strcpy(info->description, "");
    
    info = &item_info[3];
    info->id = 4;
    strcpy(info->name, "Rune Chestplate");
    info->type = type_armor;
    info->slot = slot_body;
    info->tile = V2u(1, 1);
    info->use[0] = 0;
    info->heal_amount = 0;
    info->strength = 0;
    info->defence = 1;
    strcpy(info->description, "");
    
    info = &item_info[4];
    info->id = 5;
    strcpy(info->name, "Rune Platelegs");
    info->type = type_armor;
    info->slot = slot_legs;
    info->tile = V2u(2, 1);
    info->use[0] = 0;
    info->heal_amount = 0;
    info->strength = 0;
    info->defence = 1;
    strcpy(info->description, "");
    
    info = &item_info[5];
    info->id = 6;
    strcpy(info->name, "Rune Boots");
    info->type = type_armor;
    info->slot = slot_feet;
    info->tile = V2u(3, 1);
    info->use[0] = 0;
    info->heal_amount = 0;
    info->strength = 0;
    info->defence = 1;
    strcpy(info->description, "");
    
    info = &item_info[6];
    info->id = 7;
    strcpy(info->name, "Rune Shield");
    info->type = type_armor;
    info->slot = slot_second_hand;
    info->tile = V2u(5, 1);
    info->use[0] = 0;
    info->heal_amount = 0;
    info->strength = 0;
    info->defence = 1;
    strcpy(info->description, "");
    
    info = &item_info[7];
    info->id = 8;
    strcpy(info->name, "Rune Amulet");
    info->type = type_armor;
    info->slot = slot_amulet;
    info->tile = V2u(6, 1);
    info->use[0] = 0;
    info->heal_amount = 0;
    info->strength = 0;
    info->defence = 1;
    strcpy(info->description, "");
    
    info = &item_info[8];
    info->id = 9;
    strcpy(info->name, "Rune Ring");
    info->type = type_armor;
    info->slot = slot_ring;
    info->tile = V2u(7, 1);
    info->use[0] = 0;
    info->heal_amount = 0;
    info->strength = 0;
    info->defence = 1;
    strcpy(info->description, "");
    
    info = &item_info[9];
    info->id = 10;
    strcpy(info->name, "Knight Greaves");
    info->type = type_armor;
    info->slot = slot_feet;
    info->tile = V2u(3, 2);
    info->use[0] = 0;
    info->heal_amount = 0;
    info->strength = 0;
    info->defence = 2;
    strcpy(info->description, "");
    
    info = &item_info[10];
    info->id = 11;
    strcpy(info->name, "Ring of Protection");
    info->type = type_armor;
    info->slot = slot_ring;
    info->tile = V2u(7, 2);
    info->use[0] = 0;
    info->heal_amount = 0;
    info->strength = 0;
    info->defence = 1;
    strcpy(info->description, "");
    
    info = &item_info[11];
    info->id = 12;
    strcpy(info->name, "Iron Sword New");
    info->type = type_weapon;
    info->slot = slot_ring;
    info->tile = V2u(4, 2);
    info->use[0] = 0;
    info->heal_amount = 0;
    info->strength = 2;
    info->defence = 0;
    strcpy(info->description, "");
*/
    
    item_info_t *info = 0;
    
#if 0
    item_info_t *info = &item_info[0];
    info->id = 1;
    strcpy(info->name, "Lesser Health Potion");
    info->slot = slot_ring;
    strcpy(info->description, "Heals minor cuts.");
    info->tile = V2u(8, 0);
    info->type = type_consumable;
    info->consumable.effect = effect_heal;
    strcpy(info->consumable.effect_text, "Restores 2 health");
    info->consumable.effect_amount = 2;
#endif
    
    info = &item_info[1];
    info->id = 2;
    strcpy(info->name, "Iron Sword Old");
    info->slot = slot_first_hand;
    strcpy(info->description, "Mad cuz bad");
    info->tile = V2u(4, 1);
    info->type = type_weapon;
    info->general.strength = 1;
    info->general.defence = 0;
    info->general.hp = 0;
    
#if 0
    info = &item_info[10];
    info->id = 11;
    strcpy(info->name, "Ring of Protection");
    info->slot = slot_ring;
    strcpy(info->description, "A worn ring with unknown glyphs engraved on it.");
    info->tile = V2u(7, 2);
    info->type = type_armor;
    info->general.strength = 0;
    info->general.defence = 2;
    info->general.hp = 3;
#endif
    
    info = &item_info[11];
    info->id = 12;
    strcpy(info->name, "Iron Sword New");
    info->slot = slot_first_hand;
    strcpy(info->description, "A sharp sword made of iron.");
    info->tile = V2u(4, 2);
    info->type = type_weapon;
    info->general.strength = 2;
    info->general.defence = 1;
    info->general.hp = 0;
}

internal void
move_item_in_inventory(u32 src_index, u32 dest_index)
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
    
    inventory.item_is_being_moved = false;
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

internal void
add_item_stats(u32 info_index)
{
    item_info_t *info = &item_info[info_index];
    
    // TODO(rami): We probably want a system we can enumerate through,
    // instead of checking things one by one everwhere they need to be checked
    // which is a pain.
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
remove_item_stats(u32 info_index)
{
    item_info_t *info = &item_info[info_index];
    
    // TODO(rami): We probably want a system we can enumerate through,
    // instead of checking things one by one everwhere they need to be checked
    // which is a pain.
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

// TODO(rami): Do we want to pick dropped items in the reverse order?
// Or do we want to give a list of items on that spot so you can choose?
// Or something else?
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
                    
                    u32 info_index = items[i].id - 1;
                    if(inventory.slots[inventory_index].is_equipped)
                    {
                        remove_item_stats(info_index);
                    }
                    
                    memset(&inventory.slots[inventory_index], 0, sizeof(item_t));
                    
                    if(print_drop_text)
                    {
                        add_console_text("You drop the %s.", color_white, item_info[info_index].name);
                    }
                    
                    --inventory.item_count;
                    break;
                }
            }
        }
    }
    else
    {
        add_console_text("You have nothing to drop.", color_white);
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
        u32 info_index = items[i].id - 1;
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

internal b32
is_item_slot_occupied(item_slot slot)
{
    b32 result = false;
    
    for(u32 i = 0; i < array_count(inventory.slots); ++i)
    {
        u32 info_index = inventory.slots[i].id - 1;
        if(inventory.slots[i].is_equipped && item_info[info_index].slot == slot)
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
        if(items[i].in_inventory &&
           (item_info[items[i].id - 1].type == type_weapon ||
            item_info[items[i].id - 1].type == type_armor))
        {
            u32 inventory_index = get_inventory_pos_index();
            if(items[i].unique_id == inventory.slots[inventory_index].unique_id)
            {
                if(items[i].is_equipped && inventory.slots[inventory_index].is_equipped)
                {
                    items[i].is_equipped = false;
                    inventory.slots[inventory_index].is_equipped = false;
                    
                    remove_item_stats(items[i].id - 1);
                    add_console_text("You unequip the %s.", color_white, item_info[items[i].id - 1].name);
                }
                else
                {
                    // NOTE(rami): If the item slot already has something in it,
                    // unequip it to make space for the new item.
                    item_slot_data_t slot = get_item_equip_slot_data(inventory_index);
                    if(slot.occupied)
                    {
                        u32_t data = get_item_index_from_unique_id(inventory.slots[slot.index].unique_id);
                        items[data.value].is_equipped = false;
                        inventory.slots[slot.index].is_equipped = false;
                        
                        remove_item_stats(inventory.slots[slot.index].id - 1);
                    }
                    
                    items[i].is_equipped = true;
                    inventory.slots[inventory_index].is_equipped = true;
                    
                    add_item_stats(items[i].id - 1);
                    add_console_text("You equip the %s.", color_white, item_info[items[i].id - 1].name);
                }
                
                break;
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
                        add_console_text("You pick up the %s.", color_white, item_info[items[i].id - 1].name);
                        
                        return;
                    }
                }
                
                add_console_text("Your inventory is full right now.", color_white);
            }
        }
    }
    
    add_console_text("You find nothing to pick up.", color_white);
}