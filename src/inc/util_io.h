#ifndef UTIL_IO_H
#define UTIL_IO_H

#include <game.h>

// [path] [path to the file]
// [mode] [access mode] 
// 
// [returns a malloc'd char array, REMEMBER TO FREE]
char* io_read_file(char *path, char *mode);

// [path] [path to the file
// [enable_color_key] [1 to enable, 0 to disable the color key feature]
// [color_key] [if enable_color_key is 1, this color will be used for the color key]
// 
// [returns a pointer to an SDL_Texture]]
SDL_Texture* load_texture(char *path, SDL_Color *color_key);

// [hex_color] [a hexadecimal color like so 0xAABBCC]
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

// NOTE(Rami): function interface
void plot_line(int x0, int y0, int x1, int y1);

#endif // UTIL_IO_H