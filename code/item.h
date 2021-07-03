#define MAX_ITEM_COUNT 1024

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
    ItemRarity_Mythical
} ItemRarity;

typedef enum
{
    ItemSlot_None,
    
    ItemSlot_Head,
    ItemSlot_Body,
    ItemSlot_Legs,
    ItemSlot_Feet,
    ItemSlot_Amulet,
    ItemSlot_SecondHand,
    ItemSlot_FirstHand,
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
    InteractType_None,
    
    InteractType_Identify,
    InteractType_EnchantWeapon,
    InteractType_EnchantArmor,
    InteractType_Uncurse
} InteractType;

typedef enum
{
    StatusType_None,
    
    StatusType_Damage,
    StatusType_Heal,
    StatusType_Stat,
    StatusType_Poisoned,
    StatusType_Bound,
    StatusType_Bleeding,
    StatusType_BrokenArmor,
    StatusType_Sightless,
    StatusType_Confused,
    StatusType_Summon
} StatusType;

typedef enum
{
    ItemActionType_None,
    
    ItemActionType_Pickup,
    ItemActionType_Drop,
    ItemActionType_Equip,
    ItemActionType_Unequip
} ItemActionType;

typedef enum
{
    StatType_None,
    
    StatType_Str,
    StatType_Int,
    StatType_Dex,
    StatType_Def,
    StatType_EV,
    StatType_StrIntDex
} StatType;

typedef enum
{
    DamageType_None,
    
    DamageType_Physical,
    DamageType_Fire,
    DamageType_Ice,
    DamageType_Lightning,
    DamageType_Poison,
    DamageType_Holy,
    DamageType_Dark,
    
    DamageType_Count
} DamageType;

typedef struct
{
    s32 min;
    s32 max;
    DamageType type;
    DamageType second_type;
} Damage;

typedef struct
{
    b32 stat_value_applied;
    b32 print_end_on_last_status;
    
    String32 name;
    
    String64 player_hp_max;
    String64 player_start;
    String64 player_end;
    String64 player_active;
    
    b32 is_player_active_custom;
    Color player_active_color;
    String32 player_active_target;
    
    String64 enemy_start;
    String64 enemy_end;
    String64 enemy_active;
    
    StatusType type;
    StatType stat_type;
    
    union
    {
        Damage value;
        Damage damage;
    };
    
    u32 duration;
    u32 chance;
    
    // Sometimes we want to store the original value of something and need it when the status ends.
    u32 stored_value;
    
    Spell *spell;
    ItemInfo *item_info;
} Status;

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
    Damage damage;
    f32 speed;
    s32 accuracy;
} WeaponItem;

typedef struct
{
    s32 defence;
    s32 weight;
} ArmorItem;

typedef struct
{
    ItemInfo *info;
    
    Status status;
    u32 stack_count;
    String32 depiction;
} ConsumableItem;

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
    v4u tile_src;
    v4u equip_tile_src;
    
    ItemType type;
    union
    {
        WeaponItem w;
        ArmorItem a;
        ConsumableItem c;
    };
    
    // TODO(rami): Extra stats for mythical items
    s32 enchant_level;
    u32 extra_stat_count;
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
    DeferWindow window;
    
    ItemType drop_type;
    b32 update_view_after_interact;
    InteractType interact_type;
    DeferWindow interact_window;
    
    Item *examine_item;
    Item *slots[MAX_INVENTORY_SLOT_COUNT];
    Owner item_owners[MAX_OWNER_COUNT];
} Inventory;

internal void remove_item_from_game(Item *item, Owner *item_owners);
internal char *get_damage_type_string(DamageType damage_type);
internal s32 get_index(s32 value);
internal b32 is_item_valid_and_not_in_inventory(Item *item);
internal ItemType get_random_item_type(Random *random);
internal Item *get_dungeon_pos_item(ItemState *items, u32 dungeon_level, v2u pos, ItemID id);
internal Item *add_armor_item(Random *random, ItemState *items, u32 dungeon_level, ItemID id, u32 x, u32 y, b32 is_cursed);
internal Item *add_consumable_item(Random *random, ItemState *items, u32 dungeon_level, ItemID id, u32 x, u32 y, u32 stack_count);
internal Item *add_weapon_item(Random *random, ItemState *items, ItemID id, u32 dungeon_level, ItemRarity rarity, u32 x, u32 y, b32 is_cursed);
internal DamageType get_random_damage_type(Random *random, DamageType exclude_type);