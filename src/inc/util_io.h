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
SDL_Color hex_to_rgba(int hex);

// [str] [pointer to a char string]
//
// [returns the char string in integer form]
int str_to_int(char *str);

// [from] [lowest number possible]
// [to] [highest number possible]
//
// [returns a random number between from and to]
int rand_int(int from, int to);

// NOTE(Rami): make function interface
int line_of_sight(char *level, int x0, int y0, int x1, int y1);

// [level] [pointer to the level array]
// [x] [the x position of the tile]
// [y] [the y position of the tile]
//
// [returns 1 if the tile is traversable and 0 if the tile is not traversable]
int is_traversable(char *level, int x, int y);

// NOTE(Rami): make function interface
int is_tile_close(char *level, int x, int y, int tile);

// NOTE(Rami): make function interface
double distance(double x0, double y0, double x1, double y1);

#endif // UTIL_IO_H