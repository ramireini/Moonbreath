#ifndef STRUCTS_H
#define STRUCTS_H

#include <SDL2/SDL.h>

enum
{
  ITEM_NONE = 0,
  ITEM_HEALTH_POTION,
};

typedef struct
{
  int health_points;
  int x, y;
  int w, h;
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
  char name[40];
  char use[40];
  char description[400];
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
  int hex_color;
} console_message_t;

typedef struct
{
  int x;
  int y;
  int w;
  int h;
  int advance;
} font_metrics_t;

typedef struct
{
  SDL_Texture *atlas;
  font_metrics_t metrics[52];
} font_t;

#endif // STRUCTS_H