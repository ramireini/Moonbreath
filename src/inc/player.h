#ifndef PLAYER_H
#define PLAYER_H

typedef struct
{
  int32 hp;
  int32 damage;
  int32 armor;
  int32 fov;
  int32 x;
  int32 y;
  int32 w;
  int32 h;
  int32 current_frame;
  int32 total_frames;
  int32 delay_between_frames;
  uint32 frame_last_changed_time;
} entity_t;

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