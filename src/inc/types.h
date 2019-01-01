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

// NOTE(Rami): Change the array element values
// to some minimum needed amount..
// 
// The config applying code uses these array sizes
// so make sure to change those too.

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
  int32 x;
  int32 y;
} pos_t;

typedef struct
{
  char msg[256];
  uint32 msg_color;
} console_message_t;

#endif // TYPES_H