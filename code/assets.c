internal v4u
get_color_value(color color_id)
{
    v4u result;
    
    switch(color_id)
    {
        case color_black: result = V4u(0, 0, 0, 255); break;
        case color_white: result = V4u(238, 238, 236, 255); break;
        
        case color_light_gray: result = V4u(186, 189, 182, 255); break;
        case color_dark_gray: result = V4u(85, 87, 83, 255); break;
        
        case color_light_red: result = V4u(240, 15, 15, 255); break;
        case color_dark_red: result = V4u(164, 0, 0, 255); break;
        
        case color_light_green: result = V4u(80, 248, 80, 255); break;
        case color_dark_green: result = V4u(78, 154, 6, 255); break;
        
        case color_light_blue: result = V4u(114, 159, 207, 255); break;
        case color_dark_blue: result = V4u(0, 82, 204, 255); break;
        
        case color_light_brown: result = V4u(0, 0, 0, 255); break; // TODO(Rami): Not set.
        case color_dark_brown: result = V4u(128, 79, 1, 255); break;
        
        case color_cyan: result = V4u(6, 152, 154, 255); break;
        case color_yellow: result = V4u(252, 233, 79, 255); break;
        case color_purple: result = V4u(200, 30, 120, 255); break;
        case color_orange: result = V4u(0, 0, 0, 255); break; // TODO(Rami): Not set.
        
        invalid_default_case;
    }
    
    return(result);
}

