#define array_count(array) (sizeof(array) / sizeof((array)[0]))

internal v2u
make_v2u(u32 a, u32 b)
{
    v2u result = {a, b};
    return(result);
}

internal v4s
make_v4s(s32 a, s32 b, s32 c, s32 d)
{
    v4s result = {a, b, c, d};
    return(result);
}

internal v4u
make_v4u(u32 a, u32 b, u32 c, u32 d)
{
    v4u result = {a, b, c, d};
    return(result);
}

internal b32
equal_v2u(v2u a, v2u b)
{
    b32 result = (a.x == b.x &&
                  a.y == b.y);
    
    return(result);
}

internal void
print_v2u(char *name, v2u a)
{
    printf("%s.x: %u\n", name, a.x);
    printf("%s.y: %u\n\n", name, a.y);
}

internal void
print_v4u(char *name, v4u a)
{
    printf("%s.x: %u\n", name, a.x);
    printf("%s.y: %u\n", name, a.y);
    printf("%s.w: %u\n", name, a.w);
    printf("%s.h: %u\n\n", name, a.h);
}

internal void
print_v4s(char *name, v4s a)
{
    printf("%s.x: %d\n", name, a.x);
    printf("%s.y: %d\n", name, a.y);
    printf("%s.w: %d\n", name, a.w);
    printf("%s.h: %d\n\n", name, a.h);
}

internal char
sign(s32 value)
{
    char result = value < 0 ? '-' : '+';
    return(result);
}

internal u32
absolute(s32 value)
{
    u32 result = value < 0 ? -value : value;
    return(result);
}

internal u32
tile_dist_cardinal_and_ordinal(v2u a, v2u b)
{
    u32 result = 0;
    
    while(!equal_v2u(a, b))
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
    u32 result = absolute(a.x - b.x) + absolute(a.y - b.y);
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

internal Texture
load_texture(Game *game, char *path, v4u *color_key)
{
    Texture result = {0};
    
    SDL_Surface *loaded_surf = IMG_Load(path);
    if(loaded_surf)
    {
        result.w = loaded_surf->w;
        result.h = loaded_surf->h;
        
        if(color_key)
        {
            // Store the rgb color into formatted_key in the color format of the surface.
            // All pixels with the color of formatted_key will be transparent.
            u32 formatted_key = SDL_MapRGB(loaded_surf->format, color_key->r, color_key->g, color_key->b);
            SDL_SetColorKey(loaded_surf, 1, formatted_key);
        }
        
        SDL_Texture *new_tex = SDL_CreateTextureFromSurface(game->renderer, loaded_surf);
        if(new_tex)
        {
            result.tex = new_tex;
        }
        else
        {
            printf("Error: Texture could not be created from a surface.\n");
        }
    }
    else
    {
        printf("Error: Image could not be loaded: \"%s\".\n", path);
    }
    
    SDL_FreeSurface(loaded_surf);
    return(result);
}

internal b32
strings_are_equal(char *a, char *b)
{
    while(*a && *b &&
          *a++ == *b++)
    {
        if(*a == '\0' &&
           *b == '\0')
        {
            return(true);
        }
    }
    
    return(false);
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
    f32 result = (x1 - x2) / (y1 - y2);
    return(result);
}

internal v2u
center(v4u rect)
{
    v2u result =
    {
        rect.x + (rect.w / 2),
        rect.y + (rect.h / 2),
    };
    
    return(result);
}

internal void
set_render_color(Game *game, Color color)
{
    v4u value = get_color_value(color);
    SDL_SetRenderDrawColor(game->renderer,
                           value.r,
                           value.g,
                           value.b,
                           value.a);
}

internal u32
get_ratio(f32 min, f32 max, f32 width)
{
    u32 result = (u32)((min / max) * width);
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
is_inside_rect(v4u rect, v2u pos)
{
    b32 result = (pos.x >= rect.x &&
                  pos.y >= rect.y &&
                  pos.x <= (rect.x + rect.w) &&
                  pos.y <= (rect.y + rect.h));
    
    return(result);
}

#define zero_struct(structure) zero_size(&(structure), sizeof(structure));
#define zero_array(pointer, count) zero_size(pointer, sizeof((pointer)[0]) * count);
internal void
zero_size(void *pointer, u32 size)
{
    u8 *byte = pointer;
    while(size--)
    {
        *byte++ = 0;
    }
}