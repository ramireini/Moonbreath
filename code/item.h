#ifndef ITEM_H
#define ITEM_H

#define ITEM_COUNT 10
#define ITEM_INFO_COUNT ITEM_COUNT
#define INVENTORY_SLOT_COUNT 10

#define INVENTORY_WIDTH 8
#define INVENTORY_HEIGHT 4

typedef enum
{
  id_none,
  id_lesser_health_potion,
  id_iron_sword,
  id_rune_helmet,
  id_rune_chestplate,
  id_rune_platelegs,
  id_rune_boots,
  id_rune_shoulders,
  id_rune_gloves,

  id_total
} item_id;

typedef enum
{
  category_none,
  category_weapon,
  category_armor,
  category_consumable,
} item_category;

typedef struct
{
  item_id id;
  i32 unique_id;
  b32 in_inventory;
  b32 equipped;
  i32 x;
  i32 y;
} item_t;

typedef struct
{
  item_id id;
  item_category category;
  char name[64];
  i32 tile;
  char use[64];
  i32 heal_amount;
  i32 damage;
  i32 armor;
  char description[256];
} item_info_t;

#endif // ITEM_H