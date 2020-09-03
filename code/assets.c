internal v2u
get_next_line(v2u pos, u32 start_x, u32 font_size)
{
    v2u result =
    {
        pos.x = start_x,
        pos.y += font_size
    };
    
    return(result);
}

internal u32
get_metric_index(char c)
{
    u32 result = c - FONT_START_GLYPH;
    return(result);
}

internal u32
get_glyph_advance(Font *font, char c)
{
    u32 result = 0;
    
    if(font->type == FontType_TTF)
    {
        u32 metric_index = get_metric_index(c);
        result = font->metrics[metric_index].advance;
    }
    else
    {
        result = font->shared_advance;
    }
    
    return(result);
}

internal v4u
get_color_value(Color color)
{
    v4u result;
    
    switch(color)
    {
        case Color_Black: result = make_v4u(0, 0, 0, 255); break;
        case Color_White: result = make_v4u(240, 240, 240, 255); break;
        
        case Color_LightGray: result = make_v4u(150, 150, 150, 255); break;
        case Color_DarkGray: result = make_v4u(90, 90, 90, 255); break;
        
        case Color_LightRed: result = make_v4u(230, 23, 23, 255); break;
        case Color_DarkRed: result = make_v4u(180, 18, 18, 255); break;
        
        case Color_LightGreen: result = make_v4u(80, 248, 80, 255); break;
        case Color_DarkGreen: result = make_v4u(78, 154, 6, 255); break;
        
        case Color_LightBlue: result = make_v4u(114, 159, 207, 255); break;
        case Color_DarkBlue: result = make_v4u(0, 82, 204, 255); break;
        
        case Color_LightBrown: result = make_v4u(0, 0, 0, 255); break; // TODO(Rami): Not set.
        case Color_DarkBrown: result = make_v4u(128, 79, 1, 255); break;
        
        case Color_Cyan: result = make_v4u(6, 152, 154, 255); break;
        case Color_Yellow: result = make_v4u(255, 255, 0, 255); break;
        case Color_Purple: result = make_v4u(200, 30, 120, 255); break;
        case Color_Orange: result = make_v4u(0, 0, 0, 255); break; // TODO(Rami): Not set.
        
        invalid_default_case;
    }
    
    return(result);
}

