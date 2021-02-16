internal b32
is_item_valid(Item *item)
{
    b32 result = (item && item->id);
    return(result);
}

internal b32
is_item_valid_and_selected(Item *item)
{
    b32 result = (is_item_valid(item) && is_set(item->flags, ItemFlags_Selected));
    return(result);
}

internal b32
is_item_valid_and_in_inventory(Item *item)
{
    b32 result = (is_item_valid(item) && is_set(item->flags, ItemFlags_Inventory));
    return(result);
}

internal b32
is_item_valid_and_not_in_inventory(Item *item)
{
    b32 result = (is_item_valid(item) && !is_set(item->flags, ItemFlags_Inventory));
    return(result);
}

internal Item *
get_item_with_letter(ItemState *items, char letter, LetterType letter_type, b32 search_from_inventory)
{
    Item *result = 0;
    
    for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
    {
        Item *item = &items->array[index];
        if(is_item_valid(item))
        {
            if(search_from_inventory && !is_set(item->flags, ItemFlags_Inventory))
            {
                continue;
            }
            
            if(letter_type == LetterType_Letter &&
               item->letter == letter)
            {
                result = item;
                break;
            }
            else if(letter_type == LetterType_SelectLetter &&
                    item->selection_letter == letter)
            {
                result = item;
                break;
            }
        }
    }
    
    return(result);
}

internal char
get_free_item_letter_from_range(ItemState *items, char start, char end, LetterType letter_type)
{
    char result = 0;
    
    for(char letter = start; letter <= end; ++letter)
    {
        Item *item = get_item_with_letter(items, letter, letter_type, false);
        if(!item)
        {
            result = letter;
            break;
        }
    }
    
    return(result);
}

internal char
get_free_item_letter(ItemState *items, LetterType letter_type)
{
    char result = get_free_item_letter_from_range(items, 'a', 'z', letter_type);
    
    if(!result)
    {
        result = get_free_item_letter_from_range(items, 'A', 'Z', letter_type);
    }
    
    assert(result);
    return(result);
}

internal String128
get_item_letter_string(Item *item)
{
    String128 result = {0};
    
    if(item->selection_letter)
    {
        if(is_set(item->flags, ItemFlags_Selected))
        {
            sprintf(result.str, "%c + ", item->selection_letter);
        }
        else
        {
            sprintf(result.str, "%c - ", item->selection_letter);
        }
    }
    else
    {
        sprintf(result.str, "%c - ", item->letter);
    }
    
    return(result);
}

internal char *
get_item_status_prefix(Item *item)
{
    char *result = "";
    
    if(is_set(item->flags, (ItemFlags_Identified | ItemFlags_Cursed)))
    {
        result = "Cursed ";
    }
    
    return(result);
}

