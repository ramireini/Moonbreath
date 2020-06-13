#define MAX_ITEMS 128

#define INVENTORY_WIDTH 8
#define INVENTORY_HEIGHT 4
#define INVENTORY_AREA (INVENTORY_WIDTH * INVENTORY_HEIGHT)

typedef enum
{
    item_none,
    
    item_weapon_start,
    
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
    item_halberd,
    
    item_weapon_end,
    
    item_potion_start,
    
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
    
    // TODO(Rami): Invisibility potion? Also, invisible enemies?
    
    item_potion_of_might,
    item_potion_of_wisdom,
    item_potion_of_agility,
    item_potion_of_fortitude,
    item_potion_of_resistance,
    item_potion_of_healing,
    item_potion_of_focus,
    item_potion_of_curing,
    item_potion_of_flight,
    item_potion_of_decay,
    item_potion_of_weakness,
    item_potion_of_wounding,
    item_potion_of_infection,
    item_potion_of_confusion,
    
    item_potion_end,
    
    item_scroll_start,
    
    item_scroll_of_identify,
    item_scroll_of_infuse_weapon,
    item_scroll_of_enchant_weapon,
    item_scroll_of_enchant_armor,
    item_scroll_of_magic_mapping,
    item_scroll_of_teleportation,
    // TODO(rami): Uncurse
    
    item_scroll_end
} item;

typedef enum
{
    consumable_might,
    consumable_wisdom,
    consumable_agility,
    consumable_fortitude,
    consumable_resistance,
    consumable_healing,
    consumable_focus,
    consumable_curing,
    consumable_flight,
    consumable_decay,
    consumable_weakness,
    consumable_wounding,
    consumable_infection,
    consumable_confusion,
    
    consumable_identify,
    consumable_infuse_weapon,
    consumable_enchant_weapon,
    consumable_enchant_armor,
    consumable_magic_mapping,
    consumable_teleportation,
    
    consumable_count
} consumable;

typedef enum
{
    item_type_none,
    
    item_type_weapon,
    item_type_armor,
    item_type_potion,
    item_type_scroll,
    
    item_type_count
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
    item_equip_slot_none,
    
    item_equip_slot_head,
    item_equip_slot_body,
    item_equip_slot_legs,
    item_equip_slot_feet,
    item_equip_slot_amulet,
    item_equip_slot_second_hand,
    item_equip_slot_first_hand,
    item_equip_slot_ring,
    
    item_equip_slot_count
} item_equip_slot;

// TODO(Rami): If armor can have an element in the future then
// this should be named into something like "item_element_type_none".
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
    use_type_none,
    
    use_type_move,
    use_type_identify,
    use_type_enchant_weapon,
    use_type_enchant_armor
} use_type;

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
    u32 duration;
    u32 value;
} item_consumable_t;

typedef struct
{
    item id;
    char name[32];
    
    // TODO(rami): This is here so it can be used by consumables
    // and other items. When the time comes, if we still want to
    // have flavour text for non-consumables, figure out where to
    // place it on the item window or wherever.
    char description[256];
    v2u pos;
    v2u tile;
    
    item_rarity rarity;
    item_equip_slot equip_slot;
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
    u32 index;
    item_t *item;
} slot_t;

typedef struct
{
    b32 is_open;
    b32 is_asking_player;
    v2u pos;
    item_t *slots[INVENTORY_WIDTH * INVENTORY_HEIGHT];
    
    use_type use_item_type;
    u32 use_item_src_index;
    u32 use_item_dest_index;
} inventory_t;

typedef struct
{
    b32 is_known[consumable_count];
    v2u tiles[consumable_count];
} consumable_info_t;

internal void add_weapon_item(item id, item_rarity rarity, u32 x, u32 y, game_state_t *game, item_t *items);
internal void add_consumable_item(item id, u32 x, u32 y, item_t *items, consumable_info_t *consumable);