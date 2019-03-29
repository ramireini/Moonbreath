#define RGBA_COLOR_WHITE_S (SDL_Color){255, 255, 240, 255}
#define RGBA_COLOR_RED_S (SDL_Color){178, 34, 34, 255}
#define RGBA_COLOR_BLUE_S (SDL_Color){0, 128, 255, 255}
#define RGBA_COLOR_GREEN_S (SDL_Color){0, 179, 0, 255}
#define RGBA_COLOR_YELLOW_S (SDL_Color){207, 175, 0, 255}
#define RGBA_COLOR_ORANGE_S (SDL_Color){255, 165, 0, 255}
#define RGBA_COLOR_BROWN_S (SDL_Color){231, 165, 106, 255}
#define RGBA_COLOR_BLACK_S (SDL_Color){0, 0, 0, 0}

#define RGBA_COLOR_WHITE_P 255, 255, 240, 255
#define RGBA_COLOR_RED_P 178, 34, 34, 255
#define RGBA_COLOR_BLUE_P 0, 128, 255, 255
#define RGBA_COLOR_GREEN_P 0, 179, 0, 255
#define RGBA_COLOR_YELLOW_P 207, 175, 0, 255
#define RGBA_COLOR_ORANGE_P 255, 165, 0, 255
#define RGBA_COLOR_BROWN_P 231, 165, 106, 255
#define RGBA_COLOR_BLACK_P 0, 0, 0, 0

void tilemap_render()
{
  SDL_SetRenderTarget(game_state.renderer, assets.textures[tex_tilemap]);
  SDL_RenderClear(game_state.renderer);

  iv2_t from;
  iv2_t to = {tile_div(game_state.camera.x + game_state.camera.w), tile_div(game_state.camera.y + game_state.camera.h)};

  for(from.x = tile_div(game_state.camera.x); from.x < to.x; from.x++)
  {
    for(from.y = tile_div(game_state.camera.y); from.y < to.y; from.y++)
    {
      SDL_Rect src = {tile_mul(level.tiles[(from.y * LEVEL_WIDTH_IN_TILES) + from.x]), 0, TILE_SIZE, TILE_SIZE};
      SDL_Rect dst = {tile_mul(from.x), tile_mul(from.y), TILE_SIZE, TILE_SIZE};

      SDL_Color color = get_color_for_lit_val(from);
      SDL_SetTextureColorMod(assets.textures[tex_game_tileset], color.r, color.g, color.b);
      SDL_RenderCopy(game_state.renderer, assets.textures[tex_game_tileset], &src, &dst);
    }
  }

  SDL_SetRenderTarget(game_state.renderer, NULL);

  SDL_Rect dst = {0, 0, game_state.camera.w, game_state.camera.h};
  SDL_RenderCopy(game_state.renderer, assets.textures[tex_tilemap], &game_state.camera, &dst);
}

void text_render(char *str, iv2_t p, SDL_Color color, font_t font, ...)
{
  char txt_final[256];

  va_list arg_list;
  va_start(arg_list, font);
  vsnprintf(txt_final, sizeof(txt_final), str, arg_list);
  va_end(arg_list);

  i32 origin_x = p.x;
  char *at = txt_final;

  while(*at)
  {
    i32 array_index = *at - START_ASCII_CHAR;

    if(*at == ' ')
    {
      at++;
      p.x += font.space_size;
      continue;
    }
    else if(*at == '\n')
    {
      at++;
      p.x = origin_x;
      p.y += 16;
      continue;
    }
    else if(*at == '\\' && at[1] == 'n')
    {
      at += 2;
      p.x = origin_x;
      p.y += 16;
      continue;
    }
    else if(array_index < 0)
    {
      at++;
      debug("'%c': Chracter does not exist in metrics array\n", array_index + START_ASCII_CHAR);
      continue;
    }

    SDL_SetTextureColorMod(font.atlas, color.r, color.g, color.b);

    SDL_Rect src = {font.metrics[array_index].pos.x, font.metrics[array_index].pos.y, font.metrics[array_index].aspect.w, font.metrics[array_index].aspect.h};
    SDL_Rect dst = {p.x, p.y, font.metrics[array_index].aspect.w, font.metrics[array_index].aspect.h};

    SDL_RenderCopy(game_state.renderer, font.atlas, &src, &dst);

    if(!font.shared_advance_in_px)
    {
      p.x += font.metrics[array_index].unique_advance_in_px;
    }
    else
    {
      p.x += font.shared_advance_in_px;
    }

    at++;
  }
}