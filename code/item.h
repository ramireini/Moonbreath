#define MAX_INVENTORY_SLOT_COUNT 52
#define MAX_ITEM_COUNT 256

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
    ItemFlags_Inventory = (1 << 1),
    ItemFlags_Identified = (1 << 2),
    ItemFlags_Equipped = (1 << 3),
    ItemFlags_Cursed = (1 << 4),
    ItemFlags_Marked = (1 << 5),
        ItemFlags_Selected = (1 << 6),
    ItemFlags_HasBeenSeen = (1 << 7)
} ItemFlags;

typedef enum
{
    InventoryFlags_Open = (1 << 1),
    InventoryFlags_MultiplePickup = (1 << 2),
    InventoryFlags_MultipleExamine = (1 << 3),
    InventoryFlags_Examining = (1 << 4),
    InventoryFlags_Adjusting = (1 << 5),
    InventoryFlags_Marking = (1 << 6),
    InventoryFlags_AskingPlayer = (1 << 7),
    InventoryFlags_ReadyForKeypress = (1 << 8),
} InventoryFlags;

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
    Scroll_EnchantWeapon,
    Scroll_EnchantArmor,
    Scroll_MagicMapping,
    Scroll_Teleportation,
    Scroll_Uncurse,
    
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
    UsingItemType_None,
    
    UsingItemType_Identify,
    UsingItemType_EnchantWeapon,
    UsingItemType_EnchantArmor,
    UsingItemType_Uncurse
} ItemUseType;

typedef enum
{
    ItemActionType_None,
    
    ItemActionType_PickUp,
    ItemActionType_Drop,
    ItemActionType_Equip,
    ItemActionType_Unequip
} ItemActionType;

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

typedef enum
{
    LetterType_Letter,
    LetterType_SelectLetter
} LetterType;

typedef enum
{
    StatusEffectType_None,
    
    StatusEffectType_Might,
    StatusEffectType_Wisdom,
    StatusEffectType_Agility,
    StatusEffectType_Elusion,
    StatusEffectType_Healing,
    StatusEffectType_Decay,
    StatusEffectType_Confusion,
    StatusEffectType_Poison,
    
    StatusEffectType_Bolster,
    StatusEffectType_Bind,
    
    StatusEffectType_Count
} StatusEffectType;

typedef struct
{
    StatusEffectType type;
    u32 value;
    u32 chance;
    u32 duration;
} StatusEffect;

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
    
    Mark mark;
    char name[32];
    char description[256];
    char letter;
    char selection_letter;
    v2u pos;
    v4u tile_src;
    v2u equip_tile_pos;
    
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
    b32 known;
    v4u tile_src;
    char depiction[32];
} ConsumableInfo;

typedef struct
{
    Item array[MAX_ITEM_COUNT];
    
    ConsumableInfo potion_info[Potion_Count];
    ConsumableInfo scroll_info[Scroll_Count];
    
    v2u potion_healing_range;
    v2u ration_healing_range;
} ItemState;

typedef struct
{
    u32 flags;
    
    ItemUseType item_use_type;
    ItemType view_update_item_type;
    u32 entry_size;
    View examine_view;
    View pickup_view;
    View view;
    
    Item *examine_item;
    Item *slots[MAX_INVENTORY_SLOT_COUNT];
} Inventory;

internal void remove_item_from_game(Item *item);
internal u32 item_type_chance_index(ItemType type);
internal b32 is_item_valid_and_not_in_inventory(Item *item);
internal b32 item_fits_using_item_type(ItemUseType type, Item *item);
internal ItemType random_item_type(Random *random);
internal Item *get_item_on_pos(ItemState *items, v2u pos, ItemID id);
internal Item *add_armor_item(Random *random, ItemState *items, ItemID id, u32 x, u32 y, b32 is_cursed);
internal Item *add_consumable_item(Random *random, ItemState *items, ItemID id, u32 x, u32 y, u32 stack_count);
internal Item *add_weapon_item(Random *random, ItemState *items, ItemID id, ItemRarity rarity, u32 x, u32 y, b32 is_cursed);