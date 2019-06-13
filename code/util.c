// NOTE(rami): Does not consider diagonal movement
internal inline i32
tile_dist(iv2 a, iv2 b)
{
  return abs(a.x - b.x) + abs(a.y - b.y);
}

internal inline i32
tile_div(i32 n)
{
  return n / TILE_SIZE;
}

internal inline i32
tile_mul(i32 n)
{
  return n * TILE_SIZE;
}

internal iv2
get_real_position(i32 x, i32 y)
{
  iv2 result = {0};

  result.x = tile_mul(x) - game.camera.x;
  result.y = tile_mul(y) - game.camera.y;

  return result;
}

internal i32
SDL_GetWindowRefreshRate(SDL_Window *window)
{
  i32 refresh_rate = 60;

  i32 display_index = SDL_GetWindowDisplayIndex(window);
  SDL_DisplayMode mode = {0};
  if(!SDL_GetDesktopDisplayMode(display_index, &mode))
  {
    refresh_rate = mode.refresh_rate;
  }

  if(mode.refresh_rate == 0)
  {
    refresh_rate = 60;
  }

  return refresh_rate;
}

internal r32
SDL_GetSecondsElapsed(u64 old_counter, u64 new_counter)
{
  r32 result = (r32)(new_counter - old_counter) / (r32)SDL_GetPerformanceFrequency();
  return result;
}

internal inline iv2
v2(i32 a, i32 b)
{
  iv2 result = {{a, b}};
  return result;
}

internal inline rv2
r2(r32 a, r32 b)
{
  rv2 result = {{a, b}};
  return result;
}

internal inline iv4
v4(i32 a, i32 b, i32 c, i32 d)
{
  iv4 result = {{a, b, c, d}};
  return result;
}

internal i32
v2_equal(iv2 a, iv2 b)
{
  i32 result = 0;

  if(a.x == b.x && a.y == b.y)
  {
    result = 1;
  }

  return result;
}

internal i32
inside_level(iv2 pos)
{
  i32 result = 0;

  if(pos.x >= 0 && pos.x < LEVEL_WIDTH_IN_TILES &&
     pos.y >= 0 && pos.y < LEVEL_HEIGHT_IN_TILES)
  {
    result = 1;
  }

  return result;
}

// NOTE(rami):
// This is supposed to house all of our traversable tiles so we can check against them
internal i32
traversable(iv2 p)
{
  i32 result = 0;

  if(level.tiles[(p.y * LEVEL_WIDTH_IN_TILES) + p.x] == tile_none ||
     level.tiles[(p.y * LEVEL_WIDTH_IN_TILES) + p.x] == tile_floor_stone ||
     level.tiles[(p.y * LEVEL_WIDTH_IN_TILES) + p.x] == tile_floor_grass ||
     level.tiles[(p.y * LEVEL_WIDTH_IN_TILES) + p.x] == tile_door_open ||
     level.tiles[(p.y * LEVEL_WIDTH_IN_TILES) + p.x] == tile_path_up ||
     level.tiles[(p.y * LEVEL_WIDTH_IN_TILES) + p.x] == tile_path_down)
  {
    result = 1;
  }

  return result;
}

internal char*
read_file(char *path, char *mode)
{
  FILE *file = fopen(path, mode);
  if(!file)
  {
    debug("Could not read file %s", path);
    return NULL;
  }

  fseek(file, 0, SEEK_END);
  i32 size = ftell(file);
  rewind(file);

  char *buff = malloc(size + 1);
  i32 ret = fread(buff, size, 1, file);
  if(ret != 1)
  {
    free(buff);
    return NULL;
  }

  buff[size] = '\0';
  fclose(file);
  return buff;
}

internal SDL_Texture *
load_texture(char *path, iv4 *color_key)
{
  SDL_Surface *loaded_surf = IMG_Load(path);
  if(!loaded_surf)
  {
    debug("SDL could not load image %s: %s", path, IMG_GetError());
    return NULL;
  }

  if(color_key)
  {
    // Store the rgb color into color_key in the color format of the surface
    // All pixels with the color of color_key will be transparent
    i32 formatted_key = SDL_MapRGB(loaded_surf->format, color_key->r, color_key->g, color_key->b);
    SDL_SetColorKey(loaded_surf, 1, formatted_key);
  }

  SDL_Texture *new_tex = SDL_CreateTextureFromSurface(game.renderer, loaded_surf);
  if(!new_tex)
  {
    debug("SDL could not create a texture from surface: %s", SDL_GetError());
    SDL_FreeSurface(loaded_surf);
    return NULL;
  }

  SDL_FreeSurface(loaded_surf);
  return new_tex;
}

// NOTE(rami): Do we need this?
// i32 inside_level(iv2 p)
// {
//   if(p.x < 0 || p.y < 0 || p.x >= LEVEL_WIDTH_IN_TILES || p.y >= LEVEL_HEIGHT_IN_TILES)
//   {
//     return 0;
//   }

//   return 1;
// }

// NOTE(rami): Do we need this?
// internal iv4
// hex_to_rgba(i32 hex)
// {
//   iv4 rgba = v4((hex >> 24) & 0xFF, (hex >> 16) & 0xFF, (hex >> 8) & 0xFF, hex & 0xFF);
//   return rgba;
// }

// NOTE(rami): Do we need this?
// internal i32
// str_to_num(char *str)
// {
//   i32 result = 0;

//   while(*str >= '0' && *str <= '9')
//   {
//     result *= 10;
//     result += *str++ - '0';
//   }

//   return result;
// }

internal i32
get_num(i32 min, i32 max)
{
  if(min > max)
  {
    i32 temp = max;
    max = min;
    min = temp;
  }

  return min + rand() % (max - min + 1);
}

internal i32
str_cmp(char *a, char *b)
{
  i32 result = 0;

  while(*a && *b &&
        *a++ == *b++)
  {
    if(*a == '\0' && *b == '\0')
    {
      result = 1;
      break;
    }
  }

  return result;
}

internal i32
is_tile(iv2 pos, u32 tile)
{
  i32 result = 0;

  if(level.tiles[(pos.y * LEVEL_WIDTH_IN_TILES) + pos.x] == tile)
  {
    result = 1;
  }

  return result;
}