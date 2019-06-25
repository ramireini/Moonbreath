internal font_t
create_ttf_font(char *font_path, i32 font_size, i32 space_size)
{
  // Open font
  TTF_Font *font = TTF_OpenFont(font_path, font_size);
  if(!font)
  {
    debug("Could not load file %s\n", font_path);
    font_t ret = {0};
    return(ret);
  }

  // Create a new atlas and make it the render target
  SDL_Texture *new_atlas = SDL_CreateTexture(game.renderer, SDL_PIXELFORMAT_RGBA8888,
                                             SDL_TEXTUREACCESS_TARGET, FONT_ATLAS_WIDTH,
                                             FONT_ATLAS_HEIGHT);
  SDL_SetRenderTarget(game.renderer, new_atlas);

  // Enable alpha blending
  SDL_SetTextureBlendMode(new_atlas, SDL_BLENDMODE_BLEND);

  // Malloc a new font and point its atlas to the just made atlas
  font_t new_font = {0};
  new_font.atlas = new_atlas;

  // Set space information, initialize advance
  new_font.space_size = space_size;
  new_font.shared_advance = 0;

  // We need these to create the glyph texture that we render to the atlas
  SDL_Surface *glyph_surf = 0;
  SDL_Texture *glyph_tex = 0;

  // The position where the glyph will be rendered on the atlas
  iv4 glyph = {0};
  SDL_Color glyph_color = {255, 255, 255, 255};

  for(i32 i = 0; i < FONT_METRICS_COUNT; ++i)
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
    TTF_GlyphMetrics(font, ch, 0, 0, 0, 0, &advance);

    // Set the info fetched to the metrics array of the font
    glyph_metrics_t metrics = {glyph.x, glyph.y, glyph.w, glyph.h, advance};
    new_font.metrics[i] = metrics;

    // Copy the glyph surface to the atlas
    SDL_Rect rect = {glyph.x, glyph.y, glyph.w, glyph.h};
    SDL_RenderCopy(game.renderer, glyph_tex, 0, &rect);

    // Move the rendering position
    glyph.x += glyph.w;

    SDL_FreeSurface(glyph_surf);
    glyph_surf = 0;
    SDL_DestroyTexture(glyph_tex);
    glyph_tex = 0;
  }

  // Unset atlas from being a render target
  SDL_SetRenderTarget(game.renderer, 0);

  // Close font
  TTF_CloseFont(font);

  new_font.success = 1;
  return(new_font);
}

internal font_t
create_bmp_font(char *font_path, i32 glyph_w, i32 glyph_h, i32 glyph_pitch, i32 space_size, i32 shared_advance)
{
  assert(font_path);

  // Load the atlas texture
  // Ignore the black color to make the background of the texture transparent
  iv4 color_key = v4(0, 0, 0, 0);
  SDL_Texture *new_atlas = load_texture(font_path, &color_key);
  if(!new_atlas)
  {
    debug("Could not load file %s\n", font_path);
    font_t ret = {0};
    return(ret);
  }

  // Enable alpha blending
  SDL_SetTextureBlendMode(new_atlas, SDL_BLENDMODE_BLEND);

  // Malloc a new font and point its atlas at the just loaded texture
  font_t new_font = {0};
  new_font.atlas = new_atlas;

  // Set some space size info
  new_font.space_size = space_size;
  new_font.shared_advance = shared_advance;

  // Glyph position to be used for fetching them
  // and a count so we know when to switch rows
  iv4 glyph = v4(1, 1, glyph_w, glyph_h);
  i32 glyph_count = 0;

  for(i32 i = 0; i < FONT_METRICS_COUNT; ++i)
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
    ++glyph_count;
  }

  new_font.success = 1;
  return(new_font);
}

internal void
free_assets()
{
  printf("\n");

  for(i32 i = 0; i < tex_total; ++i)
  {
    if(texture[i])
    {
      SDL_DestroyTexture(texture[i]);
      texture[i] = 0;

      printf("Tex: %d free\n", i);
    }
  }
}