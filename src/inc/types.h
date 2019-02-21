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

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef float real32;
typedef double real64;

#define true 1
#define false 0
typedef int32 bool32;

#include "text_render.h"
#include "level_gen.h"
#include "util_io.h"
#include "item.h"
#include "interface.h"
#include "util_conf.h"
#include "render.h"

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

#include "player.h"
#include "slime.h"
#include "update.h"

typedef enum
{
  tex_tilemap = 0,
  tex_game_tileset,
  tex_item_tileset,
  tex_player_sprite_sheet,
  tex_monster_sprite_sheet,
  tex_inventory_win,
  tex_inventory_item_win,
  tex_inventory_item_selected,
  tex_interface_console_win,
  tex_interface_stats_win,

  tex_max
} texture_e;

typedef struct
{
  SDL_Texture *textures[tex_max];
  font_t *fonts[font_max];
} game_assets_t;

typedef struct
{
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Rect camera;
  bool32 game_is_running;
  bool32 turn_changed;
  SDL_Keycode key_pressed;
  uint32 time_elapsed;
  game_assets_t assets;
} game_state_t;

game_state_t global_state;

#endif // TYPES_H