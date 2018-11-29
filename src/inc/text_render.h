#ifndef TEXT_RENDER_H
#define TEXT_RENDER_H

#include <game.h>

#define TEXT_COLOR_WHITE 0xFFFFF0FF
#define TEXT_COLOR_RED 0xB22222FF
#define TEXT_COLOR_BLUE 0x196D9CFF
#define TEXT_COLOR_GREEN 0x389638FF
#define TEXT_COLOR_YELLOW 0xEEEE27FF
#define TEXT_COLOR_BROWN 0xC99C70FF

// Returns a malloc'd struct, REMEMBER TO FREE!
ttf_font_t* create_ttf_font_atlas(TTF_Font *font);
void render_text_ttf(char *str, int x, int y, int text_color, ttf_font_t *font_struct, ...);

// Returns a malloc'd struct, REMEMBER TO FREE!
bmp_font_t* create_bmp_font_atlas(char *path, int glyph_w, int glyph_h, int bmp_pitch);
int render_text_bmp(char *str, int text_x, int text_y, int text_color, bmp_font_t *bmp_font, ...);

#endif // TEXT_RENDER_H