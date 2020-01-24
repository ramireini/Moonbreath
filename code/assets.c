internal font_t *
create_ttf_font(char *font_path, u32 font_size)
{
    font_t *result = calloc(1, sizeof(font_t));
    if(result)
    {
        TTF_Font *font = TTF_OpenFont(font_path, font_size);
        if(font)
        {
            SDL_Texture *atlas = SDL_CreateTexture(game.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, FONT_ATLAS_WIDTH, FONT_ATLAS_HEIGHT);
            if(atlas)
            {
                result->type = font_ttf;
                result->path = font_path;
                result->size = font_size;
                result->atlas = atlas;
                SDL_SetTextureBlendMode(result->atlas, SDL_BLENDMODE_BLEND);
                SDL_SetRenderTarget(game.renderer, result->atlas);
                
                v4u glyph = {0};
                SDL_Color glyph_color = {255, 255, 255, 255};
                SDL_Surface *glyph_surface = 0;
                SDL_Texture *glyph_texture = 0;
                
                for(u32 i = 0; i < array_count(result->metrics); ++i)
                {
                    char glyph_char = START_ASCII_GLYPH + i;
                    
                    glyph_surface = TTF_RenderGlyph_Solid(font, glyph_char, glyph_color);
                    if(glyph_surface)
                    {
                        glyph.w = glyph_surface->w;
                        glyph.h = glyph_surface->h;
                        
                        s32 glyph_advance = 0;
                        TTF_GlyphMetrics(font, glyph_char, 0, 0, 0, 0, &glyph_advance);
                        
                        glyph_metrics_t metrics = {glyph.x, glyph.y, glyph.w, glyph.h, (u32)glyph_advance};
                        result->metrics[i] = metrics;
                        
                        glyph_texture = SDL_CreateTextureFromSurface(game.renderer, glyph_surface);
                        if(glyph_texture)
                        {
                            SDL_RenderCopy(game.renderer, glyph_texture, 0, (SDL_Rect *)&glyph);
                            glyph.x += glyph.w;
                            
                            SDL_FreeSurface(glyph_surface);
                            SDL_DestroyTexture(glyph_texture);
                        }
                        else
                        {
                            SDL_FreeSurface(glyph_surface);
                            SDL_DestroyTexture(atlas);
                            TTF_CloseFont(font);
                            printf("ERROR: SDL could not create a texture from surface: %s\n", SDL_GetError());
                            return(0);
                        }
                    }
                    else
                    {
                        SDL_DestroyTexture(atlas);
                        TTF_CloseFont(font);
                        printf("ERROR: TTF_RenderGlyph_Solid returned NULL.\n");
                        return(0);
                    }
                }
                
                SDL_SetRenderTarget(game.renderer, 0);
                TTF_CloseFont(font);
                
                result->success = true;
            }
            else
            {
                TTF_CloseFont(font);
                printf("ERROR: SDL could not create a texture: %s\n", SDL_GetError());
            }
        }
        else
        {
            printf("ERROR: Could not open font: %s\n", font_path);
        }
    }
    
    return(result);
}

internal font_t *
create_bmp_font(char *font_path, u32 font_size, u32 glyph_per_row, u32 space_size, u32 shared_glyph_advance)
{
    font_t *result = calloc(1, sizeof(font_t));
    if(result)
    {
        texture_t atlas = load_texture(font_path, 0);
        if(atlas.tex)
        {
            result->type = font_bmp;
            result->path = font_path;
            result->size = font_size;
            result->shared_glyph_advance = shared_glyph_advance;
            result->atlas = atlas.tex;
            SDL_SetTextureBlendMode(result->atlas, SDL_BLENDMODE_BLEND);
            
            v4u glyph = {1, 1, font_size, font_size};
            u32 glyph_count = 0;
            
            for(u32 i = 1; i < array_count(result->metrics); ++i)
            {
                if(glyph_count >= glyph_per_row)
                {
                    glyph.x = 1;
                    glyph.y += glyph.h + 1;
                    
                    glyph_count = 0;
                }
                
                glyph_metrics_t metrics = {glyph.x, glyph.y, glyph.w, glyph.h, 0};
                result->metrics[i] = metrics;
                
                glyph.x += glyph.w + 1;
                ++glyph_count;
            }
            
            result->success = true;
        }
        else
        {
            free(result);
            printf("ERROR: Could not open texture: %s\n", font_path);
        }
    }
    
    return(result);
}

internal void
free_assets()
{
    for(u32 i = 0; i < font_total; ++i)
    {
        if(fonts[i])
        {
            SDL_DestroyTexture(fonts[i]->atlas);
            free(fonts[i]);
            printf("Font %u: deallocated\n", i);
        }
    }
    
    SDL_DestroyTexture(textures.tilemap.tex);
    SDL_DestroyTexture(textures.game_tileset.tex);
    SDL_DestroyTexture(textures.item_tileset.tex);
    SDL_DestroyTexture(textures.wearable_item_tileset.tex);
    SDL_DestroyTexture(textures.sprite_sheet.tex);
    SDL_DestroyTexture(textures.ui);
}