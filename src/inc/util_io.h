#ifndef UTIL_IO_H
#define UTIL_IO_H

// [path] [path to the file]
// [mode] [access mode] 
// 
// [returns a malloc'd char array, REMEMBER TO FREE]
static char* io_read_file(char *path, char *mode)
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

static SDL_Texture* load_texture(char *path)
{
  SDL_Texture *new_tex = NULL;

  SDL_Surface *loaded_surf = IMG_Load(path);
  if(!loaded_surf)
  {
    printf("SDL could not load image %s: %s\n", path, IMG_GetError());
  }
  else
  {
    // create texture from surface
    new_tex = SDL_CreateTextureFromSurface(renderer, loaded_surf);
    if(!new_tex)
    {
      printf("SDL could not create a texture from surface: %s\n", SDL_GetError());
    }

    SDL_FreeSurface(loaded_surf);
  }

  return new_tex;
}

static SDL_Color hex_to_rgba_color(int hex_color)
{
  // shift and mask the rgb out of the hex color
  int r = (hex_color >> 24) & 0xFF;
  int g = (hex_color >> 16) & 0xFF;
  int b = (hex_color >> 8) & 0xFF;
  int a = hex_color & 0xFF;

  SDL_Color rgb_color = {r, g, b, a};

  return rgb_color;
}

#endif // UTIL_IO_H