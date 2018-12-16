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

#define FONT_METRICS_COUNT 94
#define START_ASCII_CHAR 33

#define TILE_SIZE 32

#define to_tiles(n) ((n) / TILE_SIZE)
#define to_pixels(n) ((n) * TILE_SIZE)

// NOTE(Rami): Change the array element values
// to some good amount.

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
  STATE_UNUSED,
  STATE_USED
} entity_state_e;

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
  int tile;
  int x;
  int y;
  int w;
  int h;
} entity_t;

typedef struct
{
  entity_t *entity;
  char *name;
  int hp;
  int max_hp;
  int new_x;
  int new_y;
  int xp;
  int level;
  int money;
  int speed;
  int fov;
  int damage;
  int armor;
  int turn;
  int inventory_display;
  int inventory_item_count;
  int inventory_item_selected;
} player_t;

typedef struct
{
  entity_state_e state;
  int in_combat;
  int hp;
  // NOTE(Rami): Implement
  // int fov;
  entity_t entity;
} slime_t;

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