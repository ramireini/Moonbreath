char* io_read_file(char *path, char *mode)
{
  FILE *file = fopen(path, mode);
  if(!file)
  {
    debug("Could not read file %s\n", path);
    return NULL;
  }

  fseek(file, 0, SEEK_END);
  size_t size = ftell(file);
  rewind(file);

  char *buff = malloc(size + 1);
  fread(buff, size, 1, file);

  buff[size] = '\0';
  fclose(file);

  return buff;
}

SDL_Texture* load_texture(char *path, SDL_Color *color_key)
{
  SDL_Surface *loaded_surf = IMG_Load(path);
  if(!loaded_surf)
  {
    debug("SDL could not load image %s: %s\n", path, IMG_GetError());
    return NULL;
  }

  if(color_key)
  {
    // store the rgb color into color_key in the color format of the surface
    // all pixels with the color of color_key will be transparent
    i32 formatted_key = SDL_MapRGB(loaded_surf->format, color_key->r, 0, 0);
    SDL_SetColorKey(loaded_surf, SDL_TRUE, formatted_key);
  }

  SDL_Texture *new_tex = SDL_CreateTextureFromSurface(global_state.renderer, loaded_surf);
  if(!new_tex)
  {
    debug("SDL could not create a texture from surface: %s\n", SDL_GetError());
    SDL_FreeSurface(loaded_surf);
    return NULL;
  }

  SDL_FreeSurface(loaded_surf);
  return new_tex;
}

internal inline b32 iv2_is_equal(iv2_t a, iv2_t b)
{
  if(a.x == b.x && a.y == b.y)
  {
    return true;
  }

  return false;
}

SDL_Color hex_to_rgba(i32 hex)
{
  SDL_Color rgb_color = {(hex >> 24) & 0xFF, (hex >> 16) & 0xFF, (hex >> 8) & 0xFF, hex & 0xFF};
  return rgb_color;
}

i32 str_to_i(char *str)
{
  i32 result = 0;

  while(*str >= '0' && *str <= '9')
  {
    result *= 10;
    result += *str - '0';
    str++;
  }

  return result;
}

i32 rnum(i32 min, i32 max)
{
  return min + rand() % (max - min + 1);
}

b32 str_cmp(char *a, char *b)
{
  while(*a && *b && *a == *b)
  {
    a++;
    b++;

    if(*a == '\0' && *b == '\0')
    {
      return true;
    }
  }

  return false;
}

b32 is_player_pos(int x, int y)
{
  if(x == player->entity->x &&
     y == player->entity->y)
  {
    return true;
  }

  return false;
}

b32 is_traversable_pos(i32 x, i32 y)
{
  if(level[(y * LEVEL_WIDTH_IN_TILES) + x] == tile_floor_grass ||
     level[(y * LEVEL_WIDTH_IN_TILES) + x] == tile_floor_stone ||
     level[(y * LEVEL_WIDTH_IN_TILES) + x] == tile_door_open)
  {
    return true;
  }

  return false;
}

b32 line_of_sight(i32 x0, i32 y0, i32 x1, i32 y1)
{
  i32 dx = abs(x1 - x0);
  i32 sx = x0 < x1 ? 1 : -1;

  i32 dy = -abs(y1 - y0);
  i32 sy = y0 < y1 ? 1 : -1;

  i32 err = dx + dy, err_two;

  for(;;)
  {
    if(!is_traversable_pos(x0, y0))
    {
      return false;
    }

    err_two = err * 2;
    if(err_two <= dx)
    {
      if(y0 == y1)
      {
        break;
      }

      err += dx;
      y0 += sy;
    }

    if(err_two >= dy)
    {
      if(x0 == x1)
      {
        break;
      }

      err += dy;
      x0 += sx;
    }
  }

  return true;
}

b32 is_tile_close(iv2_t p, i32 tile)
{
     // Up, Down
  if(level[((p.y - 1) * LEVEL_WIDTH_IN_TILES) + p.x] == tile ||
     level[((p.y + 1) * LEVEL_WIDTH_IN_TILES) + p.x] == tile ||

     // Left, Right
     level[(p.y * LEVEL_WIDTH_IN_TILES) + (p.x - 1)] == tile ||
     level[(p.y * LEVEL_WIDTH_IN_TILES) + (p.x + 1)] == tile ||

     // Left Up, Right Up
     level[((p.y - 1) * LEVEL_WIDTH_IN_TILES) + (p.x - 1)] == tile ||
     level[((p.y - 1) * LEVEL_WIDTH_IN_TILES) + (p.x + 1)] == tile ||

     // Left Down, Right Down
     level[((p.y + 1) * LEVEL_WIDTH_IN_TILES) + (p.x - 1)] == tile ||
     level[((p.y + 1) * LEVEL_WIDTH_IN_TILES) + (p.x + 1)] == tile)
  {
    return true;
  }

  return false;
}

i32 distance(iv2_t a, iv2_t b)
{
  return sqrt(((b.x - a.x) * (b.x - a.x)) + ((b.y - a.y) * (b.y - a.y)));
}

i32 tile_div(i32 n)
{
  return (n) / TILE_SIZE;
}

i32 tile_mul(i32 n)
{
  return (n) * TILE_SIZE;
}