internal char *
start_color(color color_id)
{
    char *result = 0;
    
    switch(color_id)
    {
        case color_black: result = "##0"; break;
        case color_white: result = "##1"; break;
        
        case color_light_gray: result = "##2"; break;
        case color_dark_gray: result = "##3"; break;
        
        case color_light_red: result = "##4"; break;
        case color_dark_red: result = "##5"; break;
        
        case color_light_green: result = "##6"; break;
        case color_dark_green: result = "##7"; break;
        
        case color_light_blue: result = "##8"; break;
        case color_dark_blue: result = "##9"; break;
        
        case color_light_brown: result = "##A"; break;
        case color_dark_brown: result = "##B"; break;
        
        case color_cyan: result = "##C"; break;
        case color_yellow: result = "##D"; break;
        case color_purple: result = "##E"; break;
        case color_orange: result = "##F"; break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal char *
end_color()
{
    char *result = "##";
    return(result);
}

internal font_t *
create_ttf_font(game_state_t *game, char *font_path, u32 font_size)
{
    font_t *result = calloc(1, sizeof(font_t));
    if(result)
    {
        TTF_Font *font = TTF_OpenFont(font_path, font_size);
        if(font)
        {
            SDL_Texture *atlas = SDL_CreateTexture(game->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, FONT_ATLAS_WIDTH, FONT_ATLAS_HEIGHT);
            if(atlas)
            {
                result->type = font_type_ttf;
                result->size = font_size;
                result->atlas = atlas;
                SDL_SetTextureBlendMode(result->atlas, SDL_BLENDMODE_BLEND);
                SDL_SetRenderTarget(game->renderer, result->atlas);
                
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
                        
                        glyph_texture = SDL_CreateTextureFromSurface(game->renderer, glyph_surface);
                        if(glyph_texture)
                        {
                            SDL_RenderCopy(game->renderer, glyph_texture, 0, (SDL_Rect *)&glyph);
                            glyph.x += glyph.w;
                            
                            SDL_FreeSurface(glyph_surface);
                            SDL_DestroyTexture(glyph_texture);
                        }
                    }
                }
                
                SDL_SetRenderTarget(game->renderer, 0);
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
create_bmp_font(game_state_t *game, char *font_path, u32 font_size, u32 glyph_per_row, u32 space_size, u32 shared_glyph_advance)
{
    font_t *result = calloc(1, sizeof(font_t));
    if(result)
    {
        texture_t atlas = load_texture(game, font_path, 0);
        if(atlas.tex)
        {
            result->type = font_type_bmp;
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
    }
    
    return(result);
}

internal b32
initialize_assets(game_state_t *game, assets_t *assets)
{
    b32 icon_success = true;
    b32 fonts_success = true;
    b32 textures_success = true;
    
    // Set Window Icon
    SDL_Surface *icon = IMG_Load("data/images/icon.png");
    if(icon)
    {
        SDL_SetWindowIcon(game->window, icon);
        SDL_FreeSurface(icon);
    }
    else
    {
        // TODO(rami): Logging
        icon_success = false;
        printf("ERROR: Could not set window icon: %s\n", SDL_GetError());
    }
    
    // Set Fonts
    assets->fonts[font_classic] = create_bmp_font(game, "data/fonts/classic16x16.png", 16, 14, 8, 13);
    assets->fonts[font_classic_outlined] = create_bmp_font(game, "data/fonts/classic_outlined16x16.png", 16, 14, 8, 13);
    assets->fonts[font_alkhemikal] = create_ttf_font(game, "data/fonts/alkhemikal.ttf", 16);
    assets->fonts[font_monaco] = create_ttf_font(game, "data/fonts/monaco.ttf", 16);
    assets->fonts[font_dos_vga] = create_ttf_font(game, "data/fonts/dos_vga.ttf", 16);
    
    for(u32 index = 0; index < font_total; ++index)
    {
        if(!assets->fonts[index] ||
           !assets->fonts[index]->success)
        {
            // TODO(rami): Logging
            fonts_success = false;
            printf("ERROR: Font %u could not be loaded: %s\n", index, SDL_GetError());
        }
    }
    
    // Set Textures
    assets->tilemap.w = tile_mul(MAX_DUNGEON_SIZE);
    assets->tilemap.h = tile_mul(MAX_DUNGEON_SIZE);
    assets->tilemap.tex = SDL_CreateTexture(game->renderer,
                                            SDL_PIXELFORMAT_RGBA8888,
                                            SDL_TEXTUREACCESS_TARGET,
                                            assets->tilemap.w,
                                            assets->tilemap.h);
    
    assets->tileset = load_texture(game, "data/images/tileset.png", 0);
    assets->item_tileset = load_texture(game, "data/images/item_tileset.png", 0);
    assets->wearable_item_tileset = load_texture(game, "data/images/wearable_item_tileset.png", 0);
    assets->sprite_sheet = load_texture(game, "data/images/sprite_sheet.png", 0);
    
    assets->ui = load_texture(game, "data/images/ui.png", 0);
    assets->health_bar_outside = V4u(1716, 0, 204, 16);
    assets->health_bar_inside = V4u(1718, 20, 200, 12);
    
    if(game->window_size.w == 1280 &&
       game->window_size.h == 720)
    {
        assets->log_window = V4u(0, 342, 1280, 176);
    }
    else if(game->window_size.w == 1920 &&
            game->window_size.h == 1080)
    {
        assets->log_window = V4u(0, 522, 1920, 176);
    }
    
    assets->inventory_window = V4u(0, 0, 298, 338);
    assets->inventory_selected_slot = V4u(1716, 36, 32, 32);
    assets->inventory_equipped_slot = V4u(1752, 36, 32, 32);
    assets->item_window = V4u(302, 0, 274, 338);
    
    if(!assets->tileset.tex ||
       !assets->item_tileset.tex ||
       !assets->wearable_item_tileset.tex ||
       !assets->sprite_sheet.tex ||
       !assets->ui.tex)
    {
        // TODO(rami): Logging
        textures_success = false;
        printf("ERROR: A texture could not be created: %s\n", SDL_GetError());
    }
    
    return(icon_success && fonts_success && textures_success);
}

internal void
free_assets(assets_t *assets)
{
    for(u32 index = 0; index < font_total; ++index)
    {
        if(assets->fonts[index])
        {
            SDL_DestroyTexture(assets->fonts[index]->atlas);
            free(assets->fonts[index]);
            
            printf("Font %u: deallocated\n", index);
        }
    }
    
    SDL_DestroyTexture(assets->tilemap.tex);
    SDL_DestroyTexture(assets->tileset.tex);
    SDL_DestroyTexture(assets->item_tileset.tex);
    SDL_DestroyTexture(assets->wearable_item_tileset.tex);
    SDL_DestroyTexture(assets->sprite_sheet.tex);
    SDL_DestroyTexture(assets->ui.tex);
    
    printf("Textures deallocated\n");
}

internal void
set_texture_color(SDL_Texture *texture, color color_id)
{
    v4u color = get_color_value(color_id);
    SDL_SetTextureColorMod(texture, color.r, color.g, color.b);
    SDL_SetTextureAlphaMod(texture, color.a);
}

internal void
render_text(game_state_t *game, char *text, u32 x, u32 y, font_t *font, color color_id, ...)
{
    b32 applying_color_code = false;
    string_128_t formatted_text = {0};
    
    va_list arg_list;
    va_start(arg_list, color_id);
    vsnprintf(formatted_text.str, sizeof(formatted_text), text, arg_list);
    va_end(arg_list);
    
    set_texture_color(font->atlas, color_id);
    
    for(char *at = formatted_text.str; *at;)
    {
        u32 metric_index = *at - START_ASCII_GLYPH;
        
        if(at[0] == '#' &&
           at[1] == '#')
        {
            if(applying_color_code)
            {
                applying_color_code = false;
                set_texture_color(font->atlas, color_id);
                at += 2;
            }
            else
            {
                if(at[2])
                {
                    color color_code;
                    
                    switch(at[2])
                    {
                        case '0': color_code = color_white; break;
                        case '1': color_code = color_white; break;
                        
                        case '2': color_code = color_light_gray; break;
                        case '3': color_code = color_dark_gray; break;
                        
                        case '4': color_code = color_light_red; break;
                        case '5': color_code = color_dark_red; break;
                        
                        case '6': color_code = color_light_green; break;
                        case '7': color_code = color_dark_green; break;
                        
                        case '8': color_code = color_light_blue; break;
                        case '9': color_code = color_dark_blue; break;
                        
                        case 'A': color_code = color_light_brown; break;
                        case 'B': color_code = color_dark_brown; break;
                        
                        case 'C': color_code = color_cyan; break;
                        case 'D': color_code = color_yellow; break;
                        case 'E': color_code = color_purple; break;
                        case 'F': color_code = color_orange; break;
                        
                        invalid_default_case;
                    }
                    
                    applying_color_code = true;
                    set_texture_color(font->atlas, color_code);
                    at += 3;
                }
            }
        }
        else
        {
            v4u src = {font->metrics[metric_index].x, font->metrics[metric_index].y, font->metrics[metric_index].w, font->metrics[metric_index].h};
            v4u dest = {x, y, font->metrics[metric_index].w, font->metrics[metric_index].h};
            SDL_RenderCopy(game->renderer, font->atlas, (SDL_Rect *)&src, (SDL_Rect *)&dest);
            
            x += (font->type == font_type_bmp) ? font->shared_glyph_advance : font->metrics[metric_index].glyph_advance;
            ++at;
        }
    }
}