#include <text_render.h>
#include <util_io.h>

void render_text(char *str, int x, int y, int text_color, font_t *font_struct, ...)
{
  // holds the final string
  char str_final[256];

  // create an argument list and initialize it
  // to take arguments after the font_struct parameter
  va_list arg_list;
  va_start(arg_list, font_struct);

  // print str to the str_final array and
  // add the format specifiers from arg_list
  vsnprintf(str_final, sizeof(str_final), str, arg_list);

  // close the argument list
  va_end(arg_list);

  // start at the beginning of the final string
  char *current_char = str_final;

  // store the starting x of the text for wrapping
  int initial_x = x;

  // set to 1 if we want to wrap text, set to 0 if we don't want to wrap text
  int force_wrapping = 0;

  // while valid chars
  while(*current_char != '\0')
  {
    int array_index = *current_char - START_ASCII_CHAR;

    // if newline
    if(*current_char == '\\' && *(current_char + 1) == 'n')
    {
      force_wrapping = 1;

      // move to next char in text
      current_char += 2;
      
      continue;
    }
    // if space
    else if(*current_char == ' ')
    {
      // move the position of the text
      x += 5;

      // move to the next byte in the text
      current_char++;

      continue;
    }
    // if percent sign
    else if(*current_char == '%')
    {
      // move to the next byte in the text
      current_char++;

      continue;
    }

    // if the font_t doesn't have the character
    if(array_index < 0)
    {
      printf("Can't render a character that's not stored in the font_t struct\n");

      // move to the next byte in the text
      current_char++;

      continue;
    }

    // if we want a newline
    if(force_wrapping)
    {
      // move the position of the text to the original x
      x = initial_x;

      // move the position of the text to the next line
      y += 16;

      force_wrapping = 0;
    }

    // fetch the glyph metrics for the current character in the text
    font_metrics_t *glyph_metrics = &font_struct->metrics[array_index];

    // the source rectangle to take from the glyph atlas
    SDL_Rect src = {glyph_metrics->x, glyph_metrics->y, glyph_metrics->w, glyph_metrics->h};

    // the destination rectangle where to render our glyph
    SDL_Rect dst = {x, y, glyph_metrics->w, glyph_metrics->h};

    // apply color
    SDL_Color color = hex_to_rgba_color(text_color);
    SDL_SetTextureColorMod(font_struct->atlas, color.r, color.g, color.b);

    // render the text
    SDL_RenderCopy(renderer, font_struct->atlas, &src, &dst);

    // move the position of the text
    x += glyph_metrics->advance;

    // move to the next byte in the text
    current_char++;
  }
}

font_t* create_font_atlas(TTF_Font *font)
{
  // a texture to hold all the glyphs
  // SDL_Texture *glyph_atlas = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 688, 16);
  SDL_Texture *glyph_atlas = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 1376, 32);

  SDL_Surface *glyph_surf = NULL;
  SDL_Texture *glyph_tex = NULL;
  
  // this will hold the atlas and the glyph metrics
  // we will return a pointer to this struct
  font_t *font_struct = malloc(sizeof(font_t));
  font_struct->atlas = NULL;

  // enable alpha blending to remove the black background
  SDL_SetTextureBlendMode(glyph_atlas, SDL_BLENDMODE_BLEND);

  // glyph positions on the glyph atlas
  int x = 0;
  int y = 0;

  for(int i = 0; i < FONT_METRICS_COUNT; i++)
  {
    // store the current character
    char ch = i + START_ASCII_CHAR;

    // render the glyph to a surface
    SDL_Color color = {255, 255, 255, 255};
    glyph_surf = TTF_RenderGlyph_Solid(font, ch, color);
    glyph_tex = SDL_CreateTextureFromSurface(renderer, glyph_surf);

    // set the glyph atlas as the render target
    SDL_SetRenderTarget(renderer, glyph_atlas);

    // get the advance value of the glyph
    int advance;

    TTF_GlyphMetrics(font, ch, NULL, NULL, NULL, NULL, &advance);

    // set where on the atlas we render
    SDL_Rect atlas_rect = {x, y, glyph_surf->w, glyph_surf->h};

    // store the glyph metrics
    font_struct->metrics[i] = (font_metrics_t){atlas_rect.x, atlas_rect.y, atlas_rect.w, atlas_rect.h, advance};

    // advance the rendering location
    x += glyph_surf->w;

    // render the glyph on the atlas
    SDL_RenderCopy(renderer, glyph_tex, NULL, &atlas_rect);

    // free the glyph surface and texture
    SDL_FreeSurface(glyph_surf);
    glyph_surf = NULL;

    SDL_DestroyTexture(glyph_tex);
    glyph_tex = NULL;
  }

  // unset the render target
  SDL_SetRenderTarget(renderer, NULL);

  // set the struct pointer to the completed atlas
  font_struct->atlas = glyph_atlas;

  return font_struct;
}