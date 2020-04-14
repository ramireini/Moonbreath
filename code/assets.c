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
    SDL_DestroyTexture(textures.tileset.tex);
    SDL_DestroyTexture(textures.item_tileset.tex);
    SDL_DestroyTexture(textures.wearable_item_tileset.tex);
    SDL_DestroyTexture(textures.sprite_sheet.tex);
    SDL_DestroyTexture(textures.ui);
    
    printf("Textures deallocated\n");
}

internal void
render_text(char *text, u32 x, u32 y, font_t *font, u32 wrap_width, ...)
{
    char formatted_text[128] = {0};
    
    va_list arg_list;
    va_start(arg_list, wrap_width);
    vsnprintf(formatted_text, sizeof(formatted_text), text, arg_list);
    va_end(arg_list);
    
    v4u int_color = f32_to_u32_color(color_white);
    SDL_SetTextureColorMod(font->atlas, int_color.r, int_color.g, int_color.b);
    SDL_SetTextureAlphaMod(font->atlas, int_color.a);
    
    u32 start_x = x;
    u32 last_space_x = 0;
    
    for(char *at = formatted_text; *at;)
    {
        u32 metric_index = *at - START_ASCII_GLYPH;
        
        if(at[0] == '#' && at[1] == '#' && at[2] && at[3] == ' ')
        {
            v4u int_color = {0};
            
            switch(at[2])
            {
                case '0': int_color = f32_to_u32_color(color_black); break;
                case '1': int_color = f32_to_u32_color(color_white); break;
                
                case '2': int_color = f32_to_u32_color(color_light_gray); break;
                case '3': int_color = f32_to_u32_color(color_dark_gray); break;
                
                case '4': int_color = f32_to_u32_color(color_light_red); break;
                case '5': int_color = f32_to_u32_color(color_dark_red); break;
                
                case '6': int_color = f32_to_u32_color(color_light_green); break;
                case '7': int_color = f32_to_u32_color(color_dark_green); break;
                
                case '8': int_color = f32_to_u32_color(color_light_blue); break;
                case '9': int_color = f32_to_u32_color(color_dark_blue); break;
                
                case 'A': int_color = f32_to_u32_color(color_light_brown); break;
                case 'B': int_color = f32_to_u32_color(color_dark_brown); break;
                
                case 'C': int_color = f32_to_u32_color(color_cyan); break;
                case 'D': int_color = f32_to_u32_color(color_yellow); break;
                case 'E': int_color = f32_to_u32_color(color_purple); break;
                case 'F': int_color = f32_to_u32_color(color_orange); break;
                
                invalid_default_case;
            }
            
            SDL_SetTextureColorMod(font->atlas, int_color.r, int_color.g, int_color.b);
            at += 4;
        }
        else
        {
            v4u src = {font->metrics[metric_index].x, font->metrics[metric_index].y, font->metrics[metric_index].w, font->metrics[metric_index].h};
            v4u dest = {x, y, font->metrics[metric_index].w, font->metrics[metric_index].h};
            SDL_RenderCopy(game.renderer, font->atlas, (SDL_Rect *)&src, (SDL_Rect *)&dest);
            
            x += (font->type == font_bmp) ? font->shared_glyph_advance : font->metrics[metric_index].glyph_advance;
            
            // TODO(rami): If we end up not needing wrapping we can nuke this.
            if(wrap_width && *at == ' ')
            {
                last_space_x = x;
                if(last_space_x > wrap_width)
                {
                    x = start_x;
                    y += font->size;
                }
            }
            
            ++at;
        }
    }
}