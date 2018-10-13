#ifndef STRUCTS_H
#define STRUCTS_H

#include "SDL2/SDL.h"

#define FONT_METRICS_AMOUNT 85

// NOTE(Rami): maybe change the array values to something which is the minimum needed in the future

enum
{
  ITEM_HEALTH_POTION = 0,
  ITEM_IRON_SWORD
};

typedef struct
{
  char *name;
  int level;
  int money;
  int hp;
  int max_hp;
  int xp;
  int x;
  int y;
  int w;
  int h;
  int speed;
  int view_distance;
} entity_t;

typedef struct
{
  int id;
  int tile;
  int active;
  int x;
  int y;
} item_t;

typedef struct
{
  // type = 0 = consumable, type = 1 = equipable
  int id;
  int tile;
  int type;
  char name[40];
  char use[40];
  char damage;
  char armor;
  char description[200];
} item_info_t;

typedef struct
{
  entity_t *entity;
  int turns_taken;
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
  font_metrics_t metrics[FONT_METRICS_AMOUNT];
} font_t;

#endif // STRUCTS_H