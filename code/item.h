#define MAX_INVENTORY_SLOT_COUNT 52
#define MAX_ITEM_COUNT 1024
#define MAX_ITEM_STAT_COUNT (ItemStatType_Count - 1)

typedef enum
{
    ItemID_None,
    
    ItemID_WeaponStart,
    ItemID_Dagger,
    ItemID_Club,
    ItemID_Sword,
    ItemID_Battleaxe,
    ItemID_Spear,
    ItemID_Warhammer,
    ItemID_WeaponEnd,
    
    ItemID_ArmorStart,
    ItemID_LeatherHelmet,
    ItemID_LeatherChestplate,
    ItemID_LeatherGreaves,
    ItemID_LeatherBoots,
    
    ItemID_SteelHelmet,
    ItemID_SteelChestplate,
    ItemID_SteelGreaves,
    ItemID_SteelBoots,
    ItemID_ArmorEnd,
    
    ItemID_PotionStart,
    ItemID_MightPotion,
    ItemID_WisdomPotion,
    ItemID_AgilityPotion,
    ItemID_ElusionPotion,
    ItemID_HealingPotion,
    ItemID_DecayPotion,
    ItemID_ConfusionPotion,
    ItemID_PotionEnd,
    
    ItemID_ScrollStart,
    ItemID_IdentifyScroll,
    ItemID_EnchantWeaponScroll,
    ItemID_EnchantArmorScroll,
    ItemID_MagicMappingScroll,
    ItemID_TeleportationScroll,
    ItemID_UncurseScroll,
    ItemID_ScrollEnd,
    
    ItemID_Ration
} ItemID;

typedef enum
{
    ItemFlag_IsIdentified = (1 << 1),
    ItemFlag_IsEquipped = (1 << 2),
    ItemFlag_IsCursed = (1 << 3),
    ItemFlag_IsMarked = (1 << 4),
    ItemFlag_IsSelected = (1 << 5),
    ItemFlag_InInventory = (1 << 6),
    ItemFlag_HasBeenSeen = (1 << 7)
} ItemFlag;

typedef enum
{
    InventoryFlag_Open = (1 << 1),
    InventoryFlag_MultiplePickup = (1 << 2),
    InventoryFlag_MultipleExamine = (1 << 3),
    InventoryFlag_Examine = (1 << 4),
    InventoryFlag_Adjust = (1 << 5),
    InventoryFlag_Mark = (1 << 6),
    InventoryFlag_AskingPlayer = (1 << 7),
    InventoryFlag_ReadyForKeypress = (1 << 8)
} InventoryFlag;

typedef enum
{
    ArmorItemType_None,
    
    ArmorItemType_Leather,
    ArmorItemType_Steel,
    
    ArmorItemType_Count
} ArmorItemType;

typedef enum
{
    ItemType_None,
    
    ItemType_Weapon,
    ItemType_Armor,
    ItemType_Potion,
    ItemType_Scroll,
    ItemType_Ration,
    
    ItemType_Count
} ItemType;

typedef enum
{
    Potion_Might,
    Potion_Wisdom,
    Potion_Agility,
    Potion_Elusion,
    Potion_Healing,
    Potion_Decay,
    Potion_Confusion,
    
    Potion_Count
} Potion;

typedef enum
{
    Scroll_Identify,
    Scroll_Uncurse,
    Scroll_EnchantWeapon,
    Scroll_EnchantArmor,
    Scroll_MagicMapping,
    Scroll_Teleport,
    
    Scroll_Count
} Scroll;

typedef enum
{
    ItemRarity_None,
    
    ItemRarity_Common,
    ItemRarity_Magical,
    ItemRarity_Mythical,
    
        ItemRarity_Count
} ItemRarity;

typedef enum
{
    ItemSlot_None,
    
    ItemSlot_Weapon,
    ItemSlot_Shield,
    ItemSlot_Head,
    ItemSlot_Body,
    ItemSlot_Legs,
    ItemSlot_Feet,
    ItemSlot_Amulet,
    ItemSlot_Ring,
    
    ItemSlot_Count
} ItemSlot;

typedef enum
{
    ItemHandedness_None,
    
    ItemHandedness_OneHanded,
    ItemHandedness_TwoHanded
} ItemHandedness;

typedef enum
{
 ItemInteractType_None,
    
 ItemInteractType_Identify,
 ItemInteractType_EnchantWeapon,
 ItemInteractType_EnchantArmor,
 ItemInteractType_Uncurse
} ItemInteractType;

