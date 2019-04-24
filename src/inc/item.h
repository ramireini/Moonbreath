#ifndef ITEM_H
#define ITEM_H

#define ITEM_COUNT 10
#define ITEM_INFO_COUNT ITEM_COUNT
#define INVENTORY_SLOT_COUNT 10

typedef enum
{
  id_none = -1,
  id_lesser_health_potion,
  id_iron_sword,
  id_total = 3
} item_id_e;

typedef enum
{
  type_none = -1,
  type_consume,
  type_equip,
  type_use,
  type_total = 4
} item_type_e;

typedef struct
{
  item_id_e item_id;
  i32 unique_id;
  bool32 is_on_ground;
  bool32 is_equipped;
  iv2_t pos;
} item_t;

typedef struct
{
  item_id_e item_id;
  char name[256];
  item_type_e item_type;
  i32 tile;
  char use[256];
  i32 hp_healed;
  i32 damage;
  i32 armor;
  char description[256];
} item_info_t;

#endif // ITEM_H