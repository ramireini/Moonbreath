#ifndef ITEM_H
#define ITEM_H

#define ITEM_COUNT 64
#define ITEM_INFO_COUNT 64

#define INVENTORY_WIDTH 8
#define INVENTORY_HEIGHT 4
#define INVENTORY_SLOT_COUNT INVENTORY_WIDTH * INVENTORY_HEIGHT

typedef enum
{
    id_none,
    id_lesser_health_potion,
    id_iron_sword,
    id_rune_helmet,
    id_rune_chestplate,
    id_rune_platelegs,
    id_rune_boots,
    id_rune_shield,
    id_rune_amulet,
    id_rune_ring,
    id_red_chestplate,
    id_red_sword
} item_id;

typedef enum
{
    category_none,
    category_weapon,
    category_armor,
    category_consumable
} item_category;

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
    
    // NOTE(rami): Not rendered
    slot_ring,
    
    slot_total
} item_slot;

typedef struct
{
    item_id id;
    u32 unique_id;
    v2u pos;
    b32 in_inventory;
    b32 equipped;
} item_t;

typedef struct
{
    item_id id;
    item_category category;
    item_slot slot;
    v2u tile;
    char name[64];
    char use[64];
    u32 heal_amount;
    u32 damage;
    u32 armor;
    char description[256];
} item_info_t;

typedef struct
{
    b32 occupied;
    u32 index;
} item_slot_data_t;

#endif // ITEM_H