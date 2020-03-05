#define array_count(array) (sizeof(array) / sizeof((array)[0]))

internal v2s
V2s(s32 a, s32 b)
{
    v2s result = {a, b};
    return(result);
}

internal v2u
V2u(u32 a, u32 b)
{
    v2u result = {a, b};
    return(result);
}

internal v2f
V2f(f32 a, f32 b)
{
    v2f result = {a, b};
    return(result);
}

internal b32
V2u_equal(v2u a, v2u b)
{
    b32 result = (a.x == b.x && a.y == b.y);
    return(result);
}

internal v4s
V4s(s32 a, s32 b, s32 c, s32 d)
{
    v4s result = {a, b, c, d};
    return(result);
}

internal v4u
V4u(u32 a, u32 b, u32 c, u32 d)
{
    v4u result = {a, b, c, d};
    return(result);
}

internal v4f
V4f(f32 a, f32 b, f32 c, f32 d)
{
    v4f result = {a, b, c, d};
    return(result);
}

internal file_t
read_file(char *path)
{
    file_t result = {0};
    
    FILE *file = fopen(path, "rb");
    if(file)
    {
        fseek(file, 0, SEEK_END);
        result.size = ftell(file);
        fseek(file, 0, SEEK_SET);
        
        result.contents = malloc(result.size + 1);
        u32 ret = fread(result.contents, result.size, 1, file);
        if(ret != 1)
        {
            result.size = 0;
            free(result.contents);
            result.contents = 0;
        }
        
        fclose(file);
        result.contents[result.size] = 0;
    }
    else
    {
        printf("ERROR: Cannot open file %s\n", path);
    }
    
    return(result);
}

internal u32
tile_dist_cardinal_and_ordinal(v2u a, v2u b)
{
    u32 result = 0;
    
    while(!V2u_equal(a, b))
    {
        if(a.x < b.x)
        {
            ++a.x;
        }
        else if(a.x > b.x)
        {
            --a.x;
        }
        
        if(a.y < b.y)
        {
            ++a.y;
        }
        else if(a.y > b.y)
        {
            --a.y;
        }
        
        ++result;
    }
    
    return(result);
}

internal u32
tile_dist_cardinal(v2u a, v2u b)
{
    u32 result = abs(a.x - b.x) + abs(a.y - b.y);
    return(result);
}

internal u32
tile_div(u32 value)
{
    u32 result = value / 32;
    return(result);
}

internal u32
tile_mul(u32 value)
{
    u32 result = value * 32;
    return(result);
}

internal v2u
get_game_pos(v2u pos)
{
    v2u result =
    {
        tile_mul(pos.x) - game.camera.x,
        tile_mul(pos.y) - game.camera.y
    };
    
    return(result);
}

internal u32
get_window_refresh_rate()
{
    u32 refresh_rate = 60;
    
    u32 display_index = SDL_GetWindowDisplayIndex(game.window);
    SDL_DisplayMode mode = {0};
    if(!SDL_GetDesktopDisplayMode(display_index, &mode))
    {
        refresh_rate = mode.refresh_rate;
    }
    
    if(mode.refresh_rate == 0)
    {
        refresh_rate = 60;
    }
    
    return(refresh_rate);
}

internal f32
seconds_elapsed(u64 old_counter, u64 new_counter, u64 perf_count_frequency)
{
    f32 result = (f32)(new_counter - old_counter) / (f32)perf_count_frequency;
    return(result);
}

internal texture_t
load_texture(char *path, v4u *color_key)
{
    texture_t result = {0};
    
    SDL_Surface *loaded_surf = IMG_Load(path);
    if(loaded_surf)
    {
        result.w = loaded_surf->w;
        result.h = loaded_surf->h;
        
        if(color_key)
        {
            // NOTE(rami):
            // Store the rgb color into formatted_key in the color format of the surface.
            // All pixels with the color of formatted_key will be transparent.
            u32 formatted_key = SDL_MapRGB(loaded_surf->format, color_key->r, color_key->g, color_key->b);
            SDL_SetColorKey(loaded_surf, 1, formatted_key);
        }
        
        SDL_Texture *new_tex = SDL_CreateTextureFromSurface(game.renderer, loaded_surf);
        if(new_tex)
        {
            result.tex = new_tex;
        }
        else
        {
            printf("ERROR: SDL could not create a texture from surface: %s\n", SDL_GetError());
        }
    }
    else
    {
        printf("ERROR: SDL could not load image %s: %s\n", path, IMG_GetError());
    }
    
    SDL_FreeSurface(loaded_surf);
    return(result);
}

internal s32
random_number(s32 min, s32 max)
{
    if(min > max)
    {
        s32 temp = max;
        max = min;
        min = temp;
    }
    
    s32 result = min + rand() % (max - min + 1);
    return(result);
}

internal b32
str_equal(char *a, char *b)
{
    b32 result = false;
    
    while(*a && *b && *a++ == *b++)
    {
        if(*a == '\0' && *b == '\0')
        {
            result = true;
            break;
        }
    }
    
    return(result);
}

internal f32
distance(u32 x1, u32 y1, u32 x2, u32 y2)
{
    f32 result = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
    return(result);
}

internal f32
slope(f32 x1, f32 y1, f32 x2, f32 y2)
{
    if((x2 - x1) < 0.0f)
    {
        f32 result = 0;
        return(result);
    }
    else
    {
        f32 result = (x2 - x1) / (y2 - y1);
        return(result);
    }
}

internal v2u
center(v4u rect)
{
    v2u result =
    {
        rect.x + rect.w / 2,
        rect.y + rect.h / 2,
    };
    
    return(result);
}

internal u32
round_f32_to_u32(f32 value)
{
    u32 result = (u32)(value + 0.5f);
    return(result);
}

internal v4u
f32_to_u32_color(v4f color)
{
    v4u result =
    {
        round_f32_to_u32(color.r * 255.0f),
        round_f32_to_u32(color.g * 255.0f),
        round_f32_to_u32(color.b * 255.0f),
        round_f32_to_u32(color.a * 255.0f)
    };
    
    return(result);
}

internal u32
get_ratio(f32 min, f32 max, f32 width)
{
    u32 result = (u32)((min / max) * width);
    return(result);
}

internal void
set_render_color(v4f color)
{
    v4u render_color = f32_to_u32_color(color);
    SDL_SetRenderDrawColor(game.renderer,
                           render_color.r,
                           render_color.g, 
                           render_color.b,
                           render_color.a);
}

internal u32
index_from_v2u(v2u pos, u32 width)
{
    u32 result = (pos.y * width) + pos.x;
    return(result);
}

internal v2u
v2u_from_index(u32 index, u32 width)
{
    v2u result = {index, 0};
    
    if(index >= width)
    {
        result.x = index % width;
        result.y = index / width;
    }
    
    return(result);
}

internal b32
is_in_rectangle(v2u pos, v4u rect)
{
    b32 result = false;
    
    if(pos.x > rect.x && pos.x < (rect.x + rect.w) &&
       pos.y > rect.y && pos.y < (rect.y + rect.h))
    {
        result = true;
    }
    
    return(result);
}