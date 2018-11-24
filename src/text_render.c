#include <text_render.h>
#include <util_io.h>

ttf_font_t* create_ttf_font_atlas(TTF_Font *font)
{
  // a texture to hold all the glyphs
  SDL_Texture *glyph_atlas = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 1376, 32);

  SDL_Surface *glyph_surf = NULL;
  SDL_Texture *glyph_tex = NULL;
  
  // this will hold the atlas and the glyph metrics
  // we will return a pointer to this struct
  ttf_font_t *font_struct = malloc(sizeof(ttf_font_t));
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
    font_struct->metrics[i] = (ttf_glyph_metrics_t){atlas_rect.x, atlas_rect.y, atlas_rect.w, atlas_rect.h, advance};

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

void render_text_ttf(char *str, int text_x, int text_y, int text_color, ttf_font_t *font_struct, ...)
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
  int initial_x = text_x;

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
      text_x += 5;

      // move to the next byte in the text
      current_char++;

      continue;
    }
    // if the font_t doesn't have the character
    else if(array_index < 0)
    {
      printf("Character does not exist in metrics array\n");

      // move to the next byte in the text
      current_char++;

      continue;
    }

    // if we want a newline
    if(force_wrapping)
    {
      // move the position of the text to the original x
      text_x = initial_x;

      // move the position of the text to the next line
      text_y += 16;

      force_wrapping = 0;
    }

    // fetch the glyph metrics for the current character in the text
    ttf_glyph_metrics_t *glyph_metrics = &font_struct->metrics[array_index];

    // the source rectangle to take from the glyph atlas
    SDL_Rect src = {glyph_metrics->x, glyph_metrics->y, glyph_metrics->w, glyph_metrics->h};

    // the destination rectangle where to render our glyph
    SDL_Rect dst = {text_x, text_y, glyph_metrics->w, glyph_metrics->h};

    // apply color
    SDL_Color color = hex_to_rgba_color(text_color);
    SDL_SetTextureColorMod(font_struct->atlas, color.r, color.g, color.b);

    // render the text
    SDL_RenderCopy(renderer, font_struct->atlas, &src, &dst);

    // move the position of the text
    text_x += glyph_metrics->advance;

    // move to the next byte in the text
    current_char++;
  }
}

bmp_font_t* create_bmp_font_atlas(char *path, int glyph_width, int glyph_height, int bmp_pitch)
{
  // load texture
  SDL_Texture *bmp_atlas = load_texture(path, 1);
  if(!bmp_atlas)
  {
    printf("Could not load file %s\n", path);
    return 0;
  }

  // malloc a struct where we can hold the atlas pointer and the metrics
  // set the atlas to the texture we just loaded
  bmp_font_t *bmp_font = malloc(sizeof(bmp_font_t));
  bmp_font->atlas = bmp_atlas;

  // start at 1, 1 to bypass padding
  // glyph_num is for tracking how many glyphs,
  // the atlas has in a single row
  int x = 1;
  int y = 1;
  int glyph_num = 0;

  // for however many glyphs we want to store
  for(int i = 0; i < FONT_METRICS_COUNT; i++)
  {
    // move to next row if needed
    if(glyph_num > bmp_pitch)
    {
      x = 1;
      y += 17;
      glyph_num = 0;
    }

    // store the x, y, width and height of the glyph,
    // so that we can reference back to it
    bmp_font->metrics[i] = (bmp_glyph_metrics_t){x, y, glyph_width, glyph_height};

    // move rendering position forward,
    // increment glyph_num
    x += 17;
    glyph_num++;
  }

  // return the malloc'd struct
  return bmp_font;
}

void render_text_bmp(char *str, int text_x, int text_y, int text_color, bmp_font_t *bmp_font, ...)
{
  // holds the final string
  char str_final[256];

  // start a vararg list
  va_list arg_list;
  va_start(arg_list, bmp_font);

  // fill str_final with the final string that includes specifiers
  vsnprintf(str_final, sizeof(str_final), str, arg_list);

  // end the list
  va_end(arg_list);

  // points at the current character we want to render
  char *current_char = str_final;

  // holds the original starting x position of the text for wrapping
  int initial_x = text_x;

  // as long as not NULL
  while(*current_char != '\0')
  {
    // calculate the correct array index
    int array_index = *current_char - START_ASCII_CHAR;

    // if a space
    if(*current_char == ' ')
    {
      text_x += 12;
      current_char++;

      continue;
    }
    // if a newline
    else if(*current_char == '\n')
    {
      text_x = initial_x;
      text_y += 16;
      current_char++;

      continue;
    }
    // if we do not have character stored
    else if(array_index < 0)
    {
      printf("'%c': Character does not exist in metrics array\n", array_index + START_ASCII_CHAR);
      current_char++;

      continue;
    }

    // fill local structures with the information from the metrics array
    bmp_glyph_metrics_t *glyph_metrics = &bmp_font->metrics[array_index];
    SDL_Rect src = {glyph_metrics->x, glyph_metrics->y, glyph_metrics->w, glyph_metrics->h};
    SDL_Rect dst = {text_x, text_y, bmp_font->metrics[array_index].w, bmp_font->metrics[array_index].h};

    // apply color to glyph
    SDL_Color color = hex_to_rgba_color(text_color);
    SDL_SetTextureColorMod(bmp_font->atlas, color.r, color.g, color.b);

    // render the glyph
    SDL_RenderCopy(renderer, bmp_font->atlas, &src, &dst);

    text_x += 12;
    current_char++;
  }
}