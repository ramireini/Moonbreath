internal b32
is_item_weapon(ItemID id)
{
    b32 result = (id > ItemID_WeaponStart && id < ItemID_WeaponEnd);
    return(result);
}

internal b32
is_item_potion(ItemID id)
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
        
        case ItemID_Ration: result = make_v2u(12, 2); break;
        
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
get_item_enchantment_level(RandomState *random, ItemRarity rarity)
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

internal void
set_consumable_as_known_and_identify_all(ItemID id, Item *items, ItemInfo *item_info)
{
    // Set the flag of that ID for future generated items.
    switch(id)
    {
        case ItemID_MightPotion: item_info->is_potion_known[Potion_Might] = true; break;
        case ItemID_WisdomPotion: item_info->is_potion_known[Potion_Wisdom] = true; break;
        case ItemID_AgilityPotion: item_info->is_potion_known[Potion_Agility] = true; break;
        case ItemID_ElusionPotion: item_info->is_potion_known[Potion_Elusion] = true; break;
        case ItemID_HealingPotion: item_info->is_potion_known[Potion_Healing] = true; break;
        case ItemID_DecayPotion: item_info->is_potion_known[Potion_Decay] = true; break;
        case ItemID_ConfusionPotion: item_info->is_potion_known[Potion_Confusion] = true; break;
        
        case ItemID_IdentifyScroll: item_info->is_scroll_known[Scroll_Identify] = true; break;
        //case ItemID_InfuseWeaponScroll: item_info->is_scroll_known[Scroll_InfuseWeapon] = true; break;
        case ItemID_EnchantWeaponScroll: item_info->is_scroll_known[Scroll_EnchantWeapon] = true; break;
        case ItemID_EnchantArmorScroll: item_info->is_scroll_known[Scroll_EnchantArmor] = true; break;
        case ItemID_MagicMappingScroll: item_info->is_scroll_known[Scroll_MagicMapping] = true; break;
        case ItemID_TeleportationScroll: item_info->is_scroll_known[Scroll_Teleportation] = true; break;
        
        case ItemID_Ration: break;
        
        invalid_default_case;
    }
    
    // Identify the items of that ID that are already in the game.
    for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
    {
        Item *item = &items[index];
        if(id == item->id)
        {
            item->is_identified = true;
        }
    }
}

internal ItemType
random_item_type(RandomState *random)
{
    ItemType result = random_number(random, ItemType_None + 1, ItemType_Count - 1);
    return(result);
}

internal ItemID
random_weapon(RandomState *random)
{
    ItemID result = random_number(random, ItemID_WeaponStart + 1, ItemID_WeaponEnd - 1);
    return(result);
}

internal ItemID
random_leather_armor(RandomState *random)
{
    ItemID result = random_number(random, ItemID_LeatherHelmet, ItemID_LeatherBoots);
    return(result);
}

internal ItemID
random_steel_armor(RandomState *random)
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

internal b32
is_item_being_used(ItemUseType type, u32 slot_index, Inventory *inventory)
{
    b32 result = (inventory->item_use_type == type &&
                  inventory->use_item_src_index == slot_index);
    
    return(result);
}

