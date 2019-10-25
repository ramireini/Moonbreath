#define INVENTORY_WIDTH 8
#define INVENTORY_HEIGHT 4
#define INVENTORY_SLOT_COUNT INVENTORY_WIDTH * INVENTORY_HEIGHT

typedef enum
{
    id_none,
    id_knights_greaves,
    id_ring_of_protection,
    id_iron_sword,
    id_lesser_health_potion
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
    slot_ring,
    
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