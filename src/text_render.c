void render_text(char *txt, i32 x, i32 y, SDL_Color color, font_t *font, ...)
{
  char txt_final[256];

  va_list arg_list;
  va_start(arg_list, font);
  vsnprintf(txt_final, sizeof(txt_final), txt, arg_list);
  va_end(arg_list);

  i32 origin_x = x;
  char *at = txt_final;

  while(*at)
  {
    i32 array_index = *at - START_ASCII_CHAR;

    if(*at == ' ')
    {
      at++;
      x += font->space_size;
      continue;
    }
    else if(*at == '\n')
    {
      at++;
      x = origin_x;
      y += 16;
      continue;
    }
    else if(*at == '\\' && at[1] == 'n')
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

    if(!font->shared_advance_in_px)
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