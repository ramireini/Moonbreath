#ifndef PLAYER_H
#define PLAYER_H

#include <game.h>

typedef struct
{
  char *name;
  int32 max_hp;
  int32 new_x;
  int32 new_y;
  int32 xp;
  int32 level;
  int32 money;
  int32 speed;
  int32 turn;
  bool32 inventory_display;
  int32 inventory_item_count;
  int32 inventory_item_selected;
  entity_t *entity;
} player_t;

extern player_t *player;

void create_player();
void update_player();
void render_player();
void place_player(int32 tile_x, int32 tile_y);
void free_player();

#endif // PLAYER_H