internal void
set_item_tile_and_equip_tile(Item *item)
{
 assert(item);
 
 v2u tile_pos = get_item_tile_pos(item->id, item->rarity);
 item->tile_src = get_dungeon_tile_rect(tile_pos);
 
 v2u equip_tile_pos = equip_tile_pos;
 item->equip_tile_src = get_dungeon_tile_rect(equip_tile_pos);
}

internal b32
is_entity_damage_valid(EntityDamage damage)
{
    b32 result = (damage.min &&
                  damage.max &&
                  damage.type);
    
    return(result);
}

internal b32
is_item_identified_and_cursed(u32 item_flags)
{
    b32 result = is_set(item_flags, ItemFlag_IsIdentified | ItemFlag_IsCursed);
    return(result);
    }

internal void
unset_asking_player_and_ready_for_keypress(u32 *inventory_flags)
{
    assert(inventory_flags);
    unset(*inventory_flags, InventoryFlag_AskingPlayer | InventoryFlag_ReadyForKeypress);
}

internal b32
process_new_seen_items(Item *array, DungeonTiles tiles, UI *ui)
{
 assert(array);
 
 b32 result = false;
    
    for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
    {
        Item *item = &array[index];
        
  if(is_valid_non_inventory_item(item) &&
     is_tile_seen(tiles, item->pos) &&
     !is_set(item->flags, ItemFlag_HasBeenSeen))
  {
   String128 name = get_full_item_name(item);
   log_add_comes_into_your_view_string(name.s, ui);
   
   set(item->flags, ItemFlag_HasBeenSeen);
   result = true;
        }
    }
    
    return(result);
}

internal void
start_inventory_item_examine(Inventory *inventory, Item *item)
{
    assert(inventory);
    assert(item);
    
    set(inventory->flags, InventoryFlag_Examine);
    inventory->examine_item = item;
}

internal b32
can_interact_type_be_used_on_item(Item *item, ItemInteractType type)
{
 assert(item);
 
 b32 result = false;
 
 switch(type)
 {
  case ItemInteractType_Identify:
  {
   if(!is_set(item->flags, ItemFlag_IsIdentified))
   {
    result = true;
   }
  } break;
  
  case ItemInteractType_EnchantWeapon:
  {
   if(item->type == ItemType_Weapon)
   {
    result = true;
   }
  } break;
  
  case ItemInteractType_EnchantArmor:
  {
   if(item->type == ItemType_Armor)
   {
    result = true;
   }
  } break;
  
  case ItemInteractType_Uncurse:
  {
   if(is_item_identified_and_cursed(item->flags))
   {
    result = true;
   }
  } break;
  
  invalid_default_case;
 }
 
 return(result);
}

