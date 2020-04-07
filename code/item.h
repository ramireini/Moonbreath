typedef enum
{
    item_none,
    
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
    
    item_dagger_common,
    item_dagger_magical,
    item_dagger_legendary,
    item_dagger_unrandom_legendary_1,
    item_dagger_unrandom_legendary_2,
} item_id;

typedef enum
{
    type_none,
    
    type_weapon,
    type_armor,
    type_consumable
} item_type;

typedef enum
{
    // NOTE(rami): These are in render order.
    slot_none,
    
    slot_head,
    slot_body,
    slot_legs,
    slot_feet,
    slot_amulet,
    slot_off_hand,
    slot_main_hand,
    slot_ring,
    
    slot_total
} item_slot;

#if 0
// TODO(rami): This should be called like a bonus damage type.
typedef enum
{
    damage_type_physical
} item_damage_type_t;
#endif

typedef enum
{
    handedness_none,
    
    handedness_one_handed,
    handedness_two_handed
} handedness_t;

typedef enum
{
    // Potion Effects
    effect_none,
    
    effect_might,
    effect_wisdom,
    effect_fortitude,
    effect_agility,
    effect_clumsiness,
    effect_haste,
    effect_resistance,
    effect_mana,
    effect_healing,
    effect_flight,
    effect_knowledge,
    effect_poison,
    effect_curing,
    effect_vulnerability,
    
    // Scroll Effects
    effect_identify,
    effect_brand_weapon,
    effect_enchant_weapon,
    effect_enchant_armor,
    effect_magic_mapping
} consume_effect_t;

typedef struct
{
    item_id id;
    u32 unique_id;
    
    v2u pos;
    s32 enchantment_level;
    //item_damage_type_t damage_type;
    
    b32 in_inventory;
    b32 is_identified;
    b32 is_equipped;
    b32 is_cursed;
} item_t;

typedef struct
{
    item_id id;
    char name[32];
    // TODO(rami): Right now we use descriptions to show the use of
    // consumable items.
    char description[256];
    v2u tile;
    item_slot slot;
    item_type type;
    
    handedness_t handedness;
    s32 damage;
    s32 accuracy;
    // TODO(rami): Speed?
    
    s32 defence;
    u32 weight;
    
    consume_effect_t consume_effect;
    u32 effect_amount;
} item_info_t;

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