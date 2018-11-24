#ifndef UTIL_IO_H
#define UTIL_IO_H

#include <game.h>

// [path] [path to the file]
// [mode] [access mode] 
// 
// [returns a malloc'd char array, REMEMBER TO FREE]
char* io_read_file(char *path, char *mode);

// [path] [path to the file
// 
// [returns a pointer to an SDL_Texture]]
SDL_Texture* load_texture(char *path, int enable_color_key);

// [hex_color] [a hexadecimal color like so 0xAABBCC]
// 
// [returns an SDL_Color struct with the RGBA values]
SDL_Color hex_to_rgba_color(int hex_color);

#endif // UTIL_IO_H