internal void
start_item_examine(Inventory *inventory, Item *item)
{
    assert(inventory);
    assert(item);
    
    set(inventory->flags, InventoryFlag_Examine);
    inventory->examine_item = item;
}

internal b32
item_fits_using_item_type(Item *item, UsedItemType type)
{
    assert(item);
    assert(type);
    
    if(type == UsedItemType_EnchantArmor && item->type == ItemType_Armor) return(true);
    else if(type == UsedItemType_EnchantWeapon && item->type == ItemType_Weapon) return(true);
    else if(type == UsedItemType_Identify && !is_set(item->flags, ItemFlag_IsIdentified)) return(true);
    else if(type == UsedItemType_Uncurse && is_set(item->flags, ItemFlag_IsIdentified | ItemFlag_IsCursed)) return(true);
    
    return(false);
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

internal b32
is_item_id_scroll(ItemID id)
{
    b32 result = (id > ItemID_ScrollStart && id < ItemID_ScrollEnd);
    return(result);
}

internal void
copy_consumable_info_to_item(Item *item, ConsumableInfo *info)
{
    assert(item);
    assert(info);
    
    item->tile_src = info->tile_src;
    strcpy(item->c.depiction.s, info->depiction.s);
    
    if(info->is_known)
    {
        set(item->flags, ItemFlag_IsIdentified);
    }
}

internal char *
get_item_slot_text(ItemSlot slot)
{
    char *result = 0;
    
    switch(slot)
    {
        case ItemSlot_Head: result = "Head"; break;
        case ItemSlot_Body: result = "Body"; break;
        case ItemSlot_Legs: result = "Legs"; break;
        case ItemSlot_Feet: result = "Feet"; break;
        case ItemSlot_Amulet: result = "Amulet"; break;
        case ItemSlot_SecondHand: result = "Second Hand"; break;
        case ItemSlot_FirstHand: result = "First Hand"; break;
        case ItemSlot_Ring: result = "Ring"; break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal char *
get_item_handedness_text(ItemHandedness handedness)
{
    char *result = 0;
    
    switch(handedness)
    {
        case ItemHandedness_OneHanded: result = "One-Handed"; break;
        case ItemHandedness_TwoHanded: result = "Two-Handed"; break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal char *
get_item_type_text(ItemType type)
{
    char *result = 0;
    
    switch(type)
    {
        case ItemType_Weapon: result = "Weapon"; break;
        case ItemType_Armor: result = "Armor"; break;
        case ItemType_Potion: result = "Potion"; break;
        case ItemType_Scroll: result = "Scroll"; break;
        case ItemType_Ration: result = "Ration"; break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal b32
is_item_valid(Item *item, u32 dungeon_level)
{
    b32 result = (item &&
                      item->id &&
                      item->dungeon_level == dungeon_level);
    
    return(result);
}

internal b32
is_item_valid_and_selected(Item *item, u32 dungeon_level)
{
    b32 result = (is_item_valid(item, dungeon_level) &&
                      is_set(item->flags, ItemFlag_IsSelected));
    
    return(result);
}

internal b32
is_item_valid_and_in_inventory(Item *item, u32 dungeon_level)
{
    b32 result = (is_item_valid(item, dungeon_level) &&
                      is_set(item->flags, ItemFlag_InInventory));
    
    return(result);
}

internal b32
is_item_valid_and_not_in_inventory(Item *item, u32 dungeon_level)
{
    b32 result = (is_item_valid(item, dungeon_level) &&
                      !is_set(item->flags, ItemFlag_InInventory));
    
    return(result);
}

internal b32
is_item_valid_and_not_in_inventory_and_on_pos(Item *item, v2u pos, u32 dungeon_level)
{
    b32 result = (is_item_valid_and_not_in_inventory(item, dungeon_level) &&
                  is_v2u_equal(item->pos, pos));
    
    return(result);
}

internal void
print_dungeon_items(ItemState *items, Dungeon *dungeon)
{
    printf("\nDungeon Level %u Items:\n", dungeon->level);
    
    for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
    {
        Item *item = &items->array[index];
        
        if(is_item_valid_and_not_in_inventory(item, dungeon->level))
        {
            printf("%s at %u, %u\n", item->name.s, item->pos.x, item->pos.y);
        }
    }
}

internal char *
get_item_status_prefix(Item *item)
{
    char *result = "";
    
    if(is_set(item->flags, ItemFlag_IsIdentified | ItemFlag_IsCursed))
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
        case DamageType_Dark: result = "Dark"; break;
        
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
        if(is_set(item->flags, ItemFlag_IsIdentified))
        {
            if(item->w.second_damage_type)
            {
                sprintf(result.s, "%c%d %s of %s",
                            get_sign(item->enchantment_level),
                            get_absolute(item->enchantment_level),
                            item->name.s,
                        get_damage_type_text(item->w.second_damage_type));
            }
            else
            {
                sprintf(result.s, "%c%d %s",
                            get_sign(item->enchantment_level),
                            get_absolute(item->enchantment_level),
                            item->name.s);
            }
        }
        else
        {
            sprintf(result.s, "%s", get_item_id_text(item->id));
        }
    }
    else
    {
        if(is_set(item->flags, ItemFlag_IsIdentified))
        {
            sprintf(result.s, "%s", item->name.s);
        }
        else
        {
            sprintf(result.s, "%s%s", item->c.depiction.s, get_item_id_text(item->id));
        }
    }
    
    return(result);
}

internal void
update_item_adjust(Input *input,
                      Item *item,
                      ItemState *items,
                      Inventory *inventory,
                      UI *ui,
                      u32 dungeon_level)
{
    char pressed = get_pressed_alphabet_char(input);
    if(pressed)
    {
        if(pressed != item->inventory_letter)
        {
            Owner *new_owner = get_owner_from_letter(inventory->item_owners, pressed);
            if(new_owner->type)
            {
                Owner *current_owner = get_owner_from_letter(inventory->item_owners, item->inventory_letter);
                
                // Swap can only happen between two items
                assert(current_owner->type == OwnerType_Item);
                assert(new_owner->type == OwnerType_Item);
                
                // temp = current
                char temp_letter = current_owner->item->inventory_letter;
                Item *temp_item = current_owner->item;
                
                // current = new
                current_owner->item->inventory_letter = new_owner->item->inventory_letter;
                current_owner->item = new_owner->item;
                
                // new = temp
                new_owner->item->inventory_letter = temp_letter;
                new_owner->item = temp_item;
            }
            else
            {
                // Clear current letter
                clear_owners(inventory->item_owners, &item->inventory_letter);
                
                // Set new letter
                Owner *new_letter = get_owner_from_letter(inventory->item_owners, pressed);
                item->inventory_letter = set_owner_src(new_letter, item, OwnerType_Item);
            }
        }
        
        unset(inventory->flags, InventoryFlag_Adjust);
        log_add("%s%s%s",
                ui,
                    get_item_letter_string(item).s,
                get_item_status_prefix(item),
                get_full_item_name(item).s);
    }
}

internal void
set_potion_or_scroll_as_known(ItemState *items, ItemID id)
{
    assert(items);
    assert(is_item_id_potion(id) || is_item_id_scroll(id));
    
    ConsumableInfo *potion_info = items->potion_info;
    ConsumableInfo *scroll_info = items->scroll_info;
    
    // Mark the ID as known for future item generation
    switch(id)
    {
        case ItemID_MightPotion: potion_info[Potion_Might].is_known = true; break;
        case ItemID_WisdomPotion: potion_info[Potion_Wisdom].is_known = true; break;
        case ItemID_AgilityPotion: potion_info[Potion_Agility].is_known = true; break;
        case ItemID_ElusionPotion: potion_info[Potion_Elusion].is_known = true; break;
        case ItemID_HealingPotion: potion_info[Potion_Healing].is_known = true; break;
        case ItemID_DecayPotion: potion_info[Potion_Decay].is_known = true; break;
        case ItemID_ConfusionPotion: potion_info[Potion_Confusion].is_known = true; break;
        
        case ItemID_IdentifyScroll: scroll_info[Scroll_Identify].is_known = true; break;
        case ItemID_EnchantWeaponScroll: scroll_info[Scroll_EnchantWeapon].is_known = true; break;
        case ItemID_EnchantArmorScroll: scroll_info[Scroll_EnchantArmor].is_known = true; break;
        case ItemID_MagicMappingScroll: scroll_info[Scroll_MagicMapping].is_known = true; break;
        case ItemID_TeleportationScroll: scroll_info[Scroll_Teleport].is_known = true; break;
        case ItemID_UncurseScroll: scroll_info[Scroll_Uncurse].is_known = true; break;
        
        case ItemID_Ration: break;
        
        invalid_default_case;
    }
    
    // Identify all the items with the passed ID
    for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
    {
        Item *item = &items->array[index];
        if(id == item->id)
        {
            set(item->flags, ItemFlag_IsIdentified);
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
add_item_to_inventory(Game *game,
                      Entity *player,
                      Item *item,
                      ItemState *items,
                      Inventory *inventory,
                      UI *ui,
                      u32 dungeon_level,
                      b32 add_to_log)
{
    b32 added_to_stack = false;
    b32 added_to_inventory = false;
    
    // If the same item exists in the inventory then combine their stacks
    if(is_item_consumable(item->type))
    {
        for(u32 index = 0; index < MAX_INVENTORY_SLOT_COUNT; ++index)
        {
            Item *inventory_item = inventory->slots[index];
            
            if(is_item_valid(inventory_item, dungeon_level) &&
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
                if(!item->inventory_letter)
                {
                    item->inventory_letter = add_new_char_to_owners(inventory->item_owners, item, OwnerType_Item);
                }
                
                unset(player->flags, EntityFlag_NotifyAboutMultipleItems);
                set(item->flags, ItemFlag_InInventory);
                inventory->slots[index] = item;
                
                added_to_inventory = true;
                break;
            }
        }
    }
    
    if(added_to_inventory)
    {
        zero_struct(item->pos);
        
        if(add_to_log)
        {
        log_add_item_action_text(item, ui, ItemActionType_PickUp);
        }
        
        if(added_to_stack)
        {
            remove_item_from_game(item);
        }
        
        game->action_time += player->p.turn_action_time;
    }
    else
    {
        log_add("Your inventory is full.", ui);
    }
    
    return(added_to_inventory);
}

internal void
remove_item_from_inventory(Item *item,
                           ItemState *items,
                           Inventory *inventory,
                           u32 dungeon_level)
{
    
    #if MOONBREATH_SLOW
    if(is_set(item->flags, ItemFlag_IsCursed))
    {
        assert(!is_set(item->flags, ItemFlag_IsIdentified));
    }
    #endif
    
        // Unset the inventory slot pointer for the item
    b32 found_inventory_index = false;
    
        for(u32 index = 0; index < MAX_INVENTORY_SLOT_COUNT; ++index)
        {
            Item *inventory_item = inventory->slots[index];
            
        if(is_item_valid(inventory_item, dungeon_level) &&
               inventory_item->inventory_letter == item->inventory_letter)
            {
                found_inventory_index = true;
                inventory->slots[index] = 0;
                
                break;
            }
        }
        assert(found_inventory_index);
        
    unset(item->flags, ItemFlag_IsEquipped | ItemFlag_InInventory);
    unset(inventory->flags, InventoryFlag_Examine);
}

internal void
remove_item_from_inventory_and_game(Item *item,
                                    ItemState *items,
                                    Inventory *inventory,
                                    u32 dungeon_level)
{
    remove_item_from_inventory(item, items, inventory, dungeon_level);
    remove_item_from_game(item);
}

internal void
end_consumable_use(Item *item,
                   ItemState *items,
                   Inventory *inventory,
                   u32 dungeon_level)
{
    assert(is_item_consumable(item->type));
    
    if(is_item_stacked(item))
    {
        --item->c.stack_count;
    }
    else
    {
        remove_item_from_inventory_and_game(item, items, inventory, dungeon_level);
    }
}

internal void
use_consumable(Game *game,
               Entity *player,
               Dungeon *dungeon,
                   Item *item,
                   ItemState *items,
                   Inventory *inventory,
                   UI *ui)
{
    assert(item);
    assert(is_item_valid(item, dungeon->level));
    assert(is_item_consumable(item->type));
    
    if(item->type != ItemType_Ration)
    {
        set_potion_or_scroll_as_known(items, item->id);
    }
    
    if(item->type == ItemType_Scroll)
    {
        switch(item->id)
        {
            case ItemID_IdentifyScroll:
            {
                log_add("You read the scroll, choose an item to identify.", ui);
                inventory->used_item_type = UsedItemType_Identify;
            } break;
            
            case ItemID_EnchantWeaponScroll:
            {
                log_add("You read the scroll, choose a weapon to enchant.", ui);
                inventory->used_item_type = UsedItemType_EnchantWeapon;
            } break;
            
            case ItemID_EnchantArmorScroll:
            {
                log_add("You read the scroll, choose an armor to enchant.", ui);
                inventory->used_item_type = UsedItemType_EnchantArmor;
            } break;
            
            case ItemID_MagicMappingScroll:
            {
                log_add("You read the scroll, your surroundings become clear to you.", ui);
                
                for(u32 y = 0; y < dungeon->size.h; ++y)
                {
                    for(u32 x = 0; x < dungeon->size.w; ++x)
                    {
                        set_tile_has_been_seen(dungeon->tiles, make_v2u(x, y), true);
                    }
                }
                
                remove_item_from_inventory_and_game(item, items, inventory, dungeon->level);
            } break;
            
            case ItemID_TeleportationScroll:
            {
                log_add("You read the scroll, you find yourself in a different place.", ui);
                
                teleport_entity(&game->random, player, dungeon, ui);
                remove_item_from_inventory_and_game(item, items, inventory, dungeon->level);
            } break;
            
            case ItemID_UncurseScroll:
            {
                log_add("You read the scroll, choose an item to uncurse.", ui);
                inventory->used_item_type = UsedItemType_Uncurse;
            } break;
            
            invalid_default_case;
        }
    }
    
    add_entity_status(player->statuses, &item->c.status);
    end_consumable_use(item, items, inventory, dungeon->level);
    
    unset(inventory->flags, InventoryFlag_Examine);
    game->action_time = player->p.turn_action_time;
}

internal void
use_inventory_item(Random *random,
                   char pressed,
                   Item *examine_item,
                   ItemState *items,
                   Inventory *inventory,
                   UI *ui,
                   u32 dungeon_level)
{
    for(u32 index = 0; index < MAX_INVENTORY_SLOT_COUNT; ++index)
    {
        Item *item = inventory->slots[index];
        if(item &&
               item->inventory_letter == pressed &&
               item_fits_using_item_type(item, inventory->used_item_type))
        {
            assert(inventory->used_item_type);
            
            if(inventory->used_item_type == UsedItemType_Identify)
            {
                set(item->flags, ItemFlag_IsIdentified);
                log_add("You identify the %s.", ui, get_full_item_name(item).s);
            }
            else if(inventory->used_item_type == UsedItemType_EnchantWeapon)
            {
                switch(get_random_number(random, 1, 4))
                {
                    case 1: log_add("%sThe %s glows blue for a moment..", ui, start_color(Color_LightBlue), get_item_id_text(item->id)); break;
                    case 2: log_add("%sThe %s seems sharper than before..", ui, start_color(Color_LightBlue), get_item_id_text(item->id)); break;
                    case 3: log_add("%sThe %s vibrates slightly..", ui, start_color(Color_LightBlue), get_item_id_text(item->id)); break;
                    case 4: log_add("%sThe %s starts shimmering..", ui, start_color(Color_LightBlue), get_item_id_text(item->id)); break;
                    
                    invalid_default_case;
                }
                
                ++item->enchantment_level;
            }
            else if(inventory->used_item_type == UsedItemType_EnchantArmor)
            {
                switch(get_random_number(random, 1, 4))
                {
                    case 1: log_add("%sThe %s glows white for a moment..", ui, start_color(Color_LightBlue), get_item_id_text(item->id)); break;
                    case 2: log_add("%sThe %s looks sturdier than before..", ui, start_color(Color_LightBlue), get_item_id_text(item->id)); break;
                    case 3: log_add("%sThe %s feels warm for a moment..", ui, start_color(Color_LightBlue), get_item_id_text(item->id)); break;
                    case 4: log_add("%sThe %s feels different than before..", ui, start_color(Color_LightBlue), get_item_id_text(item->id)); break;
                    
                    invalid_default_case;
                }
                
                ++item->enchantment_level;
            }
            else if(inventory->used_item_type == UsedItemType_Uncurse)
            {
                unset(item->flags, ItemFlag_IsCursed);
                log_add("The %s seems slightly different now..", ui, get_item_id_text(item->id));
            }
            
            end_consumable_use(item, items, inventory, dungeon_level);
            inventory->used_item_type = UsedItemType_None;
            set_view_at_start(&inventory->window.view);
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
               is_set(item->flags, ItemFlag_IsEquipped) &&
           item->slot == slot)
        {
            result = item;
            break;
        }
    }
    
    return(result);
}

internal b32
unequip_item(Game *game, Entity *player, Item *item, UI *ui)
{
    assert(is_entity_valid_and_player(player));
    
    b32 result = false;
    
    if(is_item_equipment(item->type) && is_set(item->flags, ItemFlag_IsEquipped))
    {
        if(is_set(item->flags, ItemFlag_IsIdentified | ItemFlag_IsCursed))
        {
            log_add_item_cursed_unequip(item, ui);
        }
        else
        {
            unset(item->flags, ItemFlag_IsEquipped);
            game->action_time = player->p.turn_action_time;
            
            assert(player->p.weapon);
            player->p.weapon = 0;
            
            result = true;
        }
    }
    
    return(result);
}

internal char *
get_item_status_color(Item *item)
{
    char *result = start_color(Color_White);
    
    if(is_set(item->flags, ItemFlag_IsIdentified))
    {
        if(is_set(item->flags, ItemFlag_IsCursed))
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
        sprintf(result.s, " (%u)", item->c.stack_count);
    }
    
    return(result);
}

internal String128
get_item_mark_string(Item *item)
{
    String128 result = {0};
    
    if(is_set(item->flags, ItemFlag_IsMarked))
    {
        sprintf(result.s, " {%s}", item->mark.array);
    }
    
    return(result);
}

internal void
log_add_item_cursed_unequip(Item *item, UI *ui)
{
    log_add("You try to unequip the %s.. but a force stops you from doing so!", ui, item->name.s);
}

internal void
log_add_item_action_text(Item *item, UI *ui, ItemActionType action)
{
    String8 action_string = {0};
    
    switch(action)
    {
        case ItemActionType_PickUp: strcpy(action_string.s, "pick up"); break;
        case ItemActionType_Drop: strcpy(action_string.s, "drop"); break;
        case ItemActionType_Equip: strcpy(action_string.s, "equip"); break;
        case ItemActionType_Unequip: strcpy(action_string.s, "unequip"); break;
        
        invalid_default_case;
    }
    
    log_add("You %s the %s%s%s%s%s%s",
            ui,
            action_string.s,
            get_item_status_color(item),
            get_item_status_prefix(item),
            get_full_item_name(item).s,
            get_item_stack_string(item).s,
            get_item_mark_string(item).s,
                end_color());
    }

internal void
equip_item(Game *game, Entity *player, Item *item, Inventory *inventory, UI *ui)
{
    assert(is_entity_valid_and_player(player));
    
    if(is_item_equipment(item->type) && !is_set(item->flags, ItemFlag_IsEquipped))
    {
        b32 can_equip_item = true;
        
        Item *equipped_item = get_equipped_item_from_slot(item->slot, inventory);
        if(equipped_item)
        {
            if(!unequip_item(game, player, equipped_item, ui))
            {
                can_equip_item = false;
            }
        }
        
        if(can_equip_item)
        {
            set(item->flags, ItemFlag_IsIdentified | ItemFlag_IsEquipped);
            log_add_item_action_text(item, ui, ItemActionType_Equip);
            game->action_time += player->p.turn_action_time;
            
            if(is_set(item->flags, ItemFlag_IsCursed))
            {
                log_add("%sThe %s feels like it's stuck to your hand.", ui, start_color(Color_LightRed), get_item_id_text(item->id));
            }
            
            player->p.weapon = item;
        }
    }
}

internal void
drop_item_from_inventory(Game *game,
                         Entity *player,
                         Item *item,
                         ItemState *items,
                         Inventory *inventory,
                         Dungeon *dungeon,
                         UI *ui)
{
    if(is_set(item->flags, ItemFlag_IsEquipped | ItemFlag_IsCursed))
    {
        log_add_item_cursed_unequip(item, ui);
    }
    else
    {
        unset(player->flags, EntityFlag_NotifyAboutMultipleItems);
        unset(inventory->flags, InventoryFlag_Examine);
        
        log_add_item_action_text(item, ui, ItemActionType_Drop);
        
        if(is_set(item->flags, ItemFlag_IsEquipped))
        {
            game->action_time = player->p.turn_action_time * 2.0f;
        }
        else
        {
            game->action_time = player->p.turn_action_time;
        }
        
        b32 remove_item_from_game = false;
        
            if(is_item_consumable(item->type))
            {
            // If the same item exists on the drop position then combine their stacks
            Item *pos_item = get_dungeon_pos_item(items, dungeon->level, player->pos, item->id);
            if(pos_item)
                {
                    
#if 0
                    printf("item_on_pos->c.stack_count: %u\n", item_on_pos->c.stack_count);
                    printf("item->c.stack_count: %u\n\n", item->c.stack_count);
#endif
                
                assert(is_item_consumable(pos_item->type));
                pos_item->c.stack_count += item->c.stack_count;
                
                // Item stacks have been combined, remove the original item from the game
                remove_item_from_game = true;
                remove_item_from_inventory_and_game(item, items, inventory, dungeon->level);
                }
            }
        
        // Remove the item from inventory if we didn't remove it from the game
        if(!remove_item_from_game)
        {
            item->pos = player->pos;
            item->dungeon_level = dungeon->level;
            clear_owners(inventory->item_owners, &item->inventory_letter);
            
            inventory->dropped_item_type = item->type;
            remove_item_from_inventory(item, items, inventory, dungeon->level);
            }
        }
}

internal void
force_render_mark_cursor(Mark *mark)
{
    mark->render_cursor = true;
    mark->render_cursor_start_ms = 0;
}

internal void
add_mark_character(Mark *mark, char c)
{
    assert(mark);
    assert(c);
    
    if(mark->view.count < (MAX_MARK_SIZE - 1))
    {
        force_render_mark_cursor(mark);
        
        if(mark->cursor == mark->view.count)
        {
            mark->array[mark->view.count] = c;
        }
        else
        {
            // Move characters after mark->cursor towards buffer end
            for(u32 index = MAX_MARK_SIZE - 1; index > mark->cursor; --index)
            {
                mark->array[index] = mark->array[index - 1];
            }
            
            mark->array[mark->cursor] = c;
        }
        
#if 0
        printf("mark->cursor: %u\n", mark->cursor);
        printf("mark->view.count: %u\n", mark->view.count);
        printf("mark->view.start: %u\n", mark->view.start);
        printf("mark->view.end: %u\n\n", mark->view.end);
#endif
        
        ++mark->view.count;
        
        // Move mark view if we are currently on the boundary of it
        if(mark->cursor == get_view_range(mark->view) - 1)
        {
            if(is_view_scrolling(mark->view))
            {
                ++mark->view.start;
            }
        }
        
        ++mark->cursor;
    }
}

internal void
move_mark_cursor_left(Mark *mark)
{
    assert(mark);
    
    if(is_view_scrolling_with_count(mark->view, mark->view.count + 1) &&
                   mark->cursor == (mark->view.start - 1))
                {
                    --mark->view.start;
                }
                
                --mark->cursor;
                force_render_mark_cursor(mark);
}

internal void
move_mark_cursor_right(Mark *mark)
{
    assert(mark);
    
    if(mark->cursor < MAX_MARK_SIZE &&
       mark->cursor < mark->view.count)
    {
        if(is_view_scrolling_with_count(mark->view, mark->view.count + 1) &&
           (mark->cursor == get_view_range(mark->view) - 1))
        {
            ++mark->view.start;
        }
        
        ++mark->cursor;
        force_render_mark_cursor(mark);
    }
}

internal void
remove_mark_char(Mark *mark)
{
    assert(mark);
    
    if(mark->cursor && mark->view.count)
    {
        // Remove the character before mark->cursor and move the buffer.
        for(u32 index = mark->cursor; index < MAX_MARK_SIZE; ++index)
        {
            mark->array[index - 1] = mark->array[index];
            mark->array[index] = 0;
        }
        
        if(mark->view.start > 1)
        {
            // If we're on the left edge of the buffer, move the view start to the left a single extra
            // time so we can see the next character on the left of the cursor.
            if(mark->cursor + 1 == mark->view.start)
            {
                --mark->view.start;
            }
            
                --mark->view.start;
        }
        
        --mark->cursor;
        force_render_mark_cursor(mark);
        }
    }

internal void
move_mark_to_start(Mark *mark)
{
    assert(mark);
    
    set_view_at_start(&mark->view);
    mark->cursor = 0;
}

internal void
move_mark_to_end(Mark *mark)
{
    assert(mark);
    
    if(is_view_scrolling_with_count(mark->view, mark->view.count + 1))
    {
        set_view_at_end(&mark->view);
    }
    
    mark->cursor = mark->view.count;
}

internal void
update_item_mark(Input *input, Mark *mark, Item *item, u32 *inventory_flags)
{
    assert(mark->view.end == 24);
    mark->view.count = get_string_length(mark->array);
    
    if(was_pressed(&input->Key_Enter))
    {
        // The mark is not valid if it's empty or consists of only spaces
        b32 is_mark_valid = false;
        
        for(u32 index = 0; index < MAX_MARK_SIZE; ++index)
        {
            if(mark->array[index] &&
                   mark->array[index] != ' ')
            {
                is_mark_valid = true;
                break;
            }
        }
        
        if(is_mark_valid)
        {
            // Copy the item mark to be the temp mark if it's valid
            item->mark.view = mark->view;
            strcpy(item->mark.array, mark->array);
            
            set(item->flags, ItemFlag_IsMarked);
        }
        else
        {
            unset(item->flags, ItemFlag_IsMarked);
        }
        
        // This data has to be reset so it doesn't appear in the mark of other items
        zero_array(mark->array, MAX_MARK_SIZE);
        mark->view.count = 0;
        mark->view.start = 0;
        
        unset(*inventory_flags, InventoryFlag_Mark);
    }
    else if(was_pressed(&input->Key_Escape))
    {
        zero_array(mark->array, MAX_MARK_SIZE);
        unset(*inventory_flags, InventoryFlag_Mark);
    }
    else if(was_pressed(&input->Key_Del))
    {
        // Don't do this if we are at the end of the buffer
        if((mark->cursor < mark->view.count) && mark->view.count)
        {
            // Remove the character at mark->cursor and move the buffer
            for(u32 index = mark->cursor; index < MAX_MARK_SIZE; ++index)
            {
                mark->array[index] = mark->array[index + 1];
                mark->array[index + 1] = 0;
            }
            
            force_render_mark_cursor(mark);
        }
    }
    else if(was_pressed_core(&input->Key_Backspace))
    {
        if(input->Key_Control.is_down)
        {
            b32 should_return = false;
            
            while(mark->cursor && mark->array[mark->cursor - 1] != ' ')
            {
                should_return = true;
                remove_mark_char(mark);
            }
            
            if(should_return) return;
            
                while(mark->cursor && mark->array[mark->cursor - 1] == ' ')
            {
                    remove_mark_char(mark);
                }
        }
        else
        {
            remove_mark_char(mark);
        }
    }
    else if(was_pressed(&input->Key_ArrowLeft))
    {
        if(mark->cursor)
        {
            if(input->Key_Control.is_down)
            {
                b32 should_return = false;
                
                while(mark->cursor && mark->array[mark->cursor - 1] != ' ')
                    {
                        should_return = true;
                        move_mark_cursor_left(mark);
                    }
                
                if(should_return) return;
                
                while(mark->cursor && mark->array[mark->cursor - 1] == ' ')
                    {
                        move_mark_cursor_left(mark);
                    }
            }
            else
            {
                move_mark_cursor_left(mark);
            }
        }
    }
    else if(was_pressed(&input->Key_ArrowRight))
    {
        if(input->Key_Control.is_down)
        {
            b32 should_return = false;
            
                while(mark->cursor + 1 <= mark->view.count &&
                      mark->array[mark->cursor] != ' ')
            {
                should_return = true;
                    move_mark_cursor_right(mark);
                }
            
            if(should_return) return;
            
                while(mark->cursor + 1 <= mark->view.count &&
                      mark->array[mark->cursor] == ' ')
                {
                    move_mark_cursor_right(mark);
                }
        }
        else
        {
            move_mark_cursor_right(mark);
        }
    }
    else if(was_pressed(&input->Key_Home))
    {
        move_mark_to_start(mark);
        force_render_mark_cursor(mark);
    }
    else if(was_pressed(&input->Key_End))
    {
        move_mark_to_end(mark);
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
    printf("mark->cursor: %u\n", mark->cursor);
    ui_print_view("Mark View", mark->view);
#endif
    
}

internal u32
get_inventory_item_count(Inventory *inventory)
{
    assert(inventory);
    
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

internal void
unset_item_selections(ItemState *items, u32 dungeon_level)
{
    assert(items);
    assert(is_dungeon_level_valid(dungeon_level));
    
    for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
    {
        Item *item = &items->array[index];
        
        if(is_item_valid(item, dungeon_level))
        {
            unset(item->flags, ItemFlag_IsSelected);
        }
    }
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
        case ItemRarity_Common: result = get_random_number(random, -1, 1); break;
        case ItemRarity_Magical: result = get_random_number(random, -2, 3); break;
        case ItemRarity_Mythical: result = get_random_number(random, -3, 5); break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal ItemType
random_item_type(Random *random)
{
    ItemType result = get_random_number(random, ItemType_None + 1, ItemType_Count - 1);
    return(result);
}

internal ItemID
random_weapon(Random *random)
{
    ItemID result = get_random_number(random, ItemID_WeaponStart + 1, ItemID_WeaponEnd - 1);
    return(result);
}

internal ItemID
random_leather_armor(Random *random)
{
    ItemID result = get_random_number(random, ItemID_LeatherHelmet, ItemID_LeatherBoots);
    return(result);
}

internal ItemID
random_steel_armor(Random *random)
{
    ItemID result = get_random_number(random, ItemID_SteelHelmet, ItemID_SteelBoots);
    return(result);
}

internal ItemID
random_potion(Random *random)
{
    ItemID result = get_random_number(random, ItemID_PotionStart + 1, ItemID_PotionEnd - 1);
    return(result);
}

internal ItemID
random_scroll(Random *random)
{
    ItemID result = get_random_number(random, ItemID_ScrollStart + 1, ItemID_ScrollEnd - 1);
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
ask_for_confirm(Input *input, Game *game, UI *ui, Inventory *inventory)
{
    set(inventory->flags, InventoryFlag_AskingPlayer);
    
    log_add("%sAre you sure?, [%s] Yes [%s] No.",
            ui,
                start_color(Color_Yellow),
                get_printable_key(input, game->keybinds[GameKey_Yes]).s,
                get_printable_key(input, game->keybinds[GameKey_No]).s);
}

internal void
ask_for_item_cancel(Input *input, Game *game, UI *ui, Inventory *inventory)
{
    set(inventory->flags, InventoryFlag_AskingPlayer);
    
    log_add("%sCancel and waste the item?, [%s] Yes [%s] No.",
            ui,
                start_color(Color_Yellow),
                get_printable_key(input, game->keybinds[GameKey_Yes]).s,
                get_printable_key(input, game->keybinds[GameKey_No]).s);
}

internal DamageType
get_random_damage_type(Random *random)
{
    // Skips physical damage type
    DamageType result = get_random_number(random,
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

internal void
render_items(Game *game,
             Entity *player,
             ItemState *items,
              Dungeon *dungeon,
             Assets *assets)
{
    for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
    {
        Item *item = &items->array[index];
        if(is_item_valid_and_not_in_inventory(item, dungeon->level))
        {
            v4u dest = get_game_dest(game, item->pos);
            
            if(is_tile_seen(dungeon->tiles, item->pos))
            {
                set_flag_if_player_is_not_pathfinding(player->flags, &item->flags, ItemFlag_HasBeenSeen);
                SDL_RenderCopy(game->renderer, assets->tileset.tex, (SDL_Rect *)&item->tile_src, (SDL_Rect *)&dest);
                
                if(game->show_item_ground_outline)
                {
                    SDL_RenderCopy(game->renderer, assets->ui.tex, (SDL_Rect *)&assets->item_ground_outline_src, (SDL_Rect *)&dest);
                }
            }
            else if(has_tile_been_seen(dungeon->tiles, item->pos))
            {
                render_texture_half_color(game->renderer, assets->tileset.tex, item->tile_src, dest, false);
                
                if(game->show_item_ground_outline)
                {
                    render_texture_half_color(game->renderer, assets->ui.tex, assets->item_ground_outline_src, dest, false);
                }
            }
        }
    }
}

internal u32
get_dungeon_pos_item_count(ItemState *items, u32 dungeon_level, v2u pos)
{
    u32 result = 0;
    
    for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
    {
        Item *item = &items->array[index];
        if(is_item_valid_and_not_in_inventory(item, dungeon_level) &&
           is_v2u_equal(item->pos, pos))
        {
            ++result;
        }
    }
    
    return(result);
}

internal Item *
get_dungeon_pos_item(ItemState *items, u32 dungeon_level, v2u pos, ItemID search_id)
{
    Item *result = 0;
    
    for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
    {
        Item *item = &items->array[index];
        
        if(is_item_valid_and_not_in_inventory(item, dungeon_level) &&
           is_v2u_equal(item->pos, pos))
        {
            // If ID is set, we are searching for a specific item
            if(search_id && item->id != search_id)
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
add_weapon_item(Random *random,
                ItemState *items,
                u32 dungeon_level,
                ItemID id,
                ItemRarity rarity,
                u32 x,
                u32 y,
                b32 is_cursed)
{
    assert(id);
    assert(rarity);
    assert(dungeon_level);
    
    for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
    {
        Item *item = &items->array[index];
        if(!item->id)
        {
            if(is_cursed)
            {
                set(item->flags, ItemFlag_IsCursed);
            }
            else
            {
                unset(item->flags, ItemFlag_IsCursed);
            }
            
            item->id = id;
            item->pos = make_v2u(x, y);
            item->dungeon_level = dungeon_level;
            item->slot = ItemSlot_FirstHand;
            item->rarity = rarity;
            item->tile_src = get_dungeon_tile_rect(get_item_tile_pos(item->id, item->rarity)); 
            item->equip_tile_src = get_dungeon_tile_rect(get_item_equip_tile_pos(item->id, item->rarity));
            item->enchantment_level = get_item_enchantment_level(random, item->rarity);
            
            item->type = ItemType_Weapon;
            item->w.handedness = get_item_handedness(item->id);
            item->w.first_damage_type = DamageType_Physical;
            
            // TODO(rami): Extra stats for mythical items
            switch(id)
            {
                case ItemID_Dagger:
                {
                    item->w.damage = 4;
                    item->w.accuracy = 4;
                    item->w.speed = 1.0f;
                    
                    if(rarity == ItemRarity_Common)
                    {
                        strcpy(item->name.s, "Dagger");
                    }
                    else if(rarity == ItemRarity_Magical)
                    {
                        strcpy(item->name.s, "Dagger");
                        item->w.second_damage_type = get_random_damage_type(random);
                    }
                    else
                    {
                        get_random_name(random, item->name.s, NameType_Item);
                        item->w.second_damage_type = get_random_damage_type(random);
                        item->extra_stat_count = get_random_number(random, 1, 4);
                    }
                } break;
                
                case ItemID_Club:
                {
                    item->w.damage = 10;
                    item->w.accuracy = 0;
                    item->w.speed = 1.1f;
                    
                    if(rarity == ItemRarity_Common)
                    {
                        strcpy(item->name.s, "Club");
                    }
                    else if(rarity == ItemRarity_Magical)
                    {
                        strcpy(item->name.s, "Club");
                        item->w.second_damage_type = get_random_damage_type(random);
                    }
                    else if(rarity == ItemRarity_Mythical)
                    {
                        get_random_name(random, item->name.s, NameType_Item);
                        item->w.second_damage_type = get_random_damage_type(random);
                        item->extra_stat_count = get_random_number(random, 1, 4);
                    }
                } break;
                
                case ItemID_Sword:
                {
                    item->w.damage = 8;
                    item->w.accuracy = 2;
                    item->w.speed = 1.1f;
                    
                    if(rarity == ItemRarity_Common)
                    {
                        strcpy(item->name.s, "Sword");
                    }
                    else if(rarity == ItemRarity_Magical)
                    {
                        strcpy(item->name.s, "Sword");
                        item->w.second_damage_type = get_random_damage_type(random);
                    }
                    else if(rarity == ItemRarity_Mythical)
                    {
                        get_random_name(random, item->name.s, NameType_Item);
                        item->w.second_damage_type = get_random_damage_type(random);
                        item->extra_stat_count = get_random_number(random, 2, 4);
                    }
                } break;
                
                case ItemID_Battleaxe:
                {
                    item->w.damage = 12;
                    item->w.accuracy = -1;
                    item->w.speed = 1.3f;
                    
                    if(rarity == ItemRarity_Common)
                    {
                        strcpy(item->name.s, "Battleaxe");
                    }
                    else if(rarity == ItemRarity_Magical)
                    {
                        strcpy(item->name.s, "Battleaxe");
                        item->w.second_damage_type = get_random_damage_type(random);
                    }
                    else if(rarity == ItemRarity_Mythical)
                    {
                        get_random_name(random, item->name.s, NameType_Item);
                        item->w.second_damage_type = get_random_damage_type(random);
                        item->extra_stat_count = get_random_number(random, 1, 4);
                    }
                } break;
                
                case ItemID_Spear:
                {
                    item->w.damage = 16;
                    item->w.accuracy = -2;
                    item->w.speed = 1.4f;
                    
                    if(rarity == ItemRarity_Common)
                    {
                        strcpy(item->name.s, "Spear");
                    }
                    else if(rarity == ItemRarity_Magical)
                    {
                        strcpy(item->name.s, "Spear");
                        item->w.second_damage_type = get_random_damage_type(random);
                    }
                    else if(rarity == ItemRarity_Mythical)
                    {
                        get_random_name(random, item->name.s, NameType_Item);
                        item->w.second_damage_type = get_random_damage_type(random);
                        item->extra_stat_count = get_random_number(random, 1, 4);
                    }
                } break;
                
                case ItemID_Warhammer:
                {
                    item->w.damage = 20;
                    item->w.accuracy = -3;
                    item->w.speed = 1.5f;
                    
                    if(rarity == ItemRarity_Common)
                    {
                        strcpy(item->name.s, "Warhammer");
                    }
                    else if(rarity == ItemRarity_Magical)
                    {
                        strcpy(item->name.s, "Warhammer");
                        item->w.second_damage_type = get_random_damage_type(random);
                    }
                    else if(rarity == ItemRarity_Mythical)
                    {
                        get_random_name(random, item->name.s, NameType_Item);
                        item->w.second_damage_type = get_random_damage_type(random);
                        item->extra_stat_count = get_random_number(random, 1, 4);
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
add_armor_item(Random *random, ItemState *items, u32 dungeon_level, ItemID id, u32 x, u32 y, b32 is_cursed)
{
    assert(dungeon_level);
    assert(id);
    
    for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
    {
        Item *item = &items->array[index];
        if(!item->id)
        {
            if(is_cursed)
            {
                set(item->flags, ItemFlag_IsCursed);
            }
            else
            {
                unset(item->flags, ItemFlag_IsCursed);
            }
            
            item->id = id;
            item->pos = make_v2u(x, y);
            item->dungeon_level = dungeon_level;
            item->rarity = ItemRarity_Common;
            item->tile_src = get_dungeon_tile_rect(get_item_tile_pos(item->id, item->rarity)); 
            item->equip_tile_src = get_dungeon_tile_rect(get_item_equip_tile_pos(item->id, item->rarity));
            item->type = ItemType_Armor;
            item->enchantment_level = get_random_number(random, -1, 1);
            
            switch(item->id)
            {
                case ItemID_LeatherHelmet:
                {
                    strcpy(item->name.s, "Leather Helmet");
                    item->slot = ItemSlot_Head;
                    item->a.defence = 1;
                    item->a.weight = 1;
                } break;
                
                case ItemID_LeatherChestplate:
                {
                    strcpy(item->name.s, "Leather Chestplate");
                    item->slot = ItemSlot_Body;
                    item->a.defence = 3;
                    item->a.weight = 3;
                } break;
                
                case ItemID_LeatherGreaves:
                {
                    strcpy(item->name.s, "Leather Greaves");
                    item->slot = ItemSlot_Legs;
                    item->a.defence = 2;
                    item->a.weight = 2;
                } break;
                
                case ItemID_LeatherBoots:
                {
                    strcpy(item->name.s, "Leather Boots");
                    item->slot = ItemSlot_Feet;
                    item->a.defence = 1;
                    item->a.weight = 1;
                } break;
                
                case ItemID_SteelHelmet:
                {
                    strcpy(item->name.s, "Steel Helmet");
                    item->slot = ItemSlot_Head;
                    item->a.defence = 4;
                    item->a.weight = 2;
                } break;
                
                case ItemID_SteelChestplate:
                {
                    strcpy(item->name.s, "Steel Chestplate");
                    item->slot = ItemSlot_Body;
                    item->a.defence = 8;
                    item->a.weight = 6;
                } break;
                
                case ItemID_SteelGreaves:
                {
                    strcpy(item->name.s, "Steel Greaves");
                    item->slot = ItemSlot_Legs;
                    item->a.defence = 6;
                    item->a.weight = 4;
                } break;
                
                case ItemID_SteelBoots:
                {
                    strcpy(item->name.s, "Steel Boots");
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
add_consumable_item(Random *random, ItemState *items, u32 dungeon_level, ItemID id, u32 x, u32 y, u32 stack_count)
{
    assert(dungeon_level);
    assert(stack_count);
    assert(id);
    
    for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
    {
        Item *item = &items->array[index];
        if(!item->id)
        {
            item->id = id;
            
            if(is_item_id_potion(id))
            {
                item->type = ItemType_Potion;
            }
            else if(is_item_id_scroll(id))
            {
                item->type = ItemType_Scroll;
            }
            else
            {
                assert(id == ItemID_Ration);
                item->type = ItemType_Ration;
            }
            
            item->pos = make_v2u(x, y);
            item->dungeon_level = dungeon_level;
            item->rarity = ItemRarity_Common;
            item->c.stack_count = stack_count;
            
            switch(id)
            {
                case ItemID_MightPotion:
                {
                    strcpy(item->name.s, "Potion of Might");
                    Status *status = &item->c.status;
                    
                    status->type = StatusType_Stat;
                    status->stat_type = StatType_Str;
                        status->value = 2;
                    status->duration = 40;
                    strcpy(status->name.s, "Might");
                    strcpy(status->description.s, "None");
                    
                    sprintf(status->start_message.s, "You drink the %s, you feel more mighty.", item->name.s);
                    sprintf(status->end_message.s, "%sYou don't feel as mighty anymore.", start_color(Color_LightGray));
                    
                    sprintf(item->description.s, "Grants +%u Strength for %u turns.", status->value, status->duration);
                    copy_consumable_info_to_item(item, &items->potion_info[Potion_Might]);
                    } break;
                
                case ItemID_WisdomPotion:
                {
                    strcpy(item->name.s, "Potion of Wisdow");
                    Status *status = &item->c.status;
                    
                    status->type = StatusType_Stat;
                    status->stat_type = StatType_Int;
                    status->value = 2;
                    status->duration = 40;
                    strcpy(status->name.s, "Wisdom");
                    strcpy(status->description.s, "None");
                    
                    sprintf(status->start_message.s, "You drink the %s, you feel more wise.", item->name.s);
                    sprintf(status->end_message.s, "%sYou don't feel as wise anymore.", start_color(Color_LightGray));
                    
                    sprintf(item->description.s, "Grants +%u Intelligence for %u turns.", status->value, status->duration);
                    copy_consumable_info_to_item(item, &items->potion_info[Potion_Wisdom]);
                    } break;
                
                case ItemID_AgilityPotion:
                {
                    strcpy(item->name.s, "Potion of Agility");
                    Status *status = &item->c.status;
                    
                    status->type = StatusType_Stat;
                    status->stat_type = StatType_Dex;
                    status->value = 2;
                    status->duration = 40;
                    strcpy(status->name.s, "Agility");
                    strcpy(status->description.s, "None");
                    
                    sprintf(status->start_message.s, "You drink the %s, you feel more dexterous.", item->name.s);
                    sprintf(status->end_message.s, "%sYou don't feel as agile anymore.", start_color(Color_LightGray));
                    
                    sprintf(item->description.s, "Grants +%u Dexterity for %u turns.", status->value, status->duration);
                    copy_consumable_info_to_item(item, &items->potion_info[Potion_Agility]);
                } break;
                
                case ItemID_ElusionPotion:
                {
                    strcpy(item->name.s, "Potion of Elusion");
                    Status *status = &item->c.status;
                    
                    status->type = StatusType_Stat;
                    status->stat_type = StatType_EV;
                    status->value = 2;
                    status->duration = 40;
                    strcpy(status->name.s, "Elusion");
                    strcpy(status->description.s, "None");
                    
                    sprintf(status->start_message.s, "You drink the %s, you feel more evasive.", item->name.s);
                    sprintf(status->end_message.s, "%sYou don't feel as elusive anymore.", start_color(Color_LightGray));
                    
                    sprintf(item->description.s, "Grants +%u Evasion for %u turns.", status->value, status->duration);
                    copy_consumable_info_to_item(item, &items->potion_info[Potion_Elusion]);
                } break;
                
                case ItemID_HealingPotion:
                {
                    strcpy(item->name.s, "Potion of Healing");
                    ConsumableInfo *info = &items->potion_info[Potion_Healing];
                    
                    Status *status = &item->c.status;
                    status->type = StatusType_Heal;
                    status->value = get_random_number_from_v2u(random, info->value_range);
                    sprintf(status->max_message.s, "You drink the %s, you feel no different.", item->name.s);
                    sprintf(status->start_message.s, "%sYou drink the %s, it heals you for %u health.", start_color(Color_LightGreen), item->name.s, status->value);
                    
                    sprintf(item->description.s, "Restores %u - %u of your hitpoints.", info->value_range.min, info->value_range.max);
                    copy_consumable_info_to_item(item, info);
                } break;
                
                case ItemID_DecayPotion:
                {
                    strcpy(item->name.s, "Potion of Decay");
                    Status *status = &item->c.status;
                    
                    status->type = StatusType_Stat;
                    status->stat_type = StatType_StrIntDex;
                    status->value = -2;
                    status->duration = 40;
                    strcpy(status->name.s, "Decay");
                    strcpy(status->description.s, "None");
                    
                    sprintf(status->start_message.s, "You drink the %s, you feel much weaker.", item->name.s);
                    sprintf(status->end_message.s, "%sYou don't feel as weak anymore.", start_color(Color_LightGray));
                    
                    sprintf(item->description.s, "Reduces Strength, Intelligence and Dexterity by %u for %u turns.", status->value, status->duration);
                    copy_consumable_info_to_item(item, &items->potion_info[Potion_Decay]);
                } break;
                
                case ItemID_ConfusionPotion:
                {
                    strcpy(item->name.s, "Potion of Confusion");
                    Status *status = &item->c.status;
                    
                    status->type = StatusType_Confusion;
                    status->chance = 33;
                    status->duration = 40;
                    strcpy(status->name.s, "Confusion");
                    strcpy(status->description.s, "None");
                    
                    sprintf(status->start_message.s, "You drink the %s, you feel confused.", item->name.s);
                    sprintf(status->end_message.s, "%sYou don't feel confused anymore.", start_color(Color_LightGray));
                    
                    sprintf(item->description.s, "Confuses you for %u turns.", status->duration);
                    copy_consumable_info_to_item(item, &items->potion_info[Potion_Confusion]);
                } break;
                
                case ItemID_IdentifyScroll:
                {
                    strcpy(item->name.s, "Scroll of Identify");
                    strcpy(item->description.s, "Identify a single item.");
                    copy_consumable_info_to_item(item, &items->scroll_info[Scroll_Identify]);
                } break;
                
                case ItemID_EnchantWeaponScroll:
                {
                    strcpy(item->name.s, "Scroll of Enchant Weapon");
                    strcpy(item->description.s, "Enchant a weapon with +1 damage and accuracy.");
                    copy_consumable_info_to_item(item, &items->scroll_info[Scroll_EnchantWeapon]);
                } break;
                
                case ItemID_EnchantArmorScroll:
                {
                    strcpy(item->name.s, "Scroll of Enchant Armor");
                    strcpy(item->description.s, "Enchant a piece of armor with +1 defence.");
                    copy_consumable_info_to_item(item, &items->scroll_info[Scroll_EnchantArmor]);
                } break;
                
                case ItemID_MagicMappingScroll:
                {
                    strcpy(item->name.s, "Scroll of Magic Mapping");
                    strcpy(item->description.s, "Reveals the layout of the current level.");
                    copy_consumable_info_to_item(item, &items->scroll_info[Scroll_MagicMapping]);
                } break;
                
                case ItemID_TeleportationScroll:
                {
                    strcpy(item->name.s, "Scroll of Teleportation");
                    strcpy(item->description.s, "Teleports you to a random position on the level.");
                    copy_consumable_info_to_item(item, &items->scroll_info[Scroll_Teleport]);
                } break;
                
                case ItemID_UncurseScroll:
                {
                    strcpy(item->name.s, "Scroll of Uncurse");
                    strcpy(item->description.s, "Removes the a curse from a cursed item.");
                    copy_consumable_info_to_item(item, &items->scroll_info[Scroll_Uncurse]);
                } break;
                
                case ItemID_Ration:
                {
                    strcpy(item->name.s, "Ration");
                    ConsumableInfo *info = &items->ration_info;
                    
                    Status *status = &item->c.status;
                    status->type = StatusType_Heal;
                    status->value = get_random_number_from_v2u(random, info->value_range);
                    sprintf(status->max_message.s, "You eat the ration, you feel no different.");
                    sprintf(status->start_message.s, "%sYou eat the ration, it heals you for %u health.", start_color(Color_LightGreen), status->value);
                    
                    sprintf(item->description.s, "Restores your health for %u - %u.", info->value_range.min, info->value_range.max);
                    copy_consumable_info_to_item(item, &items->ration_info);
                    
                    set(item->flags, ItemFlag_IsIdentified);
                } break;
                
                invalid_default_case;
            }
            
            return(item);
        }
    }
    
    assert(false);
}