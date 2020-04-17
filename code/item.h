typedef enum
{
    item_none,
    
    item_dagger,
    item_short_sword,
    item_long_sword,
    item_scimitar,
    item_katana,
    item_club,
    item_morningstar,
    item_warhammer,
    item_hand_axe,
    item_war_axe,
    item_battleaxe,
    item_spear,
    item_trident,
    item_halberd,
    
    item_potion_of_might,
    item_potion_of_wisdom,
    item_potion_of_fortitude,
    item_potion_of_agility,
    item_potion_of_clumsiness,
    item_potion_of_haste,
    item_potion_of_resistance,
    item_potion_of_mana,
    item_potion_of_healing,
    item_potion_of_flight,
    item_potion_of_knowledge,
    item_potion_of_poison,
    item_potion_of_curing,
    item_potion_of_vulnerability,
    
    item_scroll_of_identify,
    item_scroll_of_brand_weapon,
    item_scroll_of_enchant_weapon,
    item_scroll_of_enchant_armor,
    item_scroll_of_magic_mapping,
} item_id;

typedef enum
{
    item_type_none,
    
    item_type_weapon,
    item_type_armor,
    item_type_potion,
    item_type_scroll
} item_type;

typedef enum
{
    item_rarity_none,
    
    item_rarity_common,
    item_rarity_magical,
    item_rarity_mythical
} item_rarity;

typedef enum
{
    // NOTE(rami): These are in render order.
    item_slot_none,
    
    item_slot_head,
    item_slot_body,
    item_slot_legs,
    item_slot_feet,
    item_slot_amulet,
    item_slot_off_hand,
    item_slot_main_hand,
    item_slot_ring,
    
    item_slot_total
} item_slot;

typedef enum
{
    item_damage_type_none,
    
    item_damage_type_physical,
    item_damage_type_fire,
    item_damage_type_ice,
    
    // TODO(rami): More of them.
    //item_damage_type_holy,
    //item_damage_type_slaying,
    
    item_damage_type_count
} item_damage_type;

typedef enum
{
    item_handedness_none,
    
    item_handedness_one_handed,
    item_handedness_two_handed
} item_handedness;

typedef enum
{
    // Potion Effects
    item_effect_none,
    
    item_effect_might,
    item_effect_wisdom,
    item_effect_fortitude,
    item_effect_agility,
    item_effect_clumsiness,
    item_effect_haste,
    item_effect_resistance,
    item_effect_mana,
    item_effect_healing,
    item_effect_flight,
    item_effect_knowledge,
    item_effect_poison,
    item_effect_curing,
    item_effect_vulnerability,
    
    // Scroll Effects
    item_effect_identify,
    item_effect_brand_weapon,
    item_effect_enchant_weapon,
    item_effect_enchant_armor,
    item_effect_magic_mapping
} item_effect_t;

typedef struct
{
    s32 damage;
    s32 accuracy;
    f32 speed;
} item_weapon_t;

typedef struct
{
    s32 defence;
    s32 weight;
} item_armor_t;

typedef struct
{
    item_effect_t effect;
    u32 effect_amount;
} item_consumable_t;

typedef struct
{
    item_id id;
    
    char name[32];
    
    // TODO(rami): This is here so it can be used by consumables
    // and other items. When the time comes, if we still want to
    // have flavour text for non-consumables, figure out where to
    // place it on the item window or wherever.
    char description[256];
    v2u pos;
    v2u tile;
    
    item_rarity rarity;
    item_slot slot;
    item_handedness handedness;
    item_damage_type primary_damage_type;
    item_damage_type secondary_damage_type;
    s32 enchantment_level;
    
    item_type type;
    union
    {
        item_weapon_t w;
        item_armor_t a;
        item_consumable_t c;
    };
    
    u32 extra_stat_count;
    // TODO(rami): Extra stats for mythical items.
    
    b32 in_inventory;
    b32 is_identified;
    b32 is_equipped;
    b32 is_cursed;
} item_t;

typedef struct
{
    b32 is_open;
    
    u32 w, h;
    v2u current;
    item_t *slots[8 * 4];
    
    b32 is_item_being_moved;
    u32 moving_item_src_index;
    u32 moving_item_dest_index;
} inventory_t;