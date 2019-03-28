#ifndef LIGHTING_H
#define LIGHTING_H

#define LIGHTING_COLOR_100 (SDL_Color){255, 255, 255, 255}
#define LIGHTING_COLOR_90 (SDL_Color){230, 230, 230, 255}
#define LIGHTING_COLOR_80 (SDL_Color){204, 204, 204, 255}
#define LIGHTING_COLOR_70 (SDL_Color){179, 179, 179, 255}
#define LIGHTING_COLOR_60 (SDL_Color){153, 153, 153, 255}
#define LIGHTING_COLOR_50 (SDL_Color){128, 128, 128, 255}
#define LIGHTING_COLOR_40 (SDL_Color){102, 102, 102, 255}
#define LIGHTING_COLOR_30 (SDL_Color){77, 77, 77, 255}
#define LIGHTING_COLOR_20 (SDL_Color){51, 51, 51, 255}
#define LIGHTING_COLOR_10 (SDL_Color){26, 26, 26, 255}
#define LIGHTING_COLOR_0 (SDL_Color){0, 0, 0, 255}

enum
{
  lighting_100,
  lighting_90,
  lighting_80,
  lighting_70,
  lighting_60,
  lighting_50,
  lighting_40,
  lighting_30,
  lighting_20,
  lighting_10,
  lighting_0
};

typedef struct
{
  bool seen;
  u8 val;
} lighting_t;

#endif // LIGHTING_H