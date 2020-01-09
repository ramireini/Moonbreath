#define INVENTORY_WIDTH 8
#define INVENTORY_HEIGHT 4
#define INVENTORY_SLOT_COUNT INVENTORY_WIDTH * INVENTORY_HEIGHT

typedef enum
{
    id_none,
    
    // Head
    
    // Body
    
    // Legs
    id_leather_pants,
    id_warlords_platelegs,
    
    // Feet
    id_leather_boots,
    id_steel_boots,
    id_leather_shoes,
    id_greaves,
    id_rangers_boots,
    
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