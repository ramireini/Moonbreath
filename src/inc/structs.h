#ifndef STRUCTS_H
#define STRUCTS_H

enum
{
  ITEM_NONE = 0,
  ITEM_HEALTH_POTION,
  ITEM_IRON_SWORD
};

typedef struct
{
  int health_points;
  int x, y;
  int width, height;
  int speed;
  int view_distance;
} entity_t;

typedef struct
{
  int id;
  int active;
  int x, y;
  int tile;
} item_t;

typedef struct
{
  // NOTE(Rami):  maybe change the value in the future to something we know is just enough
  char name[80];
} item_info_t;

typedef struct
{
  entity_t *entity;
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

typedef struct
{
  char message[80];
  unsigned int r;
  unsigned int g;
  unsigned int b;
} console_message_t;

#endif // STRUCTS_H