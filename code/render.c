internal void
render_tilemap()
{
    SDL_SetRenderTarget(game.renderer, texture[tex_tilemap]);
    SDL_RenderClear(game.renderer);
    
    for(u32 x = tile_div(game.camera.x); x <= tile_div(game.camera.x + game.camera.w); ++x)
    {
        for(u32 y = tile_div(game.camera.y); y <= tile_div(game.camera.y + game.camera.h); ++y)
        {
            v4u src = V4u(tile_mul(level.map[(y * LEVEL_TILE_WIDTH) + x]), 0,
                          TILE_SIZE, TILE_SIZE);
            
            v4u dest = V4u(tile_mul(x), tile_mul(y),
                           TILE_SIZE, TILE_SIZE);
            
            v2u pos = V2u(x, y);
            if(is_seen(pos))
            {
                SDL_SetTextureAlphaMod(texture[tex_game_tileset], 255);
                SDL_RenderCopy(game.renderer, texture[tex_game_tileset],
                               (SDL_Rect *)&src, (SDL_Rect *)&dest);
            }
            else if(has_been_seen(pos))
            {
                SDL_SetTextureAlphaMod(texture[tex_game_tileset], 32);
                SDL_RenderCopy(game.renderer, texture[tex_game_tileset],
                               (SDL_Rect *)&src, (SDL_Rect *)&dest);
            }
        }
    }
    
    SDL_SetRenderTarget(game.renderer, 0);
    
    v4u src = V4u(game.camera.x, game.camera.y,
                  game.camera.w, game.camera.h);
    v4u dest = V4u(0, 0,
                   game.camera.w, game.camera.h);
    SDL_RenderCopy(game.renderer, texture[tex_tilemap],
                   (SDL_Rect *)&src, (SDL_Rect *)&dest);
}

internal void
render_text(char *str, v2u pos, v4u color, font_t font, ...)
{
    char str_final[256] = {0};
    
    va_list arg_list;
    va_start(arg_list, font);
    vsnprintf(str_final, sizeof(str_final), str, arg_list);
    va_end(arg_list);
    
    u32 origin_x = pos.x;
    char *at = str_final;
    
    while(*at)
    {
        u32 array_index = *at - START_ASCII_CHAR;
        
        if(*at == ' ')
        {
            ++at;
            pos.x += font.space_size;
            continue;
        }
        else if(*at == '\n')
        {
            ++at;
            pos.x = origin_x;
            pos.y += 16;
            continue;
        }
        else if(*at == '\\' && at[1] == 'n')
        {
            at += 2;
            pos.x = origin_x;
            pos.y += 16;
            continue;
        }
        else if(array_index < 0)
        {
            ++at;
            printf("'%c': Character does not exist in metrics array\n", array_index + START_ASCII_CHAR);
            continue;
        }
        
        SDL_SetTextureColorMod(font.atlas, color.r, color.g, color.b);
        SDL_SetTextureAlphaMod(font.atlas, color.a);
        
        v4u src = V4u(font.metrics[array_index].x, font.metrics[array_index].y,
                      font.metrics[array_index].w, font.metrics[array_index].h);
        v4u dest = V4u(pos.x, pos.y,
                       font.metrics[array_index].w, font.metrics[array_index].h);
        
        SDL_RenderCopy(game.renderer, font.atlas,
                       (SDL_Rect *)&src, (SDL_Rect *)&dest);
        
        if(font.shared_advance)
        {
            pos.x += font.shared_advance;
        }
        else
        {
            pos.x += font.metrics[array_index].unique_advance_in_px;
        }
        
        ++at;
    }
}