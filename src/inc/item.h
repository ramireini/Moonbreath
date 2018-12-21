#ifndef ITEM_H
#define ITEM_H

#include <game.h>

void consume_item();
void equip_or_unequip_item();
void drop_or_remove_inventory_item(int32 action);
void add_game_item(item_id_e id, int32 item_x, int32 item_y);
void add_inventory_item();

#endif // ITEM_H