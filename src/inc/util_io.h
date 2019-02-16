#ifndef UTIL_IO_H
#define UTIL_IO_H

#include <game.h>

int32 tile_div(int32 n);

int32 tile_mul(int32 n);

char* io_read_file(char *path, char *mode);

SDL_Texture* load_texture(char *path, SDL_Color *color_key);

SDL_Color hex_to_rgba(int32 hex);

int32 str_to_int(char *str);

int32 num_between(int32 from, int32 to);

int32 line_of_sight(int32 x0, int32 y0, int32 x1, int32 y1);

int32 is_player_pos(int x, int y);

int32 is_traversable_pos(int32 x, int32 y);

int32 is_tile_close(int32 x, int32 y, int32 tile);

uint32 distance(uint32 x0, uint32 y0, uint32 x1, uint32 y1);

#endif // UTIL_IO_H