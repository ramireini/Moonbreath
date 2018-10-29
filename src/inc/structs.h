#ifndef STRUCTS_H
#define STRUCTS_H

#include "SDL2/SDL.h"

#define FONT_METRICS_COUNT 85

// NOTE(Rami): maybe change the array values to something which is the minimum needed in the future

typedef enum
{
  ITEM_HEALTH_POTION = 0,
  ITEM_IRON_SWORD
} item_name_e;

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
  int fov;
} entity_t;

typedef struct
{
  // NOTE(Rami): 
  // we could replace this int with a item_name_e item_type/id statement
  //
  // item_id = HEALTH_POTION etc, what the item is
  int item_id;
  // unique_id = the unique number of the item, this way we can have more than one copy of the same item in the game
  int unique_id;
  int is_on_ground;
  int is_equipped;
  int x;
  int y;
} item_t;

typedef struct
{
  // int id;
  // int number_id;
  // type = 0 = consumable, type = 1 = equipable
  int type;
  int tile;
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
} pos_t;

typedef struct
{
  int x;
  int y;
  int w;
  int h;
} room_t;

typedef struct
{
  char msg[80];
  int msg_color;
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
  font_metrics_t metrics[FONT_METRICS_COUNT];
} font_t;

#endif // STRUCTS_H