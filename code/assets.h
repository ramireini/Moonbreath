#ifndef ASSETS_H
#define ASSETS_H

#define FONT_ATLAS_WIDTH 1376
#define FONT_ATLAS_HEIGHT 32

#define FONT_METRICS_COUNT 94
#define START_ASCII_CHAR 33

// NOTE(rami):
// If you need to test a font:
// render_text("!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_'abcdefghijklmnopqrstuvwxyz{|}~", v2(100, 100), color_white, font[font_classic]);

// NOTE(rami): We could probably allocate assets so that there's one malloc and one free,
// instead of doing multiple malloc's and multiple free's

enum
{
  font_classic,
  font_cursive,

  font_total
} game_font;

enum
{
  tex_tilemap,
  tex_game_tileset,
  tex_item_tileset,
  tex_wearable_item_tileset,
  tex_sprite_sheet,
  tex_inventory_win,
  tex_inventory_item_win,
  tex_inventory_selected_item,
  tex_interface_bottom_win,
  tex_health_bar_outside,
  tex_health_bar_inside,
  tex_player_parts,

  tex_total
} game_texture;

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

#endif // ASSETS_H