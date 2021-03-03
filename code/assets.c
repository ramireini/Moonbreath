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
get_glyph_width(Font *font, char c)
{
    u32 result = 0;
    
    if(c)
    {
        u32 index = get_metric_index(c);
        result = font->metrics[index].advance;
    }
    
    return(result);
}

internal u32
get_text_width(Font *font, char *text)
{
    u32 result = 0;
    
    for(char *c = text; *c; ++c)
    {
        result += get_glyph_width(font, *c);
    }
    
    return(result);
}

internal u32
get_glyph_advance(Font *font, char c)
{
    u32 result = 0;
    
    switch(font->type)
    {
        case FontType_BMP:
        {
            result = font->bmp_advance;
        } break;
        
        case FontType_TTF:
        {
            u32 index = get_metric_index(c);
            result = font->metrics[index].advance;
        } break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal v4u
get_color_value(Color color)
{
    v4u result = {0};
    
    // TODO(rami): Make sure these are correct.
    // https://jonasjacek.github.io/colors/
    switch(color)
    {
        case Color_Black: result = make_v4u(0, 0, 0, 255); break;
        case Color_White: result = make_v4u(255, 255, 255, 255); break;
        
        case Color_LightGray: result = make_v4u(183, 186, 179, 255); break;
        case Color_DarkGray: result = make_v4u(84, 85, 82, 255); break;
        
        case Color_LightRed: result = make_v4u(232, 39, 39, 255); break;
        case Color_DarkRed: result = make_v4u(162, 0, 0, 255); break;
        
        case Color_LightGreen: result = make_v4u(138, 226, 52, 255); break;
        case Color_DarkGreen: result = make_v4u(95, 135, 0, 255); break;
        
        case Color_LightBlue: result = make_v4u(114, 159, 207, 255); break;
        case Color_DarkBlue: result = make_v4u(0, 106, 212, 255); break;
        
        // TODO(rami): Not set
        case Color_LightBrown: result = make_v4u(0, 0, 0, 255); break;
        case Color_DarkBrown: result = make_v4u(0, 0, 0, 255); break;
        
        case Color_Cyan: result = make_v4u(0, 180, 180, 255); break;
        case Color_Yellow: result = make_v4u(252, 233, 79, 255); break;
        case Color_Purple: result = make_v4u(92, 53, 102, 255); break;
        case Color_Orange: result = make_v4u(215, 95, 0, 255); break;
        
        case Color_WindowBackground: result = make_v4u(20, 35, 51, 255); break;
        case Color_WindowBorder: result = make_v4u(122, 138, 153, 255); break;
        case Color_WindowAccent: result = make_v4u(6, 13, 19, 255); break;
        
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

internal void
create_ttf_font(Game *game, Font *result, char *font_path, u32 font_size)
{
            TTF_Font *ttf_font = TTF_OpenFont(font_path, font_size);
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
                    result->size = font_size;
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
                    
                    result->set = true;
                }
            }
        }

internal void
create_bmp_font(Game *game, Font *result, char *path, u32 size, u32 glyph_per_row, u32 space_size, u32 advance)
{
    Texture atlas = load_texture(game, path, 0);
            if(atlas.tex)
            {
                result->type = FontType_BMP;
        result->size = size;
        result->bmp_advance = advance;
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
                
        result->set = true;
            }
        }

internal b32
initialize_assets(Game *game, Assets *assets)
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
        printf("Error: Window icon could not be set.\n");
    }
    
    // Set Fonts
    create_bmp_font(game, &assets->fonts[FontName_Classic], "data/fonts/classic16x16.png", 16, 14, 8, 13);
    create_bmp_font(game, &assets->fonts[FontName_ClassicOutlined], "data/fonts/classic_outlined16x16.png", 16, 14, 8, 13);
    create_ttf_font(game, &assets->fonts[FontName_Alkhemikal], "data/fonts/alkhemikal.ttf", 16);
    create_ttf_font(game, &assets->fonts[FontName_Monaco], "data/fonts/monaco.ttf", 16);
    create_ttf_font(game, &assets->fonts[FontName_DosVga], "data/fonts/dos_vga.ttf", 16);
    
    for(u32 index = 0; index < FontName_Count; ++index)
    {
        Font *font = &assets->fonts[index];
        if(!font->set)
        {
            // TODO(rami): Logging
            fonts_success = false;
            printf("Error: Font[%u] could not be loaded.\n", index);
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
    assets->ui = load_texture(game, "data/images/ui.png", 0);
    
    assets->item_ground_outline_src = make_v4u(1696, 96, 32, 32);
    assets->yellow_outline_src = make_v4u(1728, 96, 32, 32);
    
    assets->stat_and_log_window_h = 176;
    
    if(!assets->tileset.tex ||
       !assets->ui.tex)
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
        if(font->set)
        {
            SDL_DestroyTexture(font->atlas);
        }
    }
    
    SDL_DestroyTexture(assets->tilemap.tex);
    SDL_DestroyTexture(assets->tileset.tex);
    SDL_DestroyTexture(assets->ui.tex);
    }

internal void
set_texture_color(SDL_Texture *texture, Color color)
{
    v4u color_value = get_color_value(color);
    SDL_SetTextureColorMod(texture, color_value.r, color_value.g, color_value.b);
    SDL_SetTextureAlphaMod(texture, color_value.a);
}

internal void
render_text(Game *game, char *text, u32 start_x, u32 start_y, Font *font, u32 wrap_x, ...)
{
    b32 using_color_code = false;
    b32 word_is_scanned = false;
    v2u text_pos = {start_x, start_y};
    String128 formatted = {0};
    
    va_list arg_list;
    va_start(arg_list, wrap_x);
    vsnprintf(formatted.str, sizeof(formatted), text, arg_list);
    va_end(arg_list);
    
    set_texture_color(font->atlas, Color_White);
    
    for(char *at = formatted.str; *at;)
    {
        u32 index = get_metric_index(at[0]);
        GlyphMetrics *metrics = &font->metrics[index];
        
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

internal void
defer_text(UI *ui, char *text, u32 x, u32 y, ...)
{
    String128 formatted = {0};
    
    va_list arg_list;
    va_start(arg_list, y);
    vsnprintf(formatted.str, sizeof(formatted), text, arg_list);
    va_end(arg_list);
    
    // Sets the width of the rect that the text will be rendered on
    u32 new_width = x + get_text_width(ui->font, formatted.str);
    if(new_width > ui->defer_rect.w)
    {
        ui->defer_rect.w = new_width + ui->font_newline;
    }
    
    Defer *defer = ui->defer;
    for(u32 index = 0; index < MAX_DEFER_COUNT; ++index)
    {
        if(!defer[index].type)
        {
            defer[index].type = DeferType_Text;
            strcpy(defer[index].text.str, formatted.str);
            defer[index].x = x;
            defer[index].y = y;
            
            return;
        }
    }
    
    assert(0);
}

internal void
defer_texture(UI *ui, v2u pos, v4u tile_src)
{
    for(u32 index = 0; index < MAX_DEFER_COUNT; ++index)
    {
        Defer *defer = &ui->defer[index];
        
        if(!defer->type)
        {
            defer->type = DeferType_Texture;
            defer->x = pos.x;
            defer->y = pos.y;
            defer->tile_src = tile_src;
            
            return;
        }
    }
    
    assert(0);
}

internal void
defer_fill_rect(UI *ui, u32 x, u32 y, u32 w, u32 h, Color color)
{
    for(u32 index = 0; index < MAX_DEFER_COUNT; ++index)
    {
        Defer *defer = &ui->defer[index];
        
        if(!defer->type)
        {
            defer->type = DeferType_Rect;
            defer->x = x;
            defer->y = y;
            defer->w = w;
            defer->h = h;
            defer->color = color;
            
            return;
        }
    }
    
    assert(0);
}

internal void
process_defer(Game *game, Assets *assets, UI *ui)
{
    for(u32 index = 0; index < MAX_DEFER_COUNT; ++index)
    {
        Defer *defer = &ui->defer[index];
        
        u32 defer_x = defer->x + ui->defer_rect.x;
        u32 defer_y = defer->y + ui->defer_rect.y;
        
        if(defer->type == DeferType_Text)
        {
            render_text(game,
                            defer->text.str,
                        defer_x,
                        defer_y,
                        ui->font, 0);
        }
        else if(defer->type == DeferType_Texture)
        {
            v4u dest =
            {
                defer_x,
                defer_y,
                32, 32
            };
            
            SDL_RenderCopy(game->renderer,
                           assets->tileset.tex,
                               (SDL_Rect *)&defer->tile_src,
                               (SDL_Rect *)&dest);
        }
        else if(defer->type == DeferType_Rect)
        {
            set_render_color(game, Color_DarkGray);
            
            v4u rect =
            {
                defer_x,
                defer_y,
                defer->w,
                defer->h
            };
            
            render_fill_rect(game, rect, defer->color);
        }
    }
    
    zero_struct(ui->defer_rect);
    zero_array(ui->defer, MAX_DEFER_COUNT);
}