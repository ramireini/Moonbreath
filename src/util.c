// NOTE(Rami): This is supposed to house all of our traversable tiles so we can check against them.
bool is_traversable(iv2_t p)
{
  if(level.tiles[(p.y * LEVEL_WIDTH_IN_TILES) + p.x] == tile_floor_stone ||
     level.tiles[(p.y * LEVEL_WIDTH_IN_TILES) + p.x] == tile_floor_grass ||
     level.tiles[(p.y * LEVEL_WIDTH_IN_TILES) + p.x] == tile_door_open)
  {
    return true;
  }

  return false;
}

char* read_file(char *path, char *mode)
{
  FILE *file = fopen(path, mode);
  if(!file)
  {
    debug("Could not read file %s", path);
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
    debug("SDL could not load image %s: %s", path, IMG_GetError());
    return NULL;
  }

  if(color_key)
  {
    // store the rgb color into color_key in the color format of the surface
    // all pixels with the color of color_key will be transparent
    i32 formatted_key = SDL_MapRGB(loaded_surf->format, color_key->r, 0, 0);
    SDL_SetColorKey(loaded_surf, SDL_TRUE, formatted_key);
  }

  SDL_Texture *new_tex = SDL_CreateTextureFromSurface(game_state.renderer, loaded_surf);
  if(!new_tex)
  {
    debug("SDL could not create a texture from surface: %s", SDL_GetError());
    SDL_FreeSurface(loaded_surf);
    return NULL;
  }

  SDL_FreeSurface(loaded_surf);
  return new_tex;
}

bool attack_entity(entity_t *attacker, entity_t *target)
{
  target->hp -= attacker->damage;
  if(target->hp <= 0)
  {
    return true;
  }

  return false;
}

bool is_inside_level(iv2_t p)
{
  if(p.x < 0 || p.y < 0 || p.x >= LEVEL_WIDTH_IN_TILES || p.y >= LEVEL_HEIGHT_IN_TILES)
  {
    return false;
  }

  return true;
}

internal inline bool is_iv2_equal(iv2_t a, iv2_t b)
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

bool str_cmp(char *a, char *b)
{
  while(*a && *b && *a++ == *b++)
  {
    if(*a == '\0' && *b == '\0')
    {
      return true;
    }
  }

  return false;
}

bool line_of_sight(iv2_t a, iv2_t b)
{
  i32 dx = abs(a.x - b.x);
  i32 sx = a.x < b.x ? 1 : -1;

  i32 dy = -abs(a.y - b.y);
  i32 sy = a.y < b.y ? 1 : -1;

  i32 err = dx + dy;
  i32 err_two = 0;

  for(;;)
  {
    if(!is_traversable(a))
    {
      return false;
    }

    err_two = err * 2;
    if(err_two <= dx)
    {
      if(a.y == b.y)
      {
        break;
      }

      err += dx;
      a.y += sy;
    }

    if(err_two >= dy)
    {
      if(a.x == b.x)
      {
        break;
      }

      err += dy;
      a.x += sx;
    }
  }

  return true;
}

bool is_tile_in_range(iv2_t p, i32 tile, i32 w, i32 h)
{
  for(i32 y = p.y; y < p.y + h; y++)
  {
    for(i32 x = p.x; x < p.x + w; x++)
    {
      if(level.tiles[(y * LEVEL_WIDTH_IN_TILES) + x] == tile)
      {
        return true;
      }
    }
  }

  return false;
}

// NOTE(Rami): Does not consider diagonal movement.
i32 dist_in_tiles(iv2_t a, iv2_t b)
{
  return abs(a.x - b.x) + abs(a.y - b.y);
}

i32 tile_div(i32 n)
{
  return n / TILE_SIZE;
}

i32 tile_mul(i32 n)
{
  return n * TILE_SIZE;
}