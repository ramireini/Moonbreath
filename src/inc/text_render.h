#ifndef TEXT_RENDER_H
#define TEXT_RENDER_H

#include <game.h>

// NOTE(Rami): If you don't need these in the future, delete.
// #define HEX_COLOR_WHITE 0xFFFFF0FF
// #define HEX_COLOR_RED 0xB22222FF
// #define HEX_COLOR_BLUE 0x0080FFFF
// #define HEX_COLOR_GREEN 0x00B300FF
// #define HEX_COLOR_YELLOW 0xFFFF00FF
// #define HEX_COLOR_ORANGE 0xFFA500FF
// #define HEX_COLOR_BROWN 0xE7A56AFF

#define RGBA_COLOR_WHITE_S (SDL_Color){255, 255, 240, 255}
#define RGBA_COLOR_RED_S (SDL_Color){178, 34, 34, 255}
#define RGBA_COLOR_BLUE_S (SDL_Color){0, 128, 255, 255}
#define RGBA_COLOR_GREEN_S (SDL_Color){0, 179, 0, 255}
#define RGBA_COLOR_YELLOW_S (SDL_Color){255, 255, 0, 255}
#define RGBA_COLOR_ORANGE_S (SDL_Color){255, 165, 0, 255}
#define RGBA_COLOR_BROWN_S (SDL_Color){231, 165, 106, 255}
#define RGBA_COLOR_NONE_S (SDL_Color){0, 0, 0, 0}

#define RGBA_COLOR_WHITE_P 255, 255, 240, 255
#define RGBA_COLOR_RED_P 178, 34, 34, 255
#define RGBA_COLOR_BLUE_P 0, 128, 255, 255
#define RGBA_COLOR_GREEN_P 0, 179, 0, 255
#define RGBA_COLOR_YELLOW_P 255, 255, 0, 255
#define RGBA_COLOR_ORANGE_P 255, 165, 0, 255
#define RGBA_COLOR_BROWN_P 231, 165, 106, 255
#define RGBA_COLOR_NONE_P 0, 0, 0, 0

#define FONT_ATLAS_WIDTH 1376
#define FONT_ATLAS_HEIGHT 32

#define FONT_COUNT 2
#define FONT_METRICS_COUNT 94
#define START_ASCII_CHAR 33

typedef enum
{
  font_classic = 0,
  font_cursive
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

// Returns a malloc'd struct, REMEMBER TO FREE!
font_t* create_ttf_font_atlas(TTF_Font *font, int32 space_in_px);
font_t* create_bmp_font_atlas(char *path, int32 glyph_w, int32 glyph_h, int32 glyphs_per_row, int32 space_in_px, int32 shared_advance_in_px);
void render_text(char *txt, int32 x, int32 y, SDL_Color color, font_t *font, ...);

#endif // TEXT_RENDER_H