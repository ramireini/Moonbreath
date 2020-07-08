internal ItemID
random_weapon(RandomState *random)
{
    ItemID result = random_number(random, ItemID_WeaponStart + 1, ItemID_WeaponEnd - 1);
    return(result);
}

internal ItemID
random_leather_armour(RandomState *random)
{
    ItemID result = random_number(random, ItemID_LeatherHelmet, ItemID_LeatherBoots);
    return(result);
}

internal ItemID
random_steel_armour(RandomState *random)
{
    ItemID result = random_number(random, ItemID_SteelHelmet, ItemID_SteelBoots);
    return(result);
}

internal ItemID
random_potion(RandomState *random)
{
    ItemID result = random_number(random, ItemID_PotionStart + 1, ItemID_PotionEnd - 1);
    return(result);
}

internal ItemID
random_scroll(RandomState *random)
{
    ItemID result = random_number(random, ItemID_ScrollStart + 1, ItemID_ScrollEnd - 1);
    return(result);
}

internal u32
potion_spawn_chance_index(ItemID id)
{
    u32 result = id - ItemID_PotionStart - 1;
    return(result);
}

internal u32
scroll_spawn_chance_index(ItemID id)
{
    u32 result = id - ItemID_ScrollStart - 1;
    return(result);
}

internal b32
player_is_enchanting(ItemUseType type)
{
    b32 result = (type == ItemUseType_EnchantWeapon ||
                  type == ItemUseType_EnchantArmour);
    
    return(result);
}

internal b32
item_use_is_active(ItemUseType type, u32 slot_index, Inventory *inventory)
{
    b32 result = (inventory->item_use_type == type &&
                  inventory->use_item_src_index == slot_index);
    
    return(result);
}

internal b32
item_use_is_none(u32 slot_index, Inventory *inventory)
{
    b32 result = (inventory->item_use_type == ItemUseType_None &&
                  inventory->use_item_src_index != slot_index);
    
    return(result);
}

internal void
set_consumable_as_known(ItemID id, Item *items, ConsumableData *consumable_data)
{
    switch(id)
    {
        case ItemID_MightPotion: consumable_data->potion_is_known[Potion_Might] = true; break;
        case ItemID_WisdomPotion: consumable_data->potion_is_known[Potion_Wisdom] = true; break;
        case ItemID_AgilityPotion: consumable_data->potion_is_known[Potion_Agility] = true; break;
        case ItemID_FortitudePotion: consumable_data->potion_is_known[Potion_Fortitude] = true; break;
        case ItemID_ResistancePotion: consumable_data->potion_is_known[Potion_Resistance] = true; break;
        case ItemID_HealingPotion: consumable_data->potion_is_known[Potion_Healing] = true; break;
        case ItemID_FocusPotion: consumable_data->potion_is_known[Potion_Focus] = true; break;
        case ItemID_CuringPotion: consumable_data->potion_is_known[Potion_Curing] = true; break;
        case ItemID_FlightPotion: consumable_data->potion_is_known[Potion_Flight] = true; break;
        case ItemID_DecayPotion: consumable_data->potion_is_known[Potion_Decay] = true; break;
        case ItemID_WeaknessPotion: consumable_data->potion_is_known[Potion_Weakness] = true; break;
        case ItemID_WoundingPotion: consumable_data->potion_is_known[Potion_Wounding] = true; break;
        case ItemID_VenomPotion: consumable_data->potion_is_known[Potion_Venom] = true; break;
        case ItemID_ConfusionPotion: consumable_data->potion_is_known[Potion_Confusion] = true; break;
        
        case ItemID_IdentifyScroll: consumable_data->scroll_is_known[Scroll_Identify] = true; break;
        //case ItemID_InfuseWeaponScroll: consumable_data->scroll_is_known[Scroll_InfuseWeapon] = true; break;
        case ItemID_EnchantWeaponScroll: consumable_data->scroll_is_known[Scroll_EnchantWeapon] = true; break;
        case ItemID_EnchantArmourScroll: consumable_data->scroll_is_known[Scroll_EnchantArmour] = true; break;
        case ItemID_MagicMappingScroll: consumable_data->scroll_is_known[Scroll_MagicMapping] = true; break;
        case ItemID_TeleportationScroll: consumable_data->scroll_is_known[Scroll_Teleportation] = true; break;
        
        invalid_default_case;
    }
    
    for(u32 item_index = 0; item_index < MAX_ITEMS; ++item_index)
    {
        Item *item = &items[item_index];
        if(item->id == id)
        {
            item->is_identified = true;
        }
    }
}

