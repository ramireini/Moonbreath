internal char *
get_color_string(Color color)
{
    char *result = 0;
    
    switch(color)
    {
        case Color_Black: result = "Black"; break;
        case Color_White: result = "White"; break;
        
        case Color_LightGray: result = "Light Gray"; break;
        case Color_DarkGray: result = "Dark Gray"; break;
        
        case Color_LightRed: result = "Light Red"; break;
        case Color_DarkRed: result = "Dark Red"; break;
        
        case Color_Green: result = "Green"; break;
        case Color_LightGreen: result = "Light Green"; break;
        case Color_DarkGreen: result = "Dark Green"; break;
        
        case Color_LightBlue: result = "Light Blue"; break;
        case Color_DarkBlue: result = "Dark Blue"; break;
        
        case Color_LightBrown: result = "Light Brown"; break;
        case Color_DarkBrown: result = "Dark Brown"; break;
        
        case Color_Yellow: result = "Yellow"; break;
        case Color_LightYellow: result = "Light Yellow"; break;
        
        case Color_Cyan: result = "Cyan"; break;
        case Color_Purple: result = "Purple"; break;
        case Color_Orange: result = "Orange"; break;
        
         invalid_default_case;
    }
    
    return(result);
}

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
get_font_metric_index_from_char(char c)
{
    assert(c);
    
    u32 result = (c - FONT_START_GLYPH);
    return(result);
}

internal u32
get_glyph_width(char c, Font *font)
{
    assert(font);
    
    u32 result = 0;
    
    if(c)
    {
        result = font->metrics[get_font_metric_index_from_char(c)].advance;
    }
    
    return(result);
}

internal u32
get_text_width(char *text, Font *font, b32 strip_color_codes)
{
    assert(text);
    assert(font);
    
    u32 result = 0;
    
    for(char *c = text; *c; ++c)
    {
        if(strip_color_codes &&
               c[0] == '#' &&
               c[1] == '#')
        {
            assert(is_numeric(c[2]) || is_uppercase(c[2]));
            c += 3;
        }
        
        result += get_glyph_width(*c, font);
    }
    
    return(result);
}

