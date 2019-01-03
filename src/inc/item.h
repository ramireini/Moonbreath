#ifndef ITEM_H
#define ITEM_H

#include <game.h>

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
  int32 unique_id;
  bool32 is_on_ground;
  bool32 is_equipped;
  int32 x;
  int32 y;
} item_t;

typedef struct
{
  item_id_e item_id;
  char name[256];
  item_type_e item_type;
  int32 tile;
  char use[256];
  int32 hp_healed;
  int32 damage;
  int32 armor;
  char description[256];
} item_info_t;

extern item_t items[ITEM_COUNT];
extern item_info_t items_info[ITEM_INFO_COUNT];
extern item_t inventory[INVENTORY_COUNT];

void render_inventory();
void render_items();
void consume_item();
void equip_or_unequip_item();
void drop_or_remove_item(int32 action);
void add_game_item(item_id_e id, int32 item_x, int32 item_y);
void add_inventory_item();

#endif // ITEM_H