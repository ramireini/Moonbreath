#ifndef UTIL_IO_H
#define UTIL_IO_H

#include <game.h>

// [n] [the value to turn into a tile amount]
// [returns the tile amount]
int32 tile_div(int32 n);

// [n] [the value to turn into a pixel amount]
// [returns the pixel amount]
int32 tile_mul(int32 n);

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

// [level] [pointer to the level array]
// [x0] [the origin x position]
// [y0] [the origin y position]
// [x1] [the target x position]
// [y1] [the target y position]

// [returns 1 if there is a line of sight]
// [returns 0 if there is no line of sight]
int32 line_of_sight(char *level, int32 x0, int32 y0, int32 x1, int32 y1);

// [x] [x position]
// [y] [y position]
//
// [returns 1 if the player is in that position]
// [return 0 if the player is not in that position]
int32 is_player_pos(int x, int y);

// [level] [pointer to the level array]
// [x] [the array element x position of the tile]
// [y] [the array element y position of the tile]
//
// [returns 1 if the tile is traversable]
// [returns 0 if the tile is not traversable]
int32 is_traversable_pos(char *level, int32 x, int32 y);

// [level] [pointer to the level array]
// [x] [checks will be done around the given x]
// [y] [checks will be done around the given y]
// [tile] [the tile to be checked for around given coordinates]
// 
// [returns 1 if the tile was found around given coordinates]
// [returns 0 if the tile was not found around given coordinates]
int32 is_tile_close(char *level, int32 x, int32 y, int32 tile);

// [x0] [x position of the first point]
// [y0] [y position of the first point]
// [x1] [x position of the second point]
// [y1] [y position of the second point]

// [returns the distance between the two given points]
uint32 distance(uint32 x0, uint32 y0, uint32 x1, uint32 y1);

#endif // UTIL_IO_H