internal void
ask_for_item_cancel(GameState *game, String128 *log, Inventory *inventory)
{
    log_add(log, "%sCancel and waste the item?, [%c] Yes [%c] No.", start_color(Color_Yellow), game->keybinds[Key_Yes], game->keybinds[Key_No]);
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
inventory_slot_index(v2u pos)
{
    u32 result = (pos.y * INVENTORY_WIDTH) + pos.x;
    return(result);
}

internal Item *
inventory_slot_item(Inventory *inventory, v2u pos)
{
    u32 index = inventory_slot_index(pos);
    Item *result = inventory->slots[index];
    return(result);
}

internal InventorySlot
get_slot_from_pos(Inventory *inventory, v2u pos)
{
    InventorySlot result =
    {
        inventory_slot_index(pos),
        inventory_slot_item(inventory, pos)
    };
    
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

internal ItemDamageType
get_random_item_damage_type(RandomState *random)
{
    // Skips physical damage type
    ItemDamageType result = random_number(random,
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
        //case ItemID_InfuseWeaponScroll:
        case ItemID_EnchantWeaponScroll:
        case ItemID_EnchantArmorScroll:
        case ItemID_TeleportationScroll:
        case ItemID_MagicMappingScroll: result = "Scroll"; break;
        
        case ItemID_Ration: result = "Ration"; break;
        
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
        case ItemRarity_Common: result = "Common"; break;
        case ItemRarity_Magical: result = "Magical"; break;
        case ItemRarity_Mythical: result = "Mythical"; break;
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
        case ItemHandedness_OneHanded: result = "One-handed"; break;
        case ItemHandedness_TwoHanded: result = "Two-handed"; break;
        
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
        case ItemRarity_Mythical: result = start_color(Color_Orange); break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal String128
full_item_name(Item *item)
{
    String128 result = {0};
    
    if(item->type == ItemType_Weapon ||
       item->type == ItemType_Armor)
    {
        if(item->secondary_damage_type)
        {
            sprintf(result.str, "%c%d %s of %s",
                    sign(item->enchantment_level),
                    absolute(item->enchantment_level),
                    item->name,
                    item_damage_type_text(item->secondary_damage_type));
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
        sprintf(result.str, "%s", item->name);
    }
    
    return(result);
}

internal InventorySlot
equipped_inventory_slot_from_item_slot(ItemSlot slot, Inventory *inventory)
{
    InventorySlot result = {0};
    
    for(u32 index = 0; index < INVENTORY_SLOT_COUNT; ++index)
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
    for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
    {
        Item *item = &items[index];
        if(item->id && !item->in_inventory)
        {
            v4u src = get_tile_rect(item->tile_pos);
            v4u dest = get_game_dest(game, item->pos);
            
            if(is_tile_seen(dungeon->tiles, item->pos))
            {
                item->has_been_seen = true;
                SDL_RenderCopy(game->renderer, assets->tileset.tex, (SDL_Rect *)&src, (SDL_Rect *)&dest);
                
                if(game->show_item_ground_outline)
                {
                    set_render_color(game, Color_DarkGreen);
                    SDL_RenderCopy(game->renderer, assets->ui.tex, (SDL_Rect *)&assets->item_ground_outline, (SDL_Rect *)&dest);
                }
            }
            else if(tile_has_been_seen(dungeon->tiles, item->pos) &&
                    item->has_been_seen)
            {
                render_texture_half_color(game->renderer, assets->tileset.tex, src, dest, false);
                
                if(game->show_item_ground_outline)
                {
                    set_render_color(game, Color_DarkGreen);
                    render_texture_half_color(game->renderer, assets->ui.tex, assets->item_ground_outline, dest, false);
                }
            }
        }
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
        slot.item->is_equipped = false;
    }
    
    slot.item->in_inventory = false;
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
complete_inventory_item_use(Entity *player,
                            String128 *log,
                            Inventory *inventory)
{
    InventorySlot slot = {inventory->use_item_src_index, inventory->slots[slot.index]};
    remove_item_from_inventory_and_game(slot, player, log, inventory);
    reset_inventory_item_use(inventory);
}

internal Item *
get_item_on_pos(v2u pos, Item *items)
{
    Item *result = 0;
    
    for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
    {
        Item *item = &items[index];
        if(!item->in_inventory && equal_v2u(item->pos, pos))
        {
            result = item;
            break;
        }
    }
    
    return(result);
}

internal b32
add_item_to_inventory(Item *item, Inventory *inventory)
{
    for(u32 index = 0; index < INVENTORY_SLOT_COUNT; ++index)
    {
        if(!inventory->slots[index])
        {
            inventory->slots[index] = item;
            item->in_inventory = true;
            
            return(true);
        }
    }
    
    return(false);
}

internal Item *
add_weapon_item(RandomState *random, Item *items, ItemID id, ItemRarity rarity, u32 x, u32 y)
{
    assert(id);
    
    for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
    {
        Item *item = &items[index];
        if(!item->id)
        {  
            item->id = id;
            item->pos = make_v2u(x, y);
            item->slot = ItemSlot_FirstHand;
            item-> handedness = get_item_handedness(item->id);
            item->rarity = rarity;
            item->tile_pos = get_item_tile_pos(item->id, item->rarity);
            item->primary_damage_type = ItemDamageType_Physical;
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
                        item->secondary_damage_type = get_random_item_damage_type(random);
                    }
                    else
                    {
                        random_name(random, item->name, NameType_Item);
                        item->secondary_damage_type = get_random_item_damage_type(random);
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
                        item->secondary_damage_type = get_random_item_damage_type(random);
                    }
                    else if(rarity == ItemRarity_Mythical)
                    {
                        random_name(random, item->name, NameType_Item);
                        item->secondary_damage_type = get_random_item_damage_type(random);
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
                        item->secondary_damage_type = get_random_item_damage_type(random);
                    }
                    else if(rarity == ItemRarity_Mythical)
                    {
                        random_name(random, item->name, NameType_Item);
                        item->secondary_damage_type = get_random_item_damage_type(random);
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
                        item->secondary_damage_type = get_random_item_damage_type(random);
                    }
                    else if(rarity == ItemRarity_Mythical)
                    {
                        random_name(random, item->name, NameType_Item);
                        item->secondary_damage_type = get_random_item_damage_type(random);
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
                        item->secondary_damage_type = get_random_item_damage_type(random);
                    }
                    else if(rarity == ItemRarity_Mythical)
                    {
                        random_name(random, item->name, NameType_Item);
                        item->secondary_damage_type = get_random_item_damage_type(random);
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
                        item->secondary_damage_type = get_random_item_damage_type(random);
                    }
                    else if(rarity == ItemRarity_Mythical)
                    {
                        random_name(random, item->name, NameType_Item);
                        item->secondary_damage_type = get_random_item_damage_type(random);
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
add_armor_item(RandomState *random, Item *items, ItemID id, u32 x, u32 y)
{
    assert(id);
    
    for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
    {
        Item *item = &items[index];
        if(!item->id)
        {
            item->id = id;
            item->pos = make_v2u(x, y);
            item->rarity = ItemRarity_Common;
            item->tile_pos = get_item_tile_pos(item->id, item->rarity);
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
add_consumable_item(RandomState *random,
                    Item *items,
                    ItemInfo *item_info,
                    ItemID id,
                    u32 x, u32 y)
{
    assert(id);
    
    for(u32 index = 0; index < MAX_ITEM_COUNT; ++index)
    {
        Item *item = &items[index];
        if(!item->id)
        {
            item->id = id;
            item->pos = make_v2u(x, y);
            item->rarity = ItemRarity_Common;
            
            switch(id)
            {
                case ItemID_MightPotion:
                {
                    strcpy(item->name, "Potion of Might");
                    item->tile_pos = item_info->potion_tiles[Potion_Might];
                    item->type = ItemType_Potion;
                    item->c.duration = 40;
                    item->c.value = 2;
                    sprintf(item->description, "+%u Strength for %u turns.", item->c.value, item->c.duration);
                    item->is_identified = item_info->is_potion_known[Potion_Might];
                } break;
                
                case ItemID_WisdomPotion:
                {
                    strcpy(item->name, "Potion of Wisdom");
                    item->tile_pos = item_info->potion_tiles[Potion_Wisdom];
                    item->type = ItemType_Potion;
                    item->c.duration = 40;
                    item->c.value = 2;
                    sprintf(item->description, "+%u Intelligence for %u turns.", item->c.value, item->c.duration);
                    item->is_identified = item_info->is_potion_known[Potion_Wisdom];
                } break;
                
                case ItemID_AgilityPotion:
                {
                    strcpy(item->name, "Potion of Agility");
                    item->tile_pos = item_info->potion_tiles[Potion_Agility];
                    item->type = ItemType_Potion;
                    item->c.duration = 40;
                    item->c.value = 2;
                    sprintf(item->description, "+%u Dexterity for %u turns.", item->c.value, item->c.duration);
                    item->is_identified = item_info->is_potion_known[Potion_Agility];
                } break;
                
                case ItemID_ElusionPotion:
                {
                    strcpy(item->name, "Potion of Elusion");
                    item->tile_pos = item_info->potion_tiles[Potion_Elusion];
                    item->type = ItemType_Potion;
                    item->c.duration = 40;
                    item->c.value = 2;
                    sprintf(item->description, "+%u Evasion for %u turns.", item->c.value, item->c.duration);
                    item->is_identified = item_info->is_potion_known[Potion_Elusion];
                } break;
                
                case ItemID_HealingPotion:
                {
                    strcpy(item->name, "Potion of Healing");
                    item->tile_pos = item_info->potion_tiles[Potion_Healing];
                    item->type = ItemType_Potion;
                    u32 lowest_hp = 20;
                    u32 highest_hp = 40;
                    item->c.value = random_number(random, lowest_hp, highest_hp);
                    sprintf(item->description, "Heals you for %u - %u HP.", lowest_hp, highest_hp);
                    item->is_identified = item_info->is_potion_known[Potion_Healing];
                } break;
                
                case ItemID_DecayPotion:
                {
                    strcpy(item->name, "Potion of Decay");
                    item->tile_pos = item_info->potion_tiles[Potion_Decay];
                    item->type = ItemType_Potion;
                    item->c.duration = 40;
                    item->c.value = 2;
                    sprintf(item->description, "-%u Strength, Intelligence and Dexterity for %u turns.", item->c.value, item->c.duration);
                    item->is_identified = item_info->is_potion_known[Potion_Decay];
                } break;
                
                case ItemID_ConfusionPotion:
                {
                    strcpy(item->name, "Potion of Confusion");
                    item->tile_pos = item_info->potion_tiles[Potion_Confusion];
                    item->type = ItemType_Potion;
                    item->c.duration = 40;
                    item->c.value = 33;
                    sprintf(item->description, "Confusion status effect for %u turns.", item->c.duration);
                    item->is_identified = item_info->is_potion_known[Potion_Confusion];
                } break;
                
                case ItemID_IdentifyScroll:
                {
                    strcpy(item->name, "Scroll of Identify");
                    strcpy(item->description, "Identify a single item.");
                    item->tile_pos = item_info->scroll_tiles[Scroll_Identify];
                    item->type = ItemType_Scroll;
                    item->is_identified = item_info->is_scroll_known[Scroll_Identify];
                } break;
                
#if 0
                case ItemID_InfuseWeaponScroll:
                {
                    strcpy(item->name, "Scroll of Infuse Weapon");
                    strcpy(item->description, "???");
                    item->tile_pos = item_info->tiles[Scroll_InfuseWeaponScroll];
                    item->type = ItemType_Scroll;
                    item->is_identified = item_info->is_scroll_known[Scroll_InfuseWeaponScroll];
                } break;
#endif
                
                case ItemID_EnchantWeaponScroll:
                {
                    strcpy(item->name, "Scroll of Enchant Weapon");
                    strcpy(item->description, "Enchant a weapon with +1 damage and accuracy.");
                    item->tile_pos = item_info->scroll_tiles[Scroll_EnchantWeapon];
                    item->type = ItemType_Scroll;
                    item->is_identified = item_info->is_scroll_known[Scroll_EnchantWeapon];
                } break;
                
                case ItemID_EnchantArmorScroll:
                {
                    strcpy(item->name, "Scroll of Enchant Armor");
                    strcpy(item->description, "Enchant a piece of armor with +1 defence.");
                    
                    item->tile_pos = item_info->scroll_tiles[Scroll_EnchantArmor];
                    item->type = ItemType_Scroll;
                    item->is_identified = item_info->is_scroll_known[Scroll_EnchantArmor];
                } break;
                
                case ItemID_MagicMappingScroll:
                {
                    strcpy(item->name, "Scroll of Magic Mapping");
                    strcpy(item->description, "Reveals the layout of the level.");
                    item->tile_pos = item_info->scroll_tiles[Scroll_MagicMapping];
                    item->type = ItemType_Scroll;
                    item->is_identified = item_info->is_scroll_known[Scroll_MagicMapping];
                } break;
                
                case ItemID_TeleportationScroll:
                {
                    strcpy(item->name, "Scroll of Teleportation");
                    strcpy(item->description, "Teleports you to a random position on the level.");
                    item->tile_pos = item_info->scroll_tiles[Scroll_Teleportation];
                    item->type = ItemType_Scroll;
                    item->is_identified = item_info->is_scroll_known[Scroll_Teleportation];
                } break;
                
                case ItemID_Ration:
                {
                    strcpy(item->name, "Ration");
                    item->tile_pos = get_item_tile_pos(item->id, item->rarity);
                    item->type = ItemType_Ration;
                    u32 lowest_hp = 10;
                    u32 highest_hp = 20;
                    item->c.value = random_number(random, lowest_hp, highest_hp);
                    sprintf(item->description, "Heals you for %u - %u HP.", lowest_hp, highest_hp);
                    item->is_identified = true;
                } break;
                
                invalid_default_case;
            }
            
            return(item);
        }
    }
    
    assert(false);
}