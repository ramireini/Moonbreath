#ifndef PLAYER_H
#define PLAYER_H

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

#endif // PLAYER_H