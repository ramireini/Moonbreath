#ifndef TYPES_H
#define TYPES_H

// NOTE(Rami): Change the array element values to some minimum needed amount.
// The config code uses these array sizes as well so make sure to change those too.

#define debug(fmt, ...) fprintf(stderr, ("%s, %d: "fmt"\n"), __FILE__, __LINE__, ##__VA_ARGS__);

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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

typedef struct
{
  i32 frame_num;
  i32 frame_count;
  i32 frame_delay;
  u32 frame_last_changed;
} animation_t;

typedef struct
{
  i32 x;
  i32 y;
  i32 w;
  i32 h;
  animation_t anim;
} basic_entity_t;

typedef struct
{
  i32 hp;
  i32 damage;
  i32 armor;
  i32 fov;
  i32 x;
  i32 y;
  i32 w;
  i32 h;
  animation_t anim;
} entity_t;

typedef struct
{
  i32 x;
  i32 y;
} iv2_t;

typedef struct
{
  i32 x;
  i32 y;
  i32 w;
  i32 h;
} iv4_t;

#include <game.h>
#include <assets.h>
#include <text_render.h>
#include <level_gen.h>
#include <item.h>
#include <interface.h>
#include <render.h>
#include <player.h>
#include <slime.h>
#include <update.h>

typedef struct
{
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Rect camera;
  b32 game_is_running;
  b32 turn_changed;
  // NOTE(Rami): Turn into a key array
  SDL_Keycode key_pressed;
  u32 time_elapsed;
  game_assets_t assets;
} game_state_t;

game_state_t global_state;

#endif // TYPES_H