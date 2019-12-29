#define INVENTORY_WIDTH 8
#define INVENTORY_HEIGHT 4
#define INVENTORY_SLOT_COUNT INVENTORY_WIDTH * INVENTORY_HEIGHT

typedef enum
{
    id_none,
    id_warlords_platelegs,
    id_ring_of_protection,
    id_ceremony_dagger,
    id_big_health_potion,
    id_katana,
    id_leather_boots,
    id_broadsword,
    id_battle_edge,
    id_jungle_cleaver,
    id_piercing_advance,
    id_soldiers_heater,
    id_steel_boots,
    id_small_health_potion,
    id_medium_health_potion,
    id_raging_skullcleaver,
    id_ironwood_buckler,
    id_wall_of_honor,
    id_crystal_shield,
    id_knights_kite_shield,
    id_jaded_aegis,
    id_glacier,
    id_obsidian_ring,
    id_scorching_ring
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
    
    u32 strength;
    u32 defence;
    u32 hp;
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