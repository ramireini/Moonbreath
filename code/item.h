typedef enum
{
    item_none,
    
    // Head
    item_steel_visage,
    item_demonic_greathelm,
    item_crusaders_helmet,
    item_haniaras_mask,
    item_hood_of_shadows,
    item_hood_of_swiftness,
    item_hardleather_helmet,
    item_kings_crown,
    item_watchers_gaze,
    
    // Body
    item_imperial_platebody,
    item_knights_chestguard,
    item_engraved_chestpiece,
    item_steel_chainmain,
    item_mithril_chainmail,
    item_sturdy_leather_shirt,
    item_green_leather_vest,
    item_fine_clotch_shirt,
    item_holy_garb,
    
    // Legs
    item_leather_trousers,
    item_hardened_protectors,
    item_savage_guardians,
    item_unwavering_platelegs,
    item_threaded_graves,
    item_hunters_pants,
    item_bronze_platelegs,
    item_fine_legwraps,
    item_moonlight_trousers,
    
    // Feet
    item_rugged_boots,
    item_challengers_threads,
    item_mithril_deflectors,
    item_infernal_greaves,
    item_aspiring_boots,
    item_soldiers_boots,
    item_sturdy_walkers,
    item_steps_of_discipline,
    item_irontoe_boots,
    
    // First hand
    item_ceremonial_dagger,
    item_katana,
    item_broadsword,
    item_battle_edge,
    item_jungle_cleaver,
    item_piercing_advance,
    item_raging_skullcleaver,
    item_heavens_reflection,
    item_evil_intent,
    
    // Second Hand
    item_soldiers_heater,
    item_ironwood_buckler,
    item_wall_of_honor,
    item_crystal_shield,
    item_knights_kite_shield,
    item_jaded_aegis,
    item_glacier,
    item_radiant_crest,
    item_adamantite_barrier,
    
    // Amulet
    item_brave_pendant,
    item_dark_heart,
    item_last_echo,
    item_majestic_pendant,
    item_holy_grace,
    item_pendant_of_thorns,
    item_soul_siphon,
    item_calm_gem_necklace,
    item_pure_gem_necklace,
    
    // Ring
    item_ring_of_protection,
    item_obsidian_ring,
    item_scorching_ring,
    item_ring_of_fortitude,
    item_ring_of_sight,
    item_ring_of_avarice,
    item_ring_of_pain,
    item_rusty_ring,
    item_ring_of_thorns,
    
    // Potion
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
    
    // Scroll
    item_scroll_of_identify,
    item_scroll_of_brand_weapon,
    item_scroll_of_enchant_weapon,
    item_scroll_of_enchant_armor,
    item_scroll_of_magic_mapping
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
    v2u current_slot;
    item_t slots[8 * 4];
    
    u32 item_count;
    b32 item_is_being_moved;
    u32 moved_item_src_index;
    u32 moved_item_dest_index;
} inventory_t;