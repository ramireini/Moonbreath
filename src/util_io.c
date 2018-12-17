#include <util_io.h>

char* io_read_file(char *path, char *mode)
{
  // open file
  FILE *file = fopen(path, mode);
  if(!file)
  {
    printf("Could not read file %s\n", path);
    return NULL;
  }

  // get file length
  fseek(file, 0, SEEK_END);
  size_t size = ftell(file);
  rewind(file);

  // allocate memory for file data
  char *buff = malloc(size + 1);

  // copy file contents into buffer
  fread(buff, size, 1, file);

  // null-terminate buffer
  buff[size] = '\0';

  // close file
  fclose(file);
  return buff;
}

SDL_Texture* load_texture(char *path, SDL_Color *color_key)
{
  SDL_Surface *loaded_surf = IMG_Load(path);
  if(!loaded_surf)
  {
    printf("SDL could not load image %s: %s\n", path, IMG_GetError());
    return NULL;
  }

  if(color_key)
  {
    // store the rgb color into color_key in the color format of the surface
    // all pixels with the color of color_key will be transparent
    int formatted_key = SDL_MapRGB(loaded_surf->format, color_key->r, 0, 0);
    SDL_SetColorKey(loaded_surf, SDL_TRUE, formatted_key);
  }

  SDL_Texture *new_tex = SDL_CreateTextureFromSurface(renderer, loaded_surf);
  if(!new_tex)
  {
    printf("SDL could not create a texture from surface: %s\n", SDL_GetError());
    SDL_FreeSurface(loaded_surf);
    return NULL;
  }

  SDL_FreeSurface(loaded_surf);
  return new_tex;
}

SDL_Color hex_to_rgba(int hex)
{
  SDL_Color rgb_color = {(hex >> 24) & 0xFF, (hex >> 16) & 0xFF, (hex >> 8) & 0xFF, hex & 0xFF};
  return rgb_color;
}

int str_to_int(char *str)
{
  int result = 0;

  while(*str >= '0' && *str <= '9')
  {
    result *= 10;
    result += *str - '0';
    str++;
  }

  return result;
}

int rand_int(int from, int to)
{
  return from + rand() % (to - from + 1);
}

int line_of_sight(char *level, int x0, int y0, int x1, int y1)
{
  x0 = to_tiles(x0);
  y0 = to_tiles(y0);

  x1 = to_tiles(x1);
  y1 = to_tiles(y1);

  if(!is_traversable(level, x0, y0))
  {
    return 0;
  }

  int dx = abs(x1 - x0);
  int sx = x0 < x1 ? 1 : -1;

  int dy = -abs(y1 - y0);
  int sy = y0 < y1 ? 1 : -1;

  int err = dx + dy, err_two;

  for (;;)
  {
    if(!is_traversable(level, x0, y0))
    {
      return 0;
    }
    
    err_two = err * 2;

    if (err_two <= dx)
    {
      if (y0 == y1) { break; }

      err += dx;
      y0 += sy;
    }

    if (err_two >= dy)
    {
      if (x0 == x1) { break; }

      err += dy;
      x0 += sx;
    }
  }

  return 1;
}

int is_traversable(char *level, int x, int y)
{
  if(level[(y * LEVEL_SIZE) + x] == TILE_FLOOR_GRASS ||
     level[(y * LEVEL_SIZE) + x] == TILE_FLOOR_STONE ||
     level[(y * LEVEL_SIZE) + x] == TILE_DOOR_OPEN)
  {
    return 1;
  }

  return 0;
}

int is_tile_close(char *level, int x, int y, int tile)
{
  x = to_tiles(x);
  y = to_tiles(y);

     // Up, Down
  if(level[((y - 1) * LEVEL_SIZE) + x] == tile ||
     level[((y + 1) * LEVEL_SIZE) + x] == tile ||

     // Left, Right
     level[(y * LEVEL_SIZE) + (x - 1)] == tile ||
     level[(y * LEVEL_SIZE) + (x + 1)] == tile ||

     // Left Up, Right Up
     level[((y - 1) * LEVEL_SIZE) + (x - 1)] == tile ||
     level[((y - 1) * LEVEL_SIZE) + (x + 1)] == tile ||

     // Left Down, Right Down
     level[((y + 1) * LEVEL_SIZE) + (x - 1)] == tile ||
     level[((y + 1) * LEVEL_SIZE) + (x + 1)] == tile)
  {
    return 1;
  }

  return 0;
}

double distance(double x0, double y0, double x1, double y1)
{
  double x_diff = (x1 - x0) * (x1 - x0);
  double y_diff = (y1 - y0) * (y1 - y0);
  double total_diff = x_diff + y_diff;
  double dist = sqrt(total_diff);

  // double dist = sqrt(((x1 - x0) * (x1 - x0)) + ((y1 - y0) * (y1 - y0)));
  return dist;
}