typedef enum
{
    ItemActionType_None,
    
    ItemActionType_Pickup,
    ItemActionType_Drop,
    ItemActionType_Equip,
    ItemActionType_Unequip
} ItemActionType;

struct ItemInfo
{
    b32 is_known;
    
    v2u value_range;
    v4u tile_src;
    String32 depiction;
};

typedef struct
{
    ItemHandedness handedness;
    EntityDamage damage;
    s32 accuracy;
    u32 weight;
    f32 speed;
} WeaponItem;

typedef struct
{
    u32 defence;
    u32 weight;
} ArmorItem;

typedef struct
{
    ItemInfo *info;
    EntityStatus status;
 
 ItemInteractType interact_type;
    u32 stack_count;
    String32 depiction;
} ConsumableItem;

typedef enum
{
    ItemStatType_None,
    
    // These are rendered on items in this order.
    ItemStatType_Health,
    ItemStatType_Strength,
    ItemStatType_Intelligence,
    ItemStatType_Dexterity,
    ItemStatType_Evasion,
    ItemStatType_Defence,
    
    ItemStatType_PhysicalResistance,
    ItemStatType_FireResistance,
    ItemStatType_IceResistance,
    ItemStatType_LightningResistance,
    ItemStatType_PoisonResistance,
    ItemStatType_HolyResistance,
    ItemStatType_DarkResistance,
    
    ItemStatType_ViewRange,
    ItemStatType_InvulnerableToTraps,
    
    ItemStatType_Count
} ItemStatType;

typedef struct
{
 ItemStatType type;
 
    s32 value;
    EntityDamageType resist_type;
     String64 description;
} ItemStat;

struct Item
{
    u32 flags;
    
    ItemID id;
    String32 name;
    String256 description;
    
    char inventory_letter;
    char select_letter;
     Mark mark;
    
    v2u pos;
    u32 dungeon_level;
    
    ItemSlot slot;
    ItemRarity rarity;
    s32 enchant_level;
    
    v4u tile_src;
    v4u equip_tile_src;
    
    ItemType type;
    union
    {
        ArmorItem a;
        WeaponItem w;
        ConsumableItem c;
    };
    
    ItemStat stats[MAX_ITEM_STAT_COUNT];
};

typedef struct
{
    Item array[MAX_ITEM_COUNT];
    Mark temp_mark;
    
    DeferWindow pickup_window;
    DeferWindow examine_window;
    Entity *examine_window_entity;
     DungeonTrap *examine_window_trap;
    
    ItemInfo potion_info[Potion_Count];
    ItemInfo scroll_info[Scroll_Count];
    ItemInfo ration_info;
    } ItemState;

typedef struct
{
    u32 flags;
    
    b32 validate_view;
    DeferWindow window;
    
 ItemInteractType interact_type;
    DeferWindow interact_window;
    
    Item *examine_item;
    Item *slots[MAX_INVENTORY_SLOT_COUNT];
    Owner item_owners[MAX_OWNER_COUNT];
} Inventory;

// TODO(rami): Polish: Log message colors depending on message.

#define add_weapon_item(random, item_state, dungeon_level, id, rarity, x, y, is_cursed) add_item(random, item_state, dungeon_level, id, rarity, x, y, is_cursed, 0)
#define add_armor_item(random, item_state, dungeon_level, id, x, y, is_cursed) add_item(random, item_state, dungeon_level, id, ItemRarity_Common, x, y, is_cursed, 0)
#define add_consumable_item(random, item_state, dungeon_level, id, x, y, stack_count) add_item(random, item_state, dungeon_level, id, ItemRarity_None, x, y, false, stack_count)

internal void remove_item(Item *item, Owner *item_owners);
internal char *get_entity_damage_type_string(EntityDamageType damage_type);
internal char *get_item_rarity_string(ItemRarity rarity);
internal b32 is_valid_non_inventory_item(Item *item);
internal s32 get_index(s32 value);
internal s32 get_item_stat_type_value(ItemStat *stats, ItemStatType type);
internal v2u get_item_tile_pos(ItemID id, ItemRarity rarity);
internal ItemType get_random_item_type(Random *random);
internal Item *get_dungeon_pos_item(ItemState *item_state, u32 dungeon_level, v2u pos, ItemID id);
internal EntityDamageType get_random_damage_type(Random *random, EntityDamageType exclude_type);
internal String128 get_full_item_name(Item *item);
internal Item *add_item(Random *random, ItemState *item_state, u32 dungeon_level, ItemID id, ItemRarity rarity, u32 x, u32 y, b32 is_cursed, u32 stack_count);
