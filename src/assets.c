font_t* create_ttf_font_atlas(TTF_Font *font, i32 space_size)
{
  SDL_Texture *new_atlas = SDL_CreateTexture(global_state.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, FONT_ATLAS_WIDTH, FONT_ATLAS_HEIGHT);
  SDL_SetTextureBlendMode(new_atlas, SDL_BLENDMODE_BLEND);

  font_t *font_struct = malloc(sizeof(font_t));
  font_struct->atlas = NULL;
  font_struct->space_size = space_size;

  font_struct->shared_advance_in_px = 0;

  SDL_Surface *glyph_surf = NULL;
  SDL_Texture *glyph_tex = NULL;

  i32 x = 0;
  i32 y = 0;

  for(i32 i = 0; i < FONT_METRICS_COUNT; i++)
  {
    char ch = i + START_ASCII_CHAR;

    SDL_Color color = {255, 255, 255, 255};
    glyph_surf = TTF_RenderGlyph_Solid(font, ch, color);
    glyph_tex = SDL_CreateTextureFromSurface(global_state.renderer, glyph_surf);

    SDL_SetRenderTarget(global_state.renderer, new_atlas);

    i32 advance;
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

font_t* create_bmp_font_atlas(char *path, i32 glyph_w, i32 glyph_h, i32 glyph_pitch, i32 space_size, i32 shared_advance_in_px)
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

  i32 glyph_x = 1;
  i32 glyph_y = 1;
  i32 glyph_count = 0;

  for(i32 i = 0; i < FONT_METRICS_COUNT; i++)
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

void free_assets()
{
  for(i32 i = 0; i < font_max; i++)
  {
    if(global_state.assets.fonts[i])
    {
      if(global_state.assets.fonts[i]->atlas)
      {
        SDL_DestroyTexture(global_state.assets.fonts[i]->atlas);
      }

      free(global_state.assets.fonts[i]);
      global_state.assets.fonts[i] = NULL;

      debug("Font: %d free", i);
    }
  }

  for(i32 i = 0; i < tex_max; i++)
  {
    if(global_state.assets.textures[i])
    {
      SDL_DestroyTexture(global_state.assets.textures[i]);
      global_state.assets.textures[i] = NULL;

      debug("Tex: %d free", i);
    }
  }
}