internal char *
get_damage_type_text(DamageType damage_type)
{
    char *result = 0;
    
    switch(damage_type)
    {
        case DamageType_Physical: result = "Physical"; break;
        case DamageType_Fire: result = "Fire"; break;
        case DamageType_Ice: result = "Ice"; break;
        case DamageType_Lightning: result = "Lightning"; break;
        case DamageType_Poison: result = "Poison"; break;
        case DamageType_Holy: result = "Holy"; break;
        case DamageType_Darkness: result = "Darkness"; break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal char *
get_item_id_text(ItemID id)
{
    char *result = 0;
    
    switch(id)
    {
        case ItemID_Dagger: result = "Dagger"; break;
        case ItemID_Club: result = "Club"; break;
        case ItemID_Sword: result = "Sword"; break;
        case ItemID_Battleaxe: result = "Battleaxe"; break;
        case ItemID_Spear: result = "Spear"; break;
        case ItemID_Warhammer: result = "Warhammer"; break;
        
        case ItemID_LeatherHelmet: result = "Leather Helmet"; break;
        case ItemID_LeatherChestplate: result = "Leather Chestplate"; break;
        case ItemID_LeatherGreaves: result = "Leather Greaves"; break;
        case ItemID_LeatherBoots: result = "Leather Boots"; break;
        case ItemID_SteelHelmet: result = "Steel Helmet"; break;
        case ItemID_SteelChestplate: result = "Steel Chestplate"; break;
        case ItemID_SteelGreaves: result = "Steel Greaves"; break;
        case ItemID_SteelBoots: result = "Steel Boots"; break;
        
        case ItemID_MightPotion:
        case ItemID_WisdomPotion:
        case ItemID_AgilityPotion:
        case ItemID_ElusionPotion:
        case ItemID_HealingPotion:
        case ItemID_DecayPotion:
        case ItemID_ConfusionPotion: result = "Potion"; break;
        
        case ItemID_IdentifyScroll:
        case ItemID_EnchantWeaponScroll:
        case ItemID_EnchantArmorScroll:
        case ItemID_TeleportationScroll:
        case ItemID_MagicMappingScroll:
        case ItemID_UncurseScroll: result = "Scroll"; break;
        
        case ItemID_Ration: result = "Ration"; break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal String128
get_full_item_name(Item *item)
{
    String128 result = {0};
    
    if(is_item_equipment(item->type))
    {
        if(is_set(item->flags, ItemFlags_Identified))
        {
            if(item->second_damage_type)
            {
                sprintf(result.str, "%c%d %s of %s",
                        sign(item->enchantment_level),
                        absolute(item->enchantment_level),
                        item->name,
                        get_damage_type_text(item->second_damage_type));
            }
            else
            {
                sprintf(result.str, "%c%d %s",
                        sign(item->enchantment_level),
                        absolute(item->enchantment_level),
                        item->name);
            }
        }
        else
        {
            sprintf(result.str, "%s", get_item_id_text(item->id));
        }
    }
    else
    {
        if(is_set(item->flags, ItemFlags_Identified))
        {
            sprintf(result.str, "%s", item->name);
        }
        else
        {
            sprintf(result.str, "%s%s", item->c.depiction, get_item_id_text(item->id));
        }
    }
    
    return(result);
}

internal void
update_item_adjusting(Input *input, Item *item, ItemState *items, Inventory *inventory, UI *ui)
{
    char pressed = get_pressed_alphabet_char(input);
    if(pressed)
    {
        // If letter is taken, get a new one
        if(get_item_with_letter(items, pressed, LetterType_Letter, false))
        {
            item->letter = get_free_item_letter(items, LetterType_Letter);
        }
        
        item->letter = pressed;
        unset(inventory->flags, InventoryFlags_Adjusting);
        
        log_add(ui, "%s%s%s",
                get_item_letter_string(item).str,
                get_item_status_prefix(item),
                get_full_item_name(item).str);
    }
}

internal void
set_as_known_and_identify_existing(ItemID id, ItemState *items)
{
    // Set the flag of that ID for future generated items.
    switch(id)
    {
        case ItemID_MightPotion: items->potion_info[Potion_Might].known = true; break;
        case ItemID_WisdomPotion: items->potion_info[Potion_Wisdom].known = true; break;
        case ItemID_AgilityPotion: items->potion_info[Potion_Agility].known = true; break;
        case ItemID_ElusionPotion: items->potion_info[Potion_Elusion].known = true; break;
        case ItemID_HealingPotion: items->potion_info[Potion_Healing].known = true; break;
        case ItemID_DecayPotion: items->potion_info[Potion_Decay].known = true; break;
        case ItemID_ConfusionPotion: items->potion_info[Potion_Confusion].known = true; break;
        
        case ItemID_IdentifyScroll: items->scroll_info[Scroll_Identify].known = true; break;
        case ItemID_EnchantWeaponScroll: items->scroll_info[Scroll_EnchantWeapon].known = true; break;
        case ItemID_EnchantArmorScroll: items->scroll_info[Scroll_EnchantArmor].known = true; break;
        case ItemID_MagicMappingScroll: items->scroll_info[Scroll_MagicMapping].known = true; break;
        case ItemID_TeleportationScroll: items->scroll_info[Scroll_Teleportation].known = true; break;
        case ItemID_UncurseScroll: items->scroll_info[Scroll_Uncurse].known = true; break;
        
        case ItemID_Ration: break;
        
        invalid_default_case;
    }
    
    // Identify the items of that ID that are already in the game.
    for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
    {
        Item *item = &items->array[index];
        if(id == item->id)
        {
            set(item->flags, ItemFlags_Identified);
        }
    }
}

internal void
remove_item_from_game(Item *item)
{
    zero_struct(*item);
}

internal b32
is_item_equipment(ItemType type)
{
    b32 result = (type == ItemType_Weapon ||
                  type == ItemType_Armor);
    
    return(result);
}

internal b32
is_item_consumable(ItemType type)
{
    b32 result = (type == ItemType_Potion ||
                  type == ItemType_Scroll ||
                  type == ItemType_Ration);
    
    return(result);
}

internal b32
is_item_stacked(Item *item)
{
    b32 result = (item->c.stack_count > 1);
        return(result);
}

internal b32
add_item_to_inventory(Game *game, Entity *player, Item *item, ItemState *items, Inventory *inventory, UI *ui)
{
    b32 added_to_stack = false;
    b32 added_to_inventory = false;
    
    // Add item to the stack if it's already in the inventory.
    if(is_item_consumable(item->type))
    {
        for(u32 index = 0; index < MAX_INVENTORY_SLOT_COUNT; ++index)
        {
            Item *inventory_item = inventory->slots[index];
            
            if(is_item_valid(inventory_item) &&
               inventory_item->id == item->id)
            {
                added_to_stack = true;
                added_to_inventory = true;
                
                inventory_item->c.stack_count += item->c.stack_count;
                break;
            }
        }
    }
    
    if(!added_to_inventory)
    {
        for(u32 index = 0; index < MAX_INVENTORY_SLOT_COUNT; ++index)
        {
            if(!inventory->slots[index])
            {
                if(!item->letter)
                {
                    item->letter = get_free_item_letter(items, LetterType_Letter);
                }
                
                unset(player->flags, EntityFlags_MultipleItemNotify);
                set(item->flags, ItemFlags_Inventory);
                inventory->slots[index] = item;
                
                added_to_inventory = true;
                break;
            }
        }
    }
    
    if(added_to_inventory)
    {
        log_add_item_action_text(ui, item, ItemActionType_PickUp);
        zero_struct(item->pos);
        
        if(added_to_stack)
        {
            remove_item_from_game(item);
        }
        
        game->action_count += 1.0f;
    }
    else
    {
        log_add(ui, "Your inventory is full.");
    }
    
    return(added_to_inventory);
}

internal void
remove_item_from_inventory(Random *random,
                           Item *item,
                           ItemState *items,
                           Inventory *inventory,
                           v2u pos)
{
    b32 should_remove_item_from_game = false;
    
    if(is_item_consumable(item->type))
    {
        Item *found_item = get_item_on_pos(items, pos, item->id);
        if(found_item)
        {
            // Add the item stack to the found_item stack.
            found_item->c.stack_count += item->c.stack_count;
            should_remove_item_from_game = true;
        }
    }
    
        // Get the item inventory slot index so we can remove it from inventory slots.
        b32 item_inventory_index_found = false;
        for(u32 index = 0; index < MAX_INVENTORY_SLOT_COUNT; ++index)
        {
            Item *inventory_item = inventory->slots[index];
            
            if(is_item_valid(inventory_item) &&
               inventory_item->letter == item->letter)
            {
                item_inventory_index_found = true;
                inventory->slots[index] = 0;
                break;
            }
        }
        
        assert(item_inventory_index_found);
        unset(inventory->flags, InventoryFlags_Examining);
        //printf("should_remove_item_from_game: %u\n", should_remove_item_from_game);
        
    // If the item you are dropping already exists on the current tile, then we
    // add the stack_count of the item to the stack_count of the item on the floor.
    // This means we have to remove the original item from the game here.
        if(should_remove_item_from_game)
        {
            remove_item_from_game(item);
        }
        else
        {
            unset(item->flags, ItemFlags_Inventory | ItemFlags_Equipped);
            item->pos = pos;
        }
}

internal void
remove_item_from_inventory_and_game(Random *random,
                                    Item *item,
                                    ItemState *items,
                                    Inventory *inventory)
{
    remove_item_from_inventory(random, item, items, inventory, make_v2u(0, 0));
        remove_item_from_game(item);
}

internal void
consume_consumable(Game *game, Entity *player, Item *item, ItemState *items, Inventory *inventory, UI *ui)
{
    if(item->type == ItemType_Potion)
    {
        set_as_known_and_identify_existing(item->id, items);
        
        switch(item->id)
        {
            case ItemID_MightPotion:
            {
                log_add(ui, "You drink the %s, you feel more mighty.", item->name);
                start_entity_status_effect(player, item->c.status_effect);
            } break;
            
            case ItemID_WisdomPotion:
            {
                log_add(ui, "You drink the %s, you feel more wise.", item->name);
                start_entity_status_effect(player, item->c.status_effect);
            } break;
            
            case ItemID_AgilityPotion:
            {
                log_add(ui, "You drink the %s, you feel more dexterous.", item->name);
                start_entity_status_effect(player, item->c.status_effect);
            } break;
            
            case ItemID_ElusionPotion:
            {
                log_add(ui, "You drink the %s, you feel more evasive.", item->name);
                start_entity_status_effect(player, item->c.status_effect);
            } break;
            
            case ItemID_HealingPotion:
            {
                if(player->hp == player->max_hp)
                {
                    log_add(ui, "You drink the %s, you feel no different.", item->name);
                }
                else
                {
                    log_add(ui, "You drink the %s, it heals you for %u health.", start_color(Color_LightGreen), item->name, item->c.heal_value);
                    heal_entity(player, item->c.heal_value);
                }
            } break;
            
            case ItemID_DecayPotion:
            {
                log_add(ui, "You drink the %s, you feel much weaker.", item->name);
                start_entity_status_effect(player, item->c.status_effect);
            } break;
            
            case ItemID_ConfusionPotion:
            {
                log_add(ui, "You drink the %s, you feel confused.", item->name);
                start_entity_status_effect(player, item->c.status_effect);
            } break;
            
            invalid_default_case;
        }
    }
    else if(item->type == ItemType_Ration)
    {
        if(player->hp == player->max_hp)
        {
            log_add(ui, "%sYou eat the ration.", start_color(Color_LightGray));
        }
        else
        {
            log_add(ui, "%sYou eat the ration, it heals you for %u health.", start_color(Color_LightGreen), item->c.heal_value);
            heal_entity(player, item->c.heal_value);
        }
    }
    
    inventory->view_update_item_type = item->type;
    remove_item_from_inventory_and_game(&game->random, item, items, inventory);
    game->action_count = 1.0f;
}

internal void
read_scroll(Game *game, Entity *player, Item *item, ItemState *items, Inventory *inventory, Dungeon *dungeon, UI *ui)
{
    set_as_known_and_identify_existing(item->id, items);
    inventory->view_update_item_type = item->type;
    game->action_count = 1.0f;
    
    switch(item->id)
    {
        case ItemID_IdentifyScroll:
        {
            log_add(ui, "You read the scroll, choose an item to identify.");
            inventory->using_item_type = UsingItemType_Identify;
        } break;
        
        case ItemID_EnchantWeaponScroll:
        {
            log_add(ui, "You read the scroll, choose a weapon to enchant.");
            inventory->using_item_type = UsingItemType_EnchantWeapon;
        } break;
        
        case ItemID_EnchantArmorScroll:
        {
            log_add(ui, "You read the scroll, choose an armor to enchant.");
            inventory->using_item_type = UsingItemType_EnchantArmor;
        } break;
        
        case ItemID_MagicMappingScroll:
        {
            log_add(ui, "You read the scroll, your surroundings become clear to you.");
            
            for(u32 y = 0; y < dungeon->height; ++y)
            {
                for(u32 x = 0; x < dungeon->width; ++x)
                {
                    set_tile_has_been_seen(dungeon->tiles, make_v2u(x, y), true);
                }
            }
            
            remove_item_from_inventory_and_game(&game->random, item, items, inventory);
        } break;
        
        case ItemID_TeleportationScroll:
        {
            log_add(ui, "You read the scroll, you find yourself in a different place.");
            
            for(;;)
            {
                v2u pos = random_dungeon_pos(&game->random, dungeon);
                if(is_tile_traversable_and_not_occupied(dungeon->tiles, pos))
                {
                    move_entity(player, dungeon->tiles, pos);
                    break;
                }
            }
            
            update_fov(player, dungeon);
            remove_item_from_inventory_and_game(&game->random, item, items, inventory);
        } break;
        
        case ItemID_UncurseScroll:
        {
            log_add(ui, "You read the scroll, choose an item to uncurse.");
            inventory->using_item_type = UsingItemType_Uncurse;
        } break;
        
        invalid_default_case;
    }
    
    unset(inventory->flags, InventoryFlags_Examining);
}

internal void
use_inventory_item(Random *random,
                   char pressed,
                   Item *examine_item,
                   ItemState *items,
                   Inventory *inventory,
                   UI *ui)
{
    for(u32 index = 0; index < MAX_INVENTORY_SLOT_COUNT; ++index)
    {
        Item *item = inventory->slots[index];
        if(item &&
           item->letter == pressed &&
           item_fits_using_item_type(inventory->using_item_type, item))
        {
            assert(inventory->using_item_type);
            
            if(inventory->using_item_type == UsingItemType_Identify)
            {
                set(item->flags, ItemFlags_Identified);
                log_add(ui, "You identify the %s.", get_full_item_name(item).str);
            }
            else if(inventory->using_item_type == UsingItemType_EnchantWeapon)
            {
                switch(random_number(random, 1, 4))
                {
                    case 1: log_add(ui, "%sThe %s glows blue for a moment..", start_color(Color_LightBlue), get_item_id_text(item->id)); break;
                    case 2: log_add(ui, "%sThe %s seems sharper than before..", start_color(Color_LightBlue), get_item_id_text(item->id)); break;
                    case 3: log_add(ui, "%sThe %s vibrates slightly..", start_color(Color_LightBlue), get_item_id_text(item->id)); break;
                    case 4: log_add(ui, "%sThe %s starts shimmering..", start_color(Color_LightBlue), get_item_id_text(item->id)); break;
                    
                    invalid_default_case;
                }
                
                ++item->enchantment_level;
            }
            else if(inventory->using_item_type == UsingItemType_EnchantArmor)
            {
                switch(random_number(random, 1, 4))
                {
                    case 1: log_add(ui, "%sThe %s glows white for a moment..", start_color(Color_LightBlue), get_item_id_text(item->id)); break;
                    case 2: log_add(ui, "%sThe %s looks sturdier than before..", start_color(Color_LightBlue), get_item_id_text(item->id)); break;
                    case 3: log_add(ui, "%sThe %s feels warm for a moment..", start_color(Color_LightBlue), get_item_id_text(item->id)); break;
                    case 4: log_add(ui, "%sThe %s feels different than before..", start_color(Color_LightBlue), get_item_id_text(item->id)); break;
                    
                    invalid_default_case;
                }
                
                ++item->enchantment_level;
            }
            else if(inventory->using_item_type == UsingItemType_Uncurse)
            {
                unset(item->flags, ItemFlags_Cursed);
                log_add(ui, "The %s seems slightly different now..", get_item_id_text(item->id));
            }
            
            remove_item_from_inventory_and_game(random, examine_item, items, inventory);
            inventory->using_item_type = UsingItemType_None;
            set_view_at_start(&inventory->view);
        }
    }
}

internal Item *
get_equipped_item_from_slot(ItemSlot slot, Inventory *inventory)
{
    Item *result = 0;
    
    for(u32 index = 0; index < MAX_INVENTORY_SLOT_COUNT; ++index)
    {
        Item *item = inventory->slots[index];
        if(item &&
           is_set(item->flags, ItemFlags_Equipped) &&
           item->slot == slot)
        {
            result = item;
            break;
        }
    }
    
    return(result);
}

internal void
log_add_cursed_unequip(UI *ui, Item *item)
{
    log_add(ui, "You try to unequip the %s.. but a force stops you from doing so!", item->name);
}

internal b32
unequip_item(Game *game, UI *ui, Item *item)
{
    b32 result = false;
    
    if(is_item_equipment(item->type) && is_set(item->flags, ItemFlags_Equipped))
    {
        if(is_set(item->flags, ItemFlags_Identified | ItemFlags_Cursed))
        {
            log_add_cursed_unequip(ui, item);
        }
        else
        {
            unset(item->flags, ItemFlags_Equipped);
            game->action_count = 1.0f;
            
            result = true;
        }
    }
    
    return(result);
}

internal char *
get_item_status_color(Item *item)
{
    char *result = start_color(Color_White);
    
    if(is_set(item->flags, ItemFlags_Identified))
    {
        if(is_set(item->flags, ItemFlags_Cursed))
        {
            result = start_color(Color_LightRed);
        }
        else
        {
            switch(item->rarity)
            {
                case ItemRarity_Common: result = start_color(Color_White); break;
                case ItemRarity_Magical: result = start_color(Color_DarkBlue); break;
                case ItemRarity_Mythical: result = start_color(Color_Orange); break;
                
                invalid_default_case;
            }
        }
    }
    
    return(result);
}

internal String128
get_item_stack_string(Item *item)
{
    String128 result = {0};
    
    if(is_item_consumable(item->type) && is_item_stacked(item))
    {
        sprintf(result.str, " (%u)", item->c.stack_count);
    }
    
    return(result);
}

internal String128
get_item_mark_string(Item *item)
{
    String128 result = {0};
    
    if(is_set(item->flags, ItemFlags_Marked))
    {
        sprintf(result.str, " {%s}", item->mark.array);
    }
    
    return(result);
}

internal void
log_add_item_action_text(UI *ui, Item *item, ItemActionType action)
{
    assert(action);
    
    char action_text[8] = {0};
    if(action == ItemActionType_PickUp)
    {
        strcpy(action_text, "pick up");
    }
    else if(action == ItemActionType_Drop)
    {
        strcpy(action_text, "drop");
    }
    else if(action == ItemActionType_Equip)
    {
        strcpy(action_text, "equip");
    }
    else if(action == ItemActionType_Unequip)
    {
        strcpy(action_text, "unequip");
    }
    
    log_add(ui, "You %s the %s%s%s%s%s%s",
            action_text,
            get_item_status_color(item),
            get_item_status_prefix(item),
            get_full_item_name(item).str,
            get_item_stack_string(item).str,
            get_item_mark_string(item).str,
            end_color());
}

internal void
equip_item(Game *game, Item *item, Inventory *inventory, UI *ui)
{
    if(is_item_equipment(item->type) && !is_set(item->flags, ItemFlags_Equipped))
    {
        b32 can_equip_item = true;
        
        Item *equipped_item = get_equipped_item_from_slot(item->slot, inventory);
        if(equipped_item)
        {
            if(!unequip_item(game, ui, equipped_item))
            {
                can_equip_item = false;
            }
        }
        
        if(can_equip_item)
        {
            if(is_set(item->flags, ItemFlags_Cursed))
            {
                log_add(ui, "%sThe %s feels like it's stuck to your hand.", start_color(Color_LightRed), get_item_id_text(item->id));
            }
            
            set(item->flags, ItemFlags_Identified | ItemFlags_Equipped);
            log_add_item_action_text(ui, item, ItemActionType_Equip);
            game->action_count += 1.0f;
        }
    }
}

internal void
drop_item(Game *game,
          Entity *player,
          Item *item,
          ItemState *items,
          Inventory *inventory,
          UI *ui)
{
    if(is_set(item->flags, ItemFlags_Equipped | ItemFlags_Cursed))
    {
        log_add_cursed_unequip(ui, item);
    }
    else
    {
        log_add_item_action_text(ui, item, ItemActionType_Drop);
        inventory->view_update_item_type = item->type;
        
        unset(player->flags, EntityFlags_MultipleItemNotify);
        unset(inventory->flags, InventoryFlags_Examining);
        
        if(is_set(item->flags, ItemFlags_Equipped))
        {
            game->action_count = 2.0f;
        }
        else
        {
            game->action_count = 1.0f;
        }
        
        remove_item_from_inventory(&game->random, item, items, inventory, player->pos);
    }
}

internal void
force_render_mark_cursor(Mark *mark)
{
    mark->should_render = true;
    mark->render_start = 0;
}

internal void
set_mark_view_and_cursor_to_start(Mark *mark)
{
    set_view_at_start(&mark->view);
    mark->cursor_index = 0;
}

internal void
set_mark_view_and_cursor_to_end(Mark *mark)
{
    if(is_view_scrolling(mark->view, mark->view.count + 1))
    {
        set_view_at_end(&mark->view);
    }
    
    mark->cursor_index = mark->view.count;
}

internal void
add_mark_character(Mark *mark, char c)
{
    if(mark->view.count < (MAX_MARK_SIZE - 1))
    {
        force_render_mark_cursor(mark);
        
        if(mark->cursor_index == mark->view.count)
        {
            mark->array[mark->view.count] = c;
        }
        else
        {
            // Move characters after mark->cursor_index towards buffer end
            for(u32 index = MAX_MARK_SIZE - 1; index > mark->cursor_index; --index)
            {
                mark->array[index] = mark->array[index - 1];
            }
            
            mark->array[mark->cursor_index] = c;
        }
        
#if 0
        printf("mark->cursor_index: %u\n", mark->cursor_index);
        printf("mark->view.count: %u\n", mark->view.count);
        printf("mark->view.start: %u\n", mark->view.start);
        printf("mark->view.end: %u\n\n", mark->view.end);
#endif
        
        ++mark->view.count;
        
        // If we are at the end of the view we see on screen then move the view.
        if(mark->cursor_index == get_view_range(mark->view) - 1)
        {
            if(is_view_scrolling(mark->view, mark->view.count))
            {
                ++mark->view.start;
            }
        }
        
        ++mark->cursor_index;
    }
}

internal void
update_item_marking(Input *input, Item *item, Inventory *inventory, UI *ui)
{
    Mark *mark = &ui->mark;
    
    assert(mark->view.end == 24);
    mark->view.count = strlen(mark->array);
    
    if(was_pressed(&input->Key_Enter))
    {
        // The array is not valid if it is empty or consists of only spaces
        b32 mark_array_is_valid = false;
        for(u32 index = 0; index < MAX_MARK_SIZE; ++index)
        {
            if(mark->array[index] &&
               mark->array[index] != ' ')
            {
                mark_array_is_valid = true;
                break;
            }
        }
        
        if(mark_array_is_valid)
        {
            item->mark.view = ui->mark.view;
            strcpy(item->mark.array, ui->mark.array);
            
            set(item->flags, ItemFlags_Marked);
        }
        else
        {
            unset(item->flags, ItemFlags_Marked);
        }
        
        // This data has to be reset so it doesn't appear in the mark of other items.
        zero_array(mark->array, MAX_MARK_SIZE);
        mark->view.count = 0;
        mark->view.start = 0;
        
        unset(inventory->flags, InventoryFlags_Marking);
    }
    else if(was_pressed(&input->Key_Escape))
    {
        zero_array(mark->array, MAX_MARK_SIZE);
        unset(inventory->flags, InventoryFlags_Marking);
    }
    else if(was_pressed(&input->Key_Del))
    {
        // Don't do this if we are at the end of the buffer.
        if((mark->cursor_index < mark->view.count) && mark->view.count)
        {
            // Remove the character at mark->cursor_index and move the buffer.
            for(u32 index = mark->cursor_index; index < MAX_MARK_SIZE; ++index)
            {
                mark->array[index] = mark->array[index + 1];
                mark->array[index + 1] = 0;
            }
            
            force_render_mark_cursor(mark);
        }
    }
    else if(was_pressed(&input->Key_Backspace))
    {
        if(mark->cursor_index && mark->view.count)
        {
            if(mark->cursor_index == mark->view.count)
            {
                --mark->cursor_index;
                mark->array[mark->cursor_index] = 0;
            }
            else
            {
                // Remove the character before mark->cursor_index and move the buffer.
                for(u32 index = mark->cursor_index; index < MAX_MARK_SIZE; ++index)
                {
                    mark->array[index - 1] = mark->array[index];
                    mark->array[index] = 0;
                }
                
                --mark->cursor_index;
            }
            
            if(is_view_scrolling(mark->view, mark->view.count) &&
               get_view_range(mark->view) > mark->view.count)
            {
                --mark->view.start;
            }
            
            force_render_mark_cursor(mark);
        }
    }
    else if(was_pressed(&input->Key_ArrowLeft))
    {
        if(mark->cursor_index)
        {
            if(is_view_scrolling(mark->view, mark->view.count + 1) &&
               mark->cursor_index == (mark->view.start - 1))
            {
                --mark->view.start;
            }
            
            --mark->cursor_index;
            force_render_mark_cursor(mark);
        }
    }
    else if(was_pressed(&input->Key_ArrowRight))
    {
        if(mark->cursor_index < MAX_MARK_SIZE &&
           mark->cursor_index < mark->view.count)
        {
            if(is_view_scrolling(mark->view, mark->view.count + 1) &&
               (mark->cursor_index == get_view_range(mark->view) - 1))
            {
                ++mark->view.start;
            }
            
            ++mark->cursor_index;
            force_render_mark_cursor(mark);
        }
    }
    else if(was_pressed(&input->Key_Home))
    {
        set_mark_view_and_cursor_to_start(mark);
        force_render_mark_cursor(mark);
    }
    else if(was_pressed(&input->Key_End))
    {
        set_mark_view_and_cursor_to_end(mark);
        force_render_mark_cursor(mark);
    }
    else
    {
        char pressed = get_pressed_keyboard_char(input);
        if(pressed)
        {
            add_mark_character(mark, pressed);
        }
    }
    
#if 0
    printf("mark->cursor_index: %u\n", mark->cursor_index);
    printf("mark->view.count: %u\n", mark->view.count);
    printf("mark->view.start: %u\n", mark->view.start);
    printf("mark->view.end: %u\n\n", mark->view.end);
#endif
    
}

internal u32
get_inventory_item_count(Inventory *inventory)
{
    u32 result = 0;
    
    for(u32 index = 0; index < MAX_INVENTORY_SLOT_COUNT; ++index)
    {
        if(inventory->slots[index])
        {
            ++result;
        }
    }
    
    return(result);
    }

internal b32
handle_new_pathfind_items(Tiles tiles, ItemState *items)
{
    b32 result = false;
    
    for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
    {
        Item *item = &items->array[index];
        if(is_item_valid_and_not_in_inventory(item) &&
           is_tile_seen(tiles, item->pos) &&
           !is_set(item->flags, ItemFlags_HasBeenSeen))
        {
            set(item->flags, ItemFlags_HasBeenSeen);
            result = true;
        }
    }
    
    return(result);
}

internal void
reset_item_selections(ItemState *items)
{
    for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
    {
        Item *item = &items->array[index];
        if(is_item_valid(item))
        {
            unset(item->flags, ItemFlags_Selected);
            item->selection_letter = 0;
        }
    }
}

internal u32
get_pos_item_count(ItemState *items, v2u pos)
{
    u32 result = 0;
    
    for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
    {
        Item *item = &items->array[index];
        if(is_item_valid_and_not_in_inventory(item) && equal_v2u(pos, item->pos))
        {
            ++result;
        }
    }
    
    return(result);
}

internal b32
is_item_id_weapon(ItemID id)
{
    b32 result = (id > ItemID_WeaponStart && id < ItemID_WeaponEnd);
    return(result);
}

internal b32
is_item_id_potion(ItemID id)
{
    b32 result = (id > ItemID_PotionStart && id < ItemID_PotionEnd);
    return(result);
}

internal ItemHandedness
get_item_handedness(ItemID id)
{
    ItemHandedness result = ItemHandedness_None;
    
    switch(id)
    {
        case ItemID_Dagger:
        case ItemID_Club:
        case ItemID_Sword:
        case ItemID_Battleaxe: result = ItemHandedness_OneHanded; break;
        
        case ItemID_Spear:
        case ItemID_Warhammer: result = ItemHandedness_TwoHanded; break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal v2u
get_item_equip_tile_pos(ItemID id, ItemRarity rarity)
{
    v2u result = {0};
    
    switch(id)
    {
        case ItemID_Dagger: result = make_v2u(33, 3); break;
        case ItemID_Club: result = make_v2u(33, 4); break;
        case ItemID_Sword: result = make_v2u(33, 5); break;
        case ItemID_Battleaxe: result = make_v2u(33, 6); break;
        case ItemID_Spear: result = make_v2u(33, 7); break;
        case ItemID_Warhammer: result = make_v2u(33, 7); break;
        
        case ItemID_LeatherHelmet: result = make_v2u(37, 3); break;
        case ItemID_LeatherChestplate: result = make_v2u(38, 3); break;
        case ItemID_LeatherGreaves: result = make_v2u(39, 3); break;
        case ItemID_LeatherBoots: result = make_v2u(40, 3); break;
        
        case ItemID_SteelHelmet: result = make_v2u(37, 4); break;
        case ItemID_SteelChestplate: result = make_v2u(38, 4); break;
        case ItemID_SteelGreaves: result = make_v2u(39, 4); break;
        case ItemID_SteelBoots: result = make_v2u(40, 4); break;
        
        invalid_default_case;
    }
    
    switch(rarity)
    {
        case ItemRarity_Common: break;
        case ItemRarity_Magical: result.x += 1; break;
        case ItemRarity_Mythical: result.x += 2; break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal v2u
get_item_tile_pos(ItemID id, ItemRarity rarity)
{
    v2u result = {0};
    
    switch(id)
    {
        case ItemID_Dagger: result = make_v2u(1, 2); break;
        case ItemID_Club: result = make_v2u(1, 3); break;
        case ItemID_Sword: result = make_v2u(1, 4); break;
        case ItemID_Battleaxe: result = make_v2u(1, 5); break;
        case ItemID_Spear: result = make_v2u(1, 6); break;
        case ItemID_Warhammer: result = make_v2u(1, 7); break;
        
        case ItemID_LeatherHelmet: result = make_v2u(5, 2); break;
        case ItemID_LeatherChestplate: result = make_v2u(6, 2); break;
        case ItemID_LeatherGreaves: result = make_v2u(7, 2); break;
        case ItemID_LeatherBoots: result = make_v2u(8, 2); break;
        
        case ItemID_SteelHelmet: result = make_v2u(5, 3); break;
        case ItemID_SteelChestplate: result = make_v2u(6, 3); break;
        case ItemID_SteelGreaves: result = make_v2u(7, 3); break;
        case ItemID_SteelBoots: result = make_v2u(8, 3); break;
        
        invalid_default_case;
    }
    
    switch(rarity)
    {
        case ItemRarity_Common: break;
        case ItemRarity_Magical: result.x += 1; break;
        case ItemRarity_Mythical: result.x += 2; break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal s32
get_item_enchantment_level(Random *random, ItemRarity rarity)
{
    s32 result = 0;
    
    switch(rarity)
    {
        case ItemRarity_Common: result = random_number(random, -1, 1); break;
        case ItemRarity_Magical: result = random_number(random, -2, 3); break;
        case ItemRarity_Mythical: result = random_number(random, -3, 5); break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal ItemType
random_item_type(Random *random)
{
    ItemType result = random_number(random, ItemType_None + 1, ItemType_Count - 1);
    return(result);
}

internal ItemID
random_weapon(Random *random)
{
    ItemID result = random_number(random, ItemID_WeaponStart + 1, ItemID_WeaponEnd - 1);
    return(result);
}

internal ItemID
random_leather_armor(Random *random)
{
    ItemID result = random_number(random, ItemID_LeatherHelmet, ItemID_LeatherBoots);
    return(result);
}

internal ItemID
random_steel_armor(Random *random)
{
    ItemID result = random_number(random, ItemID_SteelHelmet, ItemID_SteelBoots);
    return(result);
}

internal ItemID
random_potion(Random *random)
{
    ItemID result = random_number(random, ItemID_PotionStart + 1, ItemID_PotionEnd - 1);
    return(result);
}

internal ItemID
random_scroll(Random *random)
{
    ItemID result = random_number(random, ItemID_ScrollStart + 1, ItemID_ScrollEnd - 1);
    return(result);
}

internal u32
item_type_chance_index(ItemType type)
{
    u32 result = type - 1;
    return(result);
}

internal u32
potion_chance_index(ItemID id)
{
    u32 result = id - ItemID_PotionStart - 1;
    return(result);
}

internal u32
scroll_chance_index(ItemID id)
{
    u32 result = id - ItemID_ScrollStart - 1;
    return(result);
}

internal void
ask_for_confirm(Game *game, UI *ui, Inventory *inventory)
{
    set(inventory->flags, InventoryFlags_AskingPlayer);
    log_add(ui, "%sAre you sure?, [%c] Yes [%c] No.", start_color(Color_Yellow), game->keybinds[GameKey_Yes], game->keybinds[GameKey_No]);
}

internal void
ask_for_item_cancel(Game *game, UI *ui, Inventory *inventory)
{
    set(inventory->flags, InventoryFlags_AskingPlayer);
    log_add(ui, "%sCancel and waste the item?, [%c] Yes [%c] No.", start_color(Color_Yellow), game->keybinds[GameKey_Yes], game->keybinds[GameKey_No]);
}

internal DamageType
get_random_damage_type(Random *random)
{
    // Skips physical damage type
    DamageType result = random_number(random,
                                      DamageType_None + 2,
                                      DamageType_Count - 1);
    return(result);
}

internal char *
get_item_rarity_text(ItemRarity rarity)
{
    char *result = "";
    
    switch(rarity)
    {
        case ItemRarity_Common: result = "Common"; break;
        case ItemRarity_Magical: result = "Magical"; break;
        case ItemRarity_Mythical: result = "Mythical"; break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal char *
get_item_handedness_text(ItemHandedness handedness)
{
    char *result = "";
    
    switch(handedness)
    {
        case ItemHandedness_None: break;
        case ItemHandedness_OneHanded: result = "One-handed"; break;
        case ItemHandedness_TwoHanded: result = "Two-handed"; break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal void
render_items(Game *game, Entity *player, ItemState *items, Dungeon *dungeon, Assets *assets)
{
    for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
    {
        Item *item = &items->array[index];
        if(is_item_valid_and_not_in_inventory(item))
        {
            v4u src = get_tile_rect(item->tile_pos);
            v4u dest = get_game_dest(game, item->pos);
            
            if(is_tile_seen(dungeon->tiles, item->pos))
            {
                if(!is_set(player->flags, EntityFlags_Pathfinding))
                {
                    set(item->flags, ItemFlags_HasBeenSeen);
                }
                
                SDL_RenderCopy(game->renderer, assets->tileset.tex, (SDL_Rect *)&src, (SDL_Rect *)&dest);
                
                if(game->show_item_ground_outline)
                {
                    SDL_RenderCopy(game->renderer, assets->ui.tex, (SDL_Rect *)&assets->item_ground_outline_src, (SDL_Rect *)&dest);
                }
            }
            else if(has_tile_been_seen(dungeon->tiles, item->pos))
            {
                render_texture_half_color(game->renderer, assets->tileset.tex, src, dest, false);
                
                if(game->show_item_ground_outline)
                {
                    render_texture_half_color(game->renderer, assets->ui.tex, assets->item_ground_outline_src, dest, false);
                }
            }
        }
    }
}

internal Item *
get_item_on_pos(ItemState *items, v2u pos, ItemID id)
{
    Item *result = 0;
    
    for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
    {
        Item *item = &items->array[index];
        
        if(is_item_valid_and_not_in_inventory(item) && equal_v2u(item->pos, pos))
        {
            if(id && item->id != id)
            {
                continue;
            }
            
            result = item;
            break;
        }
    }
    
    return(result);
}

internal Item *
add_weapon_item(Random *random, ItemState *items,
                ItemID id, ItemRarity rarity,
                u32 x, u32 y, b32 is_cursed)
{
    assert(id);
    
    for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
    {
        Item *item = &items->array[index];
        if(!item->id)
        {
            if(is_cursed)
            {
                set(item->flags, ItemFlags_Cursed);
            }
            else
            {
                unset(item->flags, ItemFlags_Cursed);
            }
            
            item->id = id;
            item->pos = make_v2u(x, y);
            item->slot = ItemSlot_FirstHand;
            item-> handedness = get_item_handedness(item->id);
            item->rarity = rarity;
            item->tile_pos = get_item_tile_pos(item->id, item->rarity);
            item->equip_tile_pos = get_item_equip_tile_pos(item->id, item->rarity);
            item->first_damage_type = DamageType_Physical;
            item->enchantment_level = get_item_enchantment_level(random, item->rarity);
            item->type = ItemType_Weapon;
            
            // TODO(rami): Extra stats for mythical items.
            switch(id)
            {
                case ItemID_Dagger:
                {
                    item->w.damage = 4;
                    item->w.accuracy = 4;
                    item->w.speed = 1.0f;
                    
                    if(rarity == ItemRarity_Common)
                    {
                        strcpy(item->name, "Dagger");
                    }
                    else if(rarity == ItemRarity_Magical)
                    {
                        strcpy(item->name, "Dagger");
                        item->second_damage_type = get_random_damage_type(random);
                    }
                    else
                    {
                        random_name(random, item->name, NameType_Item);
                        item->second_damage_type = get_random_damage_type(random);
                        item->extra_stat_count = random_number(random, 1, 4);
                    }
                } break;
                
                case ItemID_Club:
                {
                    item->w.damage = 10;
                    item->w.accuracy = 0;
                    item->w.speed = 1.1f;
                    
                    if(rarity == ItemRarity_Common)
                    {
                        strcpy(item->name, "Club");
                    }
                    else if(rarity == ItemRarity_Magical)
                    {
                        strcpy(item->name, "Club");
                        item->second_damage_type = get_random_damage_type(random);
                    }
                    else if(rarity == ItemRarity_Mythical)
                    {
                        random_name(random, item->name, NameType_Item);
                        item->second_damage_type = get_random_damage_type(random);
                        item->extra_stat_count = random_number(random, 1, 4);
                    }
                } break;
                
                case ItemID_Sword:
                {
                    item->w.damage = 8;
                    item->w.accuracy = 2;
                    item->w.speed = 1.1f;
                    
                    if(rarity == ItemRarity_Common)
                    {
                        strcpy(item->name, "Sword");
                    }
                    else if(rarity == ItemRarity_Magical)
                    {
                        strcpy(item->name, "Sword");
                        item->second_damage_type = get_random_damage_type(random);
                    }
                    else if(rarity == ItemRarity_Mythical)
                    {
                        random_name(random, item->name, NameType_Item);
                        item->second_damage_type = get_random_damage_type(random);
                        item->extra_stat_count = random_number(random, 2, 4);
                    }
                } break;
                
                case ItemID_Battleaxe:
                {
                    item->w.damage = 12;
                    item->w.accuracy = -1;
                    item->w.speed = 1.3f;
                    
                    if(rarity == ItemRarity_Common)
                    {
                        strcpy(item->name, "Battleaxe");
                    }
                    else if(rarity == ItemRarity_Magical)
                    {
                        strcpy(item->name, "Battleaxe");
                        item->second_damage_type = get_random_damage_type(random);
                    }
                    else if(rarity == ItemRarity_Mythical)
                    {
                        random_name(random, item->name, NameType_Item);
                        item->second_damage_type = get_random_damage_type(random);
                        item->extra_stat_count = random_number(random, 1, 4);
                    }
                } break;
                
                case ItemID_Spear:
                {
                    item->w.damage = 16;
                    item->w.accuracy = -2;
                    item->w.speed = 1.4f;
                    
                    if(rarity == ItemRarity_Common)
                    {
                        strcpy(item->name, "Spear");
                    }
                    else if(rarity == ItemRarity_Magical)
                    {
                        strcpy(item->name, "Spear");
                        item->second_damage_type = get_random_damage_type(random);
                    }
                    else if(rarity == ItemRarity_Mythical)
                    {
                        random_name(random, item->name, NameType_Item);
                        item->second_damage_type = get_random_damage_type(random);
                        item->extra_stat_count = random_number(random, 1, 4);
                    }
                } break;
                
                case ItemID_Warhammer:
                {
                    item->w.damage = 20;
                    item->w.accuracy = -3;
                    item->w.speed = 1.5f;
                    
                    if(rarity == ItemRarity_Common)
                    {
                        strcpy(item->name, "Warhammer");
                    }
                    else if(rarity == ItemRarity_Magical)
                    {
                        strcpy(item->name, "Warhammer");
                        item->second_damage_type = get_random_damage_type(random);
                    }
                    else if(rarity == ItemRarity_Mythical)
                    {
                        random_name(random, item->name, NameType_Item);
                        item->second_damage_type = get_random_damage_type(random);
                        item->extra_stat_count = random_number(random, 1, 4);
                    }
                } break;
                
                invalid_default_case;
            }
            
            return(item);
        }
    }
    
    assert(false);
}

internal Item *
add_armor_item(Random *random, ItemState *items, ItemID id, u32 x, u32 y, b32 is_cursed)
{
    assert(id);
    
    for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
    {
        Item *item = &items->array[index];
        if(!item->id)
        {
            if(is_cursed)
            {
                set(item->flags, ItemFlags_Cursed);
            }
            else
            {
                unset(item->flags, ItemFlags_Cursed);
            }
            
            item->id = id;
            item->pos = make_v2u(x, y);
            item->rarity = ItemRarity_Common;
            item->tile_pos = get_item_tile_pos(item->id, item->rarity);
            item->equip_tile_pos = get_item_equip_tile_pos(item->id, item->rarity);
            item->type = ItemType_Armor;
            item->enchantment_level = random_number(random, -1, 1);
            
            switch(item->id)
            {
                case ItemID_LeatherHelmet:
                {
                    strcpy(item->name, "Leather Helmet");
                    item->slot = ItemSlot_Head;
                    item->a.defence = 1;
                    item->a.weight = 1;
                } break;
                
                case ItemID_LeatherChestplate:
                {
                    strcpy(item->name, "Leather Chestplate");
                    item->slot = ItemSlot_Body;
                    item->a.defence = 3;
                    item->a.weight = 3;
                } break;
                
                case ItemID_LeatherGreaves:
                {
                    strcpy(item->name, "Leather Greaves");
                    item->slot = ItemSlot_Legs;
                    item->a.defence = 2;
                    item->a.weight = 2;
                } break;
                
                case ItemID_LeatherBoots:
                {
                    strcpy(item->name, "Leather Boots");
                    item->slot = ItemSlot_Feet;
                    item->a.defence = 1;
                    item->a.weight = 1;
                } break;
                
                case ItemID_SteelHelmet:
                {
                    strcpy(item->name, "Steel Helmet");
                    item->slot = ItemSlot_Head;
                    item->a.defence = 4;
                    item->a.weight = 2;
                } break;
                
                case ItemID_SteelChestplate:
                {
                    strcpy(item->name, "Steel Chestplate");
                    item->slot = ItemSlot_Body;
                    item->a.defence = 8;
                    item->a.weight = 6;
                } break;
                
                case ItemID_SteelGreaves:
                {
                    strcpy(item->name, "Steel Greaves");
                    item->slot = ItemSlot_Legs;
                    item->a.defence = 6;
                    item->a.weight = 4;
                } break;
                
                case ItemID_SteelBoots:
                {
                    strcpy(item->name, "Steel Boots");
                    item->slot = ItemSlot_Feet;
                    item->a.defence = 4;
                    item->a.weight = 2;
                } break;
                
                invalid_default_case;
            }
            
            return(item);
        }
    }
    
    assert(false);
}

internal Item *
add_consumable_item(Random *random, ItemState *items, ItemID id, u32 x, u32 y, u32 stack_count)
{
    assert(id && stack_count);
    
    for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
    {
        Item *item = &items->array[index];
        if(!item->id)
        {
            item->id = id;
            item->pos = make_v2u(x, y);
            item->rarity = ItemRarity_Common;
            item->c.stack_count = stack_count;
            
            switch(id)
            {
                case ItemID_MightPotion:
                {
                    ConsumableInfo *info = &items->potion_info[Potion_Might];
                    
                    item->c.status_effect.type = StatusEffectType_Might;
                    item->c.status_effect.value = 2;
                    item->c.status_effect.duration = 40;
                    strcpy(item->c.depiction, info->depiction);
                    
                    strcpy(item->name, "Potion of Might");
                    sprintf(item->description, "Grants +%u Strength for %u turns.", item->c.status_effect.value, item->c.status_effect.duration);
                    item->type = ItemType_Potion;
                    item->tile_pos = info->tile;
                    
                    if(info->known)
                    {
                        set(item->flags, ItemFlags_Identified);
                    }
                } break;
                
                case ItemID_WisdomPotion:
                {
                    ConsumableInfo *info = &items->potion_info[Potion_Wisdom];
                    
                    item->c.status_effect.type = StatusEffectType_Wisdom;
                    item->c.status_effect.value = 2;
                    item->c.status_effect.duration = 40;
                    strcpy(item->c.depiction, info->depiction);
                    
                    strcpy(item->name, "Potion of Wisdom");
                    sprintf(item->description, "Grants +%u Intelligence for %u turns.", item->c.status_effect.value, item->c.status_effect.duration);
                    item->type = ItemType_Potion;
                    item->tile_pos = info->tile;
                    
                    if(info->known)
                    {
                        set(item->flags, ItemFlags_Identified);
                    }
                } break;
                
                case ItemID_AgilityPotion:
                {
                    ConsumableInfo *info = &items->potion_info[Potion_Agility];
                    
                    item->c.status_effect.type = StatusEffectType_Agility;
                    item->c.status_effect.value = 2;
                    item->c.status_effect.duration = 40;
                    strcpy(item->c.depiction, info->depiction);
                    
                    strcpy(item->name, "Potion of Agility");
                    sprintf(item->description, "Grants +%u Dexterity for %u turns.", item->c.status_effect.value, item->c.status_effect.duration);
                    item->type = ItemType_Potion;
                    item->tile_pos = info->tile;
                    
                    if(info->known)
                    {
                        set(item->flags, ItemFlags_Identified);
                    }
                } break;
                
                case ItemID_ElusionPotion:
                {
                    ConsumableInfo *info = &items->potion_info[Potion_Elusion];
                    
                    item->c.status_effect.type = StatusEffectType_Elusion;
                    item->c.status_effect.value = 2;
                    item->c.status_effect.duration = 40;
                    strcpy(item->c.depiction, info->depiction);
                    
                    strcpy(item->name, "Potion of Elusion");
                    sprintf(item->description, "Grants +%u Evasion for %u turns.", item->c.status_effect.value, item->c.status_effect.duration);
                    item->type = ItemType_Potion;
                    item->tile_pos = info->tile;
                    
                    if(info->known)
                    {
                        set(item->flags, ItemFlags_Identified);
                    }
                } break;
                
                case ItemID_HealingPotion:
                {
                    ConsumableInfo *info = &items->potion_info[Potion_Healing];
                    
                    strcpy(item->c.depiction, info->depiction);
                    item->c.heal_value = random_number(random,
                                                           items->potion_healing_range.min,
                                                           items->potion_healing_range.max);
                    
                    strcpy(item->name, "Potion of Healing");
                    sprintf(item->description, "Restores your health for %u - %u.", items->potion_healing_range.min, items->potion_healing_range.max);
                    item->type = ItemType_Potion;
                    item->tile_pos = info->tile;
                    
                    if(info->known)
                    {
                        set(item->flags, ItemFlags_Identified);
                    }
                } break;
                
                case ItemID_DecayPotion:
                {
                    ConsumableInfo *info = &items->potion_info[Potion_Decay];
                    
                    item->c.status_effect.type = StatusEffectType_Decay;
                    item->c.status_effect.value = 2;
                    item->c.status_effect.duration = 40;
                    strcpy(item->c.depiction, info->depiction);
                    
                    strcpy(item->name, "Potion of Decay");
                    sprintf(item->description, "Reduces -%u Strength, Intelligence and Dexterity for %u turns.", item->c.status_effect.value, item->c.status_effect.duration);
                    item->type = ItemType_Potion;
                    item->tile_pos = info->tile;
                    
                    if(info->known)
                    {
                        set(item->flags, ItemFlags_Identified);
                    }
                } break;
                
                case ItemID_ConfusionPotion:
                {
                    ConsumableInfo *info = &items->potion_info[Potion_Confusion];
                    
                    item->c.status_effect.type = StatusEffectType_Confusion;
                    item->c.status_effect.chance = 33;
                    item->c.status_effect.duration = 40;
                    strcpy(item->c.depiction, info->depiction);
                    
                    strcpy(item->name, "Potion of Confusion");
                    sprintf(item->description, "Confuses you for %u turns.", item->c.status_effect.duration);
                    item->type = ItemType_Potion;
                    item->tile_pos = info->tile;
                    
                    if(info->known)
                    {
                        set(item->flags, ItemFlags_Identified);
                    }
                } break;
                
                case ItemID_IdentifyScroll:
                {
                    ConsumableInfo *info = &items->scroll_info[Scroll_Identify];
                    strcpy(item->c.depiction, info->depiction);
                    
                    strcpy(item->name, "Scroll of Identify");
                    strcpy(item->description, "Identify a single item.");
                    item->type = ItemType_Scroll;
                    item->tile_pos = info->tile;
                    
                    if(info->known)
                    {
                        set(item->flags, ItemFlags_Identified);
                    }
                } break;
                
                case ItemID_EnchantWeaponScroll:
                {
                    ConsumableInfo *info = &items->scroll_info[Scroll_EnchantWeapon];
                    strcpy(item->c.depiction, info->depiction);
                    
                    strcpy(item->name, "Scroll of Enchant Weapon");
                    strcpy(item->description, "Enchant a weapon with +1 damage and accuracy.");
                    item->type = ItemType_Scroll;
                    item->tile_pos = info->tile;
                    
                    if(info->known)
                    {
                        set(item->flags, ItemFlags_Identified);
                    }
                } break;
                
                case ItemID_EnchantArmorScroll:
                {
                    ConsumableInfo *info = &items->scroll_info[Scroll_EnchantArmor];
                    strcpy(item->c.depiction, info->depiction);
                    
                    strcpy(item->name, "Scroll of Enchant Armor");
                    strcpy(item->description, "Enchant a piece of armor with +1 defence.");
                    item->type = ItemType_Scroll;
                    item->tile_pos = info->tile;
                    
                    if(info->known)
                    {
                        set(item->flags, ItemFlags_Identified);
                    }
                } break;
                
                case ItemID_MagicMappingScroll:
                {
                    ConsumableInfo *info = &items->scroll_info[Scroll_MagicMapping];
                    strcpy(item->c.depiction, info->depiction);
                    
                    strcpy(item->name, "Scroll of Magic Mapping");
                    strcpy(item->description, "Reveals the layout of the current level.");
                    item->type = ItemType_Scroll;
                    item->tile_pos = info->tile;
                    
                    if(info->known)
                    {
                        set(item->flags, ItemFlags_Identified);
                    }
                } break;
                
                case ItemID_TeleportationScroll:
                {
                    ConsumableInfo *info = &items->scroll_info[Scroll_Teleportation];
                    strcpy(item->c.depiction, info->depiction);
                    
                    strcpy(item->name, "Scroll of Teleportation");
                    strcpy(item->description, "Teleports you to a random position on the level.");
                    item->type = ItemType_Scroll;
                    item->tile_pos = info->tile;
                    
                    if(info->known)
                    {
                        set(item->flags, ItemFlags_Identified);
                    }
                } break;
                
                case ItemID_UncurseScroll:
                {
                    ConsumableInfo *info = &items->scroll_info[Scroll_Uncurse];
                    strcpy(item->c.depiction, info->depiction);
                    
                    strcpy(item->name, "Scroll of Uncurse");
                    strcpy(item->description, "Removes the a curse from a cursed item.");
                    item->type = ItemType_Scroll;
                    item->tile_pos = info->tile;
                    
                    if(info->known)
                    {
                        set(item->flags, ItemFlags_Identified);
                    }
                } break;
                
                case ItemID_Ration:
                {
                    item->c.heal_value = random_number(random,
                                                           items->ration_healing_range.min,
                                                           items->ration_healing_range.max);
                    
                    strcpy(item->name, "Ration");
                    sprintf(item->description, "Restores your health for %u - %u.", items->ration_healing_range.min, items->ration_healing_range.max);
                    item->type = ItemType_Ration;
                    item->tile_pos = make_v2u(12, random_number(random, 2, 4));
                    set(item->flags, ItemFlags_Identified);
                } break;
                
                invalid_default_case;
            }
            
            return(item);
        }
    }
    
    assert(false);
}