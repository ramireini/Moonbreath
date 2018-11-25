#ifndef TEXT_RENDER_H
#define TEXT_RENDER_H

#include <game.h>
#include <util_io.h>

#define TEXT_COLOR_WHITE 0xFFFFF0FF
#define TEXT_COLOR_GREEN 0x228B22FF
#define TEXT_COLOR_BROWN 0xDEB887FF
#define TEXT_COLOR_YELLOW 0xCCCC00FF
#define TEXT_COLOR_RED 0xB22222FF
#define TEXT_COLOR_BLUE 0x005D92FF

// Returns a malloc'd struct, REMEMBER TO FREE!
ttf_font_t* create_ttf_font_atlas(TTF_Font *font);
void render_text_ttf(char *str, int x, int y, int text_color, ttf_font_t *font_struct, ...);

// Returns a malloc'd struct, REMEMBER TO FREE!
bmp_font_t* create_bmp_font_atlas(char *path, int glyph_width, int glyph_height, int bmp_pitch);
void render_text_bmp(char *str, int text_x, int text_y, int text_color, bmp_font_t *bmp_font, ...);

#endif // TEXT_RENDER_H