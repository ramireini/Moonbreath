#ifndef UTIL_IO_H
#define UTIL_IO_H

#include <game.h>

// [path] [path to the file]
// [mode] [access mode]
//
// [returns a malloc'd char array, REMEMBER TO FREE]
char* io_read_file(char *path, char *mode);

// [path] [path to the file]
// [enable_color_key] [1 to enable, 0 to disable the color key feature]
// [color_key] [if enable_color_key is 1, this color will be used for the color key]
//
// [returns a pointer to an SDL_Texture]
SDL_Texture* load_texture(char *path, SDL_Color *color_key);

// [hex_color] [a hexadecimal color format of 0xRRGGBBAA]
//
// [returns an SDL_Color struct with the RGBA values]
SDL_Color hex_to_rgba(int32 hex);

// [str] [pointer to a char string]
//
// [returns the char string in integer form]
int32 str_to_int(char *str);

// [from] [lowest number possible]
// [to] [highest number possible]
//
// [returns a random number between from and to]
int32 rand_int(int32 from, int32 to);

// NOTE(Rami): make function interface
int32 line_of_sight(char *level, int32 x0, int32 y0, int32 x1, int32 y1);

// [level] [pointer to the level array]
// [x] [the x position of the tile]
// [y] [the y position of the tile]
//
// [returns 1 if the tile is traversable and 0 if the tile is not traversable]
int32 is_traversable(char *level, int32 x, int32 y);

// NOTE(Rami): make function interface
int32 is_tile_close(char *level, int32 x, int32 y, int32 tile);

// NOTE(Rami): make function interface
double distance(real64 x0, real64 y0, real64 x1, real64 y1);

#endif // UTIL_IO_H