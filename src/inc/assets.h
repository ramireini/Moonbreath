#ifndef ASSETS_H
#define ASSETS_H

#define FONT_ATLAS_WIDTH 1376
#define FONT_ATLAS_HEIGHT 32

#define FONT_METRICS_COUNT 94
#define START_ASCII_CHAR 33

typedef enum
{
  font_classic = 0,
  font_cursive,
  font_max
} font_e;

typedef struct
{
  i32 x;
  i32 y;
  i32 w;
  i32 h;
  i32 unique_advance_in_px;
} glyph_metrics_t;

typedef struct
{
  SDL_Texture *atlas;
  glyph_metrics_t metrics[FONT_METRICS_COUNT];
  i32 space_size;
  i32 shared_advance_in_px;
} font_t;

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

#endif // ASSETS_H