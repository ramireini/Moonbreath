#ifndef ASSETS_H
#define ASSETS_H

#define FONT_ATLAS_WIDTH 1376
#define FONT_ATLAS_HEIGHT 32

#define FONT_METRICS_COUNT 94
#define START_ASCII_CHAR 33

enum
{
  font_classic,
  font_cursive,
  font_max
};

enum
{
  tex_tilemap,
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
};

typedef struct
{
  iv2_t pos;
  aspect_t aspect;
  i32 unique_advance_in_px;
} glyph_metrics_t;

typedef struct
{
  bool success;
  SDL_Texture *atlas;
  glyph_metrics_t metrics[FONT_METRICS_COUNT];
  i32 space_size;
  i32 shared_advance_in_px;
} font_t;

typedef struct
{
  SDL_Texture *textures[tex_max];
  font_t fonts[font_max];
} asset_t;

#endif // ASSETS_H