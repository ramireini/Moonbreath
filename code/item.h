#define MAX_ITEMS 128

#define INVENTORY_WIDTH 8
#define INVENTORY_HEIGHT 4
#define INVENTORY_AREA (INVENTORY_WIDTH * INVENTORY_HEIGHT)

typedef enum
{
    item_none,
    
    item_dagger,
    item_sword,
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
    
    // TODO(rami): Make sure there are enough potions with bad effects
    // so people won't just blindly drink them in order to identify them.
    
    item_potion_of_might,
    item_potion_of_wisdom,
    item_potion_of_agility,
    item_potion_of_awareness,
    
    item_potion_of_fortitude, // Increase defence
    item_potion_of_resistance, // Increase all resistances
    item_potion_of_healing, // Restore HP
    item_potion_of_haste, // Faster action speed
    item_potion_of_curing, // Removes effects like poison
    
    item_potion_of_vulnerability, // Lower defence
    item_potion_of_clumsiness, // Lower evasion
    item_potion_of_poison, // Drinking this will poison you
    item_potion_of_weakness, // Lowers stats
    
    item_potion_of_flight, // Allows flight for a limited time
    
    // TODO(rami): If we have levels and
    // exp points for player this would
    // give you a chunk of exp.
    //item_potion_of_knowledge,
    //item_poition_of_mana,
    
    item_scroll_of_identify,
    item_scroll_of_brand_weapon,
    item_scroll_of_enchant_weapon,
    item_scroll_of_enchant_armor,
    item_scroll_of_magic_mapping,
    // TODO(rami): Uncurse
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
    item_slot_second_hand,
    item_slot_first_hand,
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
    item_effect_agility,
    item_effect_awareness,
    
    item_effect_fortitude,
    item_effect_resistance,
    item_effect_healing,
    item_effect_haste,
    item_effect_curing,
    
    item_effect_vulnerability,
    item_effect_clumsiness,
    item_effect_poison,
    item_effect_weakness,
    
    item_effect_flight,
    
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
    f32 attack_speed;
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
    
    v2u current;
    item_t *slots[INVENTORY_WIDTH * INVENTORY_HEIGHT];
    
    b32 is_item_being_moved;
    u32 moving_item_src_index;
    u32 moving_item_dest_index;
} inventory_t;

internal void add_weapon_item(game_state_t *game, item_t *items, item_id id, item_rarity rarity, u32 x, u32 y);
internal void add_potion_item(item_t *items, item_id potion_id, u32 x, u32 y);