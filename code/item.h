#ifndef ITEM_H
#define ITEM_H

#define ITEM_COUNT 10
#define ITEM_INFO_COUNT ITEM_COUNT
#define INVENTORY_SLOT_COUNT 10

typedef enum
{
  id_none = 0,
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
  type_none = 0,
  type_consume,
  type_equip,
  type_use,

  type_count
} item_type;

typedef struct
{
  item_id id;
  i32 unique_id;
  b32 in_inventory;
  b32 is_equipped;
  i32 x;
  i32 y;
} item_t;

typedef struct
{
  item_id id;
  item_type type;
  char name[256];
  i32 tile;
  char use[256];
  i32 hp_healed;
  i32 damage;
  i32 armor;
  char description[256];
} item_info_t;

#endif // ITEM_H