internal void
ask_for_item_cancel(GameState *game, String128 *log, Inventory *inventory)
{
    log_text(log, "Cancel and waste the item?, [%c] Yes [%c] No.", game->keybinds[Key_Yes], game->keybinds[Key_No]);
    inventory->is_asking_player = true;
}

internal void
reset_inventory_item_use(Inventory *inventory)
{
    inventory->item_use_type = ItemUseType_None;
    inventory->use_item_src_index = U32_MAX;
    inventory->use_item_dest_index = U32_MAX;
}

internal u32
get_inventory_slot_index(v2u pos)
{
    u32 result = (pos.y * INVENTORY_WIDTH) + pos.x;
    return(result);
}

internal Item *
get_inventory_slot_item(Inventory *inventory, v2u pos)
{
    u32 slot_index = get_inventory_slot_index(pos);
    Item *result = inventory->slots[slot_index];
    return(result);
}

internal InventorySlot
get_slot_from_pos(Inventory *inventory, v2u pos)
{
    InventorySlot result =
    {
        get_inventory_slot_index(pos),
        get_inventory_slot_item(inventory, pos)
    };
    
    return(result);
}

internal b32
is_item_consumable(ItemType type)
{
    b32 result = (type == ItemType_Potion ||
                  type == ItemType_Scroll);
    
    return(result);
}

internal ItemDamageType
random_item_damage_type(GameState *game)
{
    // Skips physical damage type
    ItemDamageType result = random_number(&game->random,
                                          ItemDamageType_None + 2,
                                          ItemDamageType_Count - 1);
    return(result);
}

internal char *
item_id_text(ItemID id)
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
        
        case ItemID_LeatherHelmet:
        case ItemID_LeatherChestplate:
        case ItemID_LeatherGreaves:
        case ItemID_LeatherBoots:
        case ItemID_SteelHelmet:
        case ItemID_SteelChestplate:
        case ItemID_SteelGreaves:
        case ItemID_SteelBoots: result = "Armour"; break;
        
        case ItemID_MightPotion:
        case ItemID_WisdomPotion:
        case ItemID_AgilityPotion:
        case ItemID_FortitudePotion:
        case ItemID_ResistancePotion:
        case ItemID_HealingPotion:
        case ItemID_FocusPotion:
        case ItemID_CuringPotion:
        case ItemID_FlightPotion:
        case ItemID_DecayPotion:
        case ItemID_WeaknessPotion:
        case ItemID_WoundingPotion:
        case ItemID_VenomPotion:
        case ItemID_ConfusionPotion: result = "Potion"; break;
        
        case ItemID_IdentifyScroll:
        //case ItemID_InfuseWeaponScroll:
        case ItemID_EnchantWeaponScroll:
        case ItemID_EnchantArmourScroll:
        case ItemID_TeleportationScroll:
        case ItemID_MagicMappingScroll: result = "Scroll"; break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal char *
item_rarity_text(ItemRarity rarity)
{
    char *result = "";
    
    switch(rarity)
    {
        case ItemRarity_Common: result = "Common "; break;
        case ItemRarity_Magical: result = "Magical "; break;
        case ItemRarity_Mythical: result = "Mythical "; break;
    }
    
    return(result);
}

