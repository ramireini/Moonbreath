#ifndef ASSETS_H
#define ASSETS_H

#define FONT_ATLAS_WIDTH 1376
#define FONT_ATLAS_HEIGHT 32
#define FONT_METRICS_COUNT 94
#define START_ASCII_CHAR 33

enum
{
  classic_font,
  cursive_font,

  font_count
};

enum
{
  tilemap_tex,
  game_tileset_tex,
  item_tileset_tex,
  player_sprite_sheet_tex,
  monster_sprite_sheet_tex,
  inventory_win_tex,
  inventory_item_win_tex,
  inventory_item_selected_tex,
  interface_console_win_tex,
  interface_stats_win_tex,

  tex_count
};

typedef struct
{
  i32 x, y, w, h;
  i32 unique_advance_in_px;
} glyph_metrics_t;

typedef struct
{
  b32 success;
  SDL_Texture *atlas;
  glyph_metrics_t metrics[FONT_METRICS_COUNT];
  i32 space_size;
  i32 shared_advance;
} font_t;

typedef struct
{
  SDL_Texture *textures[tex_count];
  font_t fonts[font_count];
} asset_t;

#endif // ASSETS_H