internal char *
start_color(Color color)
{
    char *result = 0;
    
    switch(color)
    {
        case Color_Black: result = "##0"; break;
        case Color_White: result = "##1"; break;
        
        case Color_LightGray: result = "##2"; break;
        case Color_DarkGray: result = "##3"; break;
        
        case Color_LightRed: result = "##4"; break;
        case Color_DarkRed: result = "##5"; break;
        
        case Color_LightGreen: result = "##6"; break;
        case Color_DarkGreen: result = "##7"; break;
        
        case Color_LightBlue: result = "##8"; break;
        case Color_DarkBlue: result = "##9"; break;
        
        case Color_LightBrown: result = "##A"; break;
        case Color_DarkBrown: result = "##B"; break;
        
        case Color_Cyan: result = "##C"; break;
        case Color_Yellow: result = "##D"; break;
        case Color_Purple: result = "##E"; break;
        case Color_Orange: result = "##F"; break;
        
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

internal Font *
create_ttf_font(GameState *game, char *font_path, u32 font_size)
{
    Font *result = calloc(1, sizeof(Font));
    if(result)
    {
        TTF_Font *font = TTF_OpenFont(font_path, font_size);
        if(font)
        {
            SDL_Texture *atlas = SDL_CreateTexture(game->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, FONT_ATLAS_WIDTH, FONT_ATLAS_HEIGHT);
            if(atlas)
            {
                result->type = FontType_TTF;
                result->size = font_size;
                result->atlas = atlas;
                SDL_SetTextureBlendMode(result->atlas, SDL_BLENDMODE_BLEND);
                SDL_SetRenderTarget(game->renderer, result->atlas);
                
                v4u glyph = {0};
                SDL_Color glyph_color = {255, 255, 255, 255};
                SDL_Surface *glyph_surface = 0;
                SDL_Texture *glyph_texture = 0;
                
                for(u32 index = 0; index < array_count(result->metrics); ++index)
                {
                    char glyph_char = (FONT_START_GLYPH + index);
                    glyph_surface = TTF_RenderGlyph_Solid(font, glyph_char, glyph_color);
                    if(glyph_surface)
                    {
                        glyph.w = glyph_surface->w;
                        glyph.h = glyph_surface->h;
                        
                        GlyphMetrics metrics = {glyph.x, glyph.y, glyph.w, glyph.h, 0};
                        TTF_GlyphMetrics(font, glyph_char, 0, 0, 0, 0, &metrics.advance);
                        result->metrics[index] = metrics;
                        
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

internal Font *
create_bmp_font(GameState *game, char *path, u32 size, u32 glyph_per_row, u32 space_size, u32 shared_advance)
{
    Font *result = calloc(1, sizeof(Font));
    if(result)
    {
        Texture atlas = load_texture(game, path, 0);
        if(atlas.tex)
        {
            result->type = FontType_BMP;
            result->size = size;
            result->shared_advance = shared_advance;
            result->atlas = atlas.tex;
            SDL_SetTextureBlendMode(result->atlas, SDL_BLENDMODE_BLEND);
            
            v4u glyph = {1, 1, result->size, result->size};
            u32 glyph_count = 0;
            
            for(u32 index = 1; index < array_count(result->metrics); ++index)
            {
                if(glyph_count >= glyph_per_row)
                {
                    glyph.x = 1;
                    glyph.y += glyph.h + 1;
                    
                    glyph_count = 0;
                }
                
                GlyphMetrics metrics = {glyph.x, glyph.y, glyph.w, glyph.h, 0};
                result->metrics[index] = metrics;
                
                glyph.x += glyph.w + 1;
                ++glyph_count;
            }
            
            result->success = true;
        }
    }
    
    return(result);
}

internal b32
initialize_assets(GameState *game, Assets *assets)
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
    assets->fonts[FontName_Classic] = create_bmp_font(game, "data/fonts/classic16x16.png", 16, 14, 8, 13);
    assets->fonts[FontName_ClassicOutlined] = create_bmp_font(game, "data/fonts/classic_outlined16x16.png", 16, 14, 8, 13);
    assets->fonts[FontName_Alkhemikal] = create_ttf_font(game, "data/fonts/alkhemikal.ttf", 16);
    assets->fonts[FontName_Monaco] = create_ttf_font(game, "data/fonts/monaco.ttf", 16);
    assets->fonts[FontName_DosVga] = create_ttf_font(game, "data/fonts/dos_vga.ttf", 16);
    
    for(u32 index = 0; index < FontName_Count; ++index)
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
    assets->health_bar_outside = make_v4u(1716, 0, 204, 16);
    assets->health_bar_inside = make_v4u(1718, 20, 200, 12);
    
    if(game->window_size.w == 1280 &&
       game->window_size.h == 720)
    {
        assets->log_window = make_v4u(0, 345, 1280, 176);
    }
    else if(game->window_size.w == 1920 &&
            game->window_size.h == 1080)
    {
        assets->log_window = make_v4u(0, 525, 1920, 176);
    }
    
    assets->item_window = make_v4u(302, 0, 274, 341);
    assets->inventory_window = make_v4u(0, 0, 298, 341);
    assets->inventory_selected_slot = make_v4u(1716, 36, 32, 32);
    assets->inventory_equipped_slot = make_v4u(1752, 36, 32, 32);
    assets->item_ground_outline = make_v4u(1716, 72, 32, 32);
    
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
free_assets(Assets *assets)
{
    for(u32 index = 0; index < FontName_Count; ++index)
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
set_texture_color(SDL_Texture *texture, Color color)
{
    v4u color_value = get_color_value(color);
    SDL_SetTextureColorMod(texture, color_value.r, color_value.g, color_value.b);
    SDL_SetTextureAlphaMod(texture, color_value.a);
}

internal void
render_text(GameState *game, char *text, u32 start_x, u32 start_y, Font *font, u32 wrap_x, ...)
{
    b32 using_color_code = false;
    b32 word_is_scanned = false;
    v2u text_pos = {start_x, start_y};
    String128 formatted_text = {0};
    
    va_list arg_list;
    va_start(arg_list, wrap_x);
    vsnprintf(formatted_text.str, sizeof(formatted_text), text, arg_list);
    va_end(arg_list);
    
    set_texture_color(font->atlas, Color_White);
    
    for(char *at = formatted_text.str; *at;)
    {
        u32 metric_index = get_metric_index(at[0]);
        GlyphMetrics *metrics = &font->metrics[metric_index];
        
        if(at[0] == '#' &&
           at[1] == '#')
        {
            if(using_color_code)
            {
                using_color_code = false;
                set_texture_color(font->atlas, Color_White);
                at += 2;
            }
            else
            {
                if(at[2])
                {
                    Color color;
                    
                    switch(at[2])
                    {
                        case '0': color = Color_Black; break;
                        case '1': color = Color_White; break;
                        
                        case '2': color = Color_LightGray; break;
                        case '3': color = Color_DarkGray; break;
                        
                        case '4': color = Color_LightRed; break;
                        case '5': color = Color_DarkRed; break;
                        
                        case '6': color = Color_LightGreen; break;
                        case '7': color = Color_DarkGreen; break;
                        
                        case '8': color = Color_LightBlue; break;
                        case '9': color = Color_DarkBlue; break;
                        
                        case 'A': color = Color_LightBrown; break;
                        case 'B': color = Color_DarkBrown; break;
                        
                        case 'C': color = Color_Cyan; break;
                        case 'D': color = Color_Yellow; break;
                        case 'E': color = Color_Purple; break;
                        case 'F': color = Color_Orange; break;
                        
                        invalid_default_case;
                    }
                    
                    using_color_code = true;
                    set_texture_color(font->atlas, color);
                    at += 3;
                }
            }
        }
        else if(at[0] == '\n')
        {
            text_pos = get_next_line(text_pos, start_x, font->size);
            ++at;
        }
        else
        {
            if(wrap_x && !word_is_scanned)
            {
                char *scan_at = at;
                u32 scan_x = text_pos.x;
                
                while(scan_at[0] &&
                      scan_at[0] != ' ')
                {
                    scan_x += get_glyph_advance(font, scan_at[0]);
                    ++scan_at;
                }
                
                if(scan_x >= wrap_x)
                {
                    text_pos = get_next_line(text_pos, start_x, font->size);
                }
                
                word_is_scanned = true;
            }
            else if(at[0] == ' ')
            {
                word_is_scanned = false;
            }
            
            v4u src = {metrics->x, metrics->y, metrics->w, metrics->h};
            v4u dest = {text_pos.x, text_pos.y, metrics->w, metrics->h};
            SDL_RenderCopy(game->renderer, font->atlas, (SDL_Rect *)&src, (SDL_Rect *)&dest);
            
            text_pos.x += get_glyph_advance(font, at[0]);
            ++at;
        }
    }
}