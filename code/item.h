#ifndef ITEM_H
#define ITEM_H

#define ITEM_COUNT 20

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
  id_rune_ring
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
  slot_first_hand,
  slot_second_hand,
  slot_amulet,
  slot_ring
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
  char name[64];
  i32 tile_x, tile_y;
  char use[64];
  i32 heal_amount;
  i32 damage;
  i32 armor;
  char description[256];
} item_info_t;

#endif // ITEM_H