internal void
render_tilemap()
{
    SDL_SetRenderTarget(game.renderer, texture[tex_tilemap]);
    SDL_RenderClear(game.renderer);
    
    for(u32 x = tile_div(game.camera.x); x <= tile_div(game.camera.x + game.camera.w); ++x)
    {
        for(u32 y = tile_div(game.camera.y); y <= tile_div(game.camera.y + game.camera.h); ++y)
        {
            SDL_Rect src = {tile_mul(level.tiles[(y * LEVEL_TILE_WIDTH) + x]), 0, TILE_SIZE, TILE_SIZE};
            SDL_Rect dest = {tile_mul(x), tile_mul(y), TILE_SIZE, TILE_SIZE};
            
            v2u pos = V2u(x, y);
            if(is_lit(pos))
            {
                SDL_SetTextureAlphaMod(texture[tex_game_tileset], 255);
                SDL_RenderCopy(game.renderer, texture[tex_game_tileset], &src, &dest);
            }
            else if(is_seen(pos))
            {
                SDL_SetTextureAlphaMod(texture[tex_game_tileset], 32);
                SDL_RenderCopy(game.renderer, texture[tex_game_tileset], &src, &dest);
            }
        }
    }
    
    SDL_SetRenderTarget(game.renderer, 0);
    
    SDL_Rect src = {game.camera.x, game.camera.y, game.camera.w, game.camera.h};
    SDL_Rect dest = {0, 0, game.camera.w, game.camera.h};
    SDL_RenderCopy(game.renderer, texture[tex_tilemap], &src, &dest);
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
            debug("'%c': Character does not exist in metrics array\n", array_index + START_ASCII_CHAR);
            continue;
        }
        
        SDL_SetTextureColorMod(font.atlas, color.r, color.g, color.b);
        SDL_SetTextureAlphaMod(font.atlas, color.a);
        
        SDL_Rect src = {font.metrics[array_index].x, font.metrics[array_index].y,
            font.metrics[array_index].w, font.metrics[array_index].h};
        SDL_Rect dest = {pos.x, pos.y, font.metrics[array_index].w, font.metrics[array_index].h};
        
        SDL_RenderCopy(game.renderer, font.atlas, &src, &dest);
        
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