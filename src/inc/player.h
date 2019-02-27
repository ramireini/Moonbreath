#ifndef PLAYER_H
#define PLAYER_H

typedef struct
{
  char *name;
  i32 max_hp;

  i32 new_x;
  i32 new_y;

  i32 xp;
  i32 level;
  i32 money;
  i32 speed;

  i32 turn;

  b32 inventory_display;
  i32 inventory_item_count;
  i32 inventory_item_selected;

  entity_t *entity;
} player_t;

extern player_t *player;

void create_player();
void update_player();
void render_player();
void free_player();

#endif // PLAYER_H