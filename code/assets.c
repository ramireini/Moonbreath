internal font_t
create_ttf_font_atlas(TTF_Font *font, i32 space_size)
{
  assert(font);

  // Create a new atlas and make it the render target
  SDL_Texture *new_atlas = SDL_CreateTexture(game.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, FONT_ATLAS_WIDTH, FONT_ATLAS_HEIGHT);
  SDL_SetTextureBlendMode(new_atlas, SDL_BLENDMODE_BLEND);
  SDL_SetRenderTarget(game.renderer, new_atlas);

  // Malloc a new font and point its atlas to the just made atlas
  font_t new_font = {0};
  new_font.atlas = new_atlas;

  // Set space information
  new_font.space_size = space_size;
  new_font.shared_advance_in_px = 0;

  // We need these to create the glyph texture that we render to the atlas
  SDL_Surface *glyph_surf = NULL;
  SDL_Texture *glyph_tex = NULL;

  // The position where the glyph will be rendered on the atlas
  v4_t glyph = {0};
  SDL_Color glyph_color = {255, 255, 255, 255};

  for(i32 i = 0; i < FONT_METRICS_COUNT; i++)
  {
    // Get the correct character we want
    char ch = i + START_ASCII_CHAR;

    // Render the glyph to a surface, make a texture out of the surface
    glyph_surf = TTF_RenderGlyph_Solid(font, ch, glyph_color);
    glyph_tex = SDL_CreateTextureFromSurface(game.renderer, glyph_surf);

    // Fetch the glyph w/h out of the glyph surface
    glyph.w = glyph_surf->w;
    glyph.h = glyph_surf->h;

    // Fetch the advance value of the glyph
    i32 advance = 0;
    TTF_GlyphMetrics(font, ch, NULL, NULL, NULL, NULL, &advance);

    // Set the info fetched to the metrics array of the font
    glyph_metrics_t metrics = {glyph.x, glyph.y, glyph.x, glyph.y, advance};
    new_font.metrics[i] = metrics;

    // Copy the glyph surface to the atlas
    SDL_Rect rect = {glyph.x, glyph.y, glyph.w, glyph.h};
    SDL_RenderCopy(game.renderer, glyph_tex, NULL, &rect);

    // Move the rendering position
    glyph.x += glyph.w;

    SDL_FreeSurface(glyph_surf);
    SDL_DestroyTexture(glyph_tex);
    glyph_surf = NULL;
    glyph_tex = NULL;
  }

  // Unset atlas from being a render target
  SDL_SetRenderTarget(game.renderer, NULL);

  new_font.success = true;
  return new_font;
}

internal font_t
create_bmp_font_atlas(char *path, i32 glyph_w, i32 glyph_h, i32 glyph_pitch, i32 space_size, i32 shared_advance_in_px)
{
  assert(path);

  // Load the atlas texture
  // Ignore the black color to make the background of the texture transparent
  v4_t color_key = v4(0, 0, 0, 0);
  SDL_Texture *bmp_atlas = load_texture(path, &color_key);
  if(!bmp_atlas)
  {
    debug("Could not load file %s\n", path);
    font_t ret = {0};
    return ret;
  }

  // Malloc a new font and point its atlas at the just loaded texture
  font_t new_font = {0};
  new_font.atlas = bmp_atlas;

  // Set some space size info
  new_font.space_size = space_size;
  new_font.shared_advance_in_px = shared_advance_in_px;

  // Glyph position to be used for fetching them
  // and a count so we know when to switch rows
  v4_t glyph = v4(1, 1, glyph_w, glyph_h);
  i32 glyph_count = 0;

  for(i32 i = 0; i < FONT_METRICS_COUNT; i++)
  {
    if(glyph_count > glyph_pitch)
    {
      glyph.x = 1;
      glyph.y += glyph.h + 1;
      glyph_count = 0;
    }

    // Set the glyph information into the metrics array of the font
    glyph_metrics_t metrics = {glyph.x, glyph.y, glyph.w, glyph.h, 0};
    new_font.metrics[i] = metrics;

    // Set glyph_x to the position where the new glyph starts from,
    // the plus one is because we have a blue grid in the font file
    // and adding one will avoid the blue grid
    glyph.x += glyph.w + 1;
    glyph_count++;
  }

  new_font.success = true;
  return new_font;
}

internal void
free_assets()
{
  for(i32 i = 0; i < tex_count; i++)
  {
    if(assets.textures[i])
    {
      SDL_DestroyTexture(assets.textures[i]);
      assets.textures[i] = NULL;

      debug("Tex: %d free", i);
    }
  }
}