internal b32
can_item_interact_with_inventory(Inventory *inventory, ItemInteractType type, u32 item_flags)
{
 assert(inventory);
 
 b32 result = false;
 
 if(type)
 {
  if(!is_set(item_flags, ItemFlag_IsIdentified))
  {
   result = true;
  }
  else
  {
   for(u32 index = 0; index < MAX_INVENTORY_SLOT_COUNT; ++index)
   {
    Item *inventory_item = inventory->slots[index];
    
    if(inventory_item &&
       can_interact_type_be_used_on_item(inventory_item, type))
    {
     result = true;
     break;
    }
   }
  }
 }
 else
 {
  result = true;
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
is_item_id_armor(ItemID id)
{
 b32 result = (id > ItemID_ArmorStart && id < ItemID_ArmorEnd);
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

internal b32
is_item_id_potion_or_scroll(ItemID id)
{
 b32 result = (is_item_id_potion(id) ||
  is_item_id_scroll(id));
 
 return(result);
}

internal void
copy_consumable_info_to_item(Item *item, ItemInfo *info)
{
    assert(item);
    assert(info);
    
    item->tile_src = info->tile_src;
    strcpy(item->c.depiction.s, info->depiction.s);
    if(info->is_known) set(item->flags, ItemFlag_IsIdentified);
}

internal char *
get_item_slot_string(ItemSlot slot)
{
    char *result = 0;
    
    switch(slot)
    {
        case ItemSlot_Weapon: result = "Weapon"; break;
        case ItemSlot_Shield: result = "Shield"; break;
        case ItemSlot_Head: result = "Head"; break;
        case ItemSlot_Body: result = "Body"; break;
        case ItemSlot_Legs: result = "Legs"; break;
        case ItemSlot_Feet: result = "Feet"; break;
        case ItemSlot_Amulet: result = "Amulet"; break;
        case ItemSlot_Ring: result = "Ring"; break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal char *
get_item_handedness_string(ItemHandedness handedness)
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
get_item_type_string(ItemType type)
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
is_item_valid(Item *item)
{
    b32 result = (item && item->id && is_dungeon_level_valid(item->dungeon_level));
    return(result);
}

internal b32
is_valid_item_selected(Item *item)
{
    b32 result = (is_item_valid(item) && is_set(item->flags, ItemFlag_IsSelected));
    return(result);
}

internal b32
is_valid_inventory_item(Item *item)
{
    b32 result = (is_item_valid(item) && is_set(item->flags, ItemFlag_InInventory));
    return(result);
}

internal b32
is_valid_non_inventory_item(Item *item)
{
    b32 result = (is_item_valid(item) && !is_set(item->flags, ItemFlag_InInventory));
    return(result);
}

internal b32
is_valid_non_inventory_item_on_pos(Item *item, v2u pos)
{
    assert(!is_v2u_zero(pos));
    
    b32 result = (is_valid_non_inventory_item(item) && is_v2u_equal(item->pos, pos));
    return(result);
}

internal b32
is_valid_non_inventory_item_on_pos_and_dungeon_level(Item *item, u32 dungeon_level, v2u pos)
{
    assert(is_dungeon_level_valid(dungeon_level));
    assert(!is_v2u_zero(pos));
    
    b32 result = (is_valid_non_inventory_item_on_pos(item, pos) &&
                  item->dungeon_level == dungeon_level);
    
    return(result);
}

internal void
apply_item_stats_to_player(Entity *player, Item *item, b32 is_add)
{
    assert(is_player_entity_valid(player));
    assert(is_item_valid(item));
    
    if(item->type == ItemType_Weapon)
    {
            for(ItemStatType stat_type = ItemStatType_None; stat_type < MAX_ITEM_STAT_COUNT; ++stat_type)
            {
                ItemStat *stat = &item->stats[stat_type];
            if(stat->type)
   {
    EntityResist *resists = player->resists;
    
                switch(stat->type)
    {
                        case ItemStatType_Health:
                        {
      change_entity_stat(&player->max_hp, stat->value, is_add);
      if(player->hp > player->max_hp) player->hp = player->max_hp;
                        } break;
                        
     case ItemStatType_Strength: change_entity_stat(&player->stats.str, stat->value, is_add); break;
     case ItemStatType_Intelligence: change_entity_stat(&player->stats.intel, stat->value, is_add); break;
     case ItemStatType_Dexterity: change_entity_stat(&player->stats.dex, stat->value, is_add); break;
     case ItemStatType_Evasion: break; // Updated later in this function.
     case ItemStatType_Defence: break; // Updated in player update.
     case ItemStatType_ViewRange: change_entity_stat(&player->stats.fov, stat->value, is_add); break;
     case ItemStatType_InvulnerableToTraps: toggle(player->flags, EntityFlag_InvulnerableToTraps); break;
     
     case ItemStatType_PhysicalResistance: change_entity_resist(&resists[EntityDamageType_Physical], stat->value, is_add); break;
     case ItemStatType_FireResistance: change_entity_resist(&resists[EntityDamageType_Fire], stat->value, is_add); break;
     case ItemStatType_IceResistance: change_entity_resist(&resists[EntityDamageType_Ice], stat->value, is_add); break;
     case ItemStatType_LightningResistance: change_entity_resist(&resists[EntityDamageType_Lightning], stat->value, is_add); break;
     case ItemStatType_PoisonResistance: change_entity_resist(&resists[EntityDamageType_Poison], stat->value, is_add); break;
     case ItemStatType_HolyResistance: change_entity_resist(&resists[EntityDamageType_Holy], stat->value, is_add); break;
     case ItemStatType_DarkResistance: change_entity_resist(&resists[EntityDamageType_Dark], stat->value, is_add); break;
                        
                        invalid_default_case;
                    }
            }
        }
        
  change_entity_stat(&player->p.weight, item->w.weight, is_add);
        }
    else if(item->type == ItemType_Armor)
    {
  change_entity_stat(&player->stats.def, item->a.defence + item->enchant_level, is_add);
  change_entity_stat(&player->p.weight, item->a.weight, is_add);
    }
 
 // Update player evasion
 u32 evasion_from_item_stats = 0;
 if(is_add) // Don't take into consideration when item is being removed.
 {
  evasion_from_item_stats += get_item_stat_type_value(item->stats, ItemStatType_Evasion);
  }
 
    u32 evasion_from_statuses = get_total_stat_status_value(player->statuses, EntityStatusStatType_EV);
 u32 evasion_total = player->p.base_stats.ev + evasion_from_statuses + evasion_from_item_stats;
    u32 weight_penalty = player->p.weight / player->p.weight_evasion_ratio;
    
#if 0
 printf("evasion_from_item_stats: %u\n", evasion_from_item_stats);
 printf("evasion_from_statuses: %u\n", evasion_from_statuses);
 printf("player->stats.ev: %u\n\n", player->stats.ev);
 printf("player->p.weight: %u\n", player->p.weight);
 printf("weight_penalty: %u\n\n", weight_penalty);
    #endif
    
 player->stats.ev = evasion_total - weight_penalty;
}

internal char *
get_item_cursed_prefix(u32 item_flags)
{
 char *result = "";
 
 if(is_item_identified_and_cursed(item_flags))
 {
  result = "Cursed ";
 }
    
    return(result);
}

internal char *
get_entity_damage_type_string(EntityDamageType damage_type)
{
    char *result = 0;
    
    switch(damage_type)
    {
        case EntityDamageType_Physical: result = "Physical"; break;
        case EntityDamageType_Fire: result = "Fire"; break;
        case EntityDamageType_Ice: result = "Ice"; break;
        case EntityDamageType_Lightning: result = "Lightning"; break;
        case EntityDamageType_Poison: result = "Poison"; break;
        case EntityDamageType_Holy: result = "Holy"; break;
        case EntityDamageType_Dark: result = "Dark"; break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal char *
get_item_id_string(ItemID id)
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
    assert(item);
    
    String128 result = {0};
    
    if(is_item_equipment(item->type))
    {
        if(is_set(item->flags, ItemFlag_IsIdentified))
        {
            if(item->w.damage.second_type)
            {
                sprintf(result.s, "%s%c%d %s of %s",
                            get_item_cursed_prefix(item->flags),
                            sign(item->enchant_level),
                            absolute(item->enchant_level),
                            item->name.s,
                            get_entity_damage_type_string(item->w.damage.second_type));
            }
            else
            {
                sprintf(result.s, "%s%c%d %s",
                            get_item_cursed_prefix(item->flags),
                            sign(item->enchant_level),
                            absolute(item->enchant_level),
                            item->name.s);
            }
        }
        else
        {
            strcpy(result.s, get_item_id_string(item->id));
        }
    }
    else
    {
        if(is_set(item->flags, ItemFlag_IsIdentified))
        {
            strcpy(result.s, item->name.s);
        }
        else
        {
            sprintf(result.s, "%s%s", item->c.depiction.s, get_item_id_string(item->id));
        }
    }
    
    return(result);
}

internal void
adjust_item_letter(Input *input,
                      Item *item,
                   ItemState *item_state,
                   Inventory *inventory,
                   u32 dungeon_level,
                      UI *ui)
{
    assert(input);
    assert(item);
    assert(item_state);
    assert(inventory);
    assert(is_dungeon_level_valid(dungeon_level));
    assert(ui);
    
    char pressed = get_pressed_keyboard_char(input, KeyboardCharType_Alphabet);
    if(pressed)
    {
        if(pressed != item->inventory_letter)
        {
            Owner *new_owner = get_owner_from_letter(inventory->item_owners, pressed);
            if(new_owner->type)
            {
                Owner *current_owner = get_owner_from_letter(inventory->item_owners,
                                                             item->inventory_letter);
                
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
                // Clear current
                clear_owner_from_character(inventory->item_owners, &item->inventory_letter);
                
                // Set new
                Owner *new_owner = get_owner_from_letter(inventory->item_owners, pressed);
                item->inventory_letter = set_owner_src(new_owner, item, OwnerType_Item);
            }
        }
        
        unset(inventory->flags, InventoryFlag_Adjust);
        log_add("%s%s", ui, get_item_letter_string(item).s, get_full_item_name(item).s);
    }
}

internal void
set_potion_or_scroll_as_known(ItemState *item_state, ItemID id)
{
    assert(item_state);
 assert(is_item_id_potion_or_scroll(id));
    
    ItemInfo *potion_info = item_state->potion_info;
    ItemInfo *scroll_info = item_state->scroll_info;
    
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
    
    // Identify all items that match the ID
    for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
    {
  Item *item = &item_state->array[index];
  if(is_item_valid(item) && item->id == id)
  {
   set(item->flags, ItemFlag_IsIdentified);
  }
    }
}

internal void
remove_item(Item *item, Owner *item_owners)
{
    assert(item);
    assert(item_owners);
    
 // Clear the inventory letter if it has one.
    if(item->inventory_letter)
    {
        clear_owner_from_character(item_owners, &item->inventory_letter);
    }
    
    zero_struct(*item);
}

internal b32
is_item_equipment(ItemType type)
{
 assert(type);
 
    b32 result = (type == ItemType_Weapon ||
                  type == ItemType_Armor);
    
    return(result);
}

internal b32
is_item_consumable(ItemType type)
{
 assert(type);
 
    b32 result = (type == ItemType_Potion ||
                  type == ItemType_Scroll ||
                  type == ItemType_Ration);
    
    return(result);
}

internal b32
is_item_stacked(u32 stack_count)
{
    b32 result = stack_count > 1;
        return(result);
}

internal b32
add_item_to_inventory(Game *game,
                      Entity *player,
                      Item *item,
                      ItemState *item_state,
                      Inventory *inventory,
                      UI *ui,
                      u32 dungeon_level,
                      b32 add_to_log)
{
    b32 added_to_stack = false;
    b32 added_to_inventory = false;
    
    // If the same consumable item exists in the inventory then combine their stacks
    if(is_item_consumable(item->type))
    {
        for(u32 index = 0; index < MAX_INVENTORY_SLOT_COUNT; ++index)
        {
            Item *inventory_item = inventory->slots[index];
            
            if(inventory_item &&
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
        
        if(add_to_log) log_add_item_action_string(item, ui, ItemActionType_Pickup);
  if(added_to_stack) remove_item(item, inventory->item_owners);
        
        game->passed_time += player->p.action_time;
    }
    else
    {
        log_add("Your inventory is full.", ui);
    }
    
    return(added_to_inventory);
}

internal void
remove_inventory_item(Item *item,
                           Inventory *inventory,
                           u32 dungeon_level)
{
    assert(item);
    assert(inventory);
    assert(is_dungeon_level_valid(dungeon_level));
    
    #if MOONBREATH_SLOW
    if(is_set(item->flags, ItemFlag_IsCursed))
    {
        assert(!is_set(item->flags, ItemFlag_IsIdentified));
    }
    #endif
    
    // Find and unset the inventory pointer for the item we want to remove
    b32 was_unset = false;
    for(u32 index = 0; index < MAX_INVENTORY_SLOT_COUNT; ++index)
    {
        Item *inventory_item = inventory->slots[index];
            
        if(inventory_item &&
               inventory_item->inventory_letter == item->inventory_letter)
        {
            was_unset = true;
                inventory->slots[index] = 0;
            break;
            }
        }
    assert(was_unset);
    
    unset(item->flags, ItemFlag_IsEquipped | ItemFlag_InInventory);
    unset(inventory->flags, InventoryFlag_Examine);
}

internal void
remove_item_from_inventory_and_game(Item *item,
                                    ItemState *item_state,
                                    Inventory *inventory,
                                    u32 dungeon_level)
{
    assert(item);
    assert(item_state);
    assert(inventory);
    assert(is_dungeon_level_valid(dungeon_level));
    
 remove_inventory_item(item, inventory, dungeon_level);
 remove_item(item, inventory->item_owners);
}

internal void
remove_consumable_from_inventory(Item *item,
                   ItemState *item_state,
                   Inventory *inventory,
                   u32 dungeon_level)
{
    assert(item);
    assert(is_item_consumable(item->type));
    assert(inventory);
    assert(is_dungeon_level_valid(dungeon_level));
    
    if(is_item_stacked(item->c.stack_count))
    {
        --item->c.stack_count;
    }
    else
    {
        remove_item_from_inventory_and_game(item, item_state, inventory, dungeon_level);
    }
}

internal Item *
get_equipped_item_from_item_slot(ItemSlot slot, Inventory *inventory)
{
    assert(slot);
    assert(inventory);
    
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

internal char *
get_item_status_color(u32 *flags, ItemRarity rarity)
{
    assert(flags);
    assert(rarity);
    
    char *result = start_color(Color_White);
    
    if(is_set(*flags, ItemFlag_IsIdentified))
    {
        if(is_set(*flags, ItemFlag_IsCursed))
        {
            result = start_color(Color_LightRed);
        }
        else
        {
            switch(rarity)
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

internal String32
get_item_stack_string(ItemType type, u32 stack_count)
{
    assert(type);
    
    String32 result = {0};
    
    if(is_item_consumable(type) &&
           is_item_stacked(stack_count))
    {
        sprintf(result.s, " (%u)", stack_count);
    }
    
    return(result);
}

internal String128
get_item_mark_string(u32 item_flags, char *mark_array)
{
    assert(mark_array);
    
    String128 result = {0};
 if(is_set(item_flags, ItemFlag_IsMarked)) sprintf(result.s, " {%s}", mark_array);
    
    return(result);
}

internal String8
get_item_action_string(ItemActionType action)
{
    assert(action);
    
    String8 result = {0};
    switch(action)
    {
        case ItemActionType_Pickup: strcpy(result.s, "pick up"); break;
        case ItemActionType_Drop: strcpy(result.s, "drop"); break;
        case ItemActionType_Equip: strcpy(result.s, "equip"); break;
        case ItemActionType_Unequip: strcpy(result.s, "unequip"); break;
        
        invalid_default_case;
    }
    
    return(result);
    }

internal void
log_add_item_action_string(Item *item, UI *ui, ItemActionType action)
{
    assert(item);
    assert(ui);
    assert(action);
    
    log_add("You %s the %s%s%s%s%s.",
            ui,
                get_item_action_string(action).s,
            get_item_status_color(&item->flags, item->rarity),
            get_full_item_name(item).s,
                get_item_stack_string(item->type, item->c.stack_count).s,
                get_item_mark_string(item->flags, item->mark.array).s,
                end_color());
    
    if(action == ItemActionType_Equip &&
       is_set(item->flags, ItemFlag_IsCursed))
    {
        assert(is_set(item->flags, ItemFlag_IsIdentified));
        log_add("It feels like it's stuck to your hand!", ui);
    }
    }

internal b32
unequip_item(Game *game, Entity *player, Item *item, UI *ui, b32 came_from_drop_item)
{
    assert(game);
    assert(is_player_entity_valid(player));
    assert(item);
    assert(ui);
    
    if(is_item_equipment(item->type) && is_set(item->flags, ItemFlag_IsEquipped))
    {
        if(is_item_identified_and_cursed(item->flags))
        {
            log_add("You try to %s the %s%s%s, but it won't leave your hand!",
                    ui,
                        get_item_action_string(came_from_drop_item ? ItemActionType_Drop : ItemActionType_Unequip).s,
                        get_item_status_color(&item->flags, item->rarity),
                        get_full_item_name(item).s,
                        end_color());
        }
        else
        {
            unset(item->flags, ItemFlag_IsEquipped);
            apply_item_stats_to_player(player, item, false);
            log_add_item_action_string(item, ui, ItemActionType_Unequip);
            
            assert(item->slot);
            player->p.equipment[item->slot] = 0;
            
            game->passed_time = player->p.action_time;
            return(true);
        }
    }
    
    return(false);
}

internal void
equip_item(Game *game, Entity *player, Item *item, Inventory *inventory, UI *ui, b32 add_to_log)
{
    assert(game);
    assert(is_player_entity_valid(player));
    assert(item);
    assert(inventory);
    assert(ui);
    
 if(is_item_equipment(item->type) &&
    !is_set(item->flags, ItemFlag_IsEquipped))
    {
        b32 can_equip_item = true;
        
        Item *equipped_item = get_equipped_item_from_item_slot(item->slot, inventory);
        if(equipped_item && !unequip_item(game, player, equipped_item, ui, false))
        {
   // We already have an item equipped and can't unequip it.
                can_equip_item = false;
        }
        
        if(can_equip_item)
        {
            set(item->flags, ItemFlag_IsIdentified | ItemFlag_IsEquipped);
            apply_item_stats_to_player(player, item, true);
            if(add_to_log) log_add_item_action_string(item, ui, ItemActionType_Equip);
            
            assert(item->slot);
            player->p.equipment[item->slot] = item;
            
            game->passed_time += player->p.action_time;
        }
    }
}

internal void
drop_item_from_inventory(Game *game,
                         Entity *player,
                         Item *item,
                         ItemState *item_state,
                         Inventory *inventory,
                         Dungeon *dungeon,
                         UI *ui)
{
    assert(game);
    assert(is_player_entity_valid(player));
    assert(item);
    assert(item_state);
    assert(inventory);
    assert(dungeon);
    assert(ui);
    
    b32 was_unequipped = unequip_item(game, player, item, ui, true);
    
    if(!is_set(item->flags, ItemFlag_IsEquipped) || was_unequipped)
    {
        unset(player->flags, EntityFlag_NotifyAboutMultipleItems);
        unset(inventory->flags, InventoryFlag_Examine);
        
        log_add_item_action_string(item, ui, ItemActionType_Drop);
        
        // We use was_unequipped because we want a single player turn to advance if we drop or unequip
        // an item, without we would advance two player turns if we dropped an equipped item.
        if(!was_unequipped) game->passed_time += player->p.action_time;
        
        b32 was_item_removed_from_game = false;
        if(is_item_consumable(item->type))
            {
            // If the same item exists on the drop position then combine their stacks
            Item *pos_item = get_dungeon_pos_item(item_state, dungeon->level, player->pos, item->id);
            if(pos_item)
                {
                    
#if 0
                    printf("item_on_pos->c.stack_count: %u\n", item_on_pos->c.stack_count);
                    printf("item->c.stack_count: %u\n\n", item->c.stack_count);
#endif
                
                assert(is_item_consumable(pos_item->type));
                pos_item->c.stack_count += item->c.stack_count;
                
                // Item stacks have been combined, remove the original item from the game
                was_item_removed_from_game = true;
                remove_item_from_inventory_and_game(item, item_state, inventory, dungeon->level);
                }
            }
        
        // Remove the item from inventory if we didn't remove it from the game
        if(!was_item_removed_from_game)
        {
            item->pos = player->pos;
            item->dungeon_level = dungeon->level;
            clear_owner_from_character(inventory->item_owners, &item->inventory_letter);
   remove_inventory_item(item, inventory, dungeon->level);
            
            inventory->validate_view = true;
            }
        }
}

internal void
force_render_mark_cursor(Mark *mark)
{
    mark->render_cursor = true;
 mark->cursor_blink_start = 0;
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
  if(mark->cursor == (get_view_range(mark->view) - 1) &&
     is_view_scrollable(mark->view))
        {
                ++mark->view.start;
        }
        
        ++mark->cursor;
    }
}

internal void
move_mark_cursor_left(Mark *mark)
{
    assert(mark);
    
    if(is_view_scrollable_with_count(mark->view, mark->view.count + 1) &&
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
        if(is_view_scrollable_with_count(mark->view, mark->view.count + 1) &&
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
unset_all_item_is_selected_flags(ItemState *item_state, u32 dungeon_level)
{
    assert(item_state);
    assert(is_dungeon_level_valid(dungeon_level));
    
    for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
    {
        Item *item = &item_state->array[index];
        if(is_item_valid(item)) unset(item->flags, ItemFlag_IsSelected);
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
    assert(id);
    assert(rarity);
    
    v2u result = {0};
    
    v2u base = {0, 0};
    switch(id)
    {
        case ItemID_Dagger: result = make_v2u(base.x + 0, base.y + 0); break;
        case ItemID_Club: result = make_v2u(base.x + 0, base.y + 0); break;
        case ItemID_Sword: result = make_v2u(base.x + 0, base.y + 0); break;
        case ItemID_Battleaxe: result = make_v2u(base.x + 0, base.y + 0); break;
        case ItemID_Spear: result = make_v2u(base.x + 0, base.y + 0); break;
        case ItemID_Warhammer: result = make_v2u(base.x + 0, base.y + 0); break;
        
        case ItemID_LeatherHelmet: result = make_v2u(base.x + 0, base.y + 0); break;
        case ItemID_LeatherChestplate: result = make_v2u(base.x + 0, base.y + 0); break;
        case ItemID_LeatherGreaves: result = make_v2u(base.x + 0, base.y + 0); break;
        case ItemID_LeatherBoots: result = make_v2u(base.x + 0, base.y + 0); break;
        
        case ItemID_SteelHelmet: result = make_v2u(base.x + 0, base.y + 0); break;
        case ItemID_SteelChestplate: result = make_v2u(base.x + 0, base.y + 0); break;
        case ItemID_SteelGreaves: result = make_v2u(base.x + 0, base.y + 0); break;
        case ItemID_SteelBoots: result = make_v2u(base.x + 0, base.y + 0); break;
        
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
get_item_enchant_level(Random *random, Item *item)
{
    assert(random);
    assert(is_item_valid(item));
    
    s32 result = 0;
    
    switch(item->rarity)
    {
  case ItemRarity_Common:
  {
   if(is_set(item->flags, ItemFlag_IsCursed))
   {
    if(hit_random_chance(random, 25))
    {
     result = -1;
    }
    }
  } break;
  
        case ItemRarity_Magical: result = get_random(random, 1, 2); break;
        case ItemRarity_Mythical: result = get_random(random, 2, 4); break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal ItemType
get_random_item_type(Random *random)
{
    assert(random);
    
    ItemType result = get_random(random, ItemType_None + 1, ItemType_Count - 1);
    return(result);
}

internal ItemID
get_random_weapon(Random *random)
{
    assert(random);
    
    ItemID result = get_random(random, ItemID_WeaponStart + 1, ItemID_WeaponEnd - 1);
    return(result);
}

internal ItemID
get_random_leather_armor(Random *random)
{
    assert(random);
    
    ItemID result = get_random(random, ItemID_LeatherHelmet, ItemID_LeatherBoots);
    return(result);
}

internal ItemID
get_random_steel_armor(Random *random)
{
    assert(random);
    
    ItemID result = get_random(random, ItemID_SteelHelmet, ItemID_SteelBoots);
    return(result);
}

internal ItemID
get_random_potion(Random *random)
{
    assert(random);
    
    ItemID result = get_random(random, ItemID_PotionStart + 1, ItemID_PotionEnd - 1);
    return(result);
}

internal ItemID
get_random_scroll(Random *random)
{
    assert(random);
    
    ItemID result = get_random(random, ItemID_ScrollStart + 1, ItemID_ScrollEnd - 1);
    return(result);
}

internal u32
get_potion_chance_index(ItemID id)
{
    assert(is_item_id_potion(id));
    
    u32 result = id - ItemID_PotionStart - 1;
    return(result);
}

internal u32
get_scroll_chance_index(ItemID id)
{
    assert(is_item_id_scroll(id));
    
    u32 result = id - ItemID_ScrollStart - 1;
    return(result);
}

internal void
ask_for_item_cancel(Key *keybinds, Input *input, Inventory *inventory, UI *ui)
{
    assert(keybinds);
    assert(input);
    assert(inventory);
    assert(ui);
    
    // We assume the thing we are cancelling is the use of a scroll.
    char *interact_name = 0;
    
    switch(inventory->interact_type)
    {
  case ItemInteractType_Identify: interact_name = "Scroll of Identify"; break;
  case ItemInteractType_EnchantWeapon: interact_name = "Scroll of Enchant Weapon"; break;
  case ItemInteractType_EnchantArmor: interact_name = "Scroll of Enchant Armor"; break;
  case ItemInteractType_Uncurse: interact_name = "Scroll of Uncurse"; break;
        
        invalid_default_case;
    }
    
    log_add("%sCancel and waste the %s?, [%s] Yes [%s] No.", ui,
                start_color(Color_LightYellow),
                interact_name,
                get_printable_key(input, keybinds[GameKey_Yes]).s,
                get_printable_key(input, keybinds[GameKey_No]).s);
    
    set(inventory->flags, InventoryFlag_AskingPlayer);
}

internal EntityDamageType
get_random_entity_damage_type(Random *random, EntityDamageType exclude_type)
{
    assert(random);
    
    EntityDamageType result = EntityDamageType_None;
    
    for(;;)
    {
        assert_loop_count();
        
        result = get_random(random, EntityDamageType_None + 1, EntityDamageType_Count - 1);
        if(result != exclude_type) break;
        }
    
    return(result);
}

internal char *
get_item_rarity_string(ItemRarity rarity)
{
    assert(rarity);
    
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
             ItemState *item_state,
              Dungeon *dungeon,
             Assets *assets)
{
    assert(game);
    assert(player);
    assert(item_state);
    assert(dungeon);
    assert(assets);
    
    for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
    {
        Item *item = &item_state->array[index];
        
        if(is_valid_non_inventory_item(item) &&
           item->dungeon_level == dungeon->level)
        {
            v4u item_dest = get_game_dest(game->camera, item->pos);
   
   b32 render_item = false;
   b32 half_color = false;
   Color outline_color = Color_None;
   
   if(is_tile_seen(dungeon->tiles, item->pos))
   {
    render_item = true;
    outline_color = Color_Green;
    
    if(!is_set(player->flags, EntityFlag_Pathfinding))
    {
     set(item->flags, ItemFlag_HasBeenSeen);
    }
   }
   else if(has_tile_been_seen(dungeon->tiles, item->pos))
   {
    render_item = true;
    half_color = true;
    outline_color = Color_DarkGreen;
   }
   
   if(render_item)
   {
    render_texture(game->renderer, assets->tileset.tex, &item->tile_src, &item_dest, false, half_color);
    if(game->show_item_ground_outline)
    {
     render_outline_rect(game->renderer, item_dest, outline_color);
    }
   }
        }
    }
}

internal u32
get_dungeon_pos_item_count(ItemState *item_state, u32 dungeon_level, v2u pos)
{
    assert(item_state);
    assert(is_dungeon_level_valid(dungeon_level));
    assert(!is_v2u_zero(pos));
    
    u32 result = 0;
    
    for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
    {
        Item *item = &item_state->array[index];
        if(is_valid_non_inventory_item_on_pos_and_dungeon_level(item, dungeon_level, pos)) ++result;
    }
    
    return(result);
}

internal Item *
get_dungeon_pos_item(ItemState *item_state, u32 dungeon_level, v2u pos, ItemID search_id)
{
    for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
    {
        Item *item = &item_state->array[index];
        if(is_valid_non_inventory_item_on_pos_and_dungeon_level(item, dungeon_level, pos))
        {
            if(search_id && item->id != search_id) continue;
            return(item);
        }
    }
    
    return(0);
}

internal void
set_item_name(Random *random, Item *item, char *name)
{
    assert(random);
    assert(item);
    assert(name);
    
    switch(item->rarity)
    {
        case ItemRarity_Common:
        case ItemRarity_Magical: strcpy(item->name.s, name); break;
        case ItemRarity_Mythical: set_random_name(random, item->name.s, NameType_Item); break;
        
        invalid_default_case;
    }
}

internal s32
get_item_stat_type_value(ItemStat *stats, ItemStatType type)
{
 assert(stats);
 
 s32 result = 0;
 
 for(u32 stat_index = 0; stat_index < MAX_ITEM_STAT_COUNT; ++stat_index)
 {
  ItemStat *stat = &stats[stat_index];
  if(stat->type == type)
  {
     result += stat->value;
  }
 }
 
 return(result);
}

internal void
set_item_stat_description(ItemStat *stat, char *stat_name)
{
    assert(stat);
    assert(stat_name);
    
    sprintf(stat->description.s, "%c%u %s", sign(stat->value), absolute(stat->value), stat_name);
}

internal void
set_item_resistance_stat(Random *random,
                         ItemStat *stat,
                         EntityDamageType type)
{
    assert(random);
    assert(stat);
    assert(type);
    
    stat->resist_type = type;
    sprintf(stat->description.s, "%c%u %s Resistance", sign(stat->value), absolute(stat->value), get_entity_damage_type_string(stat->resist_type));
}

internal v2u
get_item_stat_value_range(ItemStatType type, ItemRarity rarity)
{
    assert(type);
 assert(rarity == ItemRarity_Magical ||
        rarity == ItemRarity_Mythical);
    
    v2u result = {0};
    
    switch(type)
    {
        case ItemStatType_Health:
        {
            if(rarity == ItemRarity_Magical)
            {
                result.min = 10;
                result.max = 20;
            }
            else
            {
                result.min = 20;
                result.max = 30;
            }
        } break;
        
        case ItemStatType_Strength:
        case ItemStatType_Intelligence:
        case ItemStatType_Dexterity:
        case ItemStatType_Evasion:
        case ItemStatType_Defence:
        {
            if(rarity == ItemRarity_Magical)
            {
                result.min = 2;
                result.max = 4;
            }
            else
            {
                result.min = 4;
                result.max = 8;
            }
        } break;
        
        case ItemStatType_ViewRange:
        {
            if(rarity == ItemRarity_Magical)
            {
                result.min = 1;
    result.max = 2;
            }
            else
            {
                result.min = 2;
                result.max = 3;
            }
        } break;
        
        case ItemStatType_PhysicalResistance:
        case ItemStatType_FireResistance:
        case ItemStatType_IceResistance:
        case ItemStatType_LightningResistance:
        case ItemStatType_PoisonResistance:
        case ItemStatType_HolyResistance:
        case ItemStatType_DarkResistance:
        {
            if(rarity == ItemRarity_Magical)
            {
                result.min = 1;
                result.max = 2;
            }
            else
            {
                result.min = 2;
                result.max = 5;
            }
        } break;
        
        case ItemStatType_InvulnerableToTraps: break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal u32
get_random_new_item_stat_index(Random *random, Item *item)
{
 assert(random);
 assert(is_item_valid(item));
 
 u32 result = 0;
 
 for(;;)
 {
  assert_loop_count();
  
  ItemStatType type = get_random(random, ItemStatType_None + 1, ItemStatType_Count - 1);
  result = type - 1;
  
  if(!item->stats[result].type)
  {
   item->stats[result].type = type;
   break;
  }
 }
 
 return(result);
}

internal void
add_item_stats(Random *random, Item *item)
{
    assert(random);
 assert(is_item_valid(item));
 
    u32 stat_count = 0;
    switch(item->rarity)
    {
        case ItemRarity_Magical: stat_count = get_random(random, 1, 2); break;
        case ItemRarity_Mythical: stat_count = get_random(random, 3, 4); break;
        
        invalid_default_case;
    }
    //stat_count = MAX_ITEM_STAT_COUNT; // Have all stat types
    
    for(u32 count = 0; count < stat_count; ++count)
 {
  u32 index = get_random_new_item_stat_index(random, item);
  ItemStat *stat = &item->stats[index];
  
  v2u value_range = get_item_stat_value_range(stat->type, item->rarity);
        if(!is_v2u_zero(value_range))
        {
            stat->value = get_random_no_zero(random, value_range.min, value_range.max);
        }
        
        switch(stat->type)
        {
            case ItemStatType_Health: set_item_stat_description(stat, "Health"); break;
            case ItemStatType_Strength: set_item_stat_description(stat, "Strength"); break;
            case ItemStatType_Intelligence: set_item_stat_description(stat, "Intelligence"); break;
            case ItemStatType_Dexterity: set_item_stat_description(stat, "Dexterity"); break;
            case ItemStatType_Evasion: set_item_stat_description(stat, "Evasion"); break;
            case ItemStatType_Defence: set_item_stat_description(stat, "Defence"); break;
            case ItemStatType_ViewRange: set_item_stat_description(stat, "View Range"); break;
            case ItemStatType_InvulnerableToTraps: strcpy(stat->description.s, "Trap Invulnerability"); break;
            
            case ItemStatType_PhysicalResistance: set_item_resistance_stat(random, stat, EntityDamageType_Physical); break;
            case ItemStatType_FireResistance: set_item_resistance_stat(random, stat, EntityDamageType_Fire); break;
            case ItemStatType_IceResistance: set_item_resistance_stat(random, stat, EntityDamageType_Ice); break;
            case ItemStatType_LightningResistance: set_item_resistance_stat(random, stat, EntityDamageType_Lightning); break;
            case ItemStatType_PoisonResistance: set_item_resistance_stat(random, stat, EntityDamageType_Poison); break;
            case ItemStatType_HolyResistance: set_item_resistance_stat(random, stat, EntityDamageType_Holy); break;
            case ItemStatType_DarkResistance: set_item_resistance_stat(random, stat, EntityDamageType_Dark); break;
            
            invalid_default_case;
        }
        
#if 0
  printf("stats[%u]\n", index);
  printf("value: %d\n", item->stats[stat_index].value);
  printf("resist_type: %d\n", item->stats[stat_index].resist_type);
  printf("description: %s\n\n", stat->description.s);
        #endif
        
    }
}

internal Item *
add_item(Random *random,
         ItemState *item_state,
         u32 dungeon_level,
         ItemID id,
         ItemRarity rarity,
         u32 x,
         u32 y,
         b32 is_cursed,
         u32 stack_count)
{
 assert(random);
 assert(item_state);
 assert(is_dungeon_level_valid(dungeon_level));
 assert(id);
 assert(x);
 assert(y);
 
 for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
 {
  Item *item = &item_state->array[index];
  if(!item->id)
  {
   // Set shared data
   item->id = id;
   item->pos = make_v2u(x, y);
   item->rarity = rarity;
   item->dungeon_level = dungeon_level;
   
   // Set type for differentiation
   if(is_item_id_weapon(id))
   {
    item->type = ItemType_Weapon;
   }
   else if(is_item_id_armor(id))
   {
    item->type = ItemType_Armor;
   }
   else if(is_item_id_potion(id))
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
   
   if(item->type == ItemType_Weapon)
   {
    // Add weapon
    if(is_cursed) set(item->flags, ItemFlag_IsCursed);
    if(item->rarity != ItemRarity_Common) add_item_stats(random, item);
    
    // TODO(rami): Second item damage type isn't being used.
    //if(rarity == ItemRarity_Mythical) item->w.damage.second_type = get_random_damage_type(random, DamageType_Physical);
    
    item->slot = ItemSlot_Weapon;
    item->enchant_level = get_item_enchant_level(random, item);
    set_item_tile_and_equip_tile(item);
    
    item->w.damage.type = EntityDamageType_Physical;
    item->w.handedness = get_item_handedness(id);
    
    switch(id)
    {
     case ItemID_Dagger:
     {
      set_item_name(random, item, "Dagger");
      
      item->w.damage.min = 1;
      item->w.damage.max = 4;
      item->w.accuracy = 4;
      item->w.weight = 1;
      item->w.speed = 1.0f;
     } break;
     
     case ItemID_Club:
     {
      set_item_name(random, item, "Club");
      
      item->w.damage.min = 1;
      item->w.damage.max = 10;
      item->w.accuracy = 0;
      item->w.weight = 1;
      item->w.speed = 1.0f;
     } break;
     
     case ItemID_Sword:
     {
      set_item_name(random, item, "Sword");
      
      item->w.damage.min = 1;
      item->w.damage.max = 8;
      item->w.accuracy = 2;
      item->w.weight = 1;
      item->w.speed = 1.0f;
     } break;
     
     case ItemID_Battleaxe:
     {
      set_item_name(random, item, "Battleaxe");
      
      item->w.damage.min = 1;
      item->w.damage.max = 12;
      item->w.accuracy = -1;
      item->w.weight = 2;
      item->w.speed = 1.2f;
     } break;
     
     case ItemID_Spear:
     {
      set_item_name(random, item, "Spear");
      
      item->w.damage.min = 1;
      item->w.damage.max = 14;
      item->w.accuracy = -2;
      item->w.weight = 2;
      item->w.speed = 1.4f;
     } break;
     
     case ItemID_Warhammer:
     {
      set_item_name(random, item, "Warhammer");
      
      item->w.damage.min = 1;
      item->w.damage.max = 16;
      item->w.accuracy = -3;
      item->w.weight = 2;
      item->w.speed = 1.2f;
     } break;
     
     invalid_default_case;
    }
    }
   else if(item->type == ItemType_Armor)
   {
    // Add armor
    if(is_cursed) set(item->flags, ItemFlag_IsCursed);
    
    item->enchant_level = get_item_enchant_level(random, item);
    set_item_tile_and_equip_tile(item);
    
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
   }
   else if(is_item_consumable(item->type))
   {
    // Add consumable
    item->c.stack_count = stack_count;
    
    v2u potion_duration_range = {10, 20};
    u32 potion_duration = get_random_from_v2u(random, potion_duration_range);
    
    switch(id)
    {
     case ItemID_MightPotion:
     {
      strcpy(item->name.s, "Potion of Might");
      EntityStatus *status = &item->c.status;
      
      status->type = EntityStatusType_Stat;
      status->stat_type = EntityStatusStatType_Str;
      status->value.max = 2;
      status->duration = potion_duration;
      strcpy(status->name.s, "Might");
      
      sprintf(status->player_start.s, "You drink the %s, you feel stronger.", item->name.s);
      sprintf(status->player_end.s, "%sYou don't feel as mighty anymore.", start_color(Color_LightGray));
      
      sprintf(item->description.s, "Grants +%u Strength for %u-%u turns.", status->value.max, potion_duration_range.min, potion_duration_range.max);
      copy_consumable_info_to_item(item, &item_state->potion_info[Potion_Might]);
     } break;
     
     case ItemID_WisdomPotion:
     {
      strcpy(item->name.s, "Potion of Wisdow");
      EntityStatus *status = &item->c.status;
      
      status->type = EntityStatusType_Stat;
      status->stat_type = EntityStatusStatType_Int;
      status->value.max = 2;
      status->duration = potion_duration;
      strcpy(status->name.s, "Wisdom");
      
      sprintf(status->player_start.s, "You drink the %s, you feel wiser.", item->name.s);
      sprintf(status->player_end.s, "%sYou don't feel as wise anymore.", start_color(Color_LightGray));
      
      sprintf(item->description.s, "Grants +%u Intelligence for %u turns.", status->value.max, status->duration);
      copy_consumable_info_to_item(item, &item_state->potion_info[Potion_Wisdom]);
     } break;
     
     case ItemID_AgilityPotion:
     {
      strcpy(item->name.s, "Potion of Agility");
      EntityStatus *status = &item->c.status;
      
      status->type = EntityStatusType_Stat;
      status->stat_type = EntityStatusStatType_Dex;
      status->value.max = 2;
      status->duration = potion_duration;
      strcpy(status->name.s, "Agility");
      
      sprintf(status->player_start.s, "You drink the %s, you feel more dexterous.", item->name.s);
      sprintf(status->player_end.s, "%sYou don't feel as agile anymore.", start_color(Color_LightGray));
      
      sprintf(item->description.s, "Grants +%u Dexterity for %u turns.", status->value.max, status->duration);
      copy_consumable_info_to_item(item, &item_state->potion_info[Potion_Agility]);
     } break;
     
     case ItemID_ElusionPotion:
     {
      strcpy(item->name.s, "Potion of Elusion");
      EntityStatus *status = &item->c.status;
      
      status->type = EntityStatusType_Stat;
      status->stat_type = EntityStatusStatType_EV;
      status->value.max = 2;
      status->duration = potion_duration;
      strcpy(status->name.s, "Elusion");
      
      sprintf(status->player_start.s, "You drink the %s, you feel more evasive.", item->name.s);
      sprintf(status->player_end.s, "%sYou don't feel as elusive anymore.", start_color(Color_LightGray));
      
      sprintf(item->description.s, "Grants +%u Evasion for %u turns.", status->value.max, status->duration);
      copy_consumable_info_to_item(item, &item_state->potion_info[Potion_Elusion]);
     } break;
     
     case ItemID_HealingPotion:
     {
      strcpy(item->name.s, "Potion of Healing");
      ItemInfo *info = &item_state->potion_info[Potion_Healing];
      
      EntityStatus *status = &item->c.status;
      status->type = EntityStatusType_Heal;
      status->is_value_percent = true;
      sprintf(status->player_max_hp.s, "You drink the %s, you feel no different.", item->name.s);
      strcpy(status->player_active.s, "%sYou drink the %s, it heals you for %u health.");
      status->player_active_color = Color_Green;
      status->player_active_target = item->name;
      status->item_info = info;
      
      sprintf(item->description.s, "Restores %u-%u%% of your health.", info->value_range.min, info->value_range.max);
      copy_consumable_info_to_item(item, info);
     } break;
     
     case ItemID_DecayPotion:
     {
      strcpy(item->name.s, "Potion of Decay");
      EntityStatus *status = &item->c.status;
      
      status->type = EntityStatusType_Stat;
      status->stat_type = EntityStatusStatType_StrIntDex;
      status->value.max = -2;
      status->duration = potion_duration;
      strcpy(status->name.s, "Decay");
      
      sprintf(status->player_start.s, "You drink the %s, you feel much weaker.", item->name.s);
      sprintf(status->player_end.s, "%sYou don't feel as weak anymore.", start_color(Color_LightGray));
      
      sprintf(item->description.s, "Reduces Strength, Intelligence and Dexterity by %u for %u turns.", absolute(status->value.max), status->duration);
      copy_consumable_info_to_item(item, &item_state->potion_info[Potion_Decay]);
     } break;
     
     case ItemID_ConfusionPotion:
     {
      strcpy(item->name.s, "Potion of Confusion");
      EntityStatus *status = &item->c.status;
      
      status->type = EntityStatusType_Confusion;
      status->chance = 33;
      status->duration = potion_duration;
      strcpy(status->name.s, "Confusion");
      
      sprintf(status->player_start.s, "You drink the %s, you feel confused.", item->name.s);
      sprintf(status->player_end.s, "%sYou don't feel confused anymore.", start_color(Color_LightGray));
      
      sprintf(item->description.s, "Confuses you for %u turns.", status->duration);
      copy_consumable_info_to_item(item, &item_state->potion_info[Potion_Confusion]);
     } break;
     
     case ItemID_IdentifyScroll:
     {
      item->c.interact_type = ItemInteractType_Identify;
      strcpy(item->name.s, "Scroll of Identify");
      strcpy(item->description.s, "Identify a single item.");
      copy_consumable_info_to_item(item, &item_state->scroll_info[Scroll_Identify]);
     } break;
     
     case ItemID_EnchantWeaponScroll:
     {
      item->c.interact_type = ItemInteractType_EnchantWeapon;
      strcpy(item->name.s, "Scroll of Enchant Weapon");
      strcpy(item->description.s, "Enchant a weapon with +1 damage and accuracy.");
      copy_consumable_info_to_item(item, &item_state->scroll_info[Scroll_EnchantWeapon]);
     } break;
     
     case ItemID_EnchantArmorScroll:
     {
      item->c.interact_type = ItemInteractType_EnchantArmor;
      strcpy(item->name.s, "Scroll of Enchant Armor");
      strcpy(item->description.s, "Enchant a piece of armor with +1 defence.");
      copy_consumable_info_to_item(item, &item_state->scroll_info[Scroll_EnchantArmor]);
     } break;
     
     case ItemID_MagicMappingScroll:
     {
      strcpy(item->name.s, "Scroll of Magic Mapping");
      strcpy(item->description.s, "Reveals the layout of the current level.");
      copy_consumable_info_to_item(item, &item_state->scroll_info[Scroll_MagicMapping]);
     } break;
     
     case ItemID_TeleportationScroll:
     {
      strcpy(item->name.s, "Scroll of Teleportation");
      strcpy(item->description.s, "Teleports you to a random position on the level.");
      copy_consumable_info_to_item(item, &item_state->scroll_info[Scroll_Teleport]);
     } break;
     
     case ItemID_UncurseScroll:
     {
      item->c.interact_type = ItemInteractType_Uncurse;
      strcpy(item->name.s, "Scroll of Uncurse");
      strcpy(item->description.s, "Removes the a curse from a cursed item.");
      copy_consumable_info_to_item(item, &item_state->scroll_info[Scroll_Uncurse]);
     } break;
     
     case ItemID_Ration:
     {
      // TODO(rami): Polish: Random ration eating adjectives, maybe store in the depiction
      // array. Senses: Taste, smell, sight.
      
      strcpy(item->name.s, "Ration");
      item->c.info = &item_state->ration_info;
      
      EntityStatus *status = &item->c.status;
      status->type = EntityStatusType_Heal;
      status->is_value_percent = true;
      sprintf(status->player_max_hp.s, "You eat the %s, you feel no different.", item->name.s);
      strcpy(status->player_active.s, "%sYou eat the %s, it heals you for %u health.");
      status->player_active_color = Color_Green;
      status->player_active_target = item->name;
      status->item_info = item->c.info;
      
      sprintf(item->description.s, "Restores %u-%u%% of your health.", item->c.info->value_range.min, item->c.info->value_range.max);
      copy_consumable_info_to_item(item, item->c.info);
      set(item->flags, ItemFlag_IsIdentified);
     } break;
     
     invalid_default_case;
    }
   }
   
   return(item);
  }
 }
 
 assert(0);
}