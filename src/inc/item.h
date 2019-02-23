#ifndef ITEM_H
#define ITEM_H

#define ITEM_COUNT 10
#define ITEM_INFO_COUNT 10
#define INVENTORY_COUNT 10

typedef enum
{
  ID_NONE = -1,
  ID_LESSER_HEALTH_POTION,
  ID_IRON_SWORD,
  ID_TOTAL = 3
} item_id_e;

typedef enum
{
  TYPE_NONE = -1,
  TYPE_CONSUME,
  TYPE_EQUIP,
  TYPE_TOTAL = 3
} item_type_e;

typedef struct
{
  item_id_e item_id;
  i32 unique_id;
  b32 is_on_ground;
  b32 is_equipped;
  i32 x;
  i32 y;
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

extern item_t items[ITEM_COUNT];
extern item_info_t items_info[ITEM_INFO_COUNT];
extern item_t inventory[INVENTORY_COUNT];

void render_inventory();
void render_items();
void drop_or_remove_item(i32 action);
void consume_item();
void equip_or_unequip_item();
void add_game_item(item_id_e id, i32 item_x, i32 item_y);
void add_inventory_item();

#endif // ITEM_H