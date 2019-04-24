font_t create_ttf_font_atlas(TTF_Font *font, i32 space_size)
{
  // create a new atlas and make it the render target
  SDL_Texture *new_atlas = SDL_CreateTexture(game.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, FONT_ATLAS_WIDTH, FONT_ATLAS_HEIGHT);
  SDL_SetTextureBlendMode(new_atlas, SDL_BLENDMODE_BLEND);
  SDL_SetRenderTarget(game.renderer, new_atlas);

  // malloc a new font and point its atlas at the just made atlas
  font_t font_struct = {0};
  font_struct.atlas = new_atlas;

  // set space information
  font_struct.space_size = space_size;
  font_struct.shared_advance_in_px = 0;

  // we need these to create the glyph texture that we render to the atlas
  SDL_Surface *glyph_surf = NULL;
  SDL_Texture *glyph_tex = NULL;

  // the position where the glyph will be rendered on the atlas
  iv2_t glyph_pos = {0, 0};

  // color of the glyphs
  SDL_Color color = {255, 255, 255, 255};

  for(i32 i = 0; i < FONT_METRICS_COUNT; i++)
  {
    // get the correct character we want
    char ch = i + START_ASCII_CHAR;

    // render the glyph to a surface, make a texture out of the surface
    glyph_surf = TTF_RenderGlyph_Solid(font, ch, color);
    glyph_tex = SDL_CreateTextureFromSurface(game.renderer, glyph_surf);

    // fetch the glyph aspects out of the glyph surface
    aspect_t glyph_aspect = {glyph_surf->w, glyph_surf->h};

    // fetch the advance value of the glyph
    i32 advance;
    TTF_GlyphMetrics(font, ch, NULL, NULL, NULL, NULL, &advance);

    // set the info fetched to the metrics array of the font
    font_struct.metrics[i] = (glyph_metrics_t){glyph_pos, glyph_aspect, advance};

    // copy the glyph surface to the atlas
    SDL_RenderCopy(game.renderer, glyph_tex, NULL, &(SDL_Rect){glyph_pos.x, glyph_pos.y, glyph_aspect.w, glyph_aspect.h});

    // move the rendering position
    glyph_pos.x += glyph_aspect.w;

    // free the current glyphs surface and texture
    SDL_FreeSurface(glyph_surf);
    glyph_surf = NULL;

    SDL_DestroyTexture(glyph_tex);
    glyph_tex = NULL;
  }

  // unset atlas from being a render target
  SDL_SetRenderTarget(game.renderer, NULL);

  // return the font
  font_struct.success = true;
  return font_struct;
}

font_t create_bmp_font_atlas(char *path, aspect_t glyph_aspect, i32 glyph_pitch, i32 space_size, i32 shared_advance_in_px)
{
  assert(path);

  // load the atlas texture
  //
  // ignore the black color to make the background of the texture transparent
  SDL_Texture *bmp_atlas = load_texture(path, &(SDL_Color){0, 0, 0, 0});
  if(!bmp_atlas)
  {
    debug("Could not load file %s\n", path);
    font_t ret = {0};
    return ret;
  }

  // malloc a new font and point its atlas at the just loaded texture
  font_t bmp_font = {0};
  bmp_font.atlas = bmp_atlas;

  // set some space size info
  bmp_font.space_size = space_size;
  bmp_font.shared_advance_in_px = shared_advance_in_px;

  // glyph position to be used for fetching them
  // and a count so we know when to switch rows
  iv2_t glyph_pos = {1, 1};
  i32 glyph_count = 0;

  for(i32 i = 0; i < FONT_METRICS_COUNT; i++)
  {
    if(glyph_count > glyph_pitch)
    {
      glyph_pos.x = 1;
      glyph_pos.y += glyph_aspect.h + 1;
      glyph_count = 0;
    }

    // set the glyph information into the metrics array of the font
    bmp_font.metrics[i] = (glyph_metrics_t){glyph_pos, glyph_aspect, 0};

    // go to the new position where another glyph can be found
    // the plus one is because we have a blue grid in the texture
    // and this will ensure we don't include it
    glyph_pos.x += glyph_aspect.w + 1;
    glyph_count++;
  }

  // return the font
  bmp_font.success = true;
  return bmp_font;
}

void assets_free()
{
  for(i32 i = 0; i < tex_max; i++)
  {
    if(assets.textures[i])
    {
      SDL_DestroyTexture(assets.textures[i]);
      assets.textures[i] = NULL;

      debug("Tex: %d free", i);
    }
  }
}