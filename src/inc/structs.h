#ifndef STRUCTS_H
#define STRUCTS_H

typedef struct
{
  int id;
  int x;
  int y;
  int x_dir; // 0 = no flip, 1 = flip
  int width;
  int height;
  int speed;
  int view_distance;
} entity_t;

typedef struct
{
  int class;
  entity_t *player_entity;
} player_t;

typedef struct
{
    int x;
    int y;
} cell_t;

typedef struct
{
    int x;
    int y;
    int w;
    int h;
} room_t;

#endif // STRUCTS_H