#ifndef TYPES_H
#define TYPES_H

// NOTE(Rami): include this and change all variables in every file to
// ones that support the typedef'd ones below
// #include <stdint.h>

// typedef uint8_t uint8;
// typedef uint16_t uint16;
// typedef uint32_t uint32;

// typedef int8_t int8;
// typedef int16_t int16;
// typedef int32_t int32;

#include <SDL2/SDL.h>

#define to_tiles(n) ((n) / TILE_SIZE)
#define to_pixels(n) ((n) * TILE_SIZE)

#define FONT_METRICS_COUNT 94
#define START_ASCII_CHAR 33

// NOTE(Rami): change the array values to something which is the minimum needed in the future
// this goes for all arrays in the game.

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

// NOTE(Rami): might need to take some members out
// and put them into player_t since were gonna have a
// monster_t as well and they have their own stuff.

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
  item_id_e item_id;
  char name[256];
  item_type_e item_type;
  int tile;
  char use[256];
  int hp_healed;
  int damage;
  int armor;
  char description[256];
} item_info_t;

typedef struct
{
  char name[256];
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
  int attack;
  int armor;
  int turns_taken;
  int inventory_display;
  int inventory_item_count;
  int inventory_hl_index;
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
  char msg[256];
  int msg_color;
} console_message_t;

typedef struct
{
  int x;
  int y;
  int w;
  int h;
  int unique_advance_in_px;
} glyph_metrics_t;

typedef struct
{
  SDL_Texture *atlas;
  glyph_metrics_t metrics[FONT_METRICS_COUNT];
  int space_in_px;
  int shared_advance_in_px;
} font_t;

#endif // TYPES_H