internal char *
item_handedness_text(ItemHandedness handedness)
{
    char *result = "";
    
    switch(handedness)
    {
        case ItemHandedness_None: break;
        case ItemHandedness_OneHanded: result = "One-Handed"; break;
        case ItemHandedness_TwoHanded: result = "Two-Handed"; break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal char *
item_damage_type_text(ItemDamageType damage_type)
{
    char *result = 0;
    
    switch(damage_type)
    {
        case ItemDamageType_Physical: result = "Physical"; break;
        case ItemDamageType_Fire: result = "Fire"; break;
        case ItemDamageType_Ice: result = "Ice"; break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal char *
item_rarity_color_code(ItemRarity rarity)
{
    char *result = 0;
    
    switch(rarity)
    {
        case ItemRarity_Common: result = start_color(Color_White); break;
        case ItemRarity_Magical: result = start_color(Color_DarkBlue); break;
        case ItemRarity_Mythical: result = start_color(Color_Purple); break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal String128
full_item_name(Item *item)
{
    String128 result = {0};
    
    if(item->type == ItemType_Weapon ||
       item->type == ItemType_Armour)
    {
        if(item->secondary_damage_type)
        {
            sprintf(result.str, "%c%d %s of %s",
                    (item->enchantment_level >= 0) ? '+' : '-',
                    abs(item->enchantment_level),
                    item->name,
                    item_damage_type_text(item->secondary_damage_type));
        }
        else
        {
            sprintf(result.str, "%c%d %s",
                    (item->enchantment_level >= 0) ? '+' : '-',
                    abs(item->enchantment_level),
                    item->name);
        }
    }
    else
    {
        sprintf(result.str, "%s", item->name);
    }
    
    return(result);
}

internal InventorySlot
equipped_inventory_slot_from_item_slot(ItemSlot slot, Inventory *inventory)
{
    InventorySlot result = {0};
    
    for(u32 index = 0; index < INVENTORY_AREA; ++index)
    {
        Item *item = inventory->slots[index];
        if(item)
        {
            if((item->slot == slot) && item->is_equipped)
            {
                result.index = index;
                result.item = inventory->slots[index];
                
                break;
            }
        }
    }
    
    return(result);
}

internal void
render_items(GameState *game, Dungeon *dungeon, Item *items, Assets *assets)
{
    for(u32 item_index = 0; item_index < MAX_ITEMS; ++item_index)
    {
        Item *item = &items[item_index];
        if(item->id &&
           !item->in_inventory &&
           tile_is_seen(dungeon->tiles, item->pos))
        {
            v4u src = tile_rect(item->tile);
            v4u dest = game_dest(game, item->pos);
            SDL_RenderCopy(game->renderer, assets->item_tileset.tex,
                           (SDL_Rect *)&src, (SDL_Rect *)&dest);
            
            if(game->show_ground_item_outline)
            {
                set_render_color(game, Color_DarkGreen);
                SDL_RenderDrawRect(game->renderer, (SDL_Rect *)&dest);
            }
        }
    }
}

internal u32
calculate_evasion(u32 weight, u32 weight_evasion_ratio)
{
    u32 result = 10;
    result -= weight / weight_evasion_ratio;
    return(result);
}

// TODO(rami): Handle additional mythical item stats
// in add_item_stats() and remove_item_stats().
internal void
add_item_stats(Item *item, Entity *player)
{
    if(item->type == ItemType_Weapon)
    {
        player->damage = item->w.damage + item->enchantment_level;
        player->p.accuracy = item->w.accuracy + item->enchantment_level;
        player->p.attack_speed = item->w.speed;
    }
    else if(item->type == ItemType_Armour)
    {
        player->p.defence += item->a.defence;
        
        player->p.weight += item->a.weight;
        player->evasion = calculate_evasion(player->p.weight, player->p.weight_evasion_ratio);
    }
}

internal void
remove_item_stats(Item *item, Entity *player)
{
    if(item->type == ItemType_Weapon)
    {
        player->damage = 1;
        player->p.accuracy = 2;
        player->p.attack_speed = 1.0f;
    }
    else if(item->type == ItemType_Armour)
    {
        player->p.defence -= item->a.defence;
        
        player->p.weight -= item->a.weight;
        player->evasion = calculate_evasion(player->p.weight, player->p.weight_evasion_ratio);
    }
}

internal void
remove_item_from_game(Item *item)
{
    memset(item, 0, sizeof(Item));
}

internal void
remove_item_from_inventory(InventorySlot slot,
                           Entity *player,
                           String128 *log,
                           Inventory *inventory)
{
    if(slot.item->is_equipped)
    {
        remove_item_stats(slot.item, player);
    }
    
    slot.item->in_inventory = false;
    slot.item->is_equipped = false;
    slot.item->pos = player->pos;
    
    inventory->slots[slot.index] = 0;
}

internal void
remove_item_from_inventory_and_game(InventorySlot slot,
                                    Entity *player,
                                    String128 *log,
                                    Inventory *inventory)
{
    remove_item_from_inventory(slot, player, log, inventory);
    remove_item_from_game(slot.item);
}

internal void
equip_item(Item *item, Entity *player)
{
    item->is_equipped = true;
    add_item_stats(item, player);
    
    // TODO(rami): Remove?
#if 0
    string_t item_name = full_item_name(item);
    add_log_string(log, "You equip the %s%s%.",
                   item_rarity_color_code(item->rarity),
                   item_name.str,
                   end_color());
#endif
}

internal void
unequip_item(Item *item, Entity *player)
{
    item->is_equipped = false;
    remove_item_stats(item, player);
    
    // TODO(rami): Remove?
#if 0
    string_t item_name = full_item_name(item);
    add_log_string(log, "You unequip the %s%s%s.",
                   item_rarity_color_code(item->rarity),
                   item_name.str,
                   end_color());
#endif
}

internal Item *
get_item_on_pos(v2u pos, Item *items)
{
    Item *result = 0;
    
    for(u32 item_index = 0; item_index < MAX_ITEMS; ++item_index)
    {
        if(!items[item_index].in_inventory &&
           V2u_equal(items[item_index].pos, pos))
        {
            result = &items[item_index];
            break;
        }
    }
    
    return(result);
}

internal b32
add_item_to_inventory(Item *item, Inventory *inventory)
{
    for(u32 slot_index = 0; slot_index < INVENTORY_AREA; ++slot_index)
    {
        if(!inventory->slots[slot_index])
        {
            inventory->slots[slot_index] = item;
            item->in_inventory = true;
            
            return(true);
        }
    }
    
    return(false);
}

internal void
pick_up_item(Item *items,
             Inventory *inventory,
             Entity *player,
             String128 *log)
{
    Item *item = get_item_on_pos(player->pos, items);
    if(item)
    {
        if(add_item_to_inventory(item, inventory))
        {
            if(item->is_identified)
            {
                String128 item_name = full_item_name(item);
                log_text(log, "You pick up a %s%s%s.",
                         item_rarity_color_code(item->rarity),
                         item_name.str,
                         end_color());
            }
            else
            {
                log_text(log, "You pick up a %s%s%s.",
                         item_rarity_color_code(item->rarity),
                         item_id_text(item->id),
                         end_color());
            }
        }
        else
        {
            log_text(log, "Your inventory is full right now.");
        }
    }
    else
    {
        log_text(log, "You find nothing to pick up.");
    }
}

internal void
add_weapon_item(GameState *game, Item *items, ItemID id, ItemRarity rarity, u32 x, u32 y)
{
    assert(id);
    
    for(u32 item_index = 0; item_index < MAX_ITEMS; ++item_index)
    {
        Item *item = &items[item_index];
        if(!item->id)
        {  
            item->id = id;
            item->pos = V2u(x, y);
            item->slot = ItemSlot_FirstHand;
            item->rarity = rarity;
            item->primary_damage_type = ItemDamageType_Physical;
            item->type = ItemType_Weapon;
            
            switch(id)
            {
                case ItemID_Dagger:
                {
                    item->handedness = ItemHandedness_OneHanded;
                    item->w.damage = 4;
                    item->w.accuracy = 4;
                    item->w.speed = 0.8f;
                    
                    if(rarity == ItemRarity_Common)
                    {
                        strcpy(item->name, "Dagger");
                        item->tile = V2u(11, 0);
                        item->enchantment_level = random_number(&game->random, -2, 2);
                    }
                    else if(rarity == ItemRarity_Magical)
                    {
                        strcpy(item->name, "Dagger");
                        item->tile = V2u(11, 1);
                        item->secondary_damage_type = random_item_damage_type(game);
                        item->enchantment_level = random_number(&game->random, -2, 4);
                    }
                    else
                    {
                        random_name(&game->random, item->name, NameType_Item);
                        item->tile = V2u(11, 2);
                        item->secondary_damage_type = random_item_damage_type(game);
                        item->enchantment_level = random_number(&game->random, -4, 8);
                        
                        // TODO(rami): Extra stats for mythical items.
                        item->extra_stat_count = random_number(&game->random, 1, 4);
                    }
                } break;
                
                case ItemID_Club:
                {
                    item->handedness = ItemHandedness_OneHanded;
                    item->w.damage = 7;
                    item->w.accuracy = 6;
                    item->w.speed = 1.0f;
                    
                    if(rarity == ItemRarity_Common)
                    {
                        strcpy(item->name, "Club");
                        item->tile = V2u(12, 0);
                        item->enchantment_level = random_number(&game->random, -2, 2);
                    }
                    else if(rarity == ItemRarity_Magical)
                    {
                        strcpy(item->name, "Club");
                        item->tile = V2u(12, 1);
                        item->secondary_damage_type = random_item_damage_type(game);
                        item->enchantment_level = random_number(&game->random, -2, 4);
                    }
                    else if(rarity == ItemRarity_Mythical)
                    {
                        random_name(&game->random, item->name, NameType_Item);
                        item->tile = V2u(12, 2);
                        item->secondary_damage_type = random_item_damage_type(game);
                        item->enchantment_level = random_number(&game->random, -4, 8);
                        
                        // TODO(rami): Extra stats for mythical items.
                        item->extra_stat_count = random_number(&game->random, 1, 4);
                    }
                } break;
                
                case ItemID_Sword:
                {
                    item->handedness = ItemHandedness_OneHanded;
                    item->w.damage = 9;
                    item->w.accuracy = 7;
                    item->w.speed = 1.0f;
                    
                    if(rarity == ItemRarity_Common)
                    {
                        strcpy(item->name, "Sword");
                        item->tile = V2u(13, 0);
                        item->enchantment_level = random_number(&game->random, -2, 2);
                    }
                    else if(rarity == ItemRarity_Magical)
                    {
                        strcpy(item->name, "Sword");
                        item->tile = V2u(13, 1);
                        item->secondary_damage_type = random_item_damage_type(game);
                        item->enchantment_level = random_number(&game->random, -2, 4);
                    }
                    else if(rarity == ItemRarity_Mythical)
                    {
                        random_name(&game->random, item->name, NameType_Item);
                        item->tile = V2u(13, 2);
                        item->secondary_damage_type = random_item_damage_type(game);
                        item->enchantment_level = random_number(&game->random, -4, 8);
                        
                        // TODO(rami): Extra stats for mythical items.
                        item->extra_stat_count = random_number(&game->random, 1, 4);
                    }
                } break;
                
                case ItemID_Battleaxe:
                {
                    item->handedness = ItemHandedness_OneHanded;
                    item->w.damage = 11;
                    item->w.accuracy = 3;
                    item->w.speed = 1.2f;
                    
                    if(rarity == ItemRarity_Common)
                    {
                        strcpy(item->name, "Battleaxe");
                        item->tile = V2u(14, 0);
                        item->enchantment_level = random_number(&game->random, -2, 2);
                    }
                    else if(rarity == ItemRarity_Magical)
                    {
                        strcpy(item->name, "Battleaxe");
                        item->tile = V2u(14, 1);
                        item->secondary_damage_type = random_item_damage_type(game);
                        item->enchantment_level = random_number(&game->random, -2, 4);
                    }
                    else if(rarity == ItemRarity_Mythical)
                    {
                        random_name(&game->random, item->name, NameType_Item);
                        item->tile = V2u(14, 2);
                        item->secondary_damage_type = random_item_damage_type(game);
                        item->enchantment_level = random_number(&game->random, -4, 8);
                        
                        // TODO(rami): Extra stats for mythical items.
                        item->extra_stat_count = random_number(&game->random, 1, 4);
                    }
                } break;
                
                case ItemID_Spear:
                {
                    item->handedness = ItemHandedness_TwoHanded;
                    item->w.damage = 13;
                    item->w.accuracy = 0;
                    item->w.speed = 1.3f;
                    
                    if(rarity == ItemRarity_Common)
                    {
                        strcpy(item->name, "Spear");
                        item->tile = V2u(15, 0);
                        item->enchantment_level = random_number(&game->random, -2, 2);
                    }
                    else if(rarity == ItemRarity_Magical)
                    {
                        strcpy(item->name, "Spear");
                        item->tile = V2u(15, 1);
                        item->secondary_damage_type = random_item_damage_type(game);
                        item->enchantment_level = random_number(&game->random, -2, 4);
                    }
                    else if(rarity == ItemRarity_Mythical)
                    {
                        random_name(&game->random, item->name, NameType_Item);
                        item->tile = V2u(15, 2);
                        item->secondary_damage_type = random_item_damage_type(game);
                        item->enchantment_level = random_number(&game->random, -4, 8);
                        
                        // TODO(rami): Extra stats for mythical items.
                        item->extra_stat_count = random_number(&game->random, 1, 4);
                    }
                } break;
                
                case ItemID_Warhammer:
                {
                    item->handedness = ItemHandedness_TwoHanded;
                    item->w.damage = 15;
                    item->w.accuracy = 0;
                    item->w.speed = 1.4f;
                    
                    if(rarity == ItemRarity_Common)
                    {
                        strcpy(item->name, "Warhammer");
                        item->tile = V2u(16, 0);
                        item->enchantment_level = random_number(&game->random, -2, 2);
                    }
                    else if(rarity == ItemRarity_Magical)
                    {
                        strcpy(item->name, "Warhammer");
                        item->tile = V2u(16, 1);
                        item->secondary_damage_type = random_item_damage_type(game);
                        item->enchantment_level = random_number(&game->random, -2, 4);
                    }
                    else if(rarity == ItemRarity_Mythical)
                    {
                        random_name(&game->random, item->name, NameType_Item);
                        item->tile = V2u(16, 2);
                        item->secondary_damage_type = random_item_damage_type(game);
                        item->enchantment_level = random_number(&game->random, -4, 8);
                        
                        // TODO(rami): Extra stats for mythical items.
                        item->extra_stat_count = random_number(&game->random, 1, 4);
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
add_armour_item(GameState *game, Item *items, ItemID id, u32 x, u32 y)
{
    assert(id);
    
    for(u32 item_index = 0; item_index < MAX_ITEMS; ++item_index)
    {
        Item *item = &items[item_index];
        if(!item->id)
        {
            item->id = id;
            item->pos = V2u(x, y);
            item->rarity = ItemRarity_Common;
            item->type = ItemType_Armour;
            
            switch(item->id)
            {
                case ItemID_LeatherHelmet:
                {
                    strcpy(item->name, "Leather Helmet");
                    item->tile = V2u(11, 4);
                    item->slot = ItemSlot_Head;
                    item->a.defence = 2;
                    item->a.weight = 1;
                } break;
                
                case ItemID_LeatherChestplate:
                {
                    strcpy(item->name, "Leather Chestplate");
                    item->tile = V2u(12, 4);
                    item->slot = ItemSlot_Body;
                    item->a.defence = 4;
                    item->a.weight = 3;
                } break;
                
                case ItemID_LeatherGreaves:
                {
                    strcpy(item->name, "Leather Greaves");
                    item->tile = V2u(13, 4);
                    item->slot = ItemSlot_Legs;
                    item->a.defence = 3;
                    item->a.weight = 2;
                } break;
                
                case ItemID_LeatherBoots:
                {
                    strcpy(item->name, "Leather Boots");
                    item->tile = V2u(14, 4);
                    item->slot = ItemSlot_Feet;
                    item->a.defence = 2;
                    item->a.weight = 1;
                } break;
                
                case ItemID_SteelHelmet:
                {
                    strcpy(item->name, "Steel Helmet");
                    item->tile = V2u(11, 5);
                    item->slot = ItemSlot_Head;
                    item->a.defence = 4;
                    item->a.weight = 2;
                } break;
                
                case ItemID_SteelChestplate:
                {
                    strcpy(item->name, "Steel Chestplate");
                    item->tile = V2u(12, 5);
                    item->slot = ItemSlot_Body;
                    item->a.defence = 8;
                    item->a.weight = 6;
                } break;
                
                case ItemID_SteelGreaves:
                {
                    strcpy(item->name, "Steel Greaves");
                    item->tile = V2u(13, 5);
                    item->slot = ItemSlot_Legs;
                    item->a.defence = 6;
                    item->a.weight = 4;
                } break;
                
                case ItemID_SteelBoots:
                {
                    strcpy(item->name, "Steel Boots");
                    item->tile = V2u(14, 5);
                    item->slot = ItemSlot_Feet;
                    item->a.defence = 4;
                    item->a.weight = 2;
                } break;
                
                invalid_default_case;
            }
            
            return;
        }
    }
    
    assert(false);
}

internal void
add_consumable_item(Item *items,
                    RandomState *random,
                    ConsumableData *consumable_data,
                    ItemID id,
                    u32 x, u32 y)
{
    assert(id);
    
    for(u32 item_index = 0; item_index < MAX_ITEMS; ++item_index)
    {
        Item *item = &items[item_index];
        if(!item->id)
        {
            item->id = id;
            item->pos = V2u(x, y);
            item->rarity = ItemRarity_Common;
            
            switch(id)
            {
                case ItemID_MightPotion:
                {
                    strcpy(item->name, "Potion of Might");
                    item->tile = consumable_data->potion_tiles[Potion_Might];
                    item->type = ItemType_Potion;
                    item->c.duration = 15;
                    item->c.value = 2;
                    sprintf(item->description, "Increases strength by %u for %u turns.", item->c.value, item->c.duration);
                    item->is_identified = consumable_data->potion_is_known[Potion_Might];
                } break;
                
                case ItemID_WisdomPotion:
                {
                    strcpy(item->name, "Potion of Wisdom");
                    item->tile = consumable_data->potion_tiles[Potion_Wisdom];
                    item->type = ItemType_Potion;
                    item->c.duration = 15;
                    item->c.value = 2;
                    sprintf(item->description, "Increases intelligence by %u for %u turns.", item->c.value, item->c.duration);
                    item->is_identified = consumable_data->potion_is_known[Potion_Wisdom];
                } break;
                
                case ItemID_AgilityPotion:
                {
                    strcpy(item->name, "Potion of Agility");
                    item->tile = consumable_data->potion_tiles[Potion_Agility];
                    item->type = ItemType_Potion;
                    item->c.duration = 15;
                    item->c.value = 2;
                    sprintf(item->description, "Increases dexterity by %u for %u turns.", item->c.value, item->c.duration);
                    item->is_identified = consumable_data->potion_is_known[Potion_Agility];
                } break;
                
                case ItemID_FortitudePotion:
                {
                    strcpy(item->name, "Potion of Fortitude");
                    item->tile = consumable_data->potion_tiles[Potion_Fortitude];
                    item->type = ItemType_Potion;
                    item->c.duration = 15;
                    item->c.value = 2;
                    sprintf(item->description, "Increases defence by %u for %u turns.", item->c.value, item->c.duration);
                    item->is_identified = consumable_data->potion_is_known[Potion_Fortitude];
                } break;
                
                case ItemID_ResistancePotion:
                {
                    strcpy(item->name, "Potion of Resistance");
                    item->tile = consumable_data->potion_tiles[Potion_Resistance];
                    item->type = ItemType_Potion;
                    item->c.duration = 15;
                    item->c.value = 2;
                    sprintf(item->description, "Increases resistances by %u for %u turns.", item->c.value, item->c.duration);
                    item->is_identified = consumable_data->potion_is_known[Potion_Resistance];
                } break;
                
                case ItemID_HealingPotion:
                {
                    strcpy(item->name, "Potion of Healing");
                    item->tile = consumable_data->potion_tiles[Potion_Healing];
                    item->type = ItemType_Potion;
                    // TODO(rami): Have a range for this, 12 - 24.
                    item->c.value = 15;
                    sprintf(item->description, "Increases HP by %u.", item->c.value);
                    item->is_identified = consumable_data->potion_is_known[Potion_Healing];
                } break;
                
                case ItemID_FocusPotion:
                {
                    strcpy(item->name, "Potion of Focus");
                    item->tile = consumable_data->potion_tiles[Potion_Focus];
                    item->type = ItemType_Potion;
                    item->c.duration = 15;
                    item->c.value = 2;
                    sprintf(item->description, "Increases evasion by %u for %u turns.", item->c.value, item->c.duration);
                    item->is_identified = consumable_data->potion_is_known[Potion_Focus];
                } break;
                
                case ItemID_CuringPotion:
                {
                    strcpy(item->name, "Potion of Curing");
                    item->tile = consumable_data->potion_tiles[Potion_Curing];
                    item->type = ItemType_Potion;
                    sprintf(item->description, "Removes the poison status effect.");
                    item->is_identified = consumable_data->potion_is_known[Potion_Curing];
                } break;
                
                case ItemID_FlightPotion:
                {
                    strcpy(item->name, "Potion of Flight");
                    item->tile = consumable_data->potion_tiles[Potion_Flight];
                    item->type = ItemType_Potion;
                    item->c.duration = 15;
                    sprintf(item->description, "Allows the ability to fly for %u turns.", item->c.duration);
                    item->is_identified = consumable_data->potion_is_known[Potion_Flight];
                } break;
                
                case ItemID_DecayPotion:
                {
                    strcpy(item->name, "Potion of Decay");
                    item->tile = consumable_data->potion_tiles[Potion_Decay];
                    item->type = ItemType_Potion;
                    item->c.duration = 15;
                    item->c.value = 2;
                    sprintf(item->description, "Decreases str, int and dex by %u for %u turns.", item->c.value, item->c.duration);
                    item->is_identified = consumable_data->potion_is_known[Potion_Decay];
                } break;
                
                case ItemID_WeaknessPotion:
                {
                    strcpy(item->name, "Potion of Weakness");
                    item->tile = consumable_data->potion_tiles[Potion_Weakness];
                    item->type = ItemType_Potion;
                    item->c.duration = 15;
                    item->c.value = 2;
                    sprintf(item->description, "Decreases defence by %u for %u turns.", item->c.value, item->c.duration);
                    item->is_identified = consumable_data->potion_is_known[Potion_Weakness];
                } break;
                
                case ItemID_WoundingPotion:
                {
                    strcpy(item->name, "Potion of Wounding");
                    item->tile = consumable_data->potion_tiles[Potion_Wounding];
                    item->type = ItemType_Potion;
                    // TODO(rami): Have a range for this, 12 - 24.
                    item->c.value = 15;
                    sprintf(item->description, "Decreases HP by %u.", item->c.value);
                    item->is_identified = consumable_data->potion_is_known[Potion_Wounding];
                } break;
                
                case ItemID_VenomPotion:
                {
                    strcpy(item->name, "Potion of Venom");
                    item->tile = consumable_data->potion_tiles[Potion_Venom];
                    item->type = ItemType_Potion;
                    item->c.duration = 5;
                    item->c.value = 2;
                    sprintf(item->description, "Poisons you for %u turns, dealing %u damage per turn.", item->c.duration, item->c.value);
                    item->is_identified = consumable_data->potion_is_known[Potion_Venom];
                } break;
                
                case ItemID_ConfusionPotion:
                {
                    strcpy(item->name, "Potion of Confusion");
                    item->tile = consumable_data->potion_tiles[Potion_Confusion];
                    item->type = ItemType_Potion;
                    item->c.duration = 15;
                    item->c.value = 40;
                    sprintf(item->description, "Confuses you for %u turns, each time you move has a %u%% chance of moving somewhere else.", item->c.duration, item->c.value);
                    item->is_identified = consumable_data->potion_is_known[Potion_Confusion];
                } break;
                
                case ItemID_IdentifyScroll:
                {
                    strcpy(item->name, "Scroll of Identify");
                    strcpy(item->description, "Allows you to identify a single item.");
                    item->tile = consumable_data->scroll_tiles[Scroll_Identify];
                    item->type = ItemType_Scroll;
                    item->is_identified = consumable_data->scroll_is_known[Scroll_Identify];
                } break;
                
#if 0
                case ItemID_InfuseWeaponScroll:
                {
                    strcpy(item->name, "Scroll of Infuse Weapon");
                    strcpy(item->description, "???");
                    item->tile = consumable_data->tiles[Scroll_InfuseWeaponScroll];
                    item->type = ItemType_Scroll;
                    item->is_identified = consumable_data->scroll_is_known[Scroll_InfuseWeaponScroll];
                } break;
#endif
                
                case ItemID_EnchantWeaponScroll:
                {
                    strcpy(item->name, "Scroll of Enchant Weapon");
                    strcpy(item->description, "Allows you to enchant a weapon giving it +1 damage and +1 accuracy.");
                    item->tile = consumable_data->scroll_tiles[Scroll_EnchantWeapon];
                    item->type = ItemType_Scroll;
                    item->is_identified = consumable_data->scroll_is_known[Scroll_EnchantWeapon];
                } break;
                
                case ItemID_EnchantArmourScroll:
                {
                    strcpy(item->name, "Scroll of Enchant Armour");
                    strcpy(item->description, "Allows you to enchant an armour giving it +1 defence.");
                    item->tile = consumable_data->scroll_tiles[Scroll_EnchantArmour];
                    item->type = ItemType_Scroll;
                    item->is_identified = consumable_data->scroll_is_known[Scroll_EnchantArmour];
                } break;
                
                case ItemID_MagicMappingScroll:
                {
                    strcpy(item->name, "Scroll of Magic Mapping");
                    strcpy(item->description, "Reveals every tile on the map.");
                    item->tile = consumable_data->scroll_tiles[Scroll_MagicMapping];
                    item->type = ItemType_Scroll;
                    item->is_identified = consumable_data->scroll_is_known[Scroll_MagicMapping];
                } break;
                
                case ItemID_TeleportationScroll:
                {
                    strcpy(item->name, "Scroll of Teleportation");
                    strcpy(item->description, "Teleports you to a random location on the map.");
                    item->tile = consumable_data->scroll_tiles[Scroll_Teleportation];
                    item->type = ItemType_Scroll;
                    item->is_identified = consumable_data->scroll_is_known[Scroll_Teleportation];
                } break;
                
                invalid_default_case;
            }
            
            return;
        }
    }
    
    assert(false);
}