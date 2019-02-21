#ifndef TEXT_RENDER_H
#define TEXT_RENDER_H

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
  int32 space_size;
  int32 shared_advance_in_px;
} font_t;

#endif // TEXT_RENDER_H