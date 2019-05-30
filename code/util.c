internal inline v2_t
v2(i32 a, i32 b)
{
  v2_t result = {{a, b}};
  return result;
}

internal inline v4_t
v4(i32 a, i32 b, i32 c, i32 d)
{
  v4_t result = {{a, b, c, d}};
  return result;
}

internal i32
v2_equal(v2_t a, v2_t b)
{
  i32 result = 0;

  if(a.x == b.x && a.y == b.y)
  {
    result = 1;
  }

  return result;
}

internal i32
is_inside_level(v2_t pos)
{
  i32 result = 0;

  if(pos.x >= 0 && pos.x < LEVEL_WIDTH_IN_TILES &&
     pos.y >= 0 && pos.y < LEVEL_HEIGHT_IN_TILES)
  {
    result = 1;
  }

  return result;
}

internal i32
is_occupied(v2_t pos)
{
  i32 result = 0;

  if(level.occupied[(pos.y * LEVEL_WIDTH_IN_TILES) + pos.x])
  {
    result = 1;
  }

  return result;
}

internal void
set_occupied(v2_t pos, b32 value)
{
  level.occupied[(pos.y * LEVEL_WIDTH_IN_TILES) + pos.x] = value;
}

// NOTE(Rami):
// This is supposed to house all of our traversable tiles so we can check against them
internal i32
is_traversable(v2_t p)
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

internal SDL_Texture*
load_texture(char *path, v4_t *color_key)
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

// NOTE(Rami): Maybe we could make a new .c file for the functions
// that do work on entities that don't belong under things like
// animation, lighting etc. Just go group them up..
internal i32
heal_entity(entity_t *entity, i32 amount)
{
  i32 result = 0;

  if(entity->hp == entity->max_hp)
  {
    result = 0;
  }
  else
  {
    result = 1;

    entity->hp += amount;
    if(entity->hp > entity->max_hp)
    {
      entity->hp = entity->max_hp;
    }
  }

  return result;
}

internal i32
attack_entity(entity_t *attacker, entity_t *defender)
{
  i32 result = 0;

  defender->hp -= attacker->damage;
  if(defender->hp <= 0)
  {
    result = 1;
  }

  return result;
}

// NOTE(Rami): Do we need this?
// i32 inside_level(v2_t p)
// {
//   if(p.x < 0 || p.y < 0 || p.x >= LEVEL_WIDTH_IN_TILES || p.y >= LEVEL_HEIGHT_IN_TILES)
//   {
//     return 0;
//   }

//   return 1;
// }

// NOTE(Rami): Do we need this?
// internal v4_t
// hex_to_rgba(i32 hex)
// {
//   v4_t rgba = v4((hex >> 24) & 0xFF, (hex >> 16) & 0xFF, (hex >> 8) & 0xFF, hex & 0xFF);
//   return rgba;
// }

// NOTE(Rami): Do we need this?
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
rand_num(i32 min, i32 max)
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
is_tile(v2_t pos, u32 tile)
{
  i32 result = 0;

  if(level.tiles[(pos.y * LEVEL_WIDTH_IN_TILES) + pos.x] == tile)
  {
    result = 1;
  }

  return result;
}

// NOTE(Rami): Does not consider diagonal movement
internal inline i32
tile_dist(v2_t a, v2_t b)
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