internal u32
get_glyph_advance(Font *font, char c)
{
    assert(font);
    
    u32 result = 0;
    
    switch(font->type)
    {
        case FontType_BMP: result = font->bmp_advance; break;
        case FontType_TTF: result = font->metrics[get_font_metric_index_from_char(c)].advance; break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal v4u
get_color_value(Color color)
{
    v4u result = {0};
    
    switch(color)
    {
        case Color_Black: result = make_v4u(0, 0, 0, 255); break;
        case Color_White: result = make_v4u(255, 255, 255, 255); break;
        
        case Color_LightGray: result = make_v4u(179, 179, 179, 255); break;
        case Color_DarkGray: result = make_v4u(84, 85, 82, 255); break;
        
        case Color_LightRed: result = make_v4u(217, 22, 22, 255); break;
        case Color_DarkRed: result = make_v4u(128, 0, 0, 255); break;
        
        case Color_Green: result = make_v4u(0, 180, 0, 255); break;
        case Color_LightGreen: result = make_v4u(0, 230, 0, 255); break;
        case Color_DarkGreen: result = make_v4u(0, 128, 0, 255); break;
        
        case Color_LightBlue: result = make_v4u(114, 159, 207, 255); break;
        case Color_DarkBlue: result = make_v4u(0, 106, 212, 255); break;
        
        case Color_LightBrown: result = make_v4u(0, 0, 0, 255); break;
        case Color_DarkBrown: result = make_v4u(0, 0, 0, 255); break;
        
        case Color_Yellow: result = make_v4u(255, 230, 0, 255); break;
        
        case Color_LightYellow:
        {
            result = get_color_value(Color_Yellow);
            result.b = 80;
        } break;
        
        case Color_EntityView:
        {
            result = get_color_value(Color_Yellow);
            result.a = 16;
        } break;
        
        case Color_Cyan: result = make_v4u(0, 180, 180, 255); break;
        case Color_Purple: result = make_v4u(92, 53, 102, 255); break;
        case Color_Orange: result = make_v4u(215, 95, 0, 255); break;
        
        case Color_DebugBackgroundLight: result = make_v4u(0, 153, 230, 255); break;
        case Color_DebugBackgroundDark: result = make_v4u(0, 102, 204, 255); break;
        case Color_AlmostWhite: result = make_v4u(230, 230, 230, 255); break;
        
        //
        
        case Color_WindowBackground: result = make_v4u(20, 35, 51, 255); break;
        case Color_WindowBorder: result = make_v4u(122, 138, 153, 255); break;
        case Color_WindowShadow: result = make_v4u(6, 13, 19, 255); break;
        
        case Color_MouseSelect:
        {
            result = get_color_value(Color_WindowBorder);
            result.a = 64;
        } break;
        
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
        
        case Color_Green: result = "##6"; break;
        case Color_LightGreen: result = "##7"; break;
        case Color_DarkGreen: result = "##8"; break;
        
        case Color_LightBlue: result = "##9"; break;
        case Color_DarkBlue: result = "##A"; break;
        
        case Color_LightBrown: result = "##B"; break;
        case Color_DarkBrown: result = "##C"; break;
        
        case Color_Yellow: result = "##D"; break;
        case Color_LightYellow: result = "##E"; break;
        
        case Color_Cyan: result = "##F"; break;
        case Color_Purple: result = "##G"; break;
        case Color_Orange: result = "##H"; break;
        
        case Color_AlmostWhite: result = "##I"; break;
        
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

internal void
create_font_ttf(Game *game, Font *result, char *path, u32 size)
{
            TTF_Font *ttf_font = TTF_OpenFont(path, size);
            if(ttf_font)
            {
                SDL_Texture *atlas = SDL_CreateTexture(game->renderer,
                                                       SDL_PIXELFORMAT_RGBA8888,
                                                       SDL_TEXTUREACCESS_TARGET,
                                                       FONT_ATLAS_WIDTH,
                                                       FONT_ATLAS_HEIGHT);
                if(atlas)
                {
                    result->type = FontType_TTF;
                    result->size = size;
                    result->atlas = atlas;
                    SDL_SetTextureBlendMode(result->atlas, SDL_BLENDMODE_BLEND);
                    SDL_SetRenderTarget(game->renderer, result->atlas);
                    
                    v4u glyph_dest = {0};
                    SDL_Color glyph_color = {255, 255, 255, 255};
                    SDL_Surface *glyph_surface = 0;
                    SDL_Texture *glyph_texture = 0;
                    
                    for(u32 index = 0; index < array_count(result->metrics); ++index)
                    {
                        char glyph_char = (FONT_START_GLYPH + index);
                        
                        glyph_surface = TTF_RenderGlyph_Solid(ttf_font, glyph_char, glyph_color);
                        if(glyph_surface)
                        {
                            glyph_dest.w = glyph_surface->w;
                            glyph_dest.h = glyph_surface->h;
                            
                            GlyphMetrics metrics =
                            {
                                glyph_dest.x,
                                glyph_dest.y,
                                glyph_dest.w,
                                glyph_dest.h,
                                0
                            };
                            
                            TTF_GlyphMetrics(ttf_font, glyph_char, 0, 0, 0, 0, &metrics.advance);
                            result->metrics[index] = metrics;
                            
                            glyph_texture = SDL_CreateTextureFromSurface(game->renderer, glyph_surface);
                            if(glyph_texture)
                            {
                                SDL_RenderCopy(game->renderer, glyph_texture, 0, (SDL_Rect *)&glyph_dest);
                                glyph_dest.x += glyph_dest.w;
                                
                                SDL_FreeSurface(glyph_surface);
                                SDL_DestroyTexture(glyph_texture);
                            }
                        }
                    }
                    
                    SDL_SetRenderTarget(game->renderer, 0);
                    TTF_CloseFont(ttf_font);
                    
            result->is_valid = true;
                }
            }
        }

internal void
create_font_bmp(Game *game, Font *result, char *path, u32 size, u32 glyphs_per_row, u32 advance)
{
    Texture atlas = load_texture(game->renderer, path, 0);
            if(atlas.tex)
            {
                result->type = FontType_BMP;
        result->size = size;
        result->bmp_advance = advance;
        result->atlas = atlas.tex;
        SDL_SetTextureBlendMode(result->atlas, SDL_BLENDMODE_BLEND);
                
                u32 glyph_count = 0;
        v4u glyph = {1, 1, result->size, result->size};
                
        for(u32 index = 1; index < array_count(result->metrics); ++index)
                {
                    if(glyph_count >= glyphs_per_row)
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
                
        result->is_valid = true;
            }
        }

internal b32
initialize_assets(Game *game, Assets *assets)
{
    b32 icon_success = true;
    b32 fonts_success = true;
    b32 textures_success = true;
    
    // Set Window Icon
    SDL_Surface *icon = IMG_Load(get_os_path("data/images/icon.png").s);
    if(icon)
    {
        SDL_SetWindowIcon(game->window, icon);
        SDL_FreeSurface(icon);
    }
    else
    {
        // TODO(rami): Logging
        icon_success = false;
        printf("Error: Window icon could not be set.\n");
    }
    
    // Set Fonts
    create_font_bmp(game, &assets->fonts[FontName_Classic], get_os_path("data/fonts/classic16x16.png").s, 16, 14, 13);
    create_font_bmp(game, &assets->fonts[FontName_ClassicOutlined], get_os_path("data/fonts/classic_outlined16x16.png").s, 16, 14, 13);
    create_font_ttf(game, &assets->fonts[FontName_Alkhemikal], get_os_path("data/fonts/alkhemikal.ttf").s, 16);
    create_font_ttf(game, &assets->fonts[FontName_Monaco], get_os_path("data/fonts/monaco.ttf").s, 16);
    create_font_ttf(game, &assets->fonts[FontName_DosVga], get_os_path("data/fonts/dos_vga.ttf").s, 16);
    
    for(u32 index = 0; index < FontName_Count; ++index)
    {
        Font *font = &assets->fonts[index];
        if(!font->is_valid)
        {
            // TODO(rami): Logging
            fonts_success = false;
            printf("Error: Font[%u] could not be loaded.\n", index);
        }
    }
    
    // Set Textures
    assets->tilemap.w = tile_mul(MAX_DUNGEON_SIZE);
    assets->tilemap.h = tile_mul(MAX_DUNGEON_SIZE);
    assets->tilemap.tex = SDL_CreateTexture(game->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, assets->tilemap.w, assets->tilemap.h);
    assets->tileset = load_texture(game->renderer, get_os_path("data/images/tileset.png").s, 0);
    
    assets->health_bar_sources[0] = get_pos_tile_mul_rect(make_v2u(1, 16));
    assets->health_bar_sources[1] = get_pos_tile_mul_rect(make_v2u(2, 16));
    assets->health_bar_sources[2] = get_pos_tile_mul_rect(make_v2u(3, 16));
    assets->health_bar_sources[3] = get_pos_tile_mul_rect(make_v2u(4, 16));
    assets->health_bar_sources[4] = get_pos_tile_mul_rect(make_v2u(5, 16));
    
    assets->stat_and_log_window_h = 176;
    
    if(!assets->tileset.tex)
    {
        // TODO(rami): Logging
        textures_success = false;
        printf("Error: Texture could not be created.\n");
    }
    
    return(icon_success && fonts_success && textures_success);
}

internal void
free_assets(Assets *assets)
{
    for(u32 index = 0; index < FontName_Count; ++index)
    {
        Font *font = &assets->fonts[index];
        if(font->is_valid)
        {
            SDL_DestroyTexture(font->atlas);
        }
    }
    
    SDL_DestroyTexture(assets->tilemap.tex);
    SDL_DestroyTexture(assets->tileset.tex);
    }

internal void
set_texture_color(SDL_Texture *texture, Color color)
{
    assert(texture);
    assert(color);
    
    v4u color_value = get_color_value(color);
    SDL_SetTextureColorMod(texture, color_value.r, color_value.g, color_value.b);
    SDL_SetTextureAlphaMod(texture, color_value.a);
}

#define render_string_and_move(game, text, pos, lines_before, lines_after, font, ...) render_string_full(game->renderer, text, pos, lines_before, lines_after, font, 0, ##__VA_ARGS__)
#define render_string(game, string, pos, font, ...) render_string_full(game->renderer, string, pos, 0, 0, font, 0, ##__VA_ARGS__)
internal void
render_string_full(SDL_Renderer *renderer,
                 char *string,
                 v2u *start_pos,
                 u32 lines_before,
                 u32 lines_after,
                 Font *font,
                 u32 wrap_x, ...)
{
    assert(renderer);
    assert(string);
    assert(font);
    assert(font->is_valid);
    
    String256 text_format = {0};
    va_list arg_list;
    va_start(arg_list, wrap_x);
    vsnprintf(text_format.s, sizeof(text_format), string, arg_list);
    va_end(arg_list);
    
    if(lines_before)
    {
        pos_newline(start_pos, font->size, lines_before);
    }
    
    b32 using_color_code = false;
    b32 word_was_scanned = false;
    v2u text_pos = {start_pos->x, start_pos->y};
    set_texture_color(font->atlas, Color_White);
    
    for(char *at = text_format.s; *at;)
    {
        GlyphMetrics *metrics = &font->metrics[get_font_metric_index_from_char(at[0])];
        
        assert(metrics);
        assert(metrics->w);
        assert(metrics->h);
        
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
                    Color color = Color_None;
                    
                    switch(at[2])
                    {
                        case '0': color = Color_Black; break;
                        case '1': color = Color_White; break;
                        
                        case '2': color = Color_LightGray; break;
                        case '3': color = Color_DarkGray; break;
                        
                        case '4': color = Color_LightRed; break;
                        case '5': color = Color_DarkRed; break;
                        
                        case '6': color = Color_Green; break;
                        case '7': color = Color_LightGreen; break;
                        case '8': color = Color_DarkGreen; break;
                        
                        case '9': color = Color_LightBlue; break;
                        case 'A': color = Color_DarkBlue; break;
                        
                        case 'B': color = Color_LightBrown; break;
                        case 'C': color = Color_DarkBrown; break;
                        
                        case 'D': color = Color_Yellow; break;
                        case 'E': color = Color_LightYellow; break;
                        
                        case 'F': color = Color_Cyan; break;
                        case 'G': color = Color_Purple; break;
                        case 'H': color = Color_Orange; break;
                        
                        case 'I': color = Color_AlmostWhite; break;
                        
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
            text_pos = get_next_line(text_pos, start_pos->x, font->size);
            ++at;
        }
        else
        {
            if(wrap_x && !word_was_scanned)
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
                    text_pos = get_next_line(text_pos, start_pos->x, font->size);
                }
                
                word_was_scanned = true;
            }
            else if(at[0] == ' ')
            {
                word_was_scanned = false;
            }
            
            v4u src = {metrics->x, metrics->y, metrics->w, metrics->h};
            v4u dest = {text_pos.x, text_pos.y, metrics->w, metrics->h};
            SDL_RenderCopy(renderer, font->atlas, (SDL_Rect *)&src, (SDL_Rect *)&dest);
            
            text_pos.x += get_glyph_advance(font, at[0]);
            ++at;
        }
    }
    
    if(lines_after)
    {
        pos_newline(start_pos, font->size, lines_after);
    }
    }