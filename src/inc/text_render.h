#ifndef TEXT_RENDER_H
#define TEXT_RENDER_H

#include <game.h>

#define TEXT_COLOR_WHITE 0xFFFFF0FF
#define TEXT_COLOR_RED 0xB22222FF
#define TEXT_COLOR_BLUE 0x196D9CFF
#define TEXT_COLOR_GREEN 0x389638FF
#define TEXT_COLOR_YELLOW 0xEEE523FF
#define TEXT_COLOR_BROWN 0xC99C70FF

// Returns a malloc'd struct, REMEMBER TO FREE!
font_t* create_ttf_font_atlas(TTF_Font *font, int space_in_px);
// Returns a malloc'd struct, REMEMBER TO FREE!
font_t* create_bmp_font_atlas(char *path, int glyph_w, int glyph_h, int glyphs_per_row, int space_in_px, int shared_advance_in_px);
void render_text(char *str, int str_x, int str_y, int str_color, font_t *font, ...);

#endif // TEXT_RENDER_H