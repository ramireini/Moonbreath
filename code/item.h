#ifndef ITEM_H
#define ITEM_H

#define ITEM_COUNT 50
#define ITEM_INFO_COUNT 50

#define INVENTORY_WIDTH 8
#define INVENTORY_HEIGHT 4
#define INVENTORY_SLOT_COUNT INVENTORY_WIDTH * INVENTORY_HEIGHT

typedef enum
{
  body_x_offset = -1,
  body_y_offset = -1
} item_offsets;

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
  slot_head,
  slot_body,
  slot_legs,
  slot_feet,
  slot_amulet,
  slot_ring,
  slot_second_hand,
  slot_first_hand,

  slot_total
} item_slot;

typedef struct
{
  item_id id;
  i32 unique_id;
  i32 x, y;
  b32 in_inventory;
  b32 equipped;
} item_t;

typedef struct
{
  item_id id;
  item_category category;
  item_slot slot;
  i32 tile_x, tile_y;
  char name[64];
  char use[64];
  i32 heal_amount;
  i32 damage;
  i32 armor;
  char description[256];
} item_info_t;

typedef struct
{
  b32 occupied;
  i32 index;
} item_slot_data_t;

#endif // ITEM_H