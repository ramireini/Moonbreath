#define MAX_ITEMS 128

#define INVENTORY_WIDTH 8
#define INVENTORY_HEIGHT 4
#define INVENTORY_AREA (INVENTORY_WIDTH * INVENTORY_HEIGHT)

typedef enum
{
    ItemID_None,
    
    ItemID_WeaponStart,
    
    ItemID_Dagger,
    ItemID_Sword,
    ItemID_Scimitar,
    ItemID_Katana,
    ItemID_Club,
    ItemID_Morningstar,
    ItemID_Warhammer,
    ItemID_HandAxe,
    ItemID_WarAxe,
    ItemID_Battleaxe,
    ItemID_Spear,
    ItemID_Halberd,
    
    ItemID_WeaponEnd,
    
    /*
Might (Increase Strength) - Decay (Lower Strength, Intelligence and Dexterity)
 Wisdom (Increase Intelligence) - Decay (Lower Strength, Intelligence and Dexterity)
 Agility (Increase Dexterity) - Decay (Lower Strength, Intelligence and Dexterity)

Fortitude (Increase Defence) - Weakness (Lower Defence)
Resistance (Increase Element Resistances) - ?
Healing (Increase HP) - Wounding (Lower HP)
Focus (Increase Evasion)
Curing (Cure any illnesses) - Infection (Give poison or some sickness)
Flight (Allow Flight) - Confusion (Chance of doing something random)
*/
    
    // TODO(rami): Invisibility potion? Also, invisible enemies?
    
    ItemID_PotionStart,
    
    ItemID_MightPotion,
    ItemID_WisdomPotion,
    ItemID_AgilityPotion,
    ItemID_FortitudePotion,
    ItemID_ResistancePotion,
    ItemID_HealingPotion,
    ItemID_FocusPotion,
    ItemID_CuringPotion,
    ItemID_FlightPotion,
    ItemID_DecayPotion,
    ItemID_WeaknessPotion,
    ItemID_WoundingPotion,
    ItemID_InfectionPotion,
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
    
    //
    
    ItemID_PotionCount = (ItemID_PotionEnd - ItemID_PotionStart - 1),
    ItemID_ScrollCount = (ItemID_ScrollEnd - ItemID_ScrollStart - 1)
} ItemID;

typedef enum
{
    Potion_Might,
    Potion_Wisdom,
    Potion_Agility,
    Potion_Fortitude,
    Potion_Resistance,
    Potion_Healing,
    Potion_Focus,
    Potion_Curing,
    Potion_Flight,
    Potion_Decay,
    Potion_Weakness,
    Potion_Wounding,
    Potion_Infection,
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

// TODO(rami): If armor can have an element in the future then
// this should be named into something like "item_element_type_none".
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
    f32 attack_speed;
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
    
    // TODO(rami): This is here so it can be used by consumables
    // and other items. When the time comes, if we still want to
    // have flavour text for non-consumables, figure out where to
    // place it on the item window or wherever.
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
    v2u pos;
    Item *slots[INVENTORY_WIDTH * INVENTORY_HEIGHT];
    
    ItemUseType item_use_type;
    u32 use_item_src_index;
    u32 use_item_dest_index;
} Inventory;

typedef struct
{
    b32 potion_is_known[Potion_Count];
    v2u potion_tiles[Potion_Count];
    u32 potion_spawn_chances[Potion_Count];
    
    b32 scroll_is_known[Scroll_Count];
    v2u scroll_tiles[Scroll_Count];
    u32 scroll_spawn_chances[Scroll_Count];
} ConsumableData;

internal void add_weapon_item(ItemID id, ItemRarity rarity, u32 x, u32 y, GameState *game, Item *items);
internal void add_consumable_item(ItemID id, u32 x, u32 y, Item *items, ConsumableData *consumable_data);
internal ItemID get_random_potion(RandomState *random);
internal ItemID get_random_scroll(RandomState *random);