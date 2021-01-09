#define MAX_ITEM_COUNT 256
#define MAX_INVENTORY_SLOT_COUNT 52

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
    Scroll_EnchantWeapon,
    Scroll_EnchantArmor,
    Scroll_MagicMapping,
    Scroll_Teleportation,
    Scroll_Uncurse,
    
    Scroll_Count
} Scroll;

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
    DamageType_None,
    
    DamageType_Physical,
    DamageType_Fire,
    DamageType_Ice,
    DamageType_Lightning,
    DamageType_Poison,
    DamageType_Holy,
    DamageType_Darkness,
    
    DamageType_Count
} DamageType;

typedef enum
{
    ItemHandedness_None,
    
    ItemHandedness_OneHanded,
    ItemHandedness_TwoHanded
} ItemHandedness;

typedef enum
{
    UsingItemType_None,
    
    UsingItemType_Identify,
    UsingItemType_EnchantWeapon,
    UsingItemType_EnchantArmor,
    UsingItemType_Uncurse
} UsingItemType;

typedef enum
{
    ItemActionType_PickUp,
    ItemActionType_Drop
} ItemActionType;

typedef enum
{
    ItemFlags_Inventory = (1 << 1),
    ItemFlags_Identified = (1 << 2),
    ItemFlags_Equipped = (1 << 3),
    ItemFlags_Cursed = (1 << 4),
    ItemFlags_HasBeenSeen = (1 << 5)
} ItemFlags;

typedef enum
{
    InventoryFlags_Open = (1 << 1),
    InventoryFlags_Inspecting = (1 << 2),
    InventoryFlags_AskingPlayer = (1 << 3),
    InventoryFlags_AdjustingLetter = (1 << 4),
    InventoryFlags_ReadyForKeypress = (1 << 5)
} InventoryFlags;

typedef struct
{
    b32 added_to_inventory;
    b32 added_to_consumable_stack;
} AddedItemResult;

typedef struct
{
    s32 damage;
    s32 accuracy;
    f32 speed;
} ItemWeapon;

typedef struct
{
    s32 defence;
    s32 weight;
} ItemArmor;

typedef struct
{
    StatusEffect status_effect;
    u32 heal_value;
    u32 stack_count;
    char depiction[32];
} ItemConsumable;

typedef struct
{
    ItemID id;
    u32 flags;
    
    char name[32];
    //char mark[64]; // TODO(rami: Mark
    char description[256];
    char inventory_letter;
    v2u pos;
    v2u tile_pos;
    
    ItemRarity rarity;
    ItemSlot slot;
    ItemHandedness handedness;
    DamageType first_damage_type;
    DamageType second_damage_type;
    s32 enchantment_level;
    
    ItemType type;
    union
    {
        ItemWeapon w;
        ItemArmor a;
        ItemConsumable c;
    };
    
    // TODO(rami): Extra stats for mythical items.
    u32 extra_stat_count;
} Item;

typedef struct
{
    u32 flags;
    
    u32 inspect_index;
    UsingItemType using_item_type;
    
    ItemType view_update_item_type;
    u32 entry_size;
    v4u rect;
    View view;
    
    Item *slots[MAX_INVENTORY_SLOT_COUNT];
} Inventory;

typedef struct
{
    b32 is_known;
    v2u tile;
    char depiction[32];
} ConsumableInfo;

typedef struct
{
    ConsumableInfo potion[Potion_Count];
    ConsumableInfo scroll[Scroll_Count];
    
    v2u potion_healing_range;
    v2u ration_healing_range;
} ItemInfo;

internal void add_player_starting_item(Random *random, Item *items, ItemInfo *item_info, Inventory *inventory, ItemID id, u32 x, u32 y);
internal void set_as_known_and_identify_existing(ItemID id, Item *items, ItemInfo *item_info);
internal u32 item_type_chance_index(ItemType type);
internal u32 potion_chance_index(ItemID id);
internal u32 scroll_chance_index(ItemID id);
internal b32 is_pos_occupied_by_item(Item *items, v2u pos);
internal b32 is_item_valid_and_not_in_inventory(Item *item);
internal b32 is_item_equipment(ItemType type);
internal ItemID random_weapon(Random *random);
internal ItemID random_leather_armor(Random *random);
internal ItemID random_steel_armor(Random *random);
internal ItemID random_potion(Random *random);
internal ItemID random_scroll(Random *random);
internal ItemType random_item_type(Random *random);
internal Item *get_item_on_pos(v2u pos, Item *items);
internal Item *add_weapon_item(Random *random, Item *items, ItemID id, ItemRarity rarity, u32 x, u32 y, b32 is_cursed);
internal Item *add_armor_item(Random *random, Item *items, ItemID id, u32 x, u32 y, b32 is_cursed);
internal Item *add_consumable_item(Random *random, Item *items, ItemInfo *item_info, ItemID id, u32 x, u32 y);
internal AddedItemResult add_item_to_inventory(Item *item, Inventory *inventory);