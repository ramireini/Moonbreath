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
#define color_red v4(178, 34, 34, 255)
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

// NOTE(rami): Do we want a new for pop up text
#define POP_UP_TEXT_COUNT 8

typedef struct
{
  b32 active;
  char str[16];
  r32 x, y;
  i32 x_offset, y_offset;
  r32 change;
  iv4 color;
  r32 speed;
  u32 duration_time;
  u32 start_time;
} pop_up_text_t;

global pop_up_text_t pop_up_text[POP_UP_TEXT_COUNT];

typedef struct
{
  b32 keys[SDL_NUM_SCANCODES];
} keyboard_t;

typedef struct
{
  iv2 frame_start;
  iv2 frame_current;
  i32 frame_count;
  i32 frame_duration;
  u32 frame_last_changed;
} render_t;

#include "game.h"
#include "assets.h"
#include "lighting.h"
#include "level_gen.h"
#include "item.h"
#include "player.h"
#include "monster.h"
#include "ui.h"

typedef struct
{
  item_t slots[INVENTORY_SLOT_COUNT];
  b32 is_open;
  i32 item_count;
  i32 item_selected;
} inventory_t;

typedef struct
{
  game_state state;
  SDL_Window *window;
  SDL_Renderer *renderer;
  iv4 camera;
  i32 turn;
  b32 turn_changed;
  u32 time_elapsed;
  r32 dt;
} game_t;

global player_t player;
global monster_t monster[MONSTER_COUNT];

global game_t game;
SDL_Texture *texture[tex_count];
font_t font[font_count];
global keyboard_t keyboard;
global inventory_t inventory;
global item_t item[ITEM_COUNT];
global item_info_t item_info[ITEM_INFO_COUNT];
global message_t console_message[CONSOLE_MESSAGE_COUNT];
global level_t level;

#endif // TYPES_H