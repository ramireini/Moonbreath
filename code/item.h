typedef enum
{
    id_none,
    id_lesser_health_potion,
    id_iron_sword_old,
    id_rune_helmet,
    id_rune_chestplate,
    id_rune_platelegs,
    id_rune_boots,
    id_rune_shield,
    id_rune_amulet,
    id_rune_ring,
    
    //
    
    id_knight_greaves,
    id_ring_of_protection,
    id_iron_sword_new
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
    slot_body,
    slot_amulet,
    slot_head,
    slot_legs,
    slot_feet,
    slot_second_hand,
    slot_first_hand,
    slot_ring, // NOTE(rami): Not rendered
    
    slot_total
} item_slot;

typedef enum
{
    effect_none,
    effect_heal
} consume_effect;

// TODO(rami): We probably want a system we can enumerate through,
// instead of checking things one by one everywhere they need to be checked.
#if 0
typedef enum
{
    stat_none,
    stat_strength,
    stat_defence,
    stat_hp,
    
    stat_total
} stat_type;

typedef struct
{
    stat_type type;
    u32 value;
} stat_t;

stat_t stats[stat_total - 1];
#endif

typedef struct
{
    u32 strength;
    u32 defence;
    u32 hp;
} general_stats;

typedef struct
{
    consume_effect effect;
    char effect_text[64];
    u32 effect_amount;
} consumable_stats;

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
    item_id id;
    char name[64];
    item_slot slot;
    char description[256];
    v2u tile;
    
    item_type type;
    union
    {
        general_stats general;
        consumable_stats consumable;
    };
} item_info_t;