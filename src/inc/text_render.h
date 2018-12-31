#ifndef TEXT_RENDER_H
#define TEXT_RENDER_H

#include <game.h>

#define ATLAS_WIDTH 1376
#define ATLAS_HEIGHT 32

#define FONT_COUNT 2
#define FONT_METRICS_COUNT 94
#define START_ASCII_CHAR 33

#define HEX_COLOR_WHITE 0xFFFFF0FF
#define HEX_COLOR_RED 0xFF0000FF
#define HEX_COLOR_BLUE 0x0080FFFF
#define HEX_COLOR_GREEN 0x00B300FF
#define HEX_COLOR_YELLOW 0xFFFF00FF
#define HEX_COLOR_ORANGE 0xFFA500FF
#define HEX_COLOR_BROWN 0xE7A56AFF

#define RGBA_CLEAR_COLOR 0, 0, 0, 255
#define RGBA_COLOR_WHITE 255, 255, 240, 255
#define RGBA_COLOR_RED 255, 0, 0, 255
#define RGBA_COLOR_BLUE 0, 128, 255, 255
#define RGBA_COLOR_GREEN 0, 179, 0, 255
#define RGBA_COLOR_YELLOW 255, 255, 0, 255
#define RGBA_COLOR_ORANGE 255, 165, 0, 255
#define RGBA_COLOR_BROWN 231, 165, 106, 255

typedef enum
{
  FONT_CLASSIC = 0,
  FONT_CURSIVE
} font_e;

typedef struct
{
  int32 x;
  int32 y;
  int32 w;
  int32 h;
  int32 unique_advance_in_px;
} glyph_metrics_t;

typedef struct
{
  SDL_Texture *atlas;
  glyph_metrics_t metrics[FONT_METRICS_COUNT];
  int32 space_in_px;
  int32 shared_advance_in_px;
} font_t;

extern font_t *fonts[FONT_COUNT];

void render_text(char *str, int32 str_x, int32 str_y, uint32 str_color, font_t *font, ...);

// Returns a malloc'd struct, REMEMBER TO FREE!
font_t* create_ttf_font_atlas(TTF_Font *font, int32 space_in_px);
font_t* create_bmp_font_atlas(char *path, int32  glyph_w, int32 glyph_h, int32 glyphs_per_row, int32 space_in_px, int32 shared_advance_in_px);

#endif // TEXT_RENDER_H