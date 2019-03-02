void render_level()
{
  SDL_SetRenderTarget(global_state.renderer, global_state.assets.textures[tex_tilemap]);
  SDL_RenderClear(global_state.renderer);

  i32 to_y = tile_div(global_state.camera.y + global_state.camera.h);
  i32 to_x = tile_div(global_state.camera.x + global_state.camera.w);
  for(i32 y = tile_div(global_state.camera.y); y < to_y; y++)
  {
    for(i32 x = tile_div(global_state.camera.x); x < to_x; x++)
    {
      SDL_Rect src = {tile_mul(level[(y * LEVEL_WIDTH_IN_TILES) + x]), 0, TILE_SIZE, TILE_SIZE};
      SDL_Rect dst = {tile_mul(x), tile_mul(y), TILE_SIZE, TILE_SIZE};

      // NOTE(Rami): continue lighting debugging from here

      //if(fov[(y * LEVEL_SIZE) + x] == 255)
      //{
        //SDL_SetTextureColorMod(tileset_tex, 255, 255, 255);
      //}
      //else if(fov[(y * LEVEL_SIZ)E + x] == 40)
      //{
        //SDL_SetTextureColorMod(tileset_tex, 40, 40, 40);
      //}

      SDL_RenderCopy(global_state.renderer, global_state.assets.textures[tex_game_tileset], &src, &dst);
    }
  }

  SDL_SetRenderTarget(global_state.renderer, NULL);

  SDL_Rect dst = {0, 0, global_state.camera.w, global_state.camera.h};
  SDL_RenderCopy(global_state.renderer, global_state.assets.textures[tex_tilemap], &global_state.camera, &dst);
}

void render_text(char *txt, iv2_t p, SDL_Color color, font_t *font, ...)
{
  char txt_final[256];

  va_list arg_list;
  va_start(arg_list, font);
  vsnprintf(txt_final, sizeof(txt_final), txt, arg_list);
  va_end(arg_list);

  i32 origin_x = p.x;
  char *at = txt_final;

  while(*at)
  {
    i32 array_index = *at - START_ASCII_CHAR;

    if(*at == ' ')
    {
      at++;
      p.x += font->space_size;
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

    SDL_SetTextureColorMod(font->atlas, color.r, color.g, color.b);

    glyph_metrics_t *glyph_metrics = &font->metrics[array_index];
    SDL_Rect src = {glyph_metrics->x, glyph_metrics->y, glyph_metrics->w, glyph_metrics->h};
    SDL_Rect dst = {p.x, p.y, font->metrics[array_index].w, font->metrics[array_index].h};

    SDL_RenderCopy(global_state.renderer, font->atlas, &src, &dst);

    if(!font->shared_advance_in_px)
    {
      p.x += font->metrics[array_index].unique_advance_in_px;
    }
    else
    {
      p.x += font->shared_advance_in_px;
    }

    at++;
  }
}