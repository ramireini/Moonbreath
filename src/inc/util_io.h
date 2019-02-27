#ifndef UTIL_IO_H
#define UTIL_IO_H

i32 tile_div(i32 n);
i32 tile_mul(i32 n);
char* io_read_file(char *path, char *mode);
SDL_Texture* load_texture(char *path, SDL_Color *color_key);
SDL_Color hex_to_rgba(i32 hex);
i32 str_to_int(char *str);
i32 rnum(i32 from, i32 to);
b32 str_cmp(char *a, char *b);
b32 line_of_sight(i32 x0, i32 y0, i32 x1, i32 y1);
b32 is_player_pos(int x, int y);
b32 is_traversable_pos(i32 x, i32 y);
b32 is_tile_close(i32 x, i32 y, i32 tile);
i32 distance(i32 x0, i32 y0, i32 x1, i32 y1);

#endif // UTIL_IO_H