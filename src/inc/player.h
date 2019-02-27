#ifndef PLAYER_H
#define PLAYER_H

typedef struct
{
  i32 frame_num;
  i32 frame_count;
  i32 frame_delay;
  u32 frame_last_changed;
} animation_t;

typedef struct
{
  i32 x;
  i32 y;
  i32 w;
  i32 h;
  animation_t anim;
} basic_entity_t;

typedef struct
{
  i32 hp;
  i32 damage;
  i32 armor;
  i32 fov;

  i32 x;
  i32 y;
  i32 w;
  i32 h;

  animation_t anim;
} entity_t;

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