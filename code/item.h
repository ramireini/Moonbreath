#define INVENTORY_WIDTH 8
#define INVENTORY_HEIGHT 4
#define INVENTORY_SLOT_COUNT INVENTORY_WIDTH * INVENTORY_HEIGHT

typedef enum
{
    id_none,
    
    // Head
    id_steel_visage,
    id_demonic_greathelm,
    id_crusader_helmet,
    id_haniaras_mask,
    id_hood_of_shadows,
    id_hood_of_swiftness,
    id_hardleather_helmet,
    id_kings_crown,
    
    // Body
    id_imperial_platebody,
    id_knights_chestguard,
    id_engraved_chestpiece,
    id_steel_chainmain,
    id_mithril_chainmail,
    id_sturdy_leather_shirt,
    id_green_leather_vest,
    id_fine_clotch_shirt,
    
    // Legs
    id_leather_trousers,
    id_hardened_protectors,
    id_savage_guardians,
    id_unwavering_platelegs,
    id_threaded_graves,
    id_hunters_pants,
    id_bronze_platelegs,
    id_fine_legwraps,
    
    // Feet
    id_rugged_boots,
    id_challengers_threads,
    id_mithril_deflectors,
    id_gem_fused_boots,
    id_aspiring_boots,
    id_soldiers_boots,
    id_sturdy_walkers,
    id_steps_of_discipline,
    id_irontoe_boots,
    
    // First hand
    id_ceremonial_dagger,
    id_katana,
    id_broadsword,
    id_battle_edge,
    id_jungle_cleaver,
    id_piercing_advance,
    id_raging_skullcleaver,
    
    // Second Hand
    id_soldiers_heater,
    id_ironwood_buckler,
    id_wall_of_honor,
    id_crystal_shield,
    id_knights_kite_shield,
    id_jaded_aegis,
    id_glacier,
    
    // Amulet
    id_brave_pendant,
    id_dark_heart,
    id_last_echo,
    id_majestic_pendant,
    id_holy_grace,
    id_pendant_of_thorns,
    id_soul_siphon,
    id_calm_gem_necklace,
    id_pure_gem_necklace,
    
    // Ring
    id_ring_of_protection,
    id_obsidian_ring,
    id_scorching_ring,
    id_ring_of_fortitude,
    id_ring_of_sight,
    id_ring_of_avarice,
    id_ring_of_pain,
    
    // Other
    id_small_health_potion,
    id_medium_health_potion,
    id_large_health_potion,
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
    slot_none,
    slot_head,
    slot_body,
    slot_legs,
    slot_feet,
    slot_amulet,
    slot_second_hand,
    slot_first_hand,
    slot_ring,
    
    slot_total
} item_slot;

typedef enum
{
    effect_none,
    effect_heal
} consume_effect;

typedef struct
{
    consume_effect effect;
    char effect_text[64];
    u32 effect_amount;
} consumable_stats_t;

typedef struct
{
    item_id id;
    u32 unique_id;
    v2u pos;
    b32 in_inventory;
    b32 is_equipped;
} item_t;

typedef struct
{
    u32 min_damage;
    u32 max_damage;
    
    // NOTE(rami): Contemplating on stats for the game
    
    u32 strength; // Increases your Damage with Melee Weapons.
    //u32 dexterity; // Increases your Damage with Ranged Weapons.
    //u32 intelligence; // Increases your Damage with Magic.
    u32 defence; // Gained from armor, reduces the amount of damage received.
    u32 vitality; // Each point increases your health amount by one.
    //u32 luck; // Affects chances of a Critical Hit and Evasion.
} stat_t;

typedef struct
{
    item_id id;
    char name[64];
    item_slot slot;
    char description[256];
    v2u tile;
    
    item_type type;
    union
    {
        stat_t stats;
        consumable_stats_t consumable;
    };
} item_info_t;

typedef struct
{
    b32 is_open;
    
    item_t slots[INVENTORY_SLOT_COUNT];
    v2u current_slot;
    u32 item_count;
    
    b32 item_is_being_moved;
    u32 moved_item_src_index;
    u32 moved_item_dest_index;
} inventory_t;