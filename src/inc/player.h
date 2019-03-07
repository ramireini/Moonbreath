#ifndef PLAYER_H
#define PLAYER_H

typedef struct
{
  bool active;
  char *name;
  i32 max_hp;
  i32 xp;
  i32 level;
  i32 money;
  i32 turn;
  entity_t entity;
  inventory_t inventory;
} player_t;

#endif // PLAYER_H