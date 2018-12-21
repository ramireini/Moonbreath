#ifndef ITEM_H
#define ITEM_H

#include <game.h>

void consume_item(player_t *player);
void equip_or_unequip_item(player_t *player);
void drop_or_remove_inventory_item(player_t *player, int32 drop);
void add_game_item(item_id_e id, int32 item_x, int32 item_y);
void add_inventory_item(player_t *player);

#endif // ITEM_H