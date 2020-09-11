#define MAX_ITEM_COUNT 256

#define INVENTORY_WIDTH 8
#define INVENTORY_HEIGHT 4
#define INVENTORY_SLOT_COUNT (INVENTORY_WIDTH * INVENTORY_HEIGHT)

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
    //ItemID_InfuseWeaponScroll,
    ItemID_EnchantWeaponScroll,
    ItemID_EnchantArmorScroll,
    ItemID_MagicMappingScroll,
    ItemID_TeleportationScroll,
    // TODO(rami): Uncurse
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
    ItemDamageType_None,
    
    ItemDamageType_Physical,
    ItemDamageType_Fire,
    ItemDamageType_Ice,
    
    // TODO(rami): More of them.
    //ItemDamageType_Holy,
    //ItemDamageType_Slaying,
    
    ItemDamageType_Count
} ItemDamageType;

typedef enum
{
    ItemHandedness_None,
    
    ItemHandedness_OneHanded,
    ItemHandedness_TwoHanded
} ItemHandedness;

typedef enum
{
    ItemUseType_None,
    
    ItemUseType_Move,
    ItemUseType_Identify,
    ItemUseType_EnchantWeapon,
    ItemUseType_EnchantArmor
} ItemUseType;

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
    u32 duration;
    u32 value;
} ItemConsumable;

typedef struct
{
    ItemID id;
    char name[32];
    char description[256];
    v2u pos;
    v2u tile;
    
    ItemRarity rarity;
    ItemSlot slot;
    ItemHandedness handedness;
    ItemDamageType primary_damage_type;
    ItemDamageType secondary_damage_type;
    s32 enchantment_level;
    
    ItemType type;
    union
    {
        ItemWeapon w;
        ItemArmor a;
        ItemConsumable c;
    };
    
    u32 extra_stat_count;
    // TODO(rami): Extra stats for mythical items.
    
    b32 in_inventory;
    b32 is_identified;
    b32 is_equipped;
    b32 is_cursed;
    b32 has_been_seen;
} Item;

typedef struct
{
    u32 index;
    Item *item;
} InventorySlot;

typedef struct
{
    b32 is_open;
    b32 is_asking_player;
    Item *slots[INVENTORY_WIDTH * INVENTORY_HEIGHT];
    v2u pos;
    
    ItemUseType item_use_type;
    u32 use_item_src_index;
    u32 use_item_dest_index;
} Inventory;

typedef struct
{
    b32 potion_is_known[Potion_Count];
    v2u potion_tiles[Potion_Count];
    
    b32 scroll_is_known[Scroll_Count];
    v2u scroll_tiles[Scroll_Count];
} ItemInfo;

internal void add_weapon_item(RandomState *random, Item *items, ItemID id, ItemRarity rarity, u32 x, u32 y);
internal void add_armor_item(RandomState *random, Item *items, ItemID id, u32 x, u32 y);
internal void add_consumable_item(RandomState *random, Item *items, ItemInfo *item_info, ItemID id, u32 x, u32 y);
internal ItemType random_item_type(RandomState *random);
internal ItemID random_weapon(RandomState *random);
internal ItemID random_leather_armor(RandomState *random);
internal ItemID random_steel_armor(RandomState *random);
internal ItemID random_potion(RandomState *random);
internal ItemID random_scroll(RandomState *random);
internal u32 item_type_chance_index(ItemType type);
internal u32 potion_chance_index(ItemID id);
internal u32 scroll_chance_index(ItemID id);
internal Item* get_item_on_pos(v2u pos, Item *items);
internal b32 add_item_to_inventory(Item *item, Inventory *inventory);