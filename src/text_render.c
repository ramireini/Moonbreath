#include <text_render.h>

void render_text(char *txt, int32 x, int32 y, SDL_Color color, font_t *font, ...)
{
  char txt_final[256];

  va_list arg_list;
  va_start(arg_list, font);
  vsnprintf(txt_final, sizeof(txt_final), txt, arg_list);
  va_end(arg_list);

  int32 origin_x = x;
  char *at = txt_final;

  bool32 share_advance;
  if(!font->shared_advance_in_px)
  {
    share_advance = false;
  }
  else
  {
    share_advance = true;
  }

  while(at[0])
  {
    int32 array_index = *at - START_ASCII_CHAR;

    if(at[0] == ' ')
    {
      at++;
      x += font->space_size;
      continue;
    }
    else if(at[0] == '\n')
    {
      at++;
      x = origin_x;
      y += 16;
      continue;
    }
    else if(at[0] == '\\' && at[1] == 'n')
    {
      at += 2;
      x = origin_x;
      y += 16;
      continue;
    }
    else if(array_index < 0)
    {
      at++;
      debug("'%c': Chracter does not exist in metrics array\n", array_index + START_ASCII_CHAR);
      continue;
    }

    SDL_SetTextureColorMod(font->atlas, color.r, color.g, color.b);

    glyph_metrics_t *glyph_metrics = &font->metrics[array_index];
    SDL_Rect src = {glyph_metrics->x, glyph_metrics->y, glyph_metrics->w, glyph_metrics->h};
    SDL_Rect dst = {x, y, font->metrics[array_index].w, font->metrics[array_index].h};

    SDL_RenderCopy(global_state.renderer, font->atlas, &src, &dst);

    if(!share_advance)
    {
      x += font->metrics[array_index].unique_advance_in_px;
    }
    else
    {
      x += font->shared_advance_in_px;
    }

    at++;
  }
}

font_t* create_ttf_font_atlas(TTF_Font *font, int32 space_size)
{
  SDL_Texture *new_atlas = SDL_CreateTexture(global_state.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, FONT_ATLAS_WIDTH, FONT_ATLAS_HEIGHT);
  SDL_SetTextureBlendMode(new_atlas, SDL_BLENDMODE_BLEND);

  font_t *font_struct = malloc(sizeof(font_t));
  font_struct->atlas = NULL;
  font_struct->space_size = space_size;

  font_struct->shared_advance_in_px = 0;

  SDL_Surface *glyph_surf = NULL;
  SDL_Texture *glyph_tex = NULL;

  int32 x = 0;
  int32 y = 0;

  for(int32 i = 0; i < FONT_METRICS_COUNT; i++)
  {
    char ch = i + START_ASCII_CHAR;

    SDL_Color color = {255, 255, 255, 255};
    glyph_surf = TTF_RenderGlyph_Solid(font, ch, color);
    glyph_tex = SDL_CreateTextureFromSurface(global_state.renderer, glyph_surf);

    SDL_SetRenderTarget(global_state.renderer, new_atlas);

    int32 advance;
    TTF_GlyphMetrics(font, ch, NULL, NULL, NULL, NULL, &advance);

    SDL_Rect atlas_rect = {x, y, glyph_surf->w, glyph_surf->h};
    font_struct->metrics[i] = (glyph_metrics_t){atlas_rect.x, atlas_rect.y, atlas_rect.w, atlas_rect.h, advance};

    SDL_RenderCopy(global_state.renderer, glyph_tex, NULL, &atlas_rect);

    x += glyph_surf->w;

    SDL_FreeSurface(glyph_surf);
    SDL_DestroyTexture(glyph_tex);
    glyph_surf = NULL;
    glyph_tex = NULL;
  }

  SDL_SetRenderTarget(global_state.renderer, NULL);

  font_struct->atlas = new_atlas;

  return font_struct;
}

font_t* create_bmp_font_atlas(char *path, int32 glyph_w, int32 glyph_h, int32 glyph_pitch, int32 space_size, int32 shared_advance_in_px)
{
  SDL_Color color = {0, 0, 0, 0};
  SDL_Texture *bmp_atlas = load_texture(path, &color);
  if(!bmp_atlas)
  {
    debug("Could not load file %s\n", path);
    return NULL;
  }

  font_t *bmp_font = malloc(sizeof(font_t));
  bmp_font->atlas = bmp_atlas;
  bmp_font->space_size = space_size;
  bmp_font->shared_advance_in_px = shared_advance_in_px;

  int32 glyph_x = 1;
  int32 glyph_y = 1;
  int32 glyph_count = 0;

  for(int32 i = 0; i < FONT_METRICS_COUNT; i++)
  {
    if(glyph_count > glyph_pitch)
    {
      glyph_x = 1;
      glyph_y += glyph_h + 1;
      glyph_count = 0;
    }

    bmp_font->metrics[i] = (glyph_metrics_t){glyph_x, glyph_y, glyph_w, glyph_h, 0};

    glyph_x += glyph_w + 1;
    glyph_count++;
  }

  return bmp_font;
}