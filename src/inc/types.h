#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

#define true 1
#define false 0
typedef int32 bool32;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef float real32;
typedef double real64;

#define FONT_METRICS_COUNT 94
#define START_ASCII_CHAR 33

#define TILE_SIZE 32

// NOTE(Rami): Change the array element values
// to some good amount.
// 
// The config applying code uses these array sizes
// so make sure to change those too.

// NOTE(Rami): Remember to add level tiles
// to the functions that use them.
typedef enum
{
  TILE_WALL_STONE = 0,
  TILE_FLOOR_GRASS,
  TILE_FLOOR_STONE,
  TILE_DOOR_CLOSED,
  TILE_DOOR_OPEN,
  TILE_PATH_UP,
  TILE_PATH_DOWN
} level_tiles_e;

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

typedef enum
{
  FONT_CLASSIC = 0,
  FONT_CURSIVE
} font_e;

typedef enum
{
  TEX_TILEMAP = 0,
  TEX_GAME_TILESET,
  TEX_ITEM_TILESET,
  TEX_PLAYER_SPRITE_SHEET,
  TEX_MONSTER_SPRITE_SHEET,
  TEX_INVENTORY_WIN,
  TEX_INVENTORY_ITEM_WIN,
  TEX_INVENTORY_ITEM_SELECTED,
  TEX_INTERFACE_CONSOLE_WIN,
  TEX_INTERFACE_STATS_WIN
} texture_e;

typedef struct
{
  int32 tile;
  int32 hp;
  int32 damage;
  int32 armor;
  int32 fov;
  int32 x;
  int32 y;
  int32 w;
  int32 h;
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

typedef struct
{
  bool32 in_combat;
  entity_t *entity;
} slime_t;

typedef struct
{
  item_id_e item_id;
  int32 unique_id;
  bool32 is_on_ground;
  bool32 is_equipped;
  int32 x;
  int32 y;
} item_t;

typedef struct
{
  item_id_e item_id;
  char name[256];
  item_type_e item_type;
  int32 tile;
  char use[256];
  int32 hp_healed;
  int32 damage;
  int32 armor;
  char description[256];
} item_info_t;

typedef struct
{
  int32 x;
  int32 y;
} pos_t;

typedef struct
{
  int32 x;
  int32 y;
  int32 w;
  int32 h;
} room_t;

typedef struct
{
  char msg[256];
  int32 msg_color;
} console_message_t;

typedef struct
{
  int32 x;
  int32 y;
  int32 w;
  int32 h;
  int32 unique_advance_in_px;
} glyph_metrics_t;

typedef struct
{
  SDL_Texture *atlas;
  glyph_metrics_t metrics[FONT_METRICS_COUNT];
  int32 space_in_px;
  int32 shared_advance_in_px;
} font_t;

#endif // TYPES_H