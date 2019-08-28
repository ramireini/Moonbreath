#define array_count(array) (sizeof(array) / sizeof((array)[0]))

internal v2i
V2i(i32 a, i32 b)
{
    v2i result = {{a, b}};
    return(result);
}

internal v2u
V2u(u32 a, u32 b)
{
    v2u result = {{a, b}};
    return(result);
}

internal v2u
V2u_add(v2u a, v2u b)
{
    v2u result = {a.x + b.x, a.y + b.y};
    return(result);
}

internal b32
V2u_equal(v2u a, v2u b)
{
    b32 result = false;
    
    if(a.x == b.x && a.y == b.y)
    {
        result = true;
    }
    
    return(result);
}

internal v2f
V2f(f32 a, f32 b)
{
    v2f result = {{a, b}};
    return(result);
}

internal v4u
V4u(u32 a, u32 b, u32 c, u32 d)
{
    v4u result = {{a, b, c, d}};
    return(result);
}

internal v4i
V4i(i32 a, i32 b, i32 c, i32 d)
{
    v4i result = {{a, b, c, d}};
    return(result);
}

internal file_t
read_file_contents(char *path)
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
        printf("ERROR: Cannot open file %s.\n", path);
    }
    
    return(result);
}

internal u32
tile_dist(v2u a, v2u b)
{
    u32 result = 0;
    result = abs(a.x - b.x) + abs(a.y - b.y);
    return(result);
}

// TODO(rami): Possibly not required
internal f32
dist_between(v2u a, v2u b)
{
    f32 result = 0.0f;
    result = sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2));
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
get_game_position(v2u pos)
{
    v2u result = {0};
    
    result.x = tile_mul(pos.x) - game.camera.x;
    result.y = tile_mul(pos.y) - game.camera.y;
    
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
get_seconds_elapsed(u64 old_counter, u64 new_counter)
{
    f32 result = (f32)(new_counter - old_counter) / game.perf_count_frequency;
    return(result);
}

internal b32
is_inside_level(v2u pos)
{
    b32 result = false;
    
    if(pos.x >= 0 && pos.x < MAX_LEVEL_WIDTH &&
       pos.y >= 0 && pos.y < MAX_LEVEL_HEIGHT)
    {
        result = true;
    }
    
    return(result);
}

internal b32
is_traversable(v2u pos)
{
    b32 result = false;
    
    if(level.tiles[pos.y][pos.x] == tile_floor_stone ||
       level.tiles[pos.y][pos.x] == tile_floor_grass ||
       level.tiles[pos.y][pos.x] == tile_door_open ||
       level.tiles[pos.y][pos.x] == tile_path_up ||
       level.tiles[pos.y][pos.x] == tile_path_down)
    {
        result = true;
    }
    
    return(result);
}

internal SDL_Texture *
load_texture(char *path, v4u *color_key)
{
    SDL_Texture *result = 0;
    
    SDL_Surface *loaded_surf = IMG_Load(path);
    if(loaded_surf)
    {
        if(color_key)
        {
            // Store the rgb color into color_key in the color format of the surface
            // All pixels with the color of color_key will be transparent
            u32 formatted_key = SDL_MapRGB(loaded_surf->format, color_key->r, color_key->g, color_key->b);
            SDL_SetColorKey(loaded_surf, 1, formatted_key);
        }
        
        SDL_Texture *new_tex = SDL_CreateTextureFromSurface(game.renderer, loaded_surf);
        if(new_tex)
        {
            result = new_tex;
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

// TODO(rami): Do we need this?
// internal v4u
// hex_to_rgba(u32 hex)
// {
//   v4u rgba = V4u((hex >> 24) & 0xFF, (hex >> 16) & 0xFF, (hex >> 8) & 0xFF, hex & 0xFF);
//   return(rgba);
// }

// TODO(rami): Do we need this?
// internal u32
// str_to_num(char *str)
// {
//   u32 result = 0;

//   while(*str >= '0' && *str <= '9')
//   {
//     result *= 10;
//     result += *str++ - '0';
//   }

//   return(result);
// }

internal u32
rand_num(u32 min, u32 max)
{
    if(min > max)
    {
        u32 temp = max;
        max = min;
        min = temp;
    }
    
    u32 result = 0;
    result = min + rand() % (max - min + 1);
    return(result);
}

internal b32
str_equal(char *a, char *b)
{
    b32 result = false;
    
    while(*a && *b &&
          *a++ == *b++)
    {
        if(*a == '\0' && *b == '\0')
        {
            result= true;
            break;
        }
    }
    
    return(result);
}

internal b32
is_tile(v2u pos, u32 tile)
{
    b32 result = false;
    
    if(level.tiles[pos.y][pos.x] == tile)
    {
        result = true;
    }
    
    return(result);
}

internal f32
distance_between(u32 x1, u32 y1, u32 x2, u32 y2)
{
    f32 result = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
    return(result);
}

internal f32
line_slope(f32 x1, f32 y1, f32 x2, f32 y2)
{
    f32 result = (x1 - x2) / (y1 - y2);
    return(result);
}

internal u32
get_index_from_pos(v2u pos, u32 pitch)
{
    u32 result = (pos.y * pitch) + pos.x;
    return(result);
}