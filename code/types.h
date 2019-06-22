#ifndef TYPES_H
#define TYPES_H

// NOTE(rami): Change the array element values to some minimum needed amount.
// The config code uses these array sizes as well so make sure to change those too

#if MOONBREATH_DEBUG
#define debug(fmt, ...) fprintf(stderr, ("%s, %d: "fmt""), __FILE__, __LINE__, ##__VA_ARGS__);
#else
#define debug(fmt, ...)
#endif

#define assert(expression) if(!(expression)) {fprintf(stderr, ("%s, %d: ASSERTION FAILED\n"), __FILE__, __LINE__); *(int *)0 = 0;}

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <time.h>
#include <stdint.h>

#define internal static
#define global static

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

typedef i32 b32;
enum {false, true};

#define color_white v4(255, 255, 240, 255)
#define color_red v4(140, 38, 38, 255)
#define color_blue v4(0, 128, 255, 255)
#define color_green v4(0, 179, 0, 255)
#define color_yellow v4(207, 175, 0, 255)
#define color_orange v4(255, 165, 0, 255)
#define color_brown v4(231, 165, 106, 255)
#define color_black v4(0, 0, 0, 255)

typedef enum
{
  state_quit,
  state_running
} game_state;

typedef union
{
  struct
  {
    i32 x, y;
  };
  struct
  {
    i32 w, h;
  };
} iv2;

typedef union
{
  struct
  {
    r32 x, y;
  };
} rv2;

typedef union
{
  struct
  {
    i32 r, g, b, a;
  };
  struct
  {
    i32 x, y, w, h;
  };
} iv4;

typedef enum
{
  dir_up,
  dir_down,
  dir_left,
  dir_right,
  dir_left_up,
  dir_right_up,
  dir_left_down,
  dir_right_down,

  dir_count
} direction;

typedef struct
{
  b32 keys[SDL_NUM_SCANCODES];
} keyboard_t;

typedef struct
{
  iv2 start_frame;
  iv2 current_frame;
  i32 frame_count;
  i32 frame_duration;
  u32 frame_last_changed;
} sprite_t;

#include "game.h"
#include "pop_up_text.h"
#include "assets.h"
#include "lighting.h"
#include "level_gen.h"
#include "item.h"
#include "player.h"
#include "monster.h"
#include "ui.h"

typedef struct
{
  item_t slot[INVENTORY_SLOT_COUNT];
  b32 open;
  i32 x, y;
  i32 item_count;
} inventory_t;

typedef struct
{
  game_state state;
  SDL_Window *window;
  SDL_Renderer *renderer;
  iv4 camera;
  i32 turn;
  b32 turn_changed;
  r32 dt;
  u64 perf_count_frequency;
} game_t;

global player_t player;
global monster_t monster[MONSTER_COUNT];

global game_t game;
global SDL_Texture *texture[tex_count];
global font_t font[font_count];
global keyboard_t keyboard;
global inventory_t inventory;
global item_t item[ITEM_COUNT];
global item_info_t item_info[ITEM_INFO_COUNT];
global message_t console_message[CONSOLE_MESSAGE_COUNT];
global pop_up_text_t pop_up_text[POP_UP_TEXT_COUNT];
global level_t level;

#endif // TYPES_H