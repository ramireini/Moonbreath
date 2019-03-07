#ifndef TYPES_H
#define TYPES_H

// NOTE(Rami): Change the array element values to some minimum needed amount.
// The config code uses these array sizes as well so make sure to change those too.

#if MOONBREATH_DEBUG
#define debug(fmt, ...) fprintf(stderr, ("%s, %d: "fmt"\n"), __FILE__, __LINE__, ##__VA_ARGS__);
#else
#define debug(fmt, ...)
#endif

#define assert(expression) if(!(expression)) {fprintf(stderr, ("%s, %d: ASSERTION FAILED\n"), __FILE__, __LINE__); *(int *)0 = 0;}

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <stdint.h>

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float r32;
typedef double r64;

#define true 1
#define false 0
typedef i32 b32;

#define internal static

typedef enum
{
  dir_up = 0,
  dir_down,
  dir_left,
  dir_right,

  dir_up_left,
  dir_up_right,
  dir_down_left,
  dir_down_right,
} direction_e;

typedef struct
{
  i32 x;
  i32 y;
} iv2_t;

typedef struct
{
  i32 w;
  i32 h;
} aspect_t;

typedef struct
{
  i32 frame_num;
  i32 frame_count;
  i32 frame_delay;
  u32 frame_last_changed;
} animation_t;

typedef struct
{
  iv2_t pos;
  aspect_t aspect;
  animation_t anim;
} simple_entity_t;

typedef struct
{
  i32 hp;
  i32 damage;
  i32 armor;
  i32 fov;
  i32 move_speed;
  iv2_t pos;
  iv2_t new_pos;
  aspect_t aspect;
  animation_t anim;
} entity_t;

typedef struct
{
  u8 keys[SDL_NUM_SCANCODES];
} keyboard_t;

#include "game.h"
#include "assets.h"
#include "render.h"
#include "level_gen.h"
#include "item.h"

typedef struct
{
  item_t slots[INVENTORY_SLOT_COUNT];
  b32 is_open;
  i32 item_count;
  i32 item_selected;
} inventory_t;

#include "interface.h"
#include "player.h"
#include "slime.h"

typedef struct
{
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Rect camera;
  b32 game_is_running;
  b32 turn_changed;
  u32 time_elapsed;
} game_state_t;

game_state_t game_state;

player_t player;
slime_t slimes[SLIME_COUNT];

asset_t assets;
keyboard_t keyboard;
message_t console_messages[CONSOLE_MESSAGE_COUNT];
item_t items[ITEM_COUNT];
item_info_t items_info[ITEM_INFO_COUNT];
level_t level;

#endif // TYPES_H