#ifndef TYPES_H
#define TYPES_H

#include <SDL2/SDL.h>

#define FONT_METRICS_COUNT 85

// NOTE(Rami): maybe change the array values to something which is the minimum needed in the future

typedef enum
{
  ID_NONE = 0,

  ID_LESSER_HEALTH_POTION,
  ID_IRON_SWORD,

  ID_LAST
} item_id_e;

typedef enum
{
  TYPE_NONE = 0,

  TYPE_CONSUME,
  TYPE_EQUIP,

  TYPE_LAST
} item_type_e;

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
  item_id_e item_id;
  int unique_id;
  int is_on_ground;
  int is_equipped;
  int x;
  int y;
} item_t;

typedef struct
{
  item_type_e item_type;
  int tile;
  char name[40];
  char use[40];
  int hp_healed;
  int damage;
  int armor;
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

#endif // TYPES_H