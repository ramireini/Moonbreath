internal void
render_tilemap()
{
    SDL_SetRenderTarget(game.renderer, textures[tex_tilemap].tex);
    SDL_RenderClear(game.renderer);
    
    for(u32 x = tile_div(game.camera.x); x <= tile_div(game.camera.x + game.camera.w); ++x)
    {
        for(u32 y = tile_div(game.camera.y); y <= tile_div(game.camera.y + game.camera.h); ++y)
        {
            v4u src = V4u(tile_mul(dungeon.tiles[y][x].value), 0, 32, 32);
            v4u dest = V4u(tile_mul(x), tile_mul(y), 32, 32);
            
            v2u pos = {x, y};
            if(is_seen(pos))
            {
                SDL_SetTextureAlphaMod(textures[tex_game_tileset].tex, 255);
                SDL_RenderCopy(game.renderer, textures[tex_game_tileset].tex, (SDL_Rect *)&src, (SDL_Rect *)&dest);
            }
            else if(has_been_seen(pos))
            {
                SDL_SetTextureAlphaMod(textures[tex_game_tileset].tex, 64);
                SDL_RenderCopy(game.renderer, textures[tex_game_tileset].tex, (SDL_Rect *)&src, (SDL_Rect *)&dest);
            }
        }
    }
    
    SDL_SetRenderTarget(game.renderer, 0);
    
    v4u src = V4u(game.camera.x, game.camera.y, game.camera.w, game.camera.h);
    v4u dest = V4u(0, 0, game.camera.w, game.camera.h);
    SDL_RenderCopy(game.renderer, textures[tex_tilemap].tex, (SDL_Rect *)&src, (SDL_Rect *)&dest);
}

internal void
render_text(char *str, v2u pos, v4f color, font_t *font, ...)
{
    SDL_SetTextureColorMod(font->atlas,
                           round_f32_to_u32(color.r * 255.0f),
                           round_f32_to_u32(color.g * 255.0f),
                           round_f32_to_u32(color.b * 255.0f));
    
    SDL_SetTextureAlphaMod(font->atlas,
                           round_f32_to_u32(color.a * 255.0f));
    
    char str_final[128] = {0};
    
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
            pos.x += font->space_size;
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
        
        v4u src = V4u(font->metrics[array_index].x, font->metrics[array_index].y, font->metrics[array_index].w, font->metrics[array_index].h);
        v4u dest = V4u(pos.x, pos.y, font->metrics[array_index].w, font->metrics[array_index].h);
        SDL_RenderCopy(game.renderer, font->atlas, (SDL_Rect *)&src, (SDL_Rect *)&dest);
        
        if(font->shared_advance)
        {
            pos.x += font->shared_advance;
        }
        else
        {
            pos.x += font->metrics[array_index].unique_advance_in_px;
        }
        
        